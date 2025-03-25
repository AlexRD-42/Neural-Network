#include "..\helper.h"

/* This is a Marsaglia multiply-with-carry generator with period
   approximately 2^255. It is faster than a scrambled linear
   generator, as its only 128-bit operations are a multiplication and sum;
   it is an excellent generator based on congruential arithmetic.

   As all MWC generators, it simulates a multiplicative LCG with prime
   modulus m = 0xfff62cf2ccc0cdaeffffffffffffffffffffffffffffffffffffffffffffffff
   and multiplier given by the inverse of 2^64 modulo m. The modulus has a
   particular form, which creates some theoretical issues, but at this
   size a generator of this kind passes all known statistical tests. For a
   generator of the same type with stronger theoretical guarantees
   consider a Goresky-Klapper generalized multiply-with-carry generator.

   Note that a previous version had a different MWC_A3. Moreover, now
   the result is computed using the current state.
*/

/* The state must be initialized so that 0 < c < MWC_A3 - 1.
   For simplicity, we suggest to set c = 1 and x, y, z to a 192-bit seed. */

uint64_t x, y, z, c;

uint64_t inline next() 
{
	const uint64_t result = z;
	const __uint128_t t = 0xfff62cf2ccc0cdaf * (__uint128_t)x + c;
	x = y;
	y = z;
	z = t;
	c = t >> 64;
	return result;
}