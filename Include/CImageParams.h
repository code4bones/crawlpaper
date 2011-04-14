/*
	CImageParams.h
	Classe base per i parametri per la manipolazione dell'immagine.
	Luca Piergentili, 10/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGEPARAMS_H
#define _CIMAGEPARAMS_H 1

#include <string.h>
#include "window.h"

#define FLAG_BLUR				0x00000001
#define FLAG_BRIGHTNESS			0x00000002
#define FLAG_CONTRAST			0x00000004
#define FLAG_DILATE				0x00000008
#define FLAG_EMBOSS				0x00000010
#define FLAG_EMBOSSDIRECTION		0x00000020
#define FLAG_EROSION			0x00000040
#define FLAG_GAMMA				0x00000080
#define FLAG_HALFTONEANGLE		0x00000100
#define FLAG_HALFTONETYPE		0x00000200
#define FLAG_HEIGHT				0x00000400
#define FLAG_HISTOCONTRAST		0x00000800
#define FLAG_HUE				0x00001000
#define FLAG_INTENSITYDETECTMAX	0x00002000
#define FLAG_INTENSITYDETECTMIN	0x00004000
#define FLAG_MEDIAN				0x00008000
#define FLAG_MOSAIC				0x00010000
#define FLAG_NOISECHANNEL		0x00020000
#define FLAG_NOISERANGE			0x00040000
#define FLAG_NOISETYPE			0x00080000
#define FLAG_POSTERIZE			0x00100000
#define FLAG_RADIUS				0x00200000
#define FLAG_SATURATION			0x00400000
#define FLAG_SHARPEN			0x00800000 
#define FLAG_SIZEFILTER			0x01000000
#define FLAG_SIZEQUALITYCONTROL	0x02000000 
#define FLAG_WIDTH				0x04000000

#define DILATION_OMNI					0
#define DILATION_HORZ					1
#define DILATION_VERT					2
#define DILATION_DIAG					3
#define EMBOSS_N						0
#define EMBOSS_NE						1
#define EMBOSS_E						2
#define EMBOSS_SE						3
#define EMBOSS_S						4
#define EMBOSS_SW						5
#define EMBOSS_W						6
#define EMBOSS_NW						7
#define EROSION_OMNI					0
#define EROSION_HORZ					1
#define EROSION_VERT					2
#define EROSION_DIAG					3
#define HT_PRINT						0 
#define HT_VIEW						1
#define NOISE_TYPE_MULTIPLICATIVE_GAUSSIAN	0
#define NOISE_TYPE_IMPULSE				1
#define NOISE_TYPE_LAPLACIAN				2
#define NOISE_TYPE_POISSON				3
#define NOISE_TYPE_UNIFORM				4
#define NOISE_TYPE_GAUSSIAN				5
#define NOISE_TYPE_UNIFORM_FLOAT			6
#define NOISE_TYPE_GAUSSIAN_FLOAT			7
#define NOISE_CHANNEL_MASTER				0			// All channels
#define NOISE_CHANNEL_RED				1			// Red channel only
#define NOISE_CHANNEL_GREEN				2			// Green channel only
#define NOISE_CHANNEL_BLUE				3			// Blue channel only
#define QUANTIZE_FIXEDPALETTE				0x01			// use the fixed palette
#define QUANTIZE_OPTIMIZEDPALETTE			0x02			// create an optimized palette, or if you supply a palette in the pPalette parameter, supply optimized colors for specified entries in the palette.
#define QUANTIZE_NETSCAPEPALETTE			0x40			// use the fixed palette that is employed by Netscape Navigator and by Microsoft Internet Explorer
#define QUANTIZE_USERPALETTE				0x10			// use the palette specified in the pPalette parameter without supplying optimized colors
#define QUANTIZE_IDENTITYPALETTE			0x08			// insert the Windows system palette
#define QUANTIZE_FASTMATCHPALETTE			0x20			// use a predefined table to speed conversion using your own palette
#define QUANTIZE_BYTEORDERBGR				0x04			// use BGR color order. This flag only has meaning when going to 16 bits per pixel or higher.
#define QUANTIZE_BYTEORDERRGB				0x00			// use RGB color. This flag only has meaning when going to 16 bits per
#define QUANTIZE_BYTEORDERGRAY			0x80			// grayscale.Destination bitmap should be 12 or 16-bit grayscale. 12 and 16-bit grayscale images are only supported in the Medical Express edition
#define QUANTIZE_NODITHERING				0x00000000	// Use nearest color matching
#define QUANTIZE_FLOYDSTEINDITHERING		0x00010000	// Use Floyd-Steinberg dithering
#define QUANTIZE_STUCKIDITHERING			0x00020000	// Use Stucki dithering
#define QUANTIZE_BURKESDITHERING			0x00030000	// Use Burkes dithering
#define QUANTIZE_SIERRADITHERING			0x00040000	// Use Sierra dithering
#define QUANTIZE_STEVENSONARCEDITHERING		0x00050000	// Use Stevenson Arce dithering
#define QUANTIZE_JARVISDITHERING			0x00060000	// Use Jarvis dithering
#define QUANTIZE_ORDEREDDITHERING			0x00070000	// Use ordered dithering, which is faster but less accurate than other dithering methods
#define QUANTIZE_CLUSTEREDDITHERING		0x00080000	// Use clustered dithering
#define RESIZE_BOX						0
#define RESIZE_BILINEAR					1
#define RESIZE_GAUSSIAN					2
#define RESIZE_HAMMING					3
#define RESIZE_BLACKMAN					4
#define RESIZE_NORMAL					0
#define RESIZE_FAVORBLACK				1
#define RESIZE_RESAMPLE					2
#define RESIZE_FAVORBLACK_OR_RESAMPLE		3

struct INT_VALUE {
	int		value;
	int		min;
	int		max;
};

struct DOUBLE_VALUE {
	double	value;
	double	min;
	double	max;
};

/*
	CImageParams
*/
class CImageParams
{
public:
	CImageParams() {}
	virtual ~CImageParams() {}

