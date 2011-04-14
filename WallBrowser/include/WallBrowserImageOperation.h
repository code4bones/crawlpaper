/*
	WallBrowserImageOperation.h
	Classe per le operazioni sull'immagine.
	Luca Piergentili, 12/08/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved
	
	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _CWALLBROWSERIMAGEOPERATION_H
#define _CWALLBROWSERIMAGEOPERATION_H 1

#include "window.h"
#include "CImageFactory.h"
#include "WallBrowserMessages.h"

/*
	CWallBrowserImageOperation
*/
class CWallBrowserImageOperation
{
public:
	CWallBrowserImageOperation() {}
	virtual ~CWallBrowserImageOperation() {}

	inline void SetImage(CImage* pImage) {m_pImage = pImage;}

	UINT		Blur					(void);
	UINT		Bitonal				(void);
	UINT		Brightness			(void);
	UINT		Contrast				(void);
	UINT		Deskew				(void);
	UINT		Despeckle				(void);
	UINT		Dilate				(void);
	UINT		EdgeEnhance			(void);
	UINT		Emboss				(void);
	UINT		Equalize				(void);
	UINT		Erosion				(void);
	UINT		FindEdge				(void);
	UINT		GammaCorrection		(void);
	UINT		Grayscale				(void);
	UINT		Halftone				(void);
	UINT		HistoContrast			(void);
	UINT		Hue					(void);
	UINT		Intensity				(void);
	UINT		IntensityDetect		(void);
	UINT		Invert				(void);
	UINT		Median				(void);
	UINT		MirrorHorizontal		(void);
	UINT		MirrorVertical			(void);
	UINT		Mosaic				(void);
	UINT		Noise				(void);
	UINT		Posterize				(void);
	UINT		Rotate90Left			(void);
	UINT		Rotate90Right			(void);
	UINT		Rotate180				(void);
	UINT		Sharpen				(void);
	UINT		Size					(void);
	UINT		SaturationHorizontal	(void);
	UINT		SaturationVertical		(void);

	// per normalizzare i valori delle librerie
	int		NormalizeRange			(int nValue,int nMin,int nMax);
	double	NormalizeRange			(double nValue,double nMin,double nMax);

	// per normalizzare il valore di default
	int		NormalizeDefaultValue	(int nValue,int nRangeMin,int nRangeMax,int nMin = 0,int nMax = 100);
	double	NormalizeDefaultValue	(double nValue,double nRangeMin,double nRangeMax,double nMin = 0.0,double nMax = 100.0);

private:
	CImage*	m_pImage;
	CImageParams m_ImageParams;
};

#endif  // _CWALLBROWSERIMAGEOPERATION_H 
