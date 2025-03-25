#include "..\helper.h"

static uint64_t res, seed, s32[2], s64[4], s64m[4];

static inline uint64_t splitmix64(void)
{
	uint64_t result = (seed += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

static inline uint64_t xoshiro128(void)
{
	const uint64_t s0 = s32[0];
	uint64_t s1 = s32[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s32[0] = ((s0 << 24) | (s0 >> 40)) ^ s1 ^ (s1 << 16);
	s32[1] = (s1 << 37) | (s1 >> 27);
	
	return result;
}

static inline uint64_t xoshiro256(void)
{
	const uint64_t result = s64[0] + s64[3];
	const uint64_t t = s64[1] << 17;

	s64[2] ^= s64[0];
	s64[3] ^= s64[1];
	s64[1] ^= s64[2];
	s64[0] ^= s64[3];

	s64[2] ^= t;

	s64[3] = (s64[3] << 45) | (s64[3] >> 19);

	return result;
}

static inline uint64_t mwc256(void)
{
	const uint64_t result = s64m[2];
	const __uint128_t t = 0xfff62cf2ccc0cdaf * (__uint128_t)s64m[0] + s64m[3];
	s64m[0] = s64m[1];
	s64m[1] = s64m[2];
	s64m[2] = t;
	s64m[3] = t >> 64;
	return result;
}

void init_seed(u64 seed)
{
	u64 i;

	for (i = 0; i < 128; i++)
		splitmix64();

	for (i = 0; i < 4; i ++)
	{
		splitmix64();
		if (i < 2) s32[i] = seed;
		s64[i] = seed;
		s64m[i] = seed;
	}
}

u64 benchmark_rng(u32 func, u64 num_iter)
{
	u64 start = ns();

	switch (func)
	{
		case 1:
			for (u64 i = 0; i < num_iter; i++)
				res += splitmix64();
			break;
		case 2:
			for (u64 i = 0; i < num_iter; i++)
				res += xoshiro128();
			break;
		case 3:
			for (u64 i = 0; i < num_iter; i++)
				res += xoshiro256();
			break;
		case 4:
			for (u64 i = 0; i < num_iter; i++)
				res += mwc256();
			break;
		default:
			break;
	}
	return (ns() - start);
}

f64 get_avg(u64 *ptr, u64 n)
{
	u64 sum = 0;
	for (u64 i = n/10; i < n; i++)
		sum += ptr[i];
	return (((f64)(sum)) / (n - n/10));
}

int main()
{
	u64 num_runs = 1e2;
	u64 num_iter = 1e6;
	u64 (*results)[num_runs] = malloc(num_runs * 4 * sizeof(u64));

	init_seed(ns());
	for (u64 run = 0; run < num_runs; run++) 
	{
		results[0][run] = benchmark_rng(1, num_iter);
		results[1][run] = benchmark_rng(2, num_iter);
		results[2][run] = benchmark_rng(3, num_iter);
		results[3][run] = benchmark_rng(4, num_iter);
    }

    printf("splitmix64: %f us\n", printus(get_avg(results[0], num_runs)));
    printf("xoshiro128: %f us\n", printus(get_avg(results[1], num_runs)));
    printf("xoshiro256: %f us\n", printus(get_avg(results[2], num_runs)));
	printf("mwc256: %f us\n", printus(get_avg(results[3], num_runs)));
	printf("\n%lld", res);
}