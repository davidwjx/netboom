#pragma once

#if (defined(__linux__) && (defined(__i386__) || defined(__x86_64__)) ) || defined(_MSC_VER)
#define USE_AVX (1)
#else
#define USE_AVX (0)
#endif

#if (defined(__linux__) && (defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)) ) || defined(_MSC_VER)
#define USE_SSE (1)
#else
#define USE_SSE (0)
#endif

// Set these as desirable. Currently, none of AVX/SSE are implemented
#define SSE_IS_NOT_SUPPORTED
#define AVX_IS_NOT_SUPPORTED

// Compile SSE2 apps to only compare output
#define ONLY_COMPARE (0)

// Configuration file for TemporalFilter algorithm code.

constexpr bool MB_ALL{ false };
constexpr unsigned int MN_FILTER_LENGTH{ 8 };
constexpr unsigned int MN_THREAD_NUM{ 1 };

constexpr bool OPENMP_DEFALUT{ false };
constexpr int WOREKER_TYPE_DEFAULT{ 0 };

// whatever
