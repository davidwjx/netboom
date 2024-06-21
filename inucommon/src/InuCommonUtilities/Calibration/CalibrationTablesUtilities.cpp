#include "OSUtilities.h"
#include "CalibrationTablesUtilities.h"
#ifdef __ANDROID__
#include <archive.h>
#include <archive_entry.h>
#endif
#include <boost/filesystem.hpp>
#include <fstream>


using namespace InuCommon;
using namespace std;

const string CCalibrationTablesUtilities::COMPRESSED_FILE_NAME("Camera.7z");
const string CCalibrationTablesUtilities::SW_REVISION_STR("SWRevision");

#ifdef _MSC_VER
const string CCalibrationTablesUtilities::COMPRESSION_APP_NAME("7z.exe");
#else
// Ubuntu / RPI / ANDDROID
const string CCalibrationTablesUtilities::COMPRESSION_APP_NAME("7za");
#endif

#ifdef __ANDROID__
//#define _DEBUG 1
#ifdef _DEBUG 
#include <android/log.h>
#endif

namespace fs = std::filesystem;

static void addFileToArchive(struct archive* a, const fs::path& filePath, const std::string& entryName) {
    struct archive_entry* entry = archive_entry_new();
    archive_entry_set_pathname(entry, entryName.c_str());
    archive_entry_set_filetype(entry, AE_IFREG); // Regular file type

#ifdef _DEBUG
    __android_log_print(ANDROID_LOG_ERROR, "InuDev", "File: %s" , entryName.c_str());   
#endif

    // Set file size
    archive_entry_set_size(entry, static_cast<int64_t>(fs::file_size(filePath)));

    // Write file entry to archive
    archive_write_header(a, entry);
    
    // Open and write file data to archive
    FILE* file = fopen(filePath.string().c_str(), "rb");
    if (!file) {
        archive_entry_free(entry);
        throw std::runtime_error("Failed to open file: " + filePath.string());
    }

    char buf[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buf, 1, sizeof(buf), file)) > 0) {
        archive_write_data(a, buf, bytesRead);
    }

    fclose(file);
    archive_entry_free(entry);
}

static void addDirectoryToArchive(struct archive* a, const const fs::path& directoryPath, const std::string& entryName) {
    struct archive_entry* entry = archive_entry_new();
    archive_entry_set_pathname(entry, entryName.c_str());
    archive_entry_set_filetype(entry, AE_IFDIR); // Directory type

#ifdef _DEBUG
        _android_log_print(ANDROID_LOG_ERROR, "InuDev", "Folder: %s" , entryName.c_str());           
#endif        
 
    // Write directory entry to archive
    archive_write_header(a, entry);
    archive_entry_free(entry);

    // Recursively add contents of the directory to the archive
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            addFileToArchive(a, entry.path(), (entryName / entry.path().filename()).string());
        } else if (entry.is_directory()) {
            addDirectoryToArchive(a, entry.path(), (entryName / entry.path().filename()).string());
        }
    }
}

static void compressFolder(const std::string& folderPath, const std::string& outputPath)
{
    struct archive* a;
    struct archive_entry* entry;
    int flags;

    // Create a new archive
    a = archive_write_new();

    //archive_write_add_filter_by_name(a, "compress");
    archive_write_set_format_zip(a);

    // Set the output file path
    auto err = archive_write_open_filename(a, outputPath.c_str());
    if (err != ARCHIVE_OK) {
        auto errStr = archive_error_string(a);
        throw std::runtime_error("Failed to open archive for writing, error code: " + std::to_string(err) + " , error string: " + errStr);
    }

    // Add the entire folder and its contents to the archive
    addDirectoryToArchive(a, folderPath, fs::path(folderPath).filename().string());

    archive_write_close(a);
    archive_write_free(a);
}

static int copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  la_int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r < ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r < ARCHIVE_OK) {
      return (r);
    }
  }
}


