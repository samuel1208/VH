#ifndef __VIDEO_RETRIEVAL_H__
#define __VIDEO_RETRIEVAL_H__

#ifndef SAM_STATIC  // Dynamic Library
#   if (defined(_WIN32) || defined(_WIN64)) && !defined(__GNUC__)
#       ifdef SAK_DLL
#           define SAM_EXPORTS __declspec(dllexport)
#       else
#           define SAM_EXPORTS __declspec(dllimport)
#       endif
#   elif (defined(linux) || defined(__linux) || defined(__ANDROID__)) && defined(__GNUC__)
#      define SAM_EXPORTS __attribute__((visibility("default")))
#   else
#       error "The OS is not supported."
#   endif
#else  // Static Library
#   define SAM_EXPORTS 
#endif


#ifndef __cplusplus
extern "C" {
#endif


#ifndef __cplusplus
}
#endif

#endif
