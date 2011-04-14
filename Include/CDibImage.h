#if 0
/*
	Module : DIBIMAGE.H
	Purpose: Defines the interface to an MFC class that encapsulates DIBs
		    and supports a variety of image manipulation functions on it
	Created: PJN / 23-07-1997

	Copyright (c) 1997 - 2000 by PJ Naughter.  
	All rights reserved.

	Modifiche minori.
	Luca Piergentili, 08/09/00
	lpiergentili@yahoo.com
*/

////////////////////////////////// Macros ///////////////////////////
#ifndef __DIBIMAGE_H__
#define __DIBIMAGE_H__

////////////////////////////////// Includes /////////////////////////
//LPI
#include "window.h"
#include <afxtempl.h>
#define IDS_DI_UNDO_SETCOLOR			0
#define IDS_DI_UNDO_FLIP				1
#define IDS_DI_UNDO_MIRROR			2
#define IDS_DI_UNDO_ADJUST_BRIGHTNESS	3
#define IDS_DI_UNDO_CONTRAST			4
#define IDS_DI_UNDO_GAMMA			5
#define IDS_DI_UNDO_HIGHLIGHT			6
#define IDS_DI_UNDO_MIDTONE			7
#define IDS_DI_UNDO_SHADOW			8
#define IDS_DI_UNDO_ADJUST_HUE		9
#define IDS_DI_UNDO_ADJUST_SATURATION	10
#define IDS_DI_UNDO_ADJUST_HSL		11
#define IDS_DI_UNDO_ADJUST_RED		12
#define IDS_DI_UNDO_ADJUST_GREEN		13
#define IDS_DI_UNDO_ADJUST_BLUE		14
#define IDS_DI_UNDO_ADJUST_GREYSCALE	15
#define IDS_DI_UNDO_ADJUST_NEGATE		16
#define IDS_DI_UNDO_FILTER			17
#define IDS_DI_UNDO_ADD_NOISE			18
#define IDS_DI_UNDO_CONVERT24BITS		19
//LPI

#include "Dibapi.h"   // The "C/SDK" style functions provided by MS to 
                      // handle DIBs which are used internally by CDibImage

/////////////////////////// Classes /////////////////////////////////

//forward declaration
class CDibImage;

//Implements a generic selection object (pure virtual)
class CWorkingArea
{
public:
  virtual BOOL PointInSelection(const CPoint& point) = 0;
  virtual CRect BoundingRectangle() = 0;
  virtual CWorkingArea* Clone() = 0;
};

//A rectangular selection
class CRectWorkingArea : public CWorkingArea
{
public:
  CRectWorkingArea(CRect rect) { m_Rect = rect; };

  virtual BOOL PointInSelection(const CPoint& point) { return m_Rect.PtInRect(point); };
  virtual CRect BoundingRectangle() { return m_Rect; };
  virtual CWorkingArea* Clone() { return new CRectWorkingArea(m_Rect); };

protected:
  CRect m_Rect;
};

//A filter which can be used in call to CDibImage::UserDefinedFilter(..)
class CUserDefinedFilter
{
public:
  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)=0;
};

//a concrete implementation of a user defined filter
class C3By3Filter : public CUserDefinedFilter
{
public:
  C3By3Filter();
  int m_nValues[3][3];
  int m_nDivision;
  int m_nBias;

  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};

//a concrete implementation of a user defined filter
class C5By5Filter : public CUserDefinedFilter
{
public:
  C5By5Filter();
  int m_nValues[5][5];
  int m_nDivision;
  int m_nBias;

  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};

//a concrete implementation of a user defined filter
class C3By3MedianFilter : public CUserDefinedFilter
{
public:
  C3By3MedianFilter();
  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
   
protected:
  static int CompareFunc(const void *elem1, const void *elem2);

  COLORREF m_Ordered[9];
};

//a concrete implementation of a user defined filter
class C3By3MeanFilter : public CUserDefinedFilter
{
public:
  C3By3MeanFilter();
  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};

//a concrete implementation of a user defined filter
class C7By7Filter : public CUserDefinedFilter
{
public:
  C7By7Filter();
  int m_nValues[7][7];
  int m_nDivision;
  int m_nBias;

  virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};

//Class to hold an undo state
class CUndoNode                                            
{
public:
  CUndoNode(CDibImage* pImage, const CString& sDescription);
  ~CUndoNode();

  CString GetDescription() const { return m_sDescription; };
  CDibImage* GetImage() const { return m_pImage; };

  void SetDescription(const CString& sDescription) { m_sDescription = sDescription; };

protected:
  CDibImage* m_pImage;
  CString m_sDescription;
};

//The DIB class itself
class CDibImage
{
public:
  //Creation & Destruction
  CDibImage();
  ~CDibImage();
  BOOL Attach(HDIB hGlobal);
  HDIB Detach(void);
  void Destroy();
  BOOL Create(CSize size, WORD nBitCount);
  void DestroyWorkingArea();
  void DestroyHDIB();
  void DestroyHPALLETTE();

  //Static functions
  static WORD     GetVersion();
  static void     RGBtoHSL(COLORREF rgb, double* H, double* S, double* L);
  static COLORREF HLStoRGB(const double& H, const double& L, const double& S);

  //Loading & Saving (File & Resource)
  BOOL Load(LPCTSTR lpszPathName);
  BOOL Load(HINSTANCE hInst, LPCTSTR lpResourceName);
  BOOL Load(HINSTANCE hInst, UINT uID) { return Load(hInst, MAKEINTRESOURCE(uID)); };
  BOOL Save(LPCTSTR lpszPathName);

