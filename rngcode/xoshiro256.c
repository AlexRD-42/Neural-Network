#include "..\helper.h"

static const uint64_t JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c};
static uint64_t s0 = 411231, s1 = 1231231, s2 = 12312454, s3 = 978321594328;

uint64_t next(void)
{
	const uint64_t result = s0 + s3;
	const uint64_t t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;

	s3 = (s3 << 45) | (s3 >> 19);
	return result;
}

void jump(void)
{
	uint64_t t0 = 0, t1 = 0, t2 = 0, t3 = 0;

	for (int i = 0; i < 4; i++)
		for (int b = 0; b < 64; b++)
		{
			if (JUMP[i] & UINT64_C(1) << b)
			{
				t0 ^= s0;
				t1 ^= s1;
				t2 ^= s2;
				t3 ^= s3;
			}
			next();	
		}
	s0 = t0;
	s1 = t1;
	s2 = t2;
	s3 = t3;
}

// static inline f32 rng_float(void)
// {
// 	return (f32)((next() >> 32) * (2.0f / 4294967296.0f) - 1.0);
// }

static inline f32 rng_float(void)
{
    uint64_t r = next();

    uint32_t bits = ((r >> 63) << 31) | (0x3F000000) | ((r >> 41) & 0x007FFFFF);
    f32 value = *(f32*)&bits;
    return value * 2.0f;
}

int main()
{
	for (u32 i = 0; i < 10; i++)
		printf("%f\n", rng_float());

}