static bool extract(const string& filename, const string& destination)
{
  struct archive *a;
  struct archive *ext;
  struct archive_entry *entry;
  int flags;
  int r;

  /* Select which attributes we want to restore. */
  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  a = archive_read_new();
  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);
  if ((r = archive_read_open_filename(a, filename.c_str(), 10240)))
	  return false;

  for (;;)
  {
    r = archive_read_next_header(a, &entry);
    if (r == ARCHIVE_EOF)
      break;
    /*
    if (r < ARCHIVE_OK)
      fprintf(stderr, "%s\n", archive_error_string(a));
      */
    if (r < ARCHIVE_WARN)
    	return false;

    //save to path
    const char* currentFile = archive_entry_pathname(entry);
    const std::string fullOutputPath = destination + currentFile;
    archive_entry_set_pathname(entry, fullOutputPath.c_str());
    r = archive_write_header(ext, entry);
    /*
    if (r < ARCHIVE_OK)
      fprintf(stderr, "%s\n", archive_error_string(ext));
    */
    if (archive_entry_size(entry) > 0)
    {
      r = copy_data(a, ext);
      if (r < ARCHIVE_WARN)
    	  return false;
    }

    r = archive_write_finish_entry(ext);
    if (r < ARCHIVE_WARN)
    	return false;
  }
  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
  return true;
}
#endif

CCalibrationTablesUtilities::ECompressionError CCalibrationTablesUtilities::Compress(const std::string& iCalibrationData, std::vector<unsigned char>& oBuffer, std::string& oCompressedFileName)
{
    CCalibrationTablesUtilities::ECompressionError result = eOK;

    std::string tmp = InuCommon::COsUtilities::getenv(InuCommon::COsUtilities::INUITIVE_PATH);
    boost::filesystem::path  exeFile(boost::filesystem::current_path() / COMPRESSION_APP_NAME.c_str());
    if (!tmp.empty())
    {
        exeFile = boost::filesystem::path(tmp.c_str()) / "bin" / COMPRESSION_APP_NAME.c_str();
    }

    boost::system::error_code boostErr;

    uintmax_t fileSize = 0;
    boost::filesystem::path cameraDir;
    boost::filesystem::path  compressedFileName(COsUtilities::GetTempFolder());

    if (iCalibrationData.size() > 0 )
    {
        cameraDir = iCalibrationData;
    }

    if (COsUtilities::FileExists(cameraDir.string()))
    {

        // Copy input directory to temporary destination
        boost::filesystem::path  tmpDir(COsUtilities::GetTempFolder());
        if(!COsUtilities::FileExists(tmpDir.string()))
        {
            // Create Data folder if it doesn't exist
            boost::filesystem::create_directory(COsUtilities::ConvertEncoding(tmpDir.string()), boostErr);
            if (boostErr != boost::system::errc::success)
            {
                return eTempFolderCreating;
            }
        }

        tmpDir /= cameraDir.leaf();
        if(COsUtilities::FileExists(tmpDir.string()))
        {
            boost::filesystem::remove_all(COsUtilities::ConvertEncoding(tmpDir.string()), boostErr);
            if (boostErr != boost::system::errc::success)
            {
                return eTempFolderCleaning;
            }
        }

        // Copy only relevant files
        if (CopyCalibrationDir(cameraDir, tmpDir) == false)
        {
            return eCopyFolder;
        }

        // Compress
        compressedFileName /= COMPRESSED_FILE_NAME.c_str();
        if(COsUtilities::FileExists(compressedFileName.string()))
        {
            boost::filesystem::remove(COsUtilities::ConvertEncoding(compressedFileName.string()), boostErr);
        }

#ifdef __ANDROID__
        try
        {
            compressFolder(tmpDir.string(), compressedFileName.string());
        }
        catch (const std::exception& e) {
#ifdef _DEBUG         
            __android_log_print(ANDROID_LOG_ERROR, "InuDev", "Failed to compress folder %s to file %s , error: %s",
                 tmpDir.string().c_str(), compressedFileName.string().c_str(), e.what());
#endif            
            return eCompressionError;
        }
        catch(...) {
#ifdef _DEBUG               
            __android_log_print(ANDROID_LOG_ERROR, "InuDev", "Failed to compress folder with uknown exception");
#endif            
            return eCompressionError;
        }
#else           
#ifdef _MSC_VER
        string command("\"\"" + exeFile.string());
        command += "\" a -tzip \"";
        command += compressedFileName.string();
        command += "\" \"";
        command += tmpDir.string();
        command += "\" -y\"";
        command += "> nul";  // avoid prints to stdout on windows
#else
        std::string command = "\"" + exeFile.string() + "\" a -tzip \"" + compressedFileName.string() + "\" \"" + tmpDir.string() + "\" -y";
#endif
        
        int errCode = system(command.c_str());
        if (errCode)
        {
            return eCompressionError;
        }
#endif
        // Prepare buffer for flash
        fileSize=boost::filesystem::file_size(COsUtilities::ConvertEncoding(compressedFileName.string()));


    }
    else
    {
        result = eMissingCameraFiles;        
    }

    if (fileSize > 0)
    {
        // concatenate file contents 
        ifstream  fileObj(COsUtilities::ConvertEncoding(compressedFileName.string()), ios::binary);
        if (fileObj.bad())
        {
            result = eCompressedFileOpenning;
        }
        else
        {
            oCompressedFileName = compressedFileName.string();
            oBuffer.resize((unsigned int)fileSize);
            fileObj.read(reinterpret_cast<char*>(&(oBuffer[0])), fileSize);
            if (fileObj.fail())
            {
                result = eCompressedFileReading;
            }
        }
    }

    return result;
}

