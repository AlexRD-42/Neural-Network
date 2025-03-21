#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <time.h>
#include <immintrin.h>
#include <omp.h>

void blas_mult(float *A, float *B, float *C, int M, int K, int N)
{
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
				M, N, K, 1.0,
				A, K, B, N, 0.0, C, N);
}

// Improved AVX-512 matrix multiplication
void avx512_mult(const float *A, const float *B, float *C, int M, int K, int N) {
    // Initialize C to zeros
    #pragma omp parallel for
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] = 0.0f;
        }
    }

    // Cache blocking parameters
    const int BLOCK_SIZE_M = 64;
    const int BLOCK_SIZE_K = 64;
    const int BLOCK_SIZE_N = 64;

    // Number of elements in a vector
    const int VEC_SIZE = 16;
    
    #pragma omp parallel for collapse(3)
    for (int i_block = 0; i_block < M; i_block += BLOCK_SIZE_M) {
        for (int k_block = 0; k_block < K; k_block += BLOCK_SIZE_K) {
            for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE_N) {
                // Block size adjustments for edge cases
                const int i_end = (i_block + BLOCK_SIZE_M > M) ? M : i_block + BLOCK_SIZE_M;
                const int k_end = (k_block + BLOCK_SIZE_K > K) ? K : k_block + BLOCK_SIZE_K;
                const int j_end = (j_block + BLOCK_SIZE_N > N) ? N : j_block + BLOCK_SIZE_N;
                
                for (int i = i_block; i < i_end; i++) {
                    for (int k = k_block; k < k_end; k++) {
                        __m512 a_val = _mm512_set1_ps(A[i * K + k]);
                        
                        // Process full 16-element blocks
                        int j = j_block;
                        for (; j <= j_end - VEC_SIZE; j += VEC_SIZE) {
                            __m512 b_vec = _mm512_loadu_ps(&B[k * N + j]);
                            __m512 c_vec = _mm512_loadu_ps(&C[i * N + j]);
                            c_vec = _mm512_fmadd_ps(a_val, b_vec, c_vec);
                            _mm512_storeu_ps(&C[i * N + j], c_vec);
                        }
                        
                        // Handle remainder elements with mask
                        if (j < j_end) {
                            int remainder = j_end - j;
                            __mmask16 tail_mask = (1ULL << remainder) - 1;
                            __m512 b_vec = _mm512_maskz_loadu_ps(tail_mask, &B[k * N + j]);
                            __m512 c_vec = _mm512_maskz_loadu_ps(tail_mask, &C[i * N + j]);
                            c_vec = _mm512_fmadd_ps(a_val, b_vec, c_vec);
                            _mm512_mask_storeu_ps(&C[i * N + j], tail_mask, c_vec);
                        }
                    }
                }
            }
        }
    }
}

// Improved AVX2 matrix multiplication
void avx2_mult(const float *A, const float *B, float *C, int M, int K, int N) {
    // Initialize C to zeros
    #pragma omp parallel for
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] = 0.0f;
        }
    }

    // Cache blocking parameters
    const int BLOCK_SIZE_M = 64;
    const int BLOCK_SIZE_K = 64;
    const int BLOCK_SIZE_N = 64;

    // Number of elements in a vector
    const int VEC_SIZE = 8;
    
    #pragma omp parallel for collapse(3)
    for (int i_block = 0; i_block < M; i_block += BLOCK_SIZE_M) {
        for (int k_block = 0; k_block < K; k_block += BLOCK_SIZE_K) {
            for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE_N) {
                // Block size adjustments for edge cases
                const int i_end = (i_block + BLOCK_SIZE_M > M) ? M : i_block + BLOCK_SIZE_M;
                const int k_end = (k_block + BLOCK_SIZE_K > K) ? K : k_block + BLOCK_SIZE_K;
                const int j_end = (j_block + BLOCK_SIZE_N > N) ? N : j_block + BLOCK_SIZE_N;
                
                for (int i = i_block; i < i_end; i++) {
                    for (int k = k_block; k < k_end; k++) {
                        __m256 a_val = _mm256_set1_ps(A[i * K + k]);
                        
                        // Process full 8-element blocks
                        int j = j_block;
                        for (; j <= j_end - VEC_SIZE; j += VEC_SIZE) {
                            __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);
                            __m256 c_vec = _mm256_loadu_ps(&C[i * N + j]);
                            c_vec = _mm256_fmadd_ps(a_val, b_vec, c_vec);
                            _mm256_storeu_ps(&C[i * N + j], c_vec);
                        }
                        
                        // Handle remainder with masked operations
                        if (j < j_end) {
                            // Implement masked operations for AVX2 (more complex than AVX-512)
                            float a_scalar = A[i * K + k];
                            for (; j < j_end; j++) {
                                C[i * N + j] += a_scalar * B[k * N + j];
                            }
                        }
                    }
                }
            }
        }
    }
}

// Utility function to choose the best implementation based on CPU capabilities
void optimized_matrix_mult(const float *A, const float *B, float *C, int M, int K, int N) {
    #if defined(__AVX512F__)
        avx512_mult(A, B, C, M, K, N);
    #elif defined(__AVX2__)
        avx2_mult(A, B, C, M, K, N);
    #else
        // Fallback to a basic implementation for CPUs without AVX support
        #pragma omp parallel for
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                float sum = 0.0f;
                for (int k = 0; k < K; k++) {
                    sum += A[i * K + k] * B[k * N + j];
                }
                C[i * N + j] = sum;
            }
        }
    #endif
}

float ft_abs(float num)
{
	if (num < 0)
		return (-num);
	else
		return (num);
}

float ft_rand() 
{
    return (float) rand() / (float) RAND_MAX;
}

int main() 
{
	// A (m x k) * B (k x n) = C (m x n)
    int m = 128; // rows of A and C
    int k = 100; // cols of A, rows of B
    int n = 128; // cols of B and C

	float *A, *B, *C1, *C2, *C3;
	A = (float *) malloc (m * k * sizeof(float));
	B = (float *) malloc (k * n * sizeof(float));
	C1 = (float *) malloc (m * n * sizeof(float));
	C2 = (float *) malloc (m * n * sizeof(float));
	C3 = (float *) malloc (m * n * sizeof(float));

	for (int i = 0; i < m * k; i++)
		A[i] = ft_rand();
	for (int i = 0; i < k * n; i++)
		B[i] = ft_rand();
	for (int i = 0; i < m * n; i++)
		C1[i] = 0;
	for (int i = 0; i < m * n; i++)
		C2[i] = 0;
	for (int i = 0; i < m * n; i++)
		C3[i] = 0;

	int i = 0, j = 1000;
	int x, y, z;
	clock_t start = clock();
	for (i = 0; i < j; i++)
		avx512_mult(A, B, C1, m, k, n);
	x = clock() - start;

	start = clock();
	for (i = 0; i < j; i++)
		blas_mult(A, B, C2, m, k, n);
	y = clock () - start;

	start = clock();
	for (i = 0; i < j; i++)
		avx2_mult(A, B, C3, m, k, n);
	z = clock () - start;

	printf("%d, %d, %d", x, y, z);
    return 0;
}