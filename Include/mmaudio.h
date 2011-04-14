/*
	mmaudio.h
	Definizioni multimedia relative all'audio.
	Luca Piergentili, 15/07/03
	lpiergentili@yahoo.com
*/
#ifndef _MMAUDIO_H
#define _MMAUDIO_H 1

#include "window.h"

/* modalita' input */
typedef enum _mmAudioInputMode {
	mmAudioImFile = 1,
	mmAudioImCallback
} mmAudioInputMode;

/* modalita' output */
typedef enum _mmAudioOutputMode {
	mmAudioOmMMSystem = 1, 
	mmAudioOmWaveFile, 
	mmAudioOmDirectSound,
	mmAudioOmCallback
} mmAudioOutputMode;

/* status del player */
typedef enum _mmAudioPlayerMode {
	mmAudioPmLoading = 1,
	mmAudioPmOpened,
	mmAudioPmReady,
	mmAudioPmPlaying,
	mmAudioPmPaused,
	mmAudioPmStopped,
	mmAudioPmClosed,
	mmAudioPmDone,
	mmAudioPmUndefined
} mmAudioPlayerMode;

/*
	MMAERROR
	tipo per il codice di ritorno
*/
#ifndef _MMAERROR_DEFINED
#define _MMAERROR_DEFINED 1
typedef DWORD MMAERROR;
#endif

/*
	MMAERROR_...
	codici di ritorno
*/
#define MMAERR_BASE					1965
#define MMAERR_NOERROR				0
#define MMAERR_PLAYER_NOT_SUPPORTED	MMAERR_BASE+1
#define MMAERR_PLAYER_INCORRECT_MODE	MMAERR_BASE+2
#define MMAERR_PLAYER_INTERNAL		MMAERR_BASE+3
#define MMAERR_PLAYER_INVALID_HANDLE	MMAERR_BASE+4
#define MMAERR_PLAYER_INPUT			MMAERR_BASE+5
#define MMAERR_PLAYER_OUTPUT			MMAERR_BASE+6
#define MMAERR_PLAYER_NO_MEMORY		MMAERR_BASE+7
#define MMAERR_PLAYER_NO_RESOURCE		MMAERR_BASE+8
#define MMAERR_PLAYER_NO_CALLBACK		MMAERR_BASE+9
#define MMAERR_PLAYER_INVALID_FILE		MMAERR_BASE+10

/* tipi mime per i files audio */

/* .wav */
#define WAV_SUFFIX		".wav"
#define WAV_EXTENSION	WAV_SUFFIX
#define WAV_TYPENAME	"Waveform audio file"
#define WAV_TYPEDESC	"Windows Audio Volume"
#define WAV_CONTENTTYPE	"audio/wav"

/* .cda */
#define CDA_SUFFIX		".cda"
#define CDA_EXTENSION	CDA_SUFFIX
#define CDA_TYPENAME	"CD Audio Track"
#define CDA_TYPEDESC	"CD Audio Track"
#define CDA_CONTENTTYPE	"audio/cda"

/* .mp3 */
#define MP3_SUFFIX		".mp3"
#define MP3_EXTENSION	MP3_SUFFIX
#define MP3_TYPENAME	"MP3 file"
#define MP3_TYPEDESC	"mpeg audio data"
#define MP3_CONTENTTYPE	"audio/mpeg"

/* formati */
typedef enum _mmAudioFormat {
	mmAudioAfUnknow = 0,
	mmAudioAfWav,
	mmAudioAfCda,
	mmAudioAfMp3
} mmAudioFormat;

#endif /* _MMAUDIO_H */