bool CCalibrationTablesUtilities::CopyCalibrationDir(const boost::filesystem::path& iSource, const boost::filesystem::path& iDestination)
{
    namespace fs = boost::filesystem;
    try
    {
        // Don't copy intermediate folder
        if ( (iSource.leaf() == "Intermediate_data") || (iSource.leaf() == "Debug") || (iSource.leaf() == "Distortion_rectification") || (iSource.leaf() == "Intensity_balance"))
        {
            return true;
        }

        // Check whether the function call is valid
        if(!COsUtilities::FileExists(iSource.string()) || COsUtilities::FileExists(iDestination.string()))
        {
            return false;
        }

        // Create the destination directory
        if(!fs::create_directory(COsUtilities::ConvertEncoding(iDestination.string())))
        {
            return false;
        }
    }
    catch(fs::filesystem_error const &)
    {
        return false;
    }

    // Iterate through the source directory
    for(fs::directory_iterator file(iSource);  file != fs::directory_iterator(); ++file)
    {
        try
        {
            fs::path current(file->path());
            if(fs::is_directory(COsUtilities::ConvertEncoding(current.string())))
            {
                // Found directory: Recursion
                if(!CopyCalibrationDir( current, iDestination / current.filename()))
                {
                    return false;
                }
            }
            else
            {
                // Found file: Copy
                fs::copy_file(COsUtilities::ConvertEncoding(current.string()), COsUtilities::ConvertEncoding((iDestination / current.filename()).string()) );
            }
        }
        catch(fs::filesystem_error&)
        {
            return false;
        }
    }
    return true;
}

// Decompress camera calibration data (which is read from flash). 
CCalibrationTablesUtilities::ECompressionError CCalibrationTablesUtilities::Decompress(
    const char* iBuffer,
    uint32_t iBufferSize,
    const std::string& iSensorID,
    uint64_t iLastBurnTime,
    const std::string& iDestinationPath)
{
    std::string tmp;
    return Decompress(tmp, iBuffer, iBufferSize, iSensorID, iLastBurnTime, iDestinationPath);
}

