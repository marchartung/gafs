
#if defined(GPU_ENABLED)
#define DEVICE __host__ __device__
#define INLINE2 __host__ __device__ __attribute__((always_inline))
#define INLINE __host__ __device__ __forceinline__
#elif defined(__GNUC__)
#define DEVICE __attribute__((always_inline)) inline
#define INLINE __attribute__((always_inline)) inline
#elif defined(__clang__)
#define DEVICE __attribute__((always_inline))
#define INLINE __attribute__((always_inline))
#endif