	inline void	SetFlagsColor			(DWORD dwFlagsColor)	{m_dwFlagsColor = dwFlagsColor;}
	inline DWORD	GetFlagsColor			(void)				{return m_dwFlagsColor;}
	inline void	SetFlagsImage			(DWORD dwFlagsImage)	{m_dwFlagsImage = dwFlagsImage;}
	inline DWORD	GetFlagsImage			(void)				{return m_dwFlagsImage;}
	
	inline void	SetBlur				(int nBlur)			{m_nBlur.value = nBlur;}
	inline void	SetBrightness			(double nBrightness)	{m_nBrightness.value = nBrightness;}
	inline void	SetContrast			(int nContrast)		{m_nContrast.value = nContrast;}
	inline void	SetDilate				(int nDilate)			{m_nDilate.value = nDilate;}
	inline void	SetEmboss				(int nEmboss)			{m_nEmboss.value = nEmboss;}
	inline void	SetEmbossDirection		(int nEmbossDirection)	{m_nEmbossDirection.value = nEmbossDirection;}
	inline void	SetErosion			(int nErosion)			{m_nErosion.value = nErosion;}
	inline void	SetFindEdge			(int nFindEdge)		{m_nFindEdge.value = nFindEdge;}
	inline void	SetGamma				(double nGamma)		{m_nGamma.value = nGamma;}
	inline void	SetHalftoneAngle		(int nHalftoneAngle)	{m_nHalftoneAngle.value = nHalftoneAngle;}
	inline void	SetHalftoneType		(int nHalftoneType)		{m_nHalftoneType.value = nHalftoneType;}
	inline void	SetHeight				(int nHeight)			{m_nHeight.value = nHeight;}
	inline void	SetHistoContrast		(int nHistoContrast)	{m_nHistoContrast.value = nHistoContrast;}
	inline void	SetHue				(double nHue)			{m_nHue.value = nHue;}
	inline void	SetIntensityDetectMax	(int nIntensityDetectMax){m_nIntensityDetectMax.value = nIntensityDetectMax;}
	inline void	SetIntensityDetectMin	(int nIntensityDetectMin){m_nIntensityDetectMin.value = nIntensityDetectMin;}
	inline void	SetInterpolation		(int nInterpolation)	{m_nInterpolation.value = nInterpolation;}
	inline void	SetMedian				(int nMedian)			{m_nMedian.value = nMedian;}
	inline void	SetMosaic				(int nMosaic)			{m_nMosaic.value = nMosaic;}
	inline void	SetNoiseChannel		(int nNoiseChannel)		{m_nNoiseChannel.value = nNoiseChannel;}
	inline void	SetNoiseRange			(int nNoiseRange)		{m_nNoiseRange.value = nNoiseRange;}
	inline void	SetNoiseType			(int nNoiseType)		{m_nNoiseType.value = nNoiseType;}
	inline void	SetPosterize			(int nPosterize)		{m_nPosterize.value = nPosterize;}
	inline void	SetRadius				(double nRadius)		{m_nRadius.value = nRadius;}
	inline void	SetSaturation			(double nSaturation)	{m_nSaturation.value = nSaturation;}
	inline void	SetSharpen			(double nSharpen)		{m_nSharpen.value = nSharpen;}
	inline void	SetSizeFilter			(int nSizeFilter)		{m_nSizeFilter.value = nSizeFilter;}
	inline void	SetSizeQualityControl	(int nSizeQualityControl){m_nSizeQualityControl.value = nSizeQualityControl;}
	inline void	SetWidth				(int nWidth)			{m_nWidth.value = nWidth;}

	inline int	GetBlur				(void)				{return m_nBlur.value;}
	inline double	GetBrightness			(void)				{return m_nBrightness.value;}
	inline int	GetContrast			(void)				{return m_nContrast.value;}
	inline int	GetDilate				(void)				{return m_nDilate.value;}	
	inline int	GetEmboss				(void)				{return m_nEmboss.value;}
	inline int	GetEmbossDirection		(void)				{return m_nEmbossDirection.value;}
	inline int	GetErosion			(void)				{return m_nErosion.value;}
	inline int	GetFindEdge			(void)				{return m_nFindEdge.value;}	
	inline double	GetGamma				(void)				{return m_nGamma.value;}
	inline int	GetHeight				(void)				{return m_nHeight.value;}
	inline int	GetHalftoneAngle		(void)				{return m_nHalftoneAngle.value;}
	inline int	GetHalftoneType		(void)				{return m_nHalftoneType.value;}
	inline int	GetHistoContrast		(void)				{return m_nHistoContrast.value;}
	inline double	GetHue				(void)				{return m_nHue.value;}
	inline int	GetIntensityDetectMax	(void)				{return m_nIntensityDetectMax.value;}
	inline int	GetIntensityDetectMin	(void)				{return m_nIntensityDetectMin.value;}
	inline int	GetInterpolation		(void)				{return m_nInterpolation.value;}
	inline int	GetMedian				(void)				{return m_nMedian.value;}
	inline int	GetMosaic				(void)				{return m_nMosaic.value;}
	inline int	GetNoiseChannel		(void)				{return m_nNoiseChannel.value;}
	inline int	GetNoiseType			(void)				{return m_nNoiseType.value;}
	inline int	GetNoiseRange			(void)				{return m_nNoiseRange.value;}
	inline int	GetPosterize			(void)				{return m_nPosterize.value;}
	inline double	GetRadius				(void)				{return m_nRadius.value;}
	inline double	GetSaturation			(void)				{return m_nSaturation.value;}
	inline double	GetSharpen			(void)				{return m_nSharpen.value;}
	inline int	GetSizeFilter			(void)				{return m_nSizeFilter.value;}
	inline int	GetSizeQualityControl	(void)				{return m_nSizeQualityControl.value;}
	inline int	GetWidth				(void)				{return m_nWidth.value;}
	
	inline int&	GetBlurByRef			(void)				{return m_nBlur.value;}
	inline double&	GetBrightnessByRef		(void)				{return m_nBrightness.value;}
	inline int&	GetContrastByRef		(void)				{return m_nContrast.value;}
	inline int&	GetDilateByRef			(void)				{return m_nDilate.value;}	
	inline int&	GetEmbossByRef			(void)				{return m_nEmboss.value;}
	inline int&	GetEmbossDirectionByRef	(void)				{return m_nEmbossDirection.value;}
	inline int&	GetErosionByRef		(void)				{return m_nErosion.value;}
	inline int&	GetFindEdgeByRef		(void)				{return m_nFindEdge.value;}	
	inline double&	GetGammaByRef			(void)				{return m_nGamma.value;}
	inline int&	GetHalftoneAngleByRef	(void)				{return m_nHalftoneAngle.value;}
	inline int&	GetHalftoneTypeByRef	(void)				{return m_nHalftoneType.value;}
	inline int&	GetHeightByRef			(void)				{return m_nHeight.value;}
	inline int&	GetHistoContrastByRef	(void)				{return m_nHistoContrast.value;}
	inline double&	GetHueByRef			(void)				{return m_nHue.value;}
	inline int&	GetIntensityDetectMaxByRef(void)				{return m_nIntensityDetectMax.value;}
	inline int&	GetIntensityDetectMinByRef(void)				{return m_nIntensityDetectMin.value;}
	inline int&	GetInterpolationByRef	(void)				{return m_nInterpolation.value;}
	inline int&	GetMedianByRef			(void)				{return m_nMedian.value;}
	inline int&	GetMosaicByRef			(void)				{return m_nMosaic.value;}
	inline int&	GetNoiseChannelByRef	(void)				{return m_nNoiseChannel.value;}
	inline int&	GetNoiseTypeByRef		(void)				{return m_nNoiseType.value;}
	inline int&	GetNoiseRangeByRef		(void)				{return m_nNoiseRange.value;}
	inline int&	GetPosterizeByRef		(void)				{return m_nPosterize.value;}
	inline double&	GetRadiusByRef			(void)				{return m_nRadius.value;}
	inline double&	GetSaturationByRef		(void)				{return m_nSaturation.value;}
	inline double&	GetSharpenByRef		(void)				{return m_nSharpen.value;}
	inline int&	GetSizeFilterByRef		(void)				{return m_nSizeFilter.value;}
	inline int&	GetSizeQualityControlByRef(void)				{return m_nSizeQualityControl.value;}
	inline int&	GetWidthByRef			(void)				{return m_nWidth.value;}

	void			Reset				(void);

private:
	DWORD		m_dwFlagsColor;
	DWORD		m_dwFlagsImage;
	
