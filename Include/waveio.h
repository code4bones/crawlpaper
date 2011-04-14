/*
	waveio.h
	I/O sui files di tipo RIFF WAVE.
	Luca Piergentili, 14/07/03
	lpiergentili@yahoo.com

	Derivato (modificato e riadattato) dal codice di esempio MS (ACMAPP):
	
	Contains routines for opening and closing RIFF WAVE files.
	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
	EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
	Copyright (C) 1992 - 1997 Microsoft Corporation.  All Rights Reserved.
*/
#ifndef _WAVEIO_H
#define _WAVEIO_H 1

#ifndef RC_INVOKED
  #pragma pack(1)
#endif

#include "window.h"

#define WIOAPI _stdcall

#ifndef SIZEOF_WAVEFORMATEX
#define SIZEOF_WAVEFORMATEX(pwfx) ((WAVE_FORMAT_PCM==(pwfx)->wFormatTag) ? sizeof(PCMWAVEFORMAT) : (sizeof(WAVEFORMATEX)+(pwfx)->cbSize))
#endif

static __inline LPVOID GlobalExactLock(HGLOBAL hg,DWORD cb) {return((LPVOID)((hg) ? (LPVOID)((LPBYTE)GlobalLock(hg) + (GlobalSize(hg) - (cb))) : (LPVOID)NULL));}

#undef GlobalAllocPtr
#undef GlobalReAllocPtr
#undef GlobalPtrHandle
#undef GlobalLockPtr
#undef GlobalUnlockPtr
#undef GlobalFreePtr

// waveio.h
#define GlobalAllocPtr(flags,cb)			(GlobalExactLock(GlobalAlloc((flags),(cb)),(cb)))
#define GlobalReAllocPtr(lp,cbNew,flags)	(GlobalUnlockPtr(lp),GlobalExactLock(GlobalReAlloc(GlobalPtrHandle(lp),(cbNew),(flags)),(cbNew)))
// windowsx.h
#define GlobalPtrHandle(lp)				((HGLOBAL)GlobalHandle(lp))
#define GlobalLockPtr(lp)				((BOOL)GlobalLock(GlobalPtrHandle(lp)))
#define GlobalUnlockPtr(lp)				GlobalUnlock(GlobalPtrHandle(lp))
#define GlobalFreePtr(lp)				(GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))

#ifndef _WIOERR_DEFINED
#define _WIOERR_DEFINED 1
typedef UINT WIOERR;
#endif

#ifndef _WAVEIOCB_DEFINED
#define _WAVEIOCB_DEFINED 1
typedef struct tWAVEIOCB
{
	DWORD	dwFlags;
	HMMIO	hmmio;

	DWORD	dwDataOffset;
	DWORD	dwDataBytes;
	DWORD	dwDataSamples;

	LPWAVEFORMATEX pwfx;

} WAVEIOCB, *PWAVEIOCB, FAR *LPWAVEIOCB;
#endif

#define WIOERR_BASE			(0)
#define WIOERR_NOERROR		(0)
#define WIOERR_ERROR		(WIOERR_BASE+1)
#define WIOERR_BADHANDLE		(WIOERR_BASE+2)
#define WIOERR_BADFLAGS		(WIOERR_BASE+3)
#define WIOERR_BADPARAM		(WIOERR_BASE+4)
#define WIOERR_BADSIZE		(WIOERR_BASE+5)
#define WIOERR_FILEERROR		(WIOERR_BASE+6)
#define WIOERR_NOMEM		(WIOERR_BASE+7)
#define WIOERR_BADFILE		(WIOERR_BASE+8)
#define WIOERR_NODEVICE		(WIOERR_BASE+9)
#define WIOERR_BADFORMAT		(WIOERR_BASE+10)
#define WIOERR_ALLOCATED		(WIOERR_BASE+11)
#define WIOERR_NOTSUPPORTED	(WIOERR_BASE+12)

WIOERR WIOAPI wioFileOpen(LPWAVEIOCB pwio,LPCSTR lpcszFilePath,DWORD fdwOpen);
WIOERR WIOAPI wioFileClose(LPWAVEIOCB pwio);

#ifndef RC_INVOKED
  #pragma pack()
#endif

#endif // _WAVEIO_H
