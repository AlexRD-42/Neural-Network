#include "..\helper.h"

static f32 res;
static u64 state[4];

u64 splitmix64()
{
	u64 result = (state[0] += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

u64 xoshiro128()
{
	const u64 s0 = state[0];
	u64 s1 = state[1];
	const u64 result = s0 + s1;

	s1 ^= s0;
	state[0] = ((s0 << 24) | (s0 >> 40)) ^ s1 ^ (s1 << 16);
	state[1] = (s1 << 37) | (s1 >> 27);
	
	return result;
}

u64 xoshiro256()
{
	const u64 result = state[0] + state[3];
	const u64 t = state[1] << 17;

	state[2] ^= state[0];
	state[3] ^= state[1];
	state[1] ^= state[2];
	state[0] ^= state[3];

	state[2] ^= t;

	state[3] = (state[3] << 45) | (state[3] >> 19);

	return result;
}

u64 mwc256()
{
	const u64 result = state[2];
	const __uint128_t t = 0xfff62cf2ccc0cdaf * (__uint128_t)state[0] + state[3];
	state[0] = state[1];
	state[1] = state[2];
	state[2] = t;
	state[3] = t >> 64;
	return result;
}

static inline f32 fast_itof(u64 value)
{
	f32 result;
	uint32_t bits = (uint32_t)(value >> 32);
	bits &= 0b10000000011111111111111111111111; // 0x807FFFFF
	bits |= 0b00111111000000000000000000000000; // 0x3F000000
	memcpy(&result, &bits, 4);
	return result;
}

u64 benchmark_rng(u32 func, u64 num_iter)
{
	res = 0.0f;
	u64 start = ns();

	switch (func)
	{
		case 1:
			for (u64 i = 0; i < num_iter; i++)
				res += fast_itof(splitmix64());
			break;
		case 2:
			for (u64 i = 0; i < num_iter; i++)
				res += fast_itof(xoshiro128());
			break;
		case 3:
			for (u64 i = 0; i < num_iter; i++)
				res += fast_itof(xoshiro256());
			break;
		case 4:
			for (u64 i = 0; i < num_iter; i++)
				res += fast_itof(mwc256());
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

void init_seed(u64 initial_seed)
{
	state[0] = splitmix64(initial_seed);
	state[1] = splitmix64(state[0]);
	state[2] = splitmix64(state[1]);
	state[3] = splitmix64(state[2]);
}

int main()
{
	u64 num_runs = 1e3;
	u64 num_iter = 1e3;
	u64 (*results)[num_runs] = malloc(num_runs * 4 * sizeof(u64));
	init_seed(ns());

	for (u64 run = 0; run < num_runs; run++) 
	{
		results[0][run] = benchmark_rng(1, num_iter);
		results[1][run] = benchmark_rng(2, num_iter);
		results[2][run] = benchmark_rng(3, num_iter);
		results[3][run] = benchmark_rng(4, num_iter);
    }

    printf("splitmix64: %f ns\n", printns(get_avg(results[0], num_runs)));
    printf("xoshiro128: %f ns\n", printns(get_avg(results[1], num_runs)));
    printf("xoshiro256: %f ns\n", printns(get_avg(results[2], num_runs)));
	printf("mwc256: %f ns\n", printns(get_avg(results[3], num_runs)));
	printf("\n%f", res);
}