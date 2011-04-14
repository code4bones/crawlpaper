/*
	WallBrowserImageOperation.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CImageFactory.h"
#include "WallBrowserImageOperation.h"
#include "WallBrowserOneParamDlg.h"
#include "WallBrowserTwoParamsDlg.h"
#include "WallBrowserSizeDlg.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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

/*
	Blur() 
*/
UINT CWallBrowserImageOperation::Blur(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	// la libreria non prevede parametri
	if(m_ImageParams.GetBlurMin()==0 && m_ImageParams.GetBlurMax()==0) 
	{
		nRet = m_pImage->Blur(&m_ImageParams);
	}
	else
	{
		int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetBlur(),m_ImageParams.GetBlurMin(),m_ImageParams.GetBlurMax());
	
		CWallBrowserOneParamDlg dlg(" Image Properties "," Blur ",nDefaultValue);
		if(dlg.DoModal()==IDOK)
		{			
			int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetBlurMin(),m_ImageParams.GetBlurMax());
			m_ImageParams.SetBlur(nValue);
			nRet = m_pImage->Blur(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	Bitonal()
*/
UINT CWallBrowserImageOperation::Bitonal(void)
{
	UINT nRet = GDI_ERROR;

//	m_ImageParams.Reset();
//	nRet = m_pImage->Bitonal(&m_ImageParams);

	return(nRet);
}

/*
	Brightness()
*/
UINT CWallBrowserImageOperation::Brightness(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	// la libreria non prevede parametri
	if(m_ImageParams.GetBrightnessMin()==0.0 && m_ImageParams.GetBrightnessMax()==0.0) 
	{
		nRet = m_pImage->Brightness(&m_ImageParams);
	}
	else
	{
		double nDefaultValue = NormalizeDefaultValue((double)m_ImageParams.GetBrightness(),m_ImageParams.GetBrightnessMin(),m_ImageParams.GetBrightnessMax());
		
		CWallBrowserOneParamDlg dlg(" Color Properties "," Brightness ",nDefaultValue);
		if(dlg.DoModal()==IDOK)
		{			
			double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetBrightnessMin(),m_ImageParams.GetBrightnessMax());
			m_ImageParams.SetBrightness(nValue);
			nRet = m_pImage->Brightness(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	Contrast()
*/
UINT CWallBrowserImageOperation::Contrast(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	// la libreria non prevede parametri
	if(m_ImageParams.GetContrastMin()==0 && m_ImageParams.GetContrastMax()==0)
	{
		nRet = m_pImage->Contrast(&m_ImageParams);
	}
	else
	{
		int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetContrast(),m_ImageParams.GetContrastMin(),m_ImageParams.GetContrastMax());

		CWallBrowserOneParamDlg dlg(" Color Properties "," Contrast ",nDefaultValue);
		if(dlg.DoModal()==IDOK)
		{
			int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetContrastMin(),m_ImageParams.GetContrastMax());
			m_ImageParams.SetContrast(nValue);
			nRet = m_pImage->Contrast(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	Deskew()
*/
UINT CWallBrowserImageOperation::Deskew(void)
{
	// radrizza
	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);
	return(m_pImage->Deskew(&m_ImageParams));
}

/*
	Despeckle()
*/
UINT CWallBrowserImageOperation::Despeckle(void)
{
	// elimina punti
	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);
	return(m_pImage->Despeckle(&m_ImageParams));
}

/*
	Dilate()
*/
UINT CWallBrowserImageOperation::Dilate(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	// la libreria non prevede parametri
	if(m_ImageParams.GetDilateMin()==0 && m_ImageParams.GetDilateMax()==0)
	{
		nRet = m_pImage->Dilate(&m_ImageParams);
	}
	else
	{
		int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetDilate(),m_ImageParams.GetDilateMin(),m_ImageParams.GetDilateMax());

		CWallBrowserOneParamDlg dlg(" Image Properties "," Dilate ",nDefaultValue);
		if(dlg.DoModal()==IDOK)
		{			
			int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetDilateMin(),m_ImageParams.GetDilateMax());
			m_ImageParams.SetDilate(nValue);
			nRet = m_pImage->Dilate(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	EdgeEnhance()
*/
UINT CWallBrowserImageOperation::EdgeEnhance(void)
{
	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);
	return(m_pImage->EdgeEnhance(&m_ImageParams));
}

/*
	Emboss()
*/
UINT CWallBrowserImageOperation::Emboss(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	// la libreria non prevede parametri
	if((m_ImageParams.GetEmbossMin()==0 && m_ImageParams.GetEmbossMax()==0) && (m_ImageParams.GetEmbossDirectionMin()==0 && m_ImageParams.GetEmbossDirectionMax()==0))
	{
		nRet = m_pImage->Emboss(&m_ImageParams);
	}
	else
	{		
		int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetEmboss(),m_ImageParams.GetEmbossMin(),m_ImageParams.GetEmbossMax());
	
		if(m_ImageParams.GetEmbossDirectionMin()==0 && m_ImageParams.GetEmbossDirectionMax()==0)
		{			
			CWallBrowserOneParamDlg dlg(" Image Properties "," Emboss ",nDefaultValue);
			if(dlg.DoModal()==IDOK)
			{			
				int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetEmbossMin(),m_ImageParams.GetEmbossMax());
				m_ImageParams.SetEmboss(nValue);
				nRet = m_pImage->Emboss(&m_ImageParams);
			}	
			else
				nRet = IDCANCEL;
		}
		else
		{
			CWallBrowserTwoParamsDlg dlg(" Image Property ","Emboss","Emboss",nDefaultValue,0,100,m_ImageParams.GetEmbossDirection(),m_ImageParams.GetEmbossDirectionMin(),m_ImageParams.GetEmbossDirectionMax());
			if(dlg.DoModal()==IDOK)
			{			
				int nValue = NormalizeRange(dlg.GetFirstValue(),m_ImageParams.GetEmbossMin(),m_ImageParams.GetEmbossMax());
				m_ImageParams.SetEmboss(nValue);
				m_ImageParams.SetEmbossDirection(dlg.GetSecondValue());
				nRet = m_pImage->Emboss(&m_ImageParams);
			}
			else
				nRet = IDCANCEL;
		}
	}

	return(nRet);
}

/*
	Equalize()
*/
UINT CWallBrowserImageOperation::Equalize(void)
{
	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	
	return(m_pImage->Equalize(&m_ImageParams));
}

/*
	Erosion()
*/
UINT CWallBrowserImageOperation::Erosion()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	// la libreria non prevede parametri
	if(m_ImageParams.GetErosionMin()==0 && m_ImageParams.GetErosionMax()==0)
	{
		nRet = m_pImage->Erosion(&m_ImageParams);
	}
	else
	{
		CWallBrowserOneParamDlg dlg(" Image Properties "," Erosion ",m_ImageParams.GetErosion(),m_ImageParams.GetErosionMin(),m_ImageParams.GetErosionMax());
		if(dlg.DoModal()==IDOK)
		{			
			int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetErosionMin(),m_ImageParams.GetErosionMax());
			m_ImageParams.SetErosion(nValue);
			nRet = m_pImage->Erosion(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	FindEdge()
*/
UINT CWallBrowserImageOperation::FindEdge()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	CWallBrowserOneParamDlg dlg(" Image Properties "," FindEdge ",m_ImageParams.GetFindEdge(),m_ImageParams.GetFindEdgeMin(),m_ImageParams.GetFindEdgeMax());
	if(dlg.DoModal()==IDOK)
	{			
		int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetFindEdgeMin(),m_ImageParams.GetFindEdgeMax());
		m_ImageParams.SetFindEdge(nValue);
		nRet = m_pImage->FindEdge(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	GammaCorrection() 
*/
UINT CWallBrowserImageOperation::GammaCorrection(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	double nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetGamma(),m_ImageParams.GetGammaMin(),m_ImageParams.GetGammaMax());
	
	CWallBrowserOneParamDlg dlg(" Color Properties "," Gamma Correction ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetGammaMin(),m_ImageParams.GetGammaMax());
		m_ImageParams.SetGamma(nValue);
		nRet = m_pImage->GammaCorrection(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Grayscale()
*/
UINT CWallBrowserImageOperation::Grayscale(void)
{
	return(m_pImage->Grayscale(&m_ImageParams));
}

/* 
	Halftone()
*/
UINT CWallBrowserImageOperation::Halftone(void) 
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);		

	// la libreria non prevede parametri
	if((m_ImageParams.GetHalftoneAngleMin()==0 && m_ImageParams.GetHalftoneAngleMax()==0) && (m_ImageParams.GetHalftoneTypeMin()==0 && m_ImageParams.GetHalftoneTypeMax()==0))
	{
		nRet = m_pImage->Halftone(&m_ImageParams);
	}
	else
	{	
		CWallBrowserTwoParamsDlg dlg(" Color Property ","Halftone","Halftone",m_ImageParams.GetHalftoneAngle(),m_ImageParams.GetHalftoneAngleMin(),m_ImageParams.GetHalftoneAngleMax(),m_ImageParams.GetHalftoneType(),m_ImageParams.GetHalftoneTypeMin(),m_ImageParams.GetHalftoneTypeMax());
		if(dlg.DoModal()==IDOK)
		{			
			m_ImageParams.SetHalftoneAngle(dlg.GetFirstValue());
			m_ImageParams.SetHalftoneType(dlg.GetSecondValue());
			nRet = m_pImage->Halftone(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	HistoContrast()
*/
UINT CWallBrowserImageOperation::HistoContrast()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetHistoContrast(),m_ImageParams.GetHistoContrastMin(),m_ImageParams.GetHistoContrastMax());
	
	CWallBrowserOneParamDlg dlg(" Color Properties "," Histogram Contrast ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetHistoContrastMin(),m_ImageParams.GetHistoContrastMax());
		m_ImageParams.SetHistoContrast(nValue);
		nRet = m_pImage->HistoContrast(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Hue()
*/
UINT CWallBrowserImageOperation::Hue()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	double nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetHue(),m_ImageParams.GetHueMin(),m_ImageParams.GetHueMax());
	
	CWallBrowserOneParamDlg dlg(" Color Properties "," Hue ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetHueMin(),m_ImageParams.GetHueMax());
		m_ImageParams.SetHue(nValue);
		nRet = m_pImage->Hue(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Intensity()
*/
UINT CWallBrowserImageOperation::Intensity()
{
	return(m_pImage->Intensity(&m_ImageParams));
}

/*
	IntensityDetect() 
*/
UINT CWallBrowserImageOperation::IntensityDetect()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	CWallBrowserTwoParamsDlg dlg(" Color Property ","Min. Intensity","Max. Intensity",m_ImageParams.GetIntensityDetectMin(),m_ImageParams.GetIntensityDetectMin_Min(),m_ImageParams.GetIntensityDetectMin_Max(),m_ImageParams.GetIntensityDetectMax(),m_ImageParams.GetIntensityDetectMax_Min(),m_ImageParams.GetIntensityDetectMax_Max());
	if(dlg.DoModal()==IDOK)
	{				
		int IntensityDetectMin = dlg.GetFirstValue();
		int IntensityDetectMax = dlg.GetSecondValue();
		if(IntensityDetectMax < IntensityDetectMin)
		{ 
			int aux = IntensityDetectMax;
			IntensityDetectMax = IntensityDetectMin;
			IntensityDetectMin = aux; 
		}
		m_ImageParams.SetIntensityDetectMin(IntensityDetectMin);
		m_ImageParams.SetIntensityDetectMax(IntensityDetectMax);
		nRet = m_pImage->IntensityDetect(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Invert()
*/
UINT CWallBrowserImageOperation::Invert()
{
	return(m_pImage->Invert(&m_ImageParams));
}

/*
	Median()
*/
UINT CWallBrowserImageOperation::Median()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	int nDefaultValue= NormalizeDefaultValue(m_ImageParams.GetMedian(),m_ImageParams.GetMedianMin(),m_ImageParams.GetMedianMax());

	CWallBrowserOneParamDlg dlg(" Image Properties "," Median ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetMedianMin(),m_ImageParams.GetMedianMax());
		m_ImageParams.SetMedian(nValue);
		nRet = m_pImage->Median(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	MirrorHorizontal()
*/
UINT CWallBrowserImageOperation::MirrorHorizontal()
{
	return(m_pImage->MirrorHorizontal(&m_ImageParams));
}

/*
	MirrorVertical()
*/
UINT CWallBrowserImageOperation::MirrorVertical()
{
	return(m_pImage->MirrorVertical(&m_ImageParams));
}

/*
	Mosaic()
*/
UINT CWallBrowserImageOperation::Mosaic()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetMosaic(),m_ImageParams.GetMosaicMin(),m_ImageParams.GetMosaicMax());

	CWallBrowserOneParamDlg dlg(" Image Properties "," Mosaic ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetMosaicMin(),m_ImageParams.GetMosaicMax());
		m_ImageParams.SetMosaic(nValue);
		nRet = m_pImage->Mosaic(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Noise()
*/
UINT CWallBrowserImageOperation::Noise()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	if(m_ImageParams.GetNoiseChannelMin()==0 && m_ImageParams.GetNoiseChannelMax()==0 && m_ImageParams.GetNoiseTypeMax()!=0)
	{
		CWallBrowserOneParamDlg dlg(" Image Properties "," Noise ",m_ImageParams.GetNoiseType(),m_ImageParams.GetNoiseTypeMin(),m_ImageParams.GetNoiseTypeMax());
		if(dlg.DoModal()==IDOK)
		{			
			m_ImageParams.SetNoiseType(dlg.GetIntValue());
			nRet = m_pImage->Noise(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}
	else if(m_ImageParams.GetNoiseTypeMin()==0 && m_ImageParams.GetNoiseTypeMax()==0)
	{
		int nMin = m_ImageParams.GetNoiseRangeMin(); 
		int nMax = m_ImageParams.GetNoiseRangeMax();
		int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetNoiseRange(),nMin,nMax);

		CWallBrowserTwoParamsDlg dlg(" Image Property ","Noise","Noise",nDefaultValue,0,100,m_ImageParams.GetNoiseChannel(),m_ImageParams.GetNoiseChannelMin(),m_ImageParams.GetNoiseChannelMax());
		if(dlg.DoModal()==IDOK)
		{			
			int nValue = NormalizeRange(dlg.GetFirstValue(),nMin,nMax);
			m_ImageParams.SetNoiseRange(nValue);
			m_ImageParams.SetNoiseChannel(dlg.GetSecondValue());
			nRet = m_pImage->Noise(&m_ImageParams);
		}
		else
			nRet = IDCANCEL;
	}

	return(nRet);
}

/*
	Posterize()
*/
UINT CWallBrowserImageOperation::Posterize()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	int nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetPosterize(),m_ImageParams.GetPosterizeMin(),m_ImageParams.GetPosterizeMax());

	CWallBrowserOneParamDlg dlg(" Image Properties "," Posterize ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		int nValue = NormalizeRange(dlg.GetIntValue(),m_ImageParams.GetPosterizeMin(),m_ImageParams.GetPosterizeMax());
		m_ImageParams.SetPosterize(nValue);
		nRet = m_pImage->Posterize(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Rotate90Left()
*/
UINT CWallBrowserImageOperation::Rotate90Left()
{
	return(m_pImage->Rotate90Left(&m_ImageParams));
}

/*
	Rotate90Right()
*/
UINT CWallBrowserImageOperation::Rotate90Right()
{
	return(m_pImage->Rotate90Right(&m_ImageParams));
}

/*
	Rotate180()
*/
UINT CWallBrowserImageOperation::Rotate180()
{
	return(m_pImage->Rotate180(&m_ImageParams));
}

/*
	Sharpen()
*/
UINT CWallBrowserImageOperation::Sharpen()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	double nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetSharpen(),m_ImageParams.GetSharpenMin(),m_ImageParams.GetSharpenMax());

	CWallBrowserOneParamDlg dlg(" Image Properties "," Sharpen ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetSharpenMin(),m_ImageParams.GetSharpenMax());
		m_ImageParams.SetSharpen(nValue);
		nRet = m_pImage->Sharpen(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	Size()
*/
UINT CWallBrowserImageOperation::Size()
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_ImageParams.SetFlagsImage(0);
	m_ImageParams.SetFlagsColor(0);
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);	

	if(m_ImageParams.GetSizeFilterMin()!=m_ImageParams.GetSizeFilterMax())
	{
		if(m_ImageParams.GetRadiusMin()!=m_ImageParams.GetRadiusMax())
			m_ImageParams.SetFlagsImage(FLAG_SIZEFILTER|FLAG_RADIUS); //Paintlib
		else
			m_ImageParams.SetFlagsImage(FLAG_SIZEFILTER); // Nexgen
	}
	else
	{
		if(m_ImageParams.GetSizeQualityControlMin()!=m_ImageParams.GetSizeQualityControlMax())
			m_ImageParams.SetFlagsImage(FLAG_SIZEQUALITYCONTROL);
	}
	
	CWallBrowserSizeDlg dlg(m_pImage,&m_ImageParams);
	if(dlg.DoModal()==IDOK)
		nRet = m_pImage->Size(&m_ImageParams);	
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	SaturationHorizontal()
*/
UINT CWallBrowserImageOperation::SaturationHorizontal(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	double nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetSaturation(),m_ImageParams.GetSaturationMin(),m_ImageParams.GetSaturationMax());
	
	CWallBrowserOneParamDlg dlg(" Color Properties "," Horizontal Saturation ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetSaturationMin(),m_ImageParams.GetSaturationMax());
		m_ImageParams.SetSaturation(nValue);
		nRet = m_pImage->SaturationHorizontal(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	SaturationVertical() 
*/
UINT CWallBrowserImageOperation::SaturationVertical(void)
{
	UINT nRet = GDI_ERROR;

	m_ImageParams.Reset();
	m_pImage->SetImageParamsMinMax(&m_ImageParams);	
	m_pImage->SetImageParamsDefaultValues(&m_ImageParams);

	double nDefaultValue = NormalizeDefaultValue(m_ImageParams.GetSaturation(),m_ImageParams.GetSaturationMin(),m_ImageParams.GetSaturationMax());
	
	CWallBrowserOneParamDlg dlg(" Color Properties "," Vertical Saturation ",nDefaultValue);
	if(dlg.DoModal()==IDOK)
	{			
		double nValue = NormalizeRange(dlg.GetDoubleValue(),m_ImageParams.GetSaturationMin(),m_ImageParams.GetSaturationMax());
		m_ImageParams.SetSaturation(nValue);
		nRet = m_pImage->SaturationVertical(&m_ImageParams);
	}
	else
		nRet = IDCANCEL;

	return(nRet);
}

/*
	NormalizeRange()

	Adatta il valore restituito dal dialogo (con un intervallo da 0 a 100) al range previsto dalla libreria.
	Restituisce il valore adattato all'intervallo della libreria.
*/
int CWallBrowserImageOperation::NormalizeRange(int nValue,int nLibraryMin,int nLibraryMax)
{
	if(nValue==0)
		return(nLibraryMin);
	else if(nValue==100)
		return(nLibraryMax);

	if(nLibraryMin!=0 || nLibraryMax!=0)
	{
		int nRange = nLibraryMax - nLibraryMin;
		int nAdaptedValue = (nRange * nValue) / 100;
		nAdaptedValue += nLibraryMin;
		return(nAdaptedValue);
	}
	else
		return(0);
}
double CWallBrowserImageOperation::NormalizeRange(double nValue,double nLibraryMin,double nLibraryMax)
{
	if(nValue==0.0)
		return(nLibraryMin);
	else if(nValue==100.0)
		return(nLibraryMax);

	if(nLibraryMin!=0.0 || nLibraryMax!=0.0)
	{
		double nRange = nLibraryMax - nLibraryMin;
		double nAdaptedValue = (nRange * nValue) / 100.0;
		nAdaptedValue += nLibraryMin;
		return(nAdaptedValue);
	}
	else
		return(0.0);
}

/*
	NormalizeDefaultValue()

	Adatta il valore di default della libreria all'intervallo previsto dal dialogo (0/100).
	Restituisce il valore adattato all'intervallo.
*/
int CWallBrowserImageOperation::NormalizeDefaultValue(int nValue,int nLibraryMin,int nLibraryMax,int nMin/*=0*/,int nMax/*=100*/)
{
	if(nLibraryMin!=0 || nLibraryMax!=0)
	{
		int nRange = nLibraryMax - nLibraryMin;
		int nNormalizedValue = (int)((nMax * (nValue - nLibraryMin)) / nRange);
		nNormalizedValue += nMin;
		return(nNormalizedValue);
	}
	else
		return(0);
}
double CWallBrowserImageOperation::NormalizeDefaultValue(double nValue,double nLibraryMin,double nLibraryMax,double nMin/*=0.0*/,double nMax/*=100.0*/)
{
	if(nLibraryMin!=0.0 || nLibraryMax!=0.0)
	{
		double nRange = nLibraryMax - nLibraryMin;
		double nNormalizedValue = (nMax * (nValue - nLibraryMin)) / nRange;
		nNormalizedValue += nMin;
		return(nNormalizedValue);
	}
	else
		return(0.0);
}
