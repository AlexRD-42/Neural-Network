#include "helper.h"

void loop_mult(float *A, float *B, float *C, float *D, int size)
{
	int i;
	for (i = 0; i < size; i++)
		D[i] = A[i] * B[i] + C[i];
}

void parloop_mult(float *A, float *B, float *C, float *D, int size)
{
	int i;
	// #pragma omp parallel for
	// cblas_saxpy(N, 1.0f, bias, 1, x, 1);
	for (i = 0; i < size; i++)
		D[i] = A[i] * B[i] + C[i];
}

int main() 
{
    int N = 100000;

	float *A, *B, *C, *D1, *D2, *D3;
	A = (float *) malloc (N * sizeof(float));
	B = (float *) malloc (N * sizeof(float));
	C = (float *) malloc (N * sizeof(float));
	D1 = (float *) malloc (N * sizeof(float));
	D2 = (float *) malloc (N * sizeof(float));
	D3 = (float *) malloc (N * sizeof(float));

	for (int i = 0; i < N; i++)
	{
		A[i] = ft_rand();
		B[i] = ft_rand();
		C[i] = ft_rand();
		D1[i] = 0;
		D2[i] = 0;
		D3[i] = 0;
	}

	int i = 0, j = 10000;
	int x, y, z;
// ================================== //
	clock_t start = clock();
	for (i = 0; i < j; i++)
		loop_mult(A, B, C, D1, N);
	x = clock() - start;

	start = clock();
	for (i = 0; i < j; i++)
		parloop_mult(A, B, C, D2, N);
	y = clock () - start;

	// start = clock();
	// for (i = 0; i < j; i++)
	// 	avx2_mult(A, B, C3, m, k, n);
	// z = clock () - start;

	printf("%d, %d, %d", x, y, z);
	printf("\n(%d, %d, %d)", check_matrix(D1, D2, N), check_matrix(D2, D3, N), check_matrix(D1, D3, N));
    return 0;
}