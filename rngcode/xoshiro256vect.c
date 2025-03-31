#include "rng_code.h"

static const uint64_t JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c};
static u64 state0[8], state1[8], state2[8], state3[8];

// 3 lower bits have low linear complexity

void init_seed()
{
	for (u32 i = 0; i < 8; i++)
	{
		state0[i] = splitmix64(ns() + i);
		state1[i] = splitmix64(state0[i]);
		state2[i] = splitmix64(state1[i]);
		state3[i] = splitmix64(state2[i]);
	}
}

static inline u64 *xoshiro256(void)
{
	u64 result[8];
	for (u32 i = 0; i < 8; i++)
		result[i] = state0[i] + state3[i];
	return result;
}

void xoshiro256_update()
{
	u64 t[8];
	for (u32 i = 0; i < 8; i++)
	{
		t[i] = state1[i] << 17;
		state2[i] ^= state0[i];
		state3[i] ^= state1[i];
		state1[i] ^= state2[i];
		state0[i] ^= state3[i];
		
		state2[i] ^= t[i];
		state3[i] = (state3[i] << 45) | (state3[i] >> 19);
	}
}