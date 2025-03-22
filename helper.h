#include <stdlib.h>
#include <immintrin.h>
#include <omp.h>
#include <cblas.h>
#include <stdio.h>
#include <time.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;
typedef float f32;
typedef double f64;
typedef long long i64;
typedef unsigned long long u64;

// f32 (*weight_ptr)[M] = (f32 (*)[M]) weight;
typedef struct s_std_layer
{
	//          Weights        Input           Output     +    Biases 
	//          (M x K)   *   (K x N)     =   (M x N)     +   (M x 1)
	// MNIST: (100 x 784) * (784 x 60000) = (100 x 60000) + (100 x 1)
	f32 *weight, *bias;
	u32 M, K;

} std_layer;

f32 ft_abs(f32 num)
{
	return (num < 0 ? -num : num);
}
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
		if (ft_abs(A[i] - B[i]) > 0.001)
			counter++;
	}
	return (counter);
}