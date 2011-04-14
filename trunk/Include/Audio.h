/*
	Audio.h
	"MPEG 1,2,3 decoder/MAPlay decoder"

	Riarrangiamento del codice originale (vedi sopra).
	Luca Piergentili, 07/06/03
	lpiergentili@yahoo.com
*/
#ifndef _AUDIO_H
#define _AUDIO_H 1

#include "audioconfig.h"
#ifdef MAPlay_MPEG_DECODER

#include "window.h"
#include "Args.h"

bool			Audio_Init			(MPArgs* Args,HWAVEOUT* pWaveDevice);
bool			Audio_Done			(void);
void			Audio_ProcessMessages	(void);
long			Audio_PlaySamples		(unsigned char* buf,long len);
long			Audio_GetBufferLoad		(void);
unsigned long	Audio_QueuedBuffers		(void);
void			Audio_FinishBuffer		(BOOL bForce = FALSE);
bool			Audio_ExecuteCallback	(void);
void			Audio_Reset			(void);
void			Audio_Pause			(void);
void			Audio_Resume			(void);
bool			Audio_CanSetVolume		(bool* Separate); 
bool			Audio_SetVolume		(unsigned long Volume);
bool			Audio_GetVolume		(unsigned long* Volume);
void			Audio_HardReset		(void);
void			Audio_ResetBufferCount	(void);

#endif // MAPlay_MPEG_DECODER
#endif // _AUDIO_H
