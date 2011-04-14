/*
	Stream.h
	"MPEG 1,2,3 decoder/MAPlay decoder"

	Riarrangiamento del codice originale (vedi sopra).
	Luca Piergentili, 07/06/03
	lpiergentili@yahoo.com
*/
#ifndef _STREAM_H
#define _STREAM_H 1

#include "audioconfig.h"
#ifdef MAPlay_MPEG_DECODER

#include "Common.h"
#include "Args.h"

// header mask definition
#define HDRCMPMASK 0xfffffd00

// maximum frame size
#define MAXFRAMESIZE 1792

bool			Stream_Init		(MPArgs* lpArgs);
bool			Stream_Initialized	(void);
void			Stream_Reset		(void);
void			Stream_Done		(void);
bool			Stream_HeadRead	(unsigned char* hbuf,unsigned long* newhead);
bool			Stream_HeadCheck	(unsigned long newhead);
int			Stream_ReadFrame	(void);
unsigned long	Stream_GetMinFrames	(void);
unsigned long	Stream_GetMaxFrames	(void);
unsigned long	Stream_FrameSize	(void);
unsigned long	Stream_GetBitrate	(void);
unsigned long	Stream_GetSampleRate(void);
unsigned long	Stream_GetFreq		(int nIndex);
double		Stream_Frame2ms	(void);
double		Stream_Totalms		(void);
unsigned long	Stream_Currentms(void);
bool			Stream_Seek		(unsigned long ulPosition);
void			Stream_SetPointer	(long lBackstep);

extern unsigned long __forceinline Stream_Get1Bit		(void); // gets the stream bits for the decoder
extern unsigned long __forceinline Stream_GetBits		(int nNumberOfBits);
extern unsigned long __forceinline Stream_GetBitsFast	(int nNumberOfBits);

#endif // MAPlay_MPEG_DECODER
#endif // _STREAM_H
