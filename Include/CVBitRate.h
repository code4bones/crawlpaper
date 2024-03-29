/*
	CVBitRate.h

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
#ifndef _CVBITRATE_H
#define _CVBITRATE_H 1

/*
   in the sdk from xing we can get four different values.
   Allthough this class will only read in one of the values,
   because more isn't needed if you just intend to read
   information from the file and not the play it.
   the information avaliable is:

   frames: this one will be used, so that we can find out
           how long (in seconds) a vbr-file is.

   bytes:  this is to find the number of bytes of .mp3-
           musical information there is in the file,
           and that's pretty useless as we can get that
           anyhow.

   toc:    this is 100 bytes with indexing values to 
           enable repositioning the playing position
           inside an vbr mp3. this is not needed as this
           code is not intended for playing and writing
           to .mp3 files, it's for reading information only

   scale:  would maybe be interesting to someone, but I
           consider it quite useless as I think it's the
           scale which says normal and so on, and in that
           case the bitrate we can get is far more exact.
*/
class CVBitRate
{
public:
	CVBitRate() {}
	virtual ~CVBitRate() {}

	// this function is quite easy to understand, it loads 12 chars of information into the CVBitRate class
	bool loadHeader(char inputheader[12]);

	// this is the only value-retrieving function in this class it returns the Number of Frames [0 -> oo] (oo == eternety)
	inline int GetFrameCount(void) const {return(m_nFrames);}

private:
	// this is the varable holding the number of frames
	int m_nFrames;
};

#endif // _CVBITRATE_H
