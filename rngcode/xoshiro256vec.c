#include "..\helper.h"

/* The current state of the generators. */
static uint64_t s[4][8];

static __inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}
#define INIT for(int i = 0; i < 8; i++) s[0][i] = 1 << i;
#define NEXT next(result, 1000);

uint64_t result[1000];

static inline uint64_t next(uint64_t * const restrict array, int len) 
{
 	uint64_t t[8];

	for(int b = 0; b < len; b += 8)
	{
		for(int i = 0; i < 8; i++) 
			array[b + i] = s[0][i] + s[3][i];

		for(int i = 0; i < 8; i++) 
			t[i] = s[1][i] << 17;

		for(int i = 0; i < 8; i++) 
			s[2][i] ^= s[0][i];
		for(int i = 0; i < 8; i++) 
			s[3][i] ^= s[1][i];
		for(int i = 0; i < 8; i++) 
			s[1][i] ^= s[2][i];
		for(int i = 0; i < 8; i++) 
			s[0][i] ^= s[3][i];

		for(int i = 0; i < 8; i++) 
			s[2][i] ^= t[i];
		for(int i = 0; i < 8; i++) 
			s[3][i] = rotl(s[3][i], 45);
	}

	// This is just to avoid dead-code elimination
	return array[0] ^ array[len - 1];
}
