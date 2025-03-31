#include "..\helper.h"

u64 splitmix64(u64 seed)
{
	u64 result = (seed += 0x9E3779B97f4A7C15);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
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