/*
	CVBitRate.cpp

	CVBitRate class is used to retrieve a VBR's Header
	and load that into a usable structure.

	This code will be well commented, so that everyone can
	understand, as it's made for the public and not for
	private use, although private use is allowed. :)

	all functions specified both in the header and .cpp file
	will have explanations in both locations.

	everything here by: Gustav "Grim Reaper" Munkby
				   http://home.swipnet.se/grd/
				   grd@swipnet.se

	LPI 7/8/03: modifiche minori al codice originale.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <string.h>
#include "CVBitRate.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// flags to know what information could be found in the headers
#define FRAMES_FLAG    0x0001
#define BYTES_FLAG     0x0002
#define TOC_FLAG       0x0004
#define VBR_SCALE_FLAG 0x0008

/*
	loadHeader()
*/
bool CVBitRate::loadHeader(char inputheader[12])
{
	// The Xing VBR headers always begin with the four
	// chars "Xing" so this tests wether we have a VBR
	// header or not
	if(memcmp(inputheader,"Xing",4))
	{
		m_nFrames = -1;
		return(false);
	}

	// now we will get the flags and number of frames,
	// this is done in the same way as the FrameHeader
	// is generated in the CFrameHeader class
	// if you're curious about how it works, go and look
	// there

	// here we get the flags from the next four bytes
	int flags = (int)(
	( (inputheader[4] & 255) << 24) |
	( (inputheader[5] & 255) << 16) |
	( (inputheader[6] & 255) <<  8) |
	( (inputheader[7] & 255)      )
	); 

	// if this tag contains the number of frames, load
	// that number into storage, if not something will
	// be wrong when calculating the bitrate and length
	// of the music
	if(flags & FRAMES_FLAG)
	{
		m_nFrames = (int)(
		( (inputheader[ 8] & 255) << 24) |
		( (inputheader[ 9] & 255) << 16) |
		( (inputheader[10] & 255) <<  8) |
		( (inputheader[11] & 255)      )
		); 
	}
	else
	{
		// returning -1 so an error would be obvious
		// not many people would believe in a bitrate
		// -21 kbps :)
		m_nFrames = -1;

		// this function was returning false before
		// as there is an error occuring, but in that
		// case the bitrate wouldn't be unbelievable
		// so that's why I changed my mind and let it
		// return true instead
		return(true);
	}

	// if it gets this far, everything went according
	// to plans, so we should return true!
	return(true);
}
