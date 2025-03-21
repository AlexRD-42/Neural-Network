#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <immintrin.h>
#include <time.h>
#include <omp.h>


void blas_mult(float *A, float *B, float *C, int M, int K, int N)
{
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
				M, N, K, 1.0,
				A, K, B, N, 0.0, C, N);
}

float ft_sum(float *A, float B, int N)
{
	int i;
	float sum = 0.0f;
	// #pragma omp parallel for
	for (i = 0; i < N; i++)
		sum += A[i] * B;
	return (sum);
}

void loop_mult(float *A, float *B, float *C, int M, int K, int N)
{
	int i = 0, j = 0, k = 0;
	float sum = 0.0f;


	// #pragma omp parallel for private(j, k) collapse(2)
	#pragma omp parallel for
    for (i = 0; i < M; i++)
	{
		// #pragma omp parallel for
        for (j = 0; j < N; j++)
		{
            C[i * N + j] = ft_sum(&A[i * K], B[j], K);
        }
    }
}

void avx512_mult(const float *A, const float *B, float *C, int M, int K, int N) 
{
    int full_blocks = N / 16;
    int remainder = N % 16;
    __mmask16 tail_mask = remainder ? ((1 << remainder) - 1) : 0;

    for (int i = 0; i < M; i++) 
    {
        // Initialize row i of C to zeros.
        for (int j = 0; j < full_blocks * 16; j += 16)
            _mm512_storeu_ps(&C[i * N + j], _mm512_setzero_ps());
        if (remainder)
            _mm512_mask_storeu_ps(&C[i * N + full_blocks * 16], tail_mask, _mm512_setzero_ps());

        for (int k = 0; k < K; k++) 
        {
            // Broadcast A[i][k] across a vector.
            __m512 a_val = _mm512_set1_ps(A[i * K + k]);
            // Process full 16-element blocks.
            for (int j = 0; j < full_blocks * 16; j += 16) 
            {
                __m512 b_vec = _mm512_loadu_ps(&B[k * N + j]);
                __m512 c_vec = _mm512_loadu_ps(&C[i * N + j]);
                c_vec = _mm512_fmadd_ps(a_val, b_vec, c_vec);
                _mm512_storeu_ps(&C[i * N + j], c_vec);
            }
            // Handle remainder columns with mask.
            if (remainder) 
            {
                __m512 b_vec = _mm512_maskz_loadu_ps(tail_mask, &B[k * N + full_blocks * 16]);
                __m512 c_vec = _mm512_maskz_loadu_ps(tail_mask, &C[i * N + full_blocks * 16]);
                c_vec = _mm512_fmadd_ps(a_val, b_vec, c_vec);
                _mm512_mask_storeu_ps(&C[i * N + full_blocks * 16], tail_mask, c_vec);
            }
        }
    }
}

void avx2_mult(const float *A, const float *B, float *C, int M, int K, int N)
{
    int full_blocks = N / 8;
    int remainder = N % 8;

    for (int i = 0; i < M; i++) 
	{
        // Initialize row i of C to zeros.
        for (int j = 0; j < full_blocks * 8; j += 8)
            _mm256_storeu_ps(&C[i * N + j], _mm256_setzero_ps());
        for (int j = full_blocks * 8; j < N; j++)
            C[i * N + j] = 0.0f;

        for (int k = 0; k < K; k++)
		{
            // Broadcast A[i][k] across a vector.
            __m256 a_val = _mm256_set1_ps(A[i * K + k]);

            // Process full 8-element blocks.
            for (int j = 0; j < full_blocks * 8; j += 8)
			{
                __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);
                __m256 c_vec = _mm256_loadu_ps(&C[i * N + j]);
                c_vec = _mm256_fmadd_ps(a_val, b_vec, c_vec);
                _mm256_storeu_ps(&C[i * N + j], c_vec);
            }

            // Handle remainder elements.
            for (int j = full_blocks * 8; j < N; j++)
			{
                C[i * N + j] += A[i * K + k] * B[k * N + j];
            }
        }
    }
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
    int m = 1280; // rows of A and C
    int k = 1000; // cols of A, rows of B
    int n = 1280; // cols of B and C

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

	int x, y, z;
	clock_t start = clock();
	avx512_mult(A, B, C1, m, k, n);
	x = clock() - start;

	start = clock();
	blas_mult(A, B, C2, m, k, n);
	y = clock () - start;

	start = clock();
	loop_mult(A, B, C3, m, k, n);
	z = clock () - start;

	printf("%d, %d, %d", x, y, z);
    return 0;
}