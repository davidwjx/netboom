// AlgStaticTemporalFilter tester

#if _WIN32
#pragma warning(disable: 26812)
#endif

#include <iostream>
#include <chrono>
#include <regex>
#include <cassert>
#if _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>
#endif

#if defined(_MSC_VER)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif defined(__ANDROID__) || defined(__linux__)
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#endif

#include "OSUtilities.h"

#if !defined(_MSC_VER)
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include "StaticTemporalFilterBaseDLL.h"
#include "TiffFile.h"
#include "..\config.h"
#include "egetopt.h"
#include "CFileInfo.h"

constexpr int MAX_DIFFS{ 100 };
//--

#ifndef _WIN32
int _getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif

static void usage(char** argv)
{
    std::cerr <<
        "\nUsage: " << argv[0] << " [OPTIONS] <directory>"
        "\n"
        "\nOPTIONS:"
        "\n"
        "\n    -A          set mbAll param to \"true\""
        "\n    -a <n>      Optimization: 0 - naive, 1 - SSE, 2 - AVX"
        "\n    -b          Benchmark mode - eliminate output while computing"
        "\n    -f          Force removal of all files from output directory."
        "\n    -M          Use OpenMP"
        "\n    -m <count>  Multiplicity. Repeat invocation <count> times."
        "\n    -n <note>   Write runtime note"
        "\n    -o <dir>    Output directory. Optional."
        "\n    -p          Pre-read"
        "\n    -r <dir>    Reference directory. Optional."
        "\n    -v          Verbose compare results"
        "\n    -y <family> Select algorith family"
        "\n    -h          This text"
        "\n";
}

