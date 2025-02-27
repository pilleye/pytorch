#ifndef THC_GENERAL_INC
#define THC_GENERAL_INC

#include <TH/THGeneral.h>

#include <c10/core/Allocator.h>
#include <c10/cuda/CUDAStream.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#include <cusparse.h>

#ifndef THAssert
#define THAssert(exp)                                                   \
  do {                                                                  \
    if (!(exp)) {                                                       \
      _THError(__FILE__, __LINE__, "assert(%s) failed", #exp);          \
    }                                                                   \
  } while(0)
#endif

/* Global state of THC. */
struct THCState {};

typedef struct _THCCudaResourcesPerDevice {
  /* Size of scratch space per each stream on this device available */
  size_t scratchSpacePerStream;
} THCCudaResourcesPerDevice;

TORCH_CUDA_CPP_API THCState* THCState_alloc(void);
TORCH_CUDA_CPP_API void THCState_free(THCState* state);

TORCH_CUDA_CPP_API void THCudaInit(THCState* state);
TORCH_CUDA_CPP_API void THCudaShutdown(THCState* state);

/* If device `dev` can access allocations on device `devToAccess`, this will return */
/* 1; otherwise, 0. */
TORCH_CUDA_CPP_API int THCState_getPeerToPeerAccess(THCState* state, int dev, int devToAccess);

/* For the current device and stream, returns the allocated scratch space */
TORCH_CUDA_CPP_API size_t THCState_getCurrentDeviceScratchSpaceSize(THCState* state);

#define THCAssertSameGPU(expr) if (!expr) THError("arguments are located on different GPUs")
#define THCudaCheck(err)  __THCudaCheck(err, __FILE__, __LINE__)
#define THCudaCheckWarn(err)  __THCudaCheckWarn(err, __FILE__, __LINE__)
#define THCublasCheck(err)  __THCublasCheck(err,  __FILE__, __LINE__)
#define THCusparseCheck(err)  __THCusparseCheck(err,  __FILE__, __LINE__)

TORCH_CUDA_CPP_API void __THCudaCheck(cudaError_t err, const char *file, const int line);
TORCH_CUDA_CPP_API void __THCudaCheckWarn(cudaError_t err, const char *file, const int line);
TORCH_CUDA_CPP_API void __THCublasCheck(cublasStatus_t status, const char *file, const int line);
TORCH_CUDA_CPP_API void __THCusparseCheck(cusparseStatus_t status, const char *file, const int line);

#endif
