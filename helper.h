#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#include <omp.h>
#include <cblas.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "hres_timer.h"

#define abs(x) ((x) < 0 ? -(x) : (x))
#define printms(x) (((f64)(x))/1e6)
#define printus(x) (((f64)(x))/1e3)

typedef unsigned char u8;
typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef int32_t i32;
typedef int64_t i64;
typedef uint64_t u64;

// f32 ft_rand(void)
// {
// 	return (f32) rand() / (f32) RAND_MAX;
// }

// Xoshiro256+ state
static uint64_t xoshiro_state[4] = {0x123456789ABCDEF0, 0xFEDCBA9876543210, 0xA1B2C3D4E5F6A7B8, 0x1A2B3C4D5E6F7A8B};

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static inline uint64_t xoshiro256plus(void) {
    const uint64_t result = xoshiro_state[0] + xoshiro_state[3];
    const uint64_t t = xoshiro_state[1] << 17;
    
    xoshiro_state[2] ^= xoshiro_state[0];
    xoshiro_state[3] ^= xoshiro_state[1];
    xoshiro_state[1] ^= xoshiro_state[2];
    xoshiro_state[0] ^= xoshiro_state[3];
    
    xoshiro_state[2] ^= t;
    xoshiro_state[3] = rotl(xoshiro_state[3], 45);
    
    return result;
}

// Seed the PRNG
static inline void seed_xoshiro(uint64_t seed) {
    xoshiro_state[0] = seed;
    xoshiro_state[1] = seed ^ 0x1234567890ABCDEF;
    xoshiro_state[2] = seed ^ 0xFEDCBA9876543210;
    xoshiro_state[3] = seed ^ 0xA1B2C3D4E5F67890;
    
    // Warm up
    for (int i = 0; i < 10; i++) 
        xoshiro256plus();
}

// Replace your existing ft_rand with this faster version
f32 ft_rand(void)
{
    // Extract top 23 bits for maximum precision
    uint32_t bits = (uint32_t)(xoshiro256plus() >> 40);
    bits = (bits & 0x007FFFFF) | 0x3F800000;  // Set exponent for 1.0 <= x < 2.0
    float f = *(float*)&bits;
    return f - 1.0f;  // Adjust range to 0.0 <= x < 1.0
}

// Initialize the PRNG with system time
void init_rand(void)
{
    seed_xoshiro((uint64_t)time(NULL));
}

i32 check_matrix(f32 *A, f32 *B, i32 size)
{
	i32 i = 0;
	i32 counter = 0;
	for (i = 0; i < size; i++)
	{
		if (abs(A[i] - B[i]) > 0.001)
			counter++;
	}
	return (counter);
}