#if 0
#ifndef _MT19937INT_H
#define _MT19937INT_H 1

#include <stdio.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)


#ifdef __cplusplus
extern "C" {
#endif

/* initializing the array with a NONZERO seed */
void
sgenrand(unsigned long seed);	

unsigned long genrand(void);

#ifdef __cplusplus
}
#endif

#endif // _MT19937INT_H
#endif