	INT_VALUE		m_nBlur;
	DOUBLE_VALUE	m_nBrightness;
	INT_VALUE		m_nContrast;
	INT_VALUE		m_nDilate;
	INT_VALUE		m_nEmboss;
	INT_VALUE		m_nEmbossDirection;
	INT_VALUE		m_nErosion;
	INT_VALUE		m_nFindEdge;
	DOUBLE_VALUE	m_nGamma;
	INT_VALUE		m_nHeight;
	INT_VALUE		m_nHalftoneAngle;
	INT_VALUE		m_nHalftoneType;
	INT_VALUE		m_nHistoContrast;
	DOUBLE_VALUE	m_nHue;
	INT_VALUE		m_nIntensityDetectMax;
	INT_VALUE		m_nIntensityDetectMin;
	INT_VALUE		m_nInterpolation;
	INT_VALUE		m_nMedian;
	INT_VALUE		m_nMosaic;
	INT_VALUE		m_nNoiseChannel;
	INT_VALUE		m_nNoiseRange;
	INT_VALUE		m_nNoiseType;
	INT_VALUE		m_nPosterize;
	DOUBLE_VALUE	m_nRadius;
	DOUBLE_VALUE	m_nSaturation;
	DOUBLE_VALUE	m_nSharpen;
	INT_VALUE		m_nSizeFilter;
	INT_VALUE		m_nSizeQualityControl;
	INT_VALUE		m_nWidth;

public:
	inline void	SetBlurMinMax			(int nMin,int nMax)		{m_nBlur.min = nMin,m_nBlur.max = nMax;}
	inline void	SetBrightnessMinMax		(double nMin,double nMax){m_nBrightness.min = nMin,m_nBrightness.max = nMax;}
	inline void	SetContrastMinMax		(int nMin,int nMax)		{m_nContrast.min = nMin,m_nContrast.max = nMax;}
	inline void	SetDilateMinMax		(int nMin,int nMax)		{m_nDilate.min = nMin,m_nDilate.max = nMax;}
	inline void	SetEmbossMinMax		(int nMin,int nMax)		{m_nEmboss.min = nMin,m_nEmboss.max = nMax;}
	inline void	SetEmbossDirectionMinMax	(int nMin,int nMax)		{m_nEmbossDirection.min = nMin,m_nEmbossDirection.max = nMax;}
	inline void	SetErosionMinMax		(int nMin,int nMax)		{m_nErosion.min = nMin,m_nErosion.max = nMax;}
	inline void	SetFindEdgeMinMax		(int nMin,int nMax)		{m_nFindEdge.min = nMin,m_nFindEdge.max = nMax;}
	inline void	SetGammaMinMax			(double nMin,double nMax){m_nGamma.min = nMin,m_nGamma.max = nMax;}
	inline void	SetHalftoneAngleMinMax	(int nMin,int nMax)		{m_nHalftoneAngle.min = nMin,m_nHalftoneAngle.max = nMax;}
	inline void	SetHalftoneTypeMinMax	(int nMin,int nMax)		{m_nHalftoneType.min = nMin,m_nHalftoneType.max = nMax;}
	inline void	SetHeightMinMax		(int nMin,int nMax)		{m_nHeight.min = nMin,m_nHeight.max = nMax;}
	inline void	SetHistoContrastMinMax	(int nMin,int nMax)		{m_nHistoContrast.min = nMin,m_nHistoContrast.max = nMax;}
	inline void	SetHueMinMax			(double nMin,double nMax){m_nHue.min = nMin,m_nHue.max = nMax;}
	inline void	SetIntensityDetectMin_MinMax(int nMin,int nMax)	{m_nIntensityDetectMin.min = nMin,m_nIntensityDetectMin.max = nMax;}
	inline void	SetIntensityDetectMax_MinMax(int nMin,int nMax)	{m_nIntensityDetectMax.min = nMin,m_nIntensityDetectMax.max = nMax;}
	inline void	SetInterpolationMinMax	(int nMin,int nMax)		{m_nInterpolation.min = nMin,m_nInterpolation.max = nMax;}
	inline void	SetMedianMinMax		(int nMin,int nMax)		{m_nMedian.min = nMin,m_nMedian.max = nMax;}
	inline void	SetMosaicMinMax		(int nMin,int nMax)		{m_nMosaic.min = nMin,m_nMosaic.max = nMax;}
	inline void	SetNoiseChannelMinMax	(int nMin,int nMax)		{m_nNoiseChannel.min = nMin,m_nNoiseChannel.max = nMax;}
	inline void	SetNoiseRangeMinMax		(int nMin,int nMax)		{m_nNoiseRange.min = nMin,m_nNoiseRange.max = nMax;}
	inline void	SetNoiseTypeMinMax		(int nMin,int nMax)		{m_nNoiseType.min = nMin,m_nNoiseType.max = nMax;}
	inline void	SetPosterizeMinMax		(int nMin,int nMax)		{m_nPosterize.min = nMin,m_nPosterize.max = nMax;}
	inline void	SetRadiusMinMax		(double nMin,double nMax){m_nRadius.min = nMin,m_nRadius.max = nMax;}
	inline void	SetSaturationMinMax		(double nMin,double nMax){m_nSaturation.min = nMin,m_nSaturation.max = nMax;}
	inline void	SetSharpenMinMax		(double nMin,double nMax){m_nSharpen.min = nMin,m_nSharpen.max = nMax;}
	inline void	SetSizeFilterMinMax		(int nMin,int nMax)		{m_nSizeFilter.min = nMin,m_nSizeFilter.max = nMax;}
	inline void	SetSizeQualityControlMinMax(int nMin,int nMax)	{m_nSizeQualityControl.min = nMin,m_nSizeQualityControl.max = nMax;}
	inline void	SetWidthMinMax			(int nMin,int nMax)		{m_nWidth.min = nMin,m_nWidth.max = nMax;}

