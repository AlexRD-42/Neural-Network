#include "..\helper.h"

void blas_mult(float *A, float *B, float *C, int M, int K, int N)
{
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
				M, N, K, 1.0,
				A, K, B, N, 0.0, C, N);
}

float ft_sum(float *A, float *B, int K, int N, int j)
{
    float sum = 0.0f;
    for (int i = 0; i < K; i++)
        sum += A[i] * B[i * N + j];
    return sum;
}

void loop_mult(float *A, float *B, float *C, int M, int K, int N)
{
	int i = 0, j = 0;

	#pragma omp parallel for private(j)
    for (i = 0; i < M; i++)
        for (j = 0; j < N; j++)
			C[i * N + j] = ft_sum(&A[i * K], B, K, N, j);
}

void avx2_mult(const float *A, const float *B, float *C, int M, int K, int N) 
{
    const int BLOCK_SIZE = 64;
    const int VEC_SIZE = 8;

    #pragma omp parallel for collapse(2)
    for (int i_block = 0; i_block < M; i_block += BLOCK_SIZE)
        for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE) {
            const int i_end = (i_block + BLOCK_SIZE > M) ? M : i_block + BLOCK_SIZE;
            const int j_end = (j_block + BLOCK_SIZE > N) ? N : j_block + BLOCK_SIZE;
            
            for (int i = i_block; i < i_end; i++) {
                for (int j = j_block; j < j_end; j += VEC_SIZE) {
                    __m256 c_vec;
                    
                    // Handle full vector case
                    if (j + VEC_SIZE <= j_end) {
                        c_vec = _mm256_loadu_ps(&C[i * N + j]);
                        
                        for (int k = 0; k < K; k++) {
                            __m256 a_val = _mm256_set1_ps(A[i * K + k]);
                            __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);
                            c_vec = _mm256_fmadd_ps(a_val, b_vec, c_vec);
                        }
                        
                        _mm256_storeu_ps(&C[i * N + j], c_vec);
                    }
                    else {
                        // Handle remaining elements (less than VEC_SIZE)
                        for (int j_remainder = j; j_remainder < j_end; j_remainder++) {
                            float sum = C[i * N + j_remainder];
                            for (int k = 0; k < K; k++) {
                                sum += A[i * K + k] * B[k * N + j_remainder];
                            }
                            C[i * N + j_remainder] = sum;
                        }
                        break; // Exit the j loop after handling remainder
                    }
                }
            }
        }
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
	{
		C1[i] = 0;
		C2[i] = 0;
		C3[i] = 0;
	}

	u64 start, x, y, z;

	start = ns();
	loop_mult(A, B, C1, m, k, n);
	x = ns() - start;

	start = ns();
	blas_mult(A, B, C2, m, k, n);
	y = ns() - start;

	start = ns();
	avx2_mult(A, B, C3, m, k, n);
	z = ns() - start;

	printf("%f, %f, %f", printms(x), printms(y), printms(z));
	printf("\n(%d, %d, %d)", check_matrix(C1, C2, m*n), check_matrix(C2, C3, m*n), check_matrix(C1, C3, m*n));
    return 0;
}