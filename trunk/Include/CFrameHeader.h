/*
	CFrameHeader.h

	CFrameHeader class is used to retrieve a MP3's FrameHeader
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
#ifndef _CFRAMEHEADER_H
#define _CFRAMEHEADER_H 1

class CFrameHeader
{
public:
	CFrameHeader() {}
	virtual ~CFrameHeader() {}

	// This function is quite easy to understand, it loads
	// 4 chars of information into the CFrameHeader class
	// The validity is not tested, so with this function
	// an invalid FrameHeader could be retrieved
	void loadHeader(char c[4]);

	// This function is a supplement to the loadHeader
	// function, the only purpose is to detect if the
	// header loaded by loadHeader is a valid header
	// or just four different chars
	bool isValidHeader(void);

	// The following are functions to get the "indexes" for the various
	// information avalaible. To know which meaning the different
	// bits you need to look into a table, instead of having to
	// do this everytime these functions get the correct index
	// from the correct bits. :)
	inline int getFrameSync		(void)	{ return((m_ulBithdr>>21) & 2047); }
	inline int getVersionIndex	(void)	{ return((m_ulBithdr>>19) & 3);    }
	inline int getLayerIndex		(void)	{ return((m_ulBithdr>>17) & 3);    }
	inline int getProtectionBit	(void)	{ return((m_ulBithdr>>16) & 1);    }
	inline int getBitrateIndex	(void)	{ return((m_ulBithdr>>12) & 15);   }
	inline int getFrequencyIndex	(void)	{ return((m_ulBithdr>>10) & 3);    }
	inline int getPaddingBit		(void)	{ return((m_ulBithdr>> 9) & 1);    }
	inline int getPrivateBit		(void)	{ return((m_ulBithdr>> 8) & 1);    }
	inline int getModeIndex		(void)	{ return((m_ulBithdr>> 6) & 3);    }
	inline int getModeExtIndex	(void)	{ return((m_ulBithdr>> 4) & 3);    }
	inline int getCoprightBit	(void)	{ return((m_ulBithdr>> 3) & 1);    }
	inline int getOrginalBit		(void)	{ return((m_ulBithdr>> 2) & 1);    }
	inline int getEmphasisIndex	(void)	{ return((m_ulBithdr    ) & 3);    }

	// now come some functions to make life easier once again
	// you don't even have to know what the different indexes
	// mean. to get the version, just use the function
	// getVersion. You can't have it easier

	// this returns the MPEG version [1.0-2.5]
	float getVersion(void);

	// this returns the Layer [1-3]
	int GetLayerNumber(void);

	// this returns the current bitrate [8-448 kbps]
	int getBitrate(void);

	// this returns the current frequency [8000-48000 Hz]
	int getFrequency(void);

	// the purpose of getMode is to get information about
	// the current playing mode, such as:
	// "Joint Stereo"
	const char* getMode(void);

private:
	// this contains the orginal header (bit-by-bit) information
	// declared private because there is not really any reason
	// to use it, as all the "indexes" functions exists
	unsigned long m_ulBithdr;
};

#endif // _CFRAMEHEADER_H