	inline int	GetBlurMin			(void)				{return m_nBlur.min;}
	inline int	GetBlurMax			(void)				{return m_nBlur.max;}
	inline double	GetBrightnessMin		(void)				{return m_nBrightness.min;}
	inline double	GetBrightnessMax		(void)				{return m_nBrightness.max;}
	inline int	GetContrastMin			(void)				{return m_nContrast.min;}
	inline int	GetContrastMax			(void)				{return m_nContrast.max;}
	inline int	GetDilateMin			(void)				{return m_nDilate.min;}
	inline int	GetDilateMax			(void)				{return m_nDilate.max;}
	inline int	GetEmbossMin			(void)				{return m_nEmboss.min;}
	inline int	GetEmbossMax			(void)				{return m_nEmboss.max;}
	inline int	GetEmbossDirectionMin	(void)				{return m_nEmbossDirection.min;}
	inline int	GetEmbossDirectionMax	(void)				{return m_nEmbossDirection.max;}
	inline int	GetErosionMin			(void)				{return m_nErosion.min;}
	inline int	GetErosionMax			(void)				{return m_nErosion.max;}
	inline int	GetFindEdgeMin			(void)				{return m_nFindEdge.min;}
	inline int	GetFindEdgeMax			(void)				{return m_nFindEdge.max;}
	inline double	GetGammaMin			(void)				{return m_nGamma.min;}
	inline double	GetGammaMax			(void)				{return m_nGamma.max;}
	inline int	GetHeightMin			(void)				{return m_nHeight.min;}
	inline int	GetHeightMax			(void)				{return m_nHeight.max;}
	inline int	GetHalftoneAngleMin		(void)				{return m_nHalftoneAngle.min;}
	inline int	GetHalftoneAngleMax		(void)				{return m_nHalftoneAngle.max;}
	inline int	GetHalftoneTypeMin		(void)				{return m_nHalftoneType.min;}
	inline int	GetHalftoneTypeMax		(void)				{return m_nHalftoneType.max;}
	inline int	GetHistoContrastMin		(void)				{return m_nHistoContrast.min;}
	inline int	GetHistoContrastMax		(void)				{return m_nHistoContrast.max;}
	inline double	GetHueMin				(void)				{return m_nHue.min;}
	inline double	GetHueMax				(void)				{return m_nHue.max;}
	inline int	GetIntensityDetectMin_Min(void)				{return m_nIntensityDetectMin.min;}
	inline int	GetIntensityDetectMin_Max(void)				{return m_nIntensityDetectMin.max;}
	inline int	GetIntensityDetectMax_Min(void)				{return m_nIntensityDetectMax.min;}
	inline int	GetIntensityDetectMax_Max(void)				{return m_nIntensityDetectMax.max;}
	inline int	GetInterpolationMin		(void)				{return m_nInterpolation.min;}
	inline int	GetInterpolationMax		(void)				{return m_nInterpolation.max;}
	inline int	GetMedianMin			(void)				{return m_nMedian.min;}
	inline int	GetMedianMax			(void)				{return m_nMedian.max;}
	inline int	GetMosaicMin			(void)				{return m_nMosaic.min;}
	inline int	GetMosaicMax			(void)				{return m_nMosaic.max;}
	inline int	GetNoiseChannelMin		(void)				{return m_nNoiseChannel.min;}
	inline int	GetNoiseChannelMax		(void)				{return m_nNoiseChannel.max;}
	inline int	GetNoiseRangeMin		(void)				{return m_nNoiseRange.min;}
	inline int	GetNoiseRangeMax		(void)				{return m_nNoiseRange.max;}
	inline int	GetNoiseTypeMin		(void)				{return m_nNoiseType.min;}
	inline int	GetNoiseTypeMax		(void)				{return m_nNoiseType.max;}
	inline int	GetPosterizeMin		(void)				{return m_nPosterize.min;}
	inline int	GetPosterizeMax		(void)				{return m_nPosterize.max;}
	inline double	GetRadiusMin			(void)				{return m_nRadius.min;}
	inline double	GetRadiusMax			(void)				{return m_nRadius.max;}
	inline double	GetSaturationMin		(void)				{return m_nSaturation.min;}
	inline double	GetSaturationMax		(void)				{return m_nSaturation.max;}
	inline double	GetSharpenMin			(void)				{return m_nSharpen.min;}
	inline double	GetSharpenMax			(void)				{return m_nSharpen.max;}
	inline int	GetSizeFilterMin		(void)				{return m_nSizeFilter.min;}
	inline int	GetSizeFilterMax		(void)				{return m_nSizeFilter.max;}
	inline int	GetSizeQualityControlMin	(void)				{return m_nSizeQualityControl.min;}
	inline int	GetSizeQualityControlMax	(void)				{return m_nSizeQualityControl.max;}
	inline int	GetWidthMin			(void)				{return m_nWidth.min;}
	inline int	GetWidthMax			(void)				{return m_nWidth.max;}
};

#endif // _CIMAGEPARAMS_H
