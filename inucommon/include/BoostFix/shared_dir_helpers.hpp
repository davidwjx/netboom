//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_SHARED_DIR_HELPERS_HPP
#define BOOST_INTERPROCESS_DETAIL_SHARED_DIR_HELPERS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/errors.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <string>

#if defined(BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME) && defined(BOOST_INTERPROCESS_WINDOWS)
   #include <boost/interprocess/detail/windows_intermodule_singleton.hpp>
#endif

namespace boost {
namespace interprocess {
namespace ipcdetail {

#if defined(BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME)
   #if defined(BOOST_INTERPROCESS_WINDOWS)
      //This type will initialize the stamp
      struct windows_bootstamp
      {
         windows_bootstamp()
         {
            //Throw if bootstamp not available
            if(!winapi::get_last_bootup_time(stamp)){
               error_info err = system_error_code();
               throw interprocess_exception(err);
            }
         }
         //Use std::string. Even if this will be constructed in shared memory, all
         //modules/dlls are from this process so internal raw pointers to heap are always valid
         std::string stamp;
      };

      inline void get_bootstamp(std::string &s, bool add = false)
      {
         const windows_bootstamp &bootstamp = windows_intermodule_singleton<windows_bootstamp>::get();
         if(add){
            s += bootstamp.stamp;
         }
         else{
            s = bootstamp.stamp;
         }
      }
   #elif defined(BOOST_INTERPROCESS_HAS_BSD_KERNEL_BOOTTIME)
      inline void get_bootstamp(std::string &s, bool add = false)
      {
         // FreeBSD specific: sysctl "kern.boottime"
         int request[2] = { CTL_KERN, KERN_BOOTTIME };
         struct ::timeval result;
         std::size_t result_len = sizeof result;

         if (::sysctl (request, 2, &result, &result_len, 0, 0) < 0)
            return;

         char bootstamp_str[256];

         const char Characters [] =
            { '0', '1', '2', '3', '4', '5', '6', '7'
            , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

         std::size_t char_counter = 0;
         //32 bit values to allow 32 and 64 bit process IPC
         boost::uint32_t fields[2] = { boost::uint32_t(result.tv_sec), boost::uint32_t(result.tv_usec) };
         for(std::size_t field = 0; field != 2; ++field){
            for(std::size_t i = 0; i != sizeof(fields[0]); ++i){
               const char *ptr = (const char *)&fields[field];
               bootstamp_str[char_counter++] = Characters[(ptr[i]&0xF0)>>4];
               bootstamp_str[char_counter++] = Characters[(ptr[i]&0x0F)];
            }
         }
         bootstamp_str[char_counter] = 0;
         if(add){
            s += bootstamp_str;
         }
         else{
            s = bootstamp_str;
         }
      }
   #else
      #error "BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME defined with no known implementation"
   #endif
#endif   //#if defined(BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME)

inline void get_shared_dir_root(std::string &dir_path)
{
   #if defined (BOOST_INTERPROCESS_WINDOWS)
      winapi::get_shared_documents_folder(dir_path);
   #else
      dir_path = "/tmp";
   #endif
   //We always need this path, so throw on error
   if(dir_path.empty()){
      error_info err = system_error_code();
      throw interprocess_exception(err);
   }
   //Remove final null.
   dir_path += "/boost_interprocess";
}

inline void get_shared_dir(std::string &shared_dir)
{
#if defined(BOOST_INTERPROCESS_SHARED_DIR_PATH)
#ifdef __ANDROID__
    shared_dir = InuCommon::COsUtilities::GetInstallationPath(shared_dir);
#endif
    if (shared_dir.empty())
    {
        shared_dir = BOOST_INTERPROCESS_SHARED_DIR_PATH;
    }
#else
    get_shared_dir_root(shared_dir);
#if defined(BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME)
    shared_dir.erase(shared_dir.find_last_of("/"));
    shared_dir += "/inuitive_interprocess";

    //get_bootstamp(shared_dir, true);
#endif
#endif
}

inline void shared_filepath(const char *filename, std::string &filepath)
{
   get_shared_dir(filepath);
   filepath += "/";
   filepath += filename;
}

inline void create_shared_dir_and_clean_old(std::string &shared_dir)
{
#if defined(BOOST_INTERPROCESS_SHARED_DIR_PATH)
#ifdef __ANDROID__
    shared_dir = InuCommon::COsUtilities::GetInstallationPath(shared_dir);
#endif
    if (shared_dir.empty())
    {
        shared_dir = BOOST_INTERPROCESS_SHARED_DIR_PATH;
    }
#else
      //First get the temp directory
      std::string root_shared_dir;
      get_shared_dir_root(root_shared_dir);

      //If fails, check that it's because already exists
      if(!create_directory(root_shared_dir.c_str())){
         error_info info(system_error_code());
         if(info.get_error_code() != already_exists_error){
            throw interprocess_exception(info);
         }
      }

      #if defined(BOOST_INTERPROCESS_HAS_KERNEL_BOOTTIME)
         get_shared_dir(shared_dir);

         //If fails, check that it's because already exists
         if(!create_directory(shared_dir.c_str())){
            error_info info(system_error_code());
            if(info.get_error_code() != already_exists_error){
               throw interprocess_exception(info);
            }
         }
         //Now erase all old directories created in the previous boot sessions
		 // This code caused to deletion of MQ files of our clients that use boost as well (IG-2232). 
		 // Since we clean old MQ files by the method CleanMemoryMappedFiles and it looks like it invoked only for 
		 // MQ then we can eliminate this piece of code.
		 /*
         std::string subdir = shared_dir;
         subdir.erase(0, root_shared_dir.size()+1);
         delete_subdirectories(root_shared_dir, subdir.c_str());
		 */
      #else
         shared_dir = root_shared_dir;
      #endif
   #endif
}

inline void create_shared_dir_cleaning_old_and_get_filepath(const char *filename, std::string &shared_dir)
{
   create_shared_dir_and_clean_old(shared_dir);
   shared_dir += "/";
   shared_dir += filename;
}

inline void add_leading_slash(const char *name, std::string &new_name)
{
   if(name[0] != '/'){
      new_name = '/';
   }
   new_name += name;
}

}  //namespace boost{
}  //namespace interprocess {
}  //namespace ipcdetail {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //ifndef BOOST_INTERPROCESS_DETAIL_SHARED_DIR_HELPERS_HPP
