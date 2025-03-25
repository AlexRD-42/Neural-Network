#include "..\helper.h"

static uint64_t state[4];

uint64_t splitmix64_seed(uint64_t seed)
{
	uint64_t result = (seed += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

static inline uint64_t next(void)
{
	const uint64_t result = state[0] + state[3];
	const uint64_t t = state[1] << 17;

	state[2] ^= state[0];
	state[3] ^= state[1];
	state[1] ^= state[2];
	state[0] ^= state[3];
	state[2] ^= t;
	state[3] =  (state[3] << 45) | (state[3] >> 19);

	return result;
}

static inline f32 frand2(void)
{
	return (f32)((next() >> 32) * (2.0 / 4294967296.0) - 1.0);
}

static inline f32 frand(void)
{
    uint32_t bits = (uint32_t)(next() >> 32);
	bits &= 0b10111111011111111111111111111111;
	bits |= 0b00111111000000000000000000000000;
    return *(f32*)&bits;
}

int main()
{
	u64 seed = ns();
	for (int i = 0; i < 4; i++)
		state[i] = splitmix64_seed(seed + i);

	u64 start = ns();
	for (int i = 0; i < 10; i++)
		frand2();
	u64 end = ns();
	printf("%lld", end-start);
}
