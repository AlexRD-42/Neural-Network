#include "..\helper.h"

// For some reason, if one passes these functions as function pointers, the code runs faster

static uint64_t seed, s32[2], s64[4], s64m[4];

static inline uint64_t rotate_l(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

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
	s32[0] = rotate_l(s0, 24) ^ s1 ^ (s1 << 16);
	s32[1] = rotate_l(s1, 37);

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

	s64[3] = rotate_l(s64[3], 45);

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

// how the fuck does this run faster
static inline u64 benchmark_rng(uint64_t (*rng_func)(void))
{
    u64 start = ns();
    for (u64 i = 0; i < 1e6; i++)
        rng_func();
    return ns() - start;
}

u64 benchmark_rng2(u32 func)
{
	u64 start = ns();

	if (func == 1)
	{
		for (u64 i = 0; i < 1e6; i++)
			splitmix64();	
	}
	else if (func == 2)
	{
		for (u64 i = 0; i < 1e6; i++)
			xoshiro128();
	}
	else
	{
		for (u64 i = 0; i < 1e6; i++)
			xoshiro256();
	}
	return (ns() - start);
}

f64 get_avg(u64 *ptr, u32 start, u32 n)
{
	u64 sum = 0;
	for (u32 i = start; i < n; i++)
		sum += ptr[i];
	return (((f64)(sum)) / (n - start));
}

int main()
{
    init_seed(ns());
    const int num_runs = 100;
    const int WARMUP = 10;
    
	u64 results[3][num_runs];

	for (int run = 0; run < num_runs; run++) 
	{
		results[0][run] = benchmark_rng(splitmix64);
		results[1][run] = benchmark_rng(xoshiro128);
		results[2][run] = benchmark_rng(xoshiro256);
    }
	printf("With function pointers\n");
    printf("splitmix64: %f ms\n", printfloat(get_avg(results[0], WARMUP, num_runs)));
    printf("xoshiro128: %f ms\n", printfloat(get_avg(results[1], WARMUP, num_runs)));
    printf("xoshiro256: %f ms\n", printfloat(get_avg(results[2], WARMUP, num_runs)));
	printf("==================\n");

	for (int run = 0; run < num_runs; run++) 
	{
		results[0][run] = benchmark_rng2(1);
		results[1][run] = benchmark_rng2(2);
		results[2][run] = benchmark_rng2(3);
    }

	printf("Function encapsulation but manual branching\n");
    printf("splitmix64: %f ms\n", printfloat(get_avg(results[0], WARMUP, num_runs)));
    printf("xoshiro128: %f ms\n", printfloat(get_avg(results[1], WARMUP, num_runs)));
    printf("xoshiro256: %f ms\n", printfloat(get_avg(results[2], WARMUP, num_runs)));
	printf("==================\n");

    for (int run = 0; run < num_runs; run++) 
	{
        u64 start;

        {
            start = ns();
            for (u64 i = 0; i < 1e6; i++)
                splitmix64();
			results[0][run] = ns() - start;
        }

        {
            start = ns();
            for (u64 i = 0; i < 1e6; i++)
                xoshiro128();
			results[1][run] = ns() - start;
        }

        {
            start = ns();
            for (u64 i = 0; i < 1e6; i++)
                xoshiro256();
			results[2][run] = ns() - start;
        }
    }

	printf("Main code, direct function calls\n");
    printf("splitmix64: %f ms\n", printfloat(get_avg(results[0], WARMUP, num_runs)));
    printf("xoshiro128: %f ms\n", printfloat(get_avg(results[1], WARMUP, num_runs)));
    printf("xoshiro256: %f ms\n", printfloat(get_avg(results[2], WARMUP, num_runs)));
}