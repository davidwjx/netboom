#pragma once

#if defined(_OPENMP)
#include <omp.h>
#endif

#if defined(_MSC_VER)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif defined(__ANDROID__) || defined(__linux__)
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#endif

#include <stdlib.h>

#include "OSUtilities.h"

#ifndef _WIN32
#define __forceinline inline
#endif

template<typename T>
__forceinline void transpose_chunk(
    int j,
    T *src,
    T *dst,
    int xsize,
    int ysize,
    int blocksize
)
{
    for (int i = 0; i < xsize; i += blocksize)
    {
        for (int l = j; l < j + blocksize; ++l)
        {
            for (int k = i; k < i + blocksize; ++k)
            {
                dst[l+k*ysize] = src[k+l*xsize];
            }
        }
    }
}

template<typename T>
__forceinline void transpose_chunk_xy(
    int j,
    T *src,
    T *dst,
    int xsize,
    int ysize,
    int blocksize_x,
    int blocksize_y
)
{
    for (int i = 0; i < xsize; i += blocksize_x) 
    {
        for (int l = j; l < j + blocksize_y; ++l) 
        {
            for (int k = i; k < i + blocksize_x; ++k) 
            {
                dst[l+k*ysize] = src[k+l*xsize];
            }
        }
    }
}


#ifdef _OPENMP
template<typename T>
void transpose_array(
    T *src,
    T *dst,
    int xsize,
    int ysize,
    int blocksize_x,
    int blocksize_y
)
{
    int j = 0;
#pragma omp parallel num_threads(omp_get_num_procs()/2) default(shared) private(j)
    {
#pragma omp for 
        for (j = 0; j < ysize; j += blocksize_y)
        {
            transpose_chunk_xy(j, src, dst, xsize, ysize, blocksize_x, blocksize_y);
        }
    }
}

template<typename T>
void transpose_array_xy(
    T *src,
    T *dst,
    int xsize,
    int ysize,
    int blocksize_x,
    int blocksize_y,
    T* pbuf = nullptr
)
{
    int thread_num = omp_get_thread_num();
    T *tmp_dst = pbuf ? pbuf + thread_num*xsize*ysize : dst;


    for (int j = 0; j < ysize; j += blocksize_y) 
    {
        for (int i = 0; i < xsize; i += blocksize_x) 
        {
            for (int l = j; l < j + blocksize_y; ++l) 
            {
                for (int k = i; k < i + blocksize_x; ++k) 
                {
                    tmp_dst[l+k*ysize] = src[k+l*xsize];
                }
            }
        }
    }

    if (pbuf)
    {
        memcpy(src, pbuf + thread_num * xsize * ysize, xsize * ysize * sizeof(T));
    }
}

template<typename T>
void transpose_array_internal(
    T *src,
    T *dst,
    int zsize,
    int xsize,
    int ysize,
    int blocksize_x,
    int blocksize_y
)
{
    T* im_buffer = nullptr;
    int num_threads = omp_get_num_procs();

    bool in_place = (src == dst);

    if (in_place)
    {
#ifdef __ANDROID__
        int res = posix_memalign((void**)&im_buffer ,16 ,num_threads*xsize*ysize*sizeof(T));
#else
        im_buffer = (T*) aligned_alloc(num_threads*xsize*ysize*sizeof(T), 16);
#endif
    }

    int z = 0;

#pragma omp parallel num_threads(num_threads) default(shared) private(z)
    {
#pragma omp for
        for (z = 0; z < zsize; z++)
        {
            transpose_array_xy(src + z * xsize * ysize, dst + z * xsize * ysize, xsize, ysize, blocksize_x, blocksize_y, im_buffer);
        }
    }

    _aligned_free(im_buffer);
}


template<typename T>
void rearrange_array_internal(
    T *src,
    T *dst,
    int zsize,
    int xsize,
    int ysize,
    bool do_replace
)
{
    int num_threads = omp_get_num_procs();
    int y = 0, x = 0;
#pragma omp parallel num_threads(num_threads) default(shared) private(x,y)
    {
#pragma omp for
        for (y = 0; y < ysize; y++)
        {
            for (x = 0; x < xsize; x++)
            {
                memcpy(dst + zsize*(ysize*x+y), src+zsize*(xsize*y+x), zsize*sizeof(T));
            }
        }
    }

    if (do_replace)
    {
        memcpy(src, dst, zsize * xsize * ysize * sizeof(T));
    }
}
#endif

// Gets a filename (without path) and return its full path as application config file, according to directory structure, defined by env variables
inline std::string AddGeneralConfigPath(const std::string& filename)
{
    fs::path path(InuCommon::COsUtilities::GetCommonConfigPath());
    path /= filename.c_str();

    return path.string();
}
