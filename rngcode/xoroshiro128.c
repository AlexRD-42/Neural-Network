#include "rng_code.h"

static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };
static uint64_t state[2];

// 4 lower bits have low linear
uint64_t xoroshiro128()
{
	const uint64_t s0 = state[0];
	uint64_t s1 = state[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	state[0] = ((s0 << 24) | (s0 >> 40)) ^ s1 ^ (s1 << 16);
	state[1] = (s1 << 37) | (s1 >> 27);

	return result;
}

__m256i xorshift128plus_avx2(__m256i &state0, __m256i &state1)
{
    __m256i s1 = state0;
    const __m256i s0 = state1;
    state0 = s0;
    s1 = _mm256_xor_si256(s1, _mm256_slli_epi64(s1, 23));
    state1 = _mm256_xor_si256(_mm256_xor_si256(_mm256_xor_si256(s1, s0),
                                               _mm256_srli_epi64(s1, 18)),
                              _mm256_srli_epi64(s0, 5));
    return _mm256_add_epi64(state1, s0);
}

void jump()
{
	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for(int i = 0; i < 4; i++)
		for(int b = 0; b < 64; b++)
		{
			if (JUMP[i] & UINT64_C(1) << b)
			{
				s0 ^= state[0];
				s1 ^= state[1];
			}
			xoroshiro128();
		}
	state[0] = s0;
	state[1] = s1;
}
