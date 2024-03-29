/*
	Args.h
	"MPEG 1,2,3 decoder/MAPlay decoder"

	Riarrangiamento del codice originale (vedi sopra).
	Luca Piergentili, 07/06/03
	lpiergentili@yahoo.com
*/
#ifndef _ARGS_H
#define _ARGS_H 1

#include "audioconfig.h"
#ifdef MAPlay_MPEG_DECODER

#include "window.h"
#include "mmaudio.h"
#include "Common.h"

typedef bool (_stdcall *WaveOutCB)(void* SampleData,int SBits,int Channels,unsigned long SampleRate,unsigned long Size,unsigned long userdata);
typedef bool (_stdcall *WaveOutActionCB)(bool Init,unsigned long userdata);
typedef bool (_stdcall *InFileOpenCB)(unsigned long UserData1,unsigned long* UserData2,bool* IsSeekable);
typedef bool (_stdcall *InFileCloseCB)(unsigned long UserData1,unsigned long UserData2);
typedef bool (_stdcall *InFileGetSizeCB)(unsigned long UserData1,unsigned long UserData2,unsigned long* InSize);
typedef bool (_stdcall *InFileSeekCB)(unsigned long UserData1,unsigned long UserData2,LPDWORD NewPos,DWORD dwMoveMethod);
typedef bool (_stdcall *InFileReadCB)(unsigned long UserData1,unsigned long UserData2,LPVOID buffer,DWORD bytes_to_read,DWORD* bytes_read);
typedef int (*synthProc)(double*,int,unsigned char*,int*);
typedef int (*synth_monoProc)(double*,unsigned char*,int*);

struct al_table {
	short bits;
	short d;
};

struct frame {
    synthProc synth;
    synth_monoProc synth_mono;

    const struct al_table *alloc;
    long stereo;
    long jsbound;
    long single;
    long II_sblimit;
    long lsf;
    long mpeg25;
    long down_sample;
    long header_change;
    unsigned long block_size;
    long lay;
    long WhatLayer;
    long error_protection;
    long bitrate_index;
    long sampling_frequency;
    long padding;
    long extension;
    long mode;
    long mode_ext;
    long copyright;
    long original;
    long emphasis;    
};

class MPArgs {
public:
	DWORD PlayerThreadID;
	frame fr;
	MMAERROR LastError;
    unsigned long CurrentPos;   // Current player position in frames
    unsigned long StartFrame,StartPos,EndFrame,EndPos;
    unsigned long SampleRate;   // Samplerate
    unsigned long BufferSize;   // The size of the output buffer
    unsigned long Buffers;      // The number of buffers 
    long Channels;              // Channels(1=mono;2=stereo)
    long AudioBits;             // Bits per sample(ex. 16 bit)
    long AudioDevice;           // The audio device number which should be opened
    long AudioMode;             // Audio type(ex. 16bit signed),currently not used  
    long ScaleFactor;           // Output scale factor(ex. 32768 which is normal)
    long ForceFreq;             // Force frequency?
    bool TryResync;             // Resync on bad data?
    bool BufferedInput;         // Not used
    bool Seekable;              // Is the stream seekable?
    bool IsVBR;                   // Used to determine stream lehgth
    bool UseHdrCB;              // Use output header callback
    char *InName;               // Input stream filename 
    char *OutName;              // Output filename for wave output
    long OutDeviceNum;          // Device number for MMSystem output
    long ForceMono;             // Force mono output?
    long Force8bit;             // Force 8-Bit output?
    long ForceStereo;           // Force stereo output? 
    long DownSample;            // Downsample Modes
    long UseEqualizer;          // Activate equalizer? 
    long PlayPriority;          // Play thread priority
    void *Player;
    void *Decoder;
    mmAudioInputMode  InputMode;    
    mmAudioOutputMode OutputMode;
    WaveOutCB BufferCB;
    WaveOutActionCB OutActionCB;
    InFileOpenCB InOpenCB;
    InFileCloseCB InCloseCB;
    InFileGetSizeCB InGetSizeCB;
    InFileSeekCB InSeekCB;
    InFileReadCB InReadCB;

    // user-defined data for output callbacks
    unsigned long OutCBData;
    
    // user-defined data for input callbacks
    unsigned long InCBData1;
    unsigned long InCBData2;

    HWND wnd;
    UINT nMsg;

    MPArgs() {
        InName = NULL;
        OutName= NULL;
        OutDeviceNum = 0;
        BufferedInput = true;
        CurrentPos = 0;
        StartPos = 0;
        EndPos = 0;
        // No downsampling
        DownSample = 0;
        // Force Stereo output
        ForceStereo = 0;
        AudioBits = 16;
        SampleRate = 44100;
        Channels = 2;
        AudioMode = 1;
        AudioDevice = 0;
        BufferSize = 16384;
        Buffers = 8;
        BufferCB    = NULL;
        UseHdrCB    = false;
        TryResync   = TRUE;
        Seekable    = true;
        Force8bit   = 0;
        ForceFreq   = -1;
        ForceMono   = 0;
        ScaleFactor = 32768;
        UseEqualizer= true;//false;
        PlayPriority= THREAD_PRIORITY_NORMAL;
        OutCBData   = 0;
        InCBData1   = 0;
        InCBData2   = 0; 
        OutActionCB = NULL;
        BufferCB    = NULL;
        wnd         = 0;
	   nMsg = 0;
    }

    ~MPArgs() {
        if (InName) delete [] InName,InName = NULL;
        if (OutName) delete [] OutName,OutName = NULL;
    }
};

#endif // MAPlay_MPEG_DECODER
#endif // _ARGS_H