int main(int argc, char** argv)
{
    std::vector<CFileInfo> filesInfo;

    bool errFlag{ false };
    int c;

    fs::path outputDirectory;
    fs::path referenceDirectory;
    fs::path inputPath; // directory for files

    bool optVerbose{ false };
    bool optForceOutputCleanup{ false };
    bool optBenchmark{ false };
    int optMultiplicity{ 1 };
    InuCommon::COsUtilities::EOptimizationLevel optOptimizationLevel{ InuCommon::COsUtilities::EOptimizationLevel::eNone };
    unsigned int optFamily{ 0 };
    std::string optNote;
    bool optOpenMP{ false };
    bool optAll{ MB_ALL };
    bool optPreRead{ false };

    std::vector<fs::path> inputFileNames;

    std::chrono::nanoseconds totalTime{ 0 };
    CStaticTemporalFilterBase* pStaticTemporalFilter{ nullptr };

    while ((c = egetopt(argc, argv, _T("Aa:bho:r:fvm:My:n:p"))) != -1)
    {
        switch (c)
        {
        case -1:

            std::cout << "Invocation error" << std::endl;
            errFlag = true;
            break;

        case 'A': 

            optAll = true;
            break;

        case 'a': // optimization level

            optOptimizationLevel = (InuCommon::COsUtilities::EOptimizationLevel)std::atoi(eoptarg);
            break;

        case 'b':

            optBenchmark = true;
            break;

        case 'f':

            optForceOutputCleanup = true;
            break;

        case 'h': // Help

            usage(argv);
            return 0;

        case 'n': // Note

            optNote = std::string(eoptarg);
            break;

        case 'M':

            optOpenMP = true;
            break;

        case 'm':

            optMultiplicity = std::atoi(eoptarg);
            break;

        case 'o': // Output directory

            outputDirectory = fs::path(eoptarg);
            break;

        case 'p':

            optPreRead = true;
            break;

        case 'r':

            referenceDirectory = fs::path(eoptarg);
            break;

        case 'v':

            optVerbose = true;
            break;

        case 'y':

            optFamily = atoi(eoptarg);
            break;

        default:

            std::cerr << "Unknown option '" << static_cast<char>(eoptopt) << "'\n";
            errFlag = true;
            break;
        }
    }

    if (!referenceDirectory.empty())
    {
        if (!fs::exists(referenceDirectory))
        {
            std::cerr << "Error: Reference directory \"" << referenceDirectory.string() << "\" does not exist\n";
            errFlag = true;
        }
        else
        {
            if (fs::is_empty(referenceDirectory))
            {
                std::cerr << "Error: Output directory \"" << referenceDirectory.string() << "\" empty\n";
                errFlag = true;
            }
        }
    }

    if (argc - eoptind != 1)
    {
        std::cerr << "Error: No input directory supplied\n";
        errFlag = true;
    }
    else
    {
        inputPath = fs::path(argv[eoptind]);

        if (!fs::is_directory(inputPath) || !fs::exists(inputPath))
        {
            std::cerr << "Error: Input argument \"" << inputPath.string() << "\" supplied is either not directory or does not exist\n";
            errFlag = true;
        }
    }

    if (false
        || optOptimizationLevel < InuCommon::COsUtilities::EOptimizationLevel::eNone
        || optOptimizationLevel >= InuCommon::COsUtilities::EOptimizationLevel::eNumberOfOptimizationLevels
        )
    {
        std::cerr << "Error: Optimization level value specificed (" << optOptimizationLevel << ") is outside of permitted values\n";
        errFlag = true;
    }

    if (!outputDirectory.empty())
    {
        if (true
            && !errFlag
            && !fs::exists(outputDirectory)
            )
        {
            std::cerr << "Error: Output directory \"" << outputDirectory.string() << "\" does not exist\n";
            errFlag = true;
        }

        if (true
            && !errFlag
            && !fs::is_directory(outputDirectory)
            )
        {
            std::cerr << "Error: Output \"" << outputDirectory.string() << "\" exists but is not a directory\n";
            errFlag = true;
        }

#if !ONLY_COMPARE
        if (true
            && !errFlag
            && !fs::is_empty(outputDirectory)
            )
        {
            std::cerr << "Error: Output directory \"" << outputDirectory.string() << "\" not empty\n";

            if (optForceOutputCleanup)
            {
                std::cerr << "Force cleanup specified. Enter 'y' to perform cleanup\n";
                char c = _getch();

                if (c == 'y' || c == 'Y')
                {
                    for (auto& path : fs::directory_iterator(outputDirectory))
                    {
                        fs::remove_all(path);
                    }

                    std::cerr << "Cleaned.\n";
                }
                else
                {
                    errFlag = true;
                }
            }
            else
            {
                errFlag = true;
            }
        }
#endif
    }

    if (errFlag)
    {
        return 2;
    }

#if !ONLY_COMPARE

    CFileInfo::FindMatchingFilesInDir(inputPath, inputFileNames);

    if (!optNote.empty())
    {
        std::cout << optNote << "\n";
    }

    pStaticTemporalFilter =
        CStaticTemporalFilterBase::Factory(optOptimizationLevel, optFamily, optOpenMP);

    if (!pStaticTemporalFilter)
    {
        std::cerr << "Error: Unable to instantiate StaticTemporalFilter\n";
        return 3;
    }

    pStaticTemporalFilter->SetParams(optAll, MN_THREAD_NUM);

    bool firstTime{ true };

    CFileInfo* fi{ CFileInfo::Factory(inputFileNames[0]) };

    if (!fi)
    {
        std::cerr << "\nError creating CFileInfo for \"" << inputFileNames[0].filename().string() << "\"";
        return 10;
    }

    unsigned int bufferSize = (unsigned int)(size_t(fi->mBpp) * fi->mWidth * fi->mHeight);

    pStaticTemporalFilter->Init(fi->mWidth, fi->mHeight);

    uint16_t* outputBuffer =
        reinterpret_cast<uint16_t*>(InuCommon::COsUtilities::AllocteBuffer(bufferSize));
    assert(outputBuffer);

    std::vector<CFileInfo> fiv;
    if (optPreRead)
    {
        for (int fileIndex = 0; fileIndex < inputFileNames.size(); fileIndex++)
        {
            fiv.push_back(*CFileInfo::Factory(inputFileNames[fileIndex]));
        }
    }

    for (int mm = 0; mm < optMultiplicity; mm++)
    {
        for (int fileIndex = 0; fileIndex < inputFileNames.size(); fileIndex++)
        {
            auto fn = inputFileNames[fileIndex];

            if (!optBenchmark)
            {
                std::cout << "Processing \"" << fn.filename().string() << "\" (iteration " << mm << ")... ";
            }

            if (optPreRead)
            {
                fi = &fiv[fileIndex];
            }
            else
            {
                fi = { CFileInfo::Factory(fn) };
            }

            if (!fi)
            {
                std::cerr << "\nError creating CFileInfo for \"" << fn.filename().string() << "\"";
                break;
            }

            const auto begin = std::chrono::high_resolution_clock::now();

            // TODO: Cannot call process_frame multiple times with same input,
            // as it's internal state changes... (must use "optMultiplicity=1")

            pStaticTemporalFilter->process_frame(
                fi->mBuffer,
                outputBuffer
            );

            const auto end = std::chrono::high_resolution_clock::now();

            if (!firstTime)
            {
                // process_frame does initialization on it's first iteration. Skip it.
                totalTime += end - begin;
            }
            firstTime = false;

            if (!outputDirectory.empty())
            {
                // Write output

                long long ts = CFileInfo::GetFilenameTimestamp(fn);

                std::string outputFilename("output_");
                outputFilename += std::to_string(ts) + ".tif";

                fs::path outputFilenamePath = outputDirectory / outputFilename;

                InuCommon::CTiffFile::EError ret = InuCommon::CTiffFile::Write(
                    outputFilenamePath.string(),
                    reinterpret_cast<const unsigned char*>(outputBuffer),
                    fi->mWidth,
                    fi->mHeight,
                    fi->format
                );

                if (ret != InuCommon::CTiffFile::EError::eOK)
                {
                    std::cerr << "\nError writing output file \"" << outputFilename << "\"";
                }
            }

            if (!optBenchmark)
            {
                std::cout << std::endl;
            }

            if (!optPreRead)
            {
                fi->FreeBuffer();
                delete fi;
                fi = nullptr;
            }
        }
    }

    if (optPreRead)
    {
        fiv.clear();
    }
#endif

    // Compare results

    int sumMmatchFailCnt{ 0 };

    if (true
        && !referenceDirectory.empty()
        && !outputDirectory.empty()
        )
    {
        std::vector<fs::path> referenceFilenames;
        CFileInfo::FindMatchingFilesInDir(referenceDirectory, referenceFilenames);

        std::vector<fs::path> outputFilenames;
        CFileInfo::FindMatchingFilesInDir(outputDirectory, outputFilenames);

        for (auto fn : outputFilenames)
        {
            CFileInfo* fi{ CFileInfo::Factory(fn) };

            int matchFailCnt{ 0 };

            long long ts = CFileInfo::GetFilenameTimestamp(fn);

            auto it = std::find_if(
                referenceFilenames.begin(),
                referenceFilenames.end(),
                [&](fs::path& p) { return CFileInfo::GetFilenameTimestamp(p) == ts; }
            );

            if (it == referenceFilenames.end())
            {
                std::cerr << "Error: matching reference file not found, input \"" << fn.string() << "\"\n";
                continue;
            }

            unsigned char* buffer{ nullptr };
            unsigned int width, height, bpp;
            InuCommon::CTiffFile::ETiffFormat format;
            InuCommon::CTiffFile::CMetaData metaData;

            InuCommon::CTiffFile::EError ret =
                InuCommon::CTiffFile::ReadHeader(
                    (*it).string().c_str(),
                    width,
                    height,
                    bpp,
                    format,
                    metaData
                );

            if (false
                || ret == InuCommon::CTiffFile::EError::eOK
                || ret == InuCommon::CTiffFile::EError::eGetField
                )
            {
                ret = InuCommon::CTiffFile::ReadData(
                    (*it).string().c_str(),
                    buffer,
                    width, height,
                    bpp
                );
            }

            if (ret != InuCommon::CTiffFile::EError::eOK)
            {
                std::cerr << "Error: Unable to read reference file \"" << (*it).string() << "\" (rc=" << ret << ")\n";
                continue;
            }

            if (width != fi->mWidth)
            {
                std::cerr << "Error: refernce file \"" << (*it).string() << "\" does not match in width to input file\n";
                continue;
            }

            if (height != fi->mHeight)
            {
                std::cerr << "Error: refernce file \"" << (*it).string() << "\" does not match in height to input file\n";
                continue;
            }

            if (bpp != fi->mBpp)
            {
                std::cerr << "Error: refernce file \"" << (*it).string() << "\" does not match in bpp to input file\n";
                continue;
            }

            int cmpResult{ 0 };

            if (optVerbose)
            {
                uint16_t* pOutput{ fi->mBuffer.get() };
                uint16_t* pReference{ reinterpret_cast<uint16_t*>(buffer) };

                int diffCnt{ 0 };

                for (unsigned int row = 0; row < height; row++)
                {
                    for (unsigned int col = 0; col < width; col++)
                    {
                        uint16_t bOutputPixel = *pOutput++;
                        uint16_t bReferencePixel = *pReference++;

                        int16_t diff = bOutputPixel - bReferencePixel;

                        if (diff)
                        {
                            diffCnt++;

                            if (diffCnt < MAX_DIFFS)
                            {
                                std::cerr << "Diff at x=" << col << ", y=" << row << ": (" << bOutputPixel << "!=" << bReferencePixel << ")\n";
                            }

                            cmpResult = diff;
                        }
                    }
                }

                if (diffCnt - MAX_DIFFS > 0)
                {
                    std::cerr << "Skipped " << (diffCnt - MAX_DIFFS) << " additional diffrences...\n";
                }

                if (diffCnt > 0)
                {
                    matchFailCnt++;
                    std::cerr << "Result mismatch for " << diffCnt << " pixel" << (diffCnt > 1 ? "s" : "") << "\n";
                }
            }
            else
            {
                cmpResult = memcmp(buffer, fi->mBuffer.get(), size_t(bpp) * width * height);

                if (cmpResult)
                {
                    matchFailCnt++;

                    if (!optBenchmark)
                    {
                        std::cerr << "Result mismatch\n";
                    }
                }
            }

            if (cmpResult && !optBenchmark)
            {
                std::cerr << "Error: Reference outout file \"" << (*it).string() << "\" content does not match filter's output";
            }

            delete buffer;

            sumMmatchFailCnt += matchFailCnt;

            if (sumMmatchFailCnt && optBenchmark)
            {
                // No use finding additional failures...
                break;
            }
        }
    }

    const char* n[]{ "Naive", "SSE", "AVX" };
    std::cout << n[optOptimizationLevel] << " ";
    std::cout << "Per-file time: " << std::chrono::duration<double, std::milli>(totalTime)
        .count() / (optMultiplicity * inputFileNames.size()) << " mSec\n";

    delete pStaticTemporalFilter;

    if (sumMmatchFailCnt)
    {
        return 1; // failed match
    }

    return 0;
}
