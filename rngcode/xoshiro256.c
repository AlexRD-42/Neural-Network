#include "rng_code.h"

static const uint64_t JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c};
static u64 state[4];

// 3 lower bits have low linear complexity

void init_seed()
{
	state[0] = splitmix64(ns());
	state[1] = splitmix64(state[0]);
	state[2] = splitmix64(state[1]);
	state[3] = splitmix64(state[2]);
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

void jump(void)
{
	uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

	for(int i = 0; i < 4; i++)
		for(int b = 0; b < 64; b++)
		{
			if (JUMP[i] & UINT64_C(1) << b)
			{
				s0 ^= state[0];
				s1 ^= state[1];
				s2 ^= state[2];
				s3 ^= state[3];
			}
			next();	
		}
		
	state[0] = s0;
	state[1] = s1;
	state[2] = s2;
	state[3] = s3;
}