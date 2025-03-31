#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <omp.h>
#include <cblas.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "hres_timer.h"

#define abs(x) ((x) < 0 ? -(x) : (x))
#define printms(x) (((f64)(x))/1e6)
#define printus(x) (((f64)(x))/1e3)
#define printns(x) (((f64)(x)))

typedef unsigned char u8;
typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;
typedef int64_t i64;
typedef uint64_t u64;

f32 ft_rand(void)
{
	return (f32) rand() / (f32) RAND_MAX;
}

i32 check_matrix(f32 *A, f32 *B, i32 size)
{
	i32 i = 0;
	i32 counter = 0;
	for (i = 0; i < size; i++)
	{
		if (abs(A[i] - B[i]) > 0.001)
			counter++;
	}
	return (counter);
}