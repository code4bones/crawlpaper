/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/
#ifndef _CRAND_H
#define _CRAND_H 1

#include <stdio.h>
#include <stdlib.h>

#define N_SEED 624
#define M_SEED 397

/*
	CRand
*/
class CRand
{
public:
	CRand(unsigned long s = (unsigned long)-1L);
	virtual ~CRand() {}

	unsigned long Rand(void)
	{
		unsigned long n = labs(genrand_int32());
		if(n==m_nPrevious)
		{
			while(n==m_nPrevious)
				n = labs(genrand_int32());
		}
		else
			m_nPrevious = n;
		return(n);
	}
	
	unsigned long RandMax(unsigned long nMax) {return(Rand() % nMax);}
	unsigned long RandRange(unsigned long nMin,unsigned long nMax) {return(Rand() % (nMax - nMin + 1) + nMin);}
	
	void Seed(unsigned long s) {init_genrand(s);}
	void SeedArray(unsigned long init_key[],int key_length) {init_by_array(init_key,key_length);}

private:
	void init_genrand(unsigned long s);
	void init_by_array(unsigned long init_key[],int key_length);
	unsigned long genrand_int32(void);
	long genrand_int31(void);
	double genrand_real1(void);
	double genrand_real2(void);
	double genrand_real3(void);
	double genrand_res53(void);

	unsigned long m_nPrevious;
	unsigned long mt[N_SEED]; /* the array for the state vector  */
	int mti; /* mti==N_SEED+1 means mt[N_SEED] is not initialized */
};

#endif // _CRAND_H