  //Clipboard / Screen Capture support
  BOOL CopyToClipboard();
  BOOL PasteFromClipboard();
  static BOOL PasteAvailable();
  BOOL CopyFromBitmap(HBITMAP hBitmap, HPALETTE hPal);
  HBITMAP CopyToBitmap(void);
  BOOL CopyFromMemBmpFile(HMEMBMPFILE hMemBmpFile);
  HMEMBMPFILE CopyToMemBmpFile(void);
  HDIB /*BOOL*/ CopyFromWindow(CWnd *pWnd, CRect* pScreenRect = NULL);

  //Copying
  CDibImage(const CDibImage& ds);
  CDibImage& operator=(const CDibImage& ds);
  BOOL CopySelection(CDibImage& dib);

  //Multi level Undo / Redo support
  void SetUndoSize(int nUndoSize);
  int  UndoSize() const { return m_nUndoSize; };
  BOOL SaveState(const CString& sDescription);
  BOOL SaveState(UINT nID);
  BOOL UndoAvailable();
  BOOL Undo();
  BOOL Redo();
  BOOL RedoAvailable();
  CString UndoDescription() const;
  CString RedoDescription() const;

  //Selection / Working Area support
  CWorkingArea* GetWorkingArea();
  void  SetWorkingArea(CWorkingArea* pWorkingArea);

  //Misc functions
  int ColorsUsed() const;
  LPSTR GetDIBBits();
  BOOL DataFormatSupported(void) const;
  BOOL FilteringFormatSupported(void) const;
  BOOL ConvertTo24Bits(void);
  BOOL IsRunLengthEncoded() const;

  //Area Image processing support
  BOOL SetColor(COLORREF color);
  BOOL Flip();
  BOOL Mirror();

  //Color Image Processing support
  BOOL AdjustBrightness(int Percentage);
  BOOL AdjustContrast(int Percentage);
  BOOL AdjustGammaCorrection(float Value);
  BOOL AdjustHighLight(int Percentage);
  BOOL AdjustMidtone(int Percentage);
  BOOL AdjustShadow(int Percentage);
  BOOL AdjustHue(int Percentage);
  BOOL AdjustSaturation(int Percentage);
  BOOL AdjustHSL(int PercentHue, int PercentSaturation, int PercentLuminosity);
  BOOL AdjustRed(int Percentage);
  BOOL AdjustGreen(int Percentage);
  BOOL AdjustBlue(int Percentage);
  BOOL Grayscale();
  BOOL Negate();

  //Filter Image processing support
  BOOL FindEdgesFilter();
  BOOL FindVerticalEdgesFilter();
  BOOL FindHorizontalEdgesFilter();
  BOOL BlurFilter();
  BOOL AddNoiseFilter(int Percentage);
  BOOL MedianFilter();
  BOOL UserDefinedFilter(CUserDefinedFilter& Filter);
  BOOL MeanFilter();

  //Channel processing support
  BOOL SplitChannels(CDibImage& red, CDibImage& green, CDibImage& blue);
  BOOL CombineChannels(const CDibImage& red, const CDibImage& green, const CDibImage& blue);
  BOOL GetRedChannel(CDibImage& red);
  BOOL GetGreenChannel(CDibImage& green);
  BOOL GetBlueChannel(CDibImage& blue);

  //Drawing support
  BOOL Draw(CDC& dc, const CRect* rcDst=NULL, const CRect* rcSrc=NULL, CPalette* pPal=NULL);
	
  //Data accessors
  CSize Size() const          { return CSize(m_nWidth, m_nHeight); };
  CRect Rect() const          { return CRect(CPoint(0, 0), Size()); };
  int   Width() const         { return m_nWidth; };
  int   Height() const        { return m_nHeight; };
  int   ScanWidth() const     { return m_nScanWidth; };
  int   BitsPerPixel() const  { return m_nBitsPerPixel; };

  //Direct Pixel access
  inline BOOL GetPixel(int x, int y, COLORREF& value, LPSTR lpDibBits = NULL) const;
  inline BOOL SetPixel(int x, int y, const COLORREF& value, LPSTR lpDibBits = NULL);
  BOOL GetPixelData(int x, int y, ULONG& value, LPSTR lpDibBits) const;
  BOOL SetPixelData(int x, int y, const ULONG& value, LPSTR lpDibBits);
  LPBYTE GetPixelAddress(int x, int y, LPBYTE lpDibBits) const;

  //Channel access support
  BOOL GetRedHistogram(int* RedChannel, int nSize);
  BOOL GetGreenHistogram(int* GreenChannel, int nSize);
  BOOL GetBlueHistogram(int* BlueChannel, int nSize);
  BOOL GetHistogram(int* RedChannel, int nRedSize, int* GreenChannel, int nGreenSize, int* BlueChannel, int nBlueSize);

  //If you really must muck around with the internals 
  //of the class
  HDIB GetHDIB() const { return m_hDib; };
  HPALETTE GetHPALLETTE() const { return m_Pal; };

protected:
  //Internal functions
  int GetBitsPerPixel() const;
  int ComputePaletteSize(DWORD nBitCount);
  static double HuetoRGB(double m1, double m2, double h);

  //member variables
  HDIB          m_hDib;
  HPALETTE      m_Pal;
  int           m_nWidth;
  int           m_nHeight;
  int           m_nScanWidth;
  int           m_nBitsPerPixel;
  CWorkingArea* m_pWorkingArea;
  DWORD         m_dwChannel;
  int           m_nUndoSize;

  //Undo / redo support member variables
  CArray<CUndoNode*, CUndoNode*&> m_UndoStack;
  CArray<CUndoNode*, CUndoNode*&> m_RedoStack;
  CString m_sCurrentDescription;
};

#endif //__DIBIMAGE_H__
#endif