// Decompress camera calibration data (which is read from flash). 
CCalibrationTablesUtilities::ECompressionError CCalibrationTablesUtilities::Decompress(
    std::string& oCalibrationDirName,
    const char* iBuffer,
    uint32_t iBufferSize,
    const std::string& iSensorID, 
    uint64_t iLastBurnTime,
    const std::string& iDestinationPath)
{
	if (iBufferSize == 0)
    {
        return eEmptyInputBuffer;
    }

    boost::system::error_code boostErr;

    std::string tmp = InuCommon::COsUtilities::getenv(InuCommon::COsUtilities::INUITIVE_PATH);
#ifdef __ANDROID__
    boost::filesystem::path  exeFile("/data/data/" + InuCommon::COsUtilities::GetExeFileName());
    exeFile /= COMPRESSION_APP_NAME.c_str();
#else
    //#ifdef _MSC_VER
    boost::filesystem::path  exeFile(boost::filesystem::current_path() / COMPRESSION_APP_NAME.c_str());

    if (!tmp.empty())
    {
        exeFile = boost::filesystem::path(tmp.c_str()) / "bin" / COMPRESSION_APP_NAME.c_str();
    }

    if (COsUtilities::FileExists(exeFile.string()) == false)
    {
        return eMissingCompressionApp;
    }

#endif

    // Save buffer to temporary file
    boost::filesystem::path  compressedFileName(COsUtilities::GetTempFolder());

    if (COsUtilities::FileExists(compressedFileName.string()) == false)
    {
        boost::filesystem::create_directory(COsUtilities::ConvertEncoding(compressedFileName.string()), boostErr);
        if (boostErr != boost::system::errc::success)
        {
            return eTempFolderCreating;
        }
    }

    compressedFileName /= COMPRESSED_FILE_NAME.c_str();
    if (COsUtilities::FileExists(compressedFileName.string()))
    {
        boost::filesystem::remove(COsUtilities::ConvertEncoding(compressedFileName.string()), boostErr);
        if (boostErr != boost::system::errc::success)
        {
            return eCompressedFileEarasing;
        }
    }

    std::ofstream tmpOutputFile(COsUtilities::ConvertEncoding(compressedFileName.string()), ios::binary);
    if (tmpOutputFile.bad())
    {
        return eCompressedFileCreating;
    }

    tmpOutputFile.write(reinterpret_cast<const char*>(&(iBuffer[0])), iBufferSize);

    if (tmpOutputFile.fail())
    {
        printf("Failed here 3 %d %s\n", (int)iBufferSize, compressedFileName.string().c_str());
        return eCompressedFileWriting;
    }
    tmpOutputFile.close();

    // Decompressed folder
    boost::filesystem::path  decompressFolder(COsUtilities::GetTempFolder());
    decompressFolder /= "Decompress";
    decompressFolder /= string(1, COsUtilities::PATH_SEPARATOR).c_str();
    if (COsUtilities::FileExists(decompressFolder.string()) == true)
    {
        boost::filesystem::remove_all(COsUtilities::ConvertEncoding(decompressFolder.string()), boostErr);
        if (boostErr != boost::system::errc::success)
        {
            return eTempFolderCleaning;
        }
    }

    boost::filesystem::create_directory(COsUtilities::ConvertEncoding(decompressFolder.string()), boostErr);
    if (boostErr != boost::system::errc::success)
    {
        return eTempFolderCreating;
    }
    // Decompress
#ifdef __ANDROID__
    if(!extract(COsUtilities::ConvertEncoding(compressedFileName.string()).c_str(),COsUtilities::ConvertEncoding(decompressFolder.string()).c_str()))
    {
   	    return eCompressionError;
    }
#else
    string command = BuildDecompressCommand(exeFile, compressedFileName, decompressFolder, "zip");

#ifdef _MSC_VER
    command += "> nul";  // avoid prints to stdout on windows
#endif

    int errCode = system(command.c_str());
    if (errCode)
    {
        // Backward compatibility, 7z compression  
        command = BuildDecompressCommand(exeFile, compressedFileName, decompressFolder, "7z");
        int errCode = system(command.c_str());
        if (errCode)
        {
            return eCompressionError;
        }
    }
#endif

#if  defined(__arm__) || defined(__aarch64__)
#ifndef  __ANDROID__
    // Should execute on ARM but not on ANDROID
    string chmodCommand("chmod 777 " + decompressFolder.string() + " -R");
    errCode = system(chmodCommand.c_str());
    if (errCode)
    {
        return eChmodError;
    }
#endif
#endif
    

    // At first, find the last modified folder
    boost::filesystem::path addedFolder;
    vector<boost::filesystem::directory_entry> subDir;
    copy(boost::filesystem::directory_iterator(COsUtilities::ConvertEncoding(decompressFolder.string())), boost::filesystem::directory_iterator(), back_inserter(subDir));
    //All names from here are UTF16 (If used from iterators)
    for (const boost::filesystem::directory_entry& iter : subDir)
    {
        if (boost::filesystem::is_directory(iter))
        {
            addedFolder = iter;
            break;
        }
    }
    if (addedFolder.empty())
    {
        return eMissingCameraFiles;
    }
    // Create Sensor folder
    boost::filesystem::path dir(iDestinationPath);
    if (iDestinationPath.empty())
    {
        dir = boost::filesystem::path(COsUtilities::GetCommonConfigPath()) / "InuSensors" / "";
    } 

    if (COsUtilities::FileExists(dir.string()) == false)
    {
        boost::filesystem::create_directory(COsUtilities::ConvertEncoding(dir.string()), boostErr);
        if (boostErr != boost::system::errc::success)
        {
            return eSensorsFolderCreating;
        }
    }

    // Add a prefix of sensorID
    string dirName = addedFolder.leaf().string();
    if (iSensorID.size() > dirName.size() || iSensorID != dirName.substr(0, iSensorID.size()))
    {
        boost::filesystem::path newPath(addedFolder.parent_path());
        newPath /= iSensorID.c_str();
        newPath += "_";
        newPath += dirName;
        boost::filesystem::rename(COsUtilities::ConvertEncoding(addedFolder.string()), COsUtilities::ConvertEncoding(newPath.string()), boostErr);
        if (boostErr == boost::system::errc::success)
        {
            addedFolder = newPath;
        }
    }

    dir /= addedFolder.leaf();

    // Delete older folder (if exists)
    if (COsUtilities::FileExists(dir.string()))
    {
        boost::filesystem::remove_all(COsUtilities::ConvertEncoding(dir.string()), boostErr);
        if (boostErr != boost::system::errc::success)
        {
            return eCopyFolder;
        }
    }

    // Copy the directory to its final location
    if (CopyCalibrationDir(addedFolder, dir) == false)
    {
        return eCopyFolder;
    }

    oCalibrationDirName = dir.string();

    // Overwrite metadata file Sensor ID - not required in InuCalibration workflow
    if (!iSensorID.empty() && (iLastBurnTime != std::numeric_limits<uint64_t>::max()) )
    {
        dir /= iSensorID;
        std::ofstream file(COsUtilities::ConvertEncoding(dir.string()));
        // Add current SWRevision to metadata file 
            file << SW_REVISION_STR << " = " << iLastBurnTime << endl;
        if (!file.good())
        {
                return eFailedToCreateMetaDataFile;
        }
    }

#ifdef __linux__ 
    // Fixing an issue with -ENET devices where EXT4 filesystem is used on embedded board with async mounting
    // Make sure calibartion files are saved to disk before next power down
    // See RND-
    system("sync");
#endif


    return eOK;
}

string CCalibrationTablesUtilities::BuildDecompressCommand(boost::filesystem::path& iExeFile, boost::filesystem::path& iCompressedFileName, boost::filesystem::path& iDecompressFolder, const std::string& iCompressionFormat)
{
#ifdef _MSC_VER
    string command("\"\"" + iExeFile.string());
    command += "\" x -t";
    command += iCompressionFormat;
    command += " \"";
    command += iCompressedFileName.string();
    command += "\" ";
    command += "-o\"";
    command += iDecompressFolder.string();
    command += "\" -y\"";
#else
    string command(iExeFile.string());
    command += " x -t";
    command += iCompressionFormat;
    command += " ";
    command += iCompressedFileName.string();
    command += " -o";
    command += iDecompressFolder.string();
    command += " -y";
#endif

    return command;
}
