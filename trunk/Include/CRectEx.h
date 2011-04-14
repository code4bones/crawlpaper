/*
	CRectEx.h
	Classe per la normalizzazione del rettangolo.
	Luca Piergentili, 08/10/00
	lpiergentili@yahoo.com
*/
#ifndef _CRECTEX_H
#define _CRECTEX_H 1

#include "window.h"

class CRectEx : public CRect
{
public:
	void NormalizeRect(int nMinValue = 1)
	{
		CRect rect;
		rect.CopyRect(this);
		if(rect.left > rect.right)
		{
			int i = rect.left;
			rect.left = rect.right;
			rect.right = i;
		}
		if(rect.top > rect.bottom)
		{
			int i = rect.top;
			rect.top = rect.bottom;
			rect.bottom = i;
		}
		if(rect.left <= 0)
			rect.left = nMinValue;
		if(rect.top <= 0)
			rect.top = nMinValue;
		CopyRect(rect);
	}
	
	static void NormalizeRect(CRect& rect,int nMinValue = 1)
	{
		CRect This;
		This.CopyRect(rect);
		
		if(This.left > This.right)
		{
			int i = This.left;
			This.left = This.right;
			This.right = i;
		}
		if(This.top > This.bottom)
		{
			int i = This.top;
			This.top = This.bottom;
			This.bottom = i;
		}
		
		if(This.left <= 0)
			This.left = nMinValue;
		if(This.top <= 0)
			This.top = nMinValue;
		
		rect.CopyRect(This);
	}
	
	static void NormalizeRect(CRect& rect,CRect& rectBase,int nMinValue = 1)
	{
		CRect This;
		This.CopyRect(rect);
		
		if(This.left > This.right)
		{
			int i = This.left;
			This.left = This.right;
			This.right = i;
		}
		if(This.top > This.bottom)
		{
			int i = This.top;
			This.top = This.bottom;
			This.bottom = i;
		}
		
		if(This.left <= rectBase.left)
			This.left = nMinValue;
		if(This.top <= rectBase.top)
			This.top = nMinValue;
		if(This.right > rectBase.right)
			This.right = rectBase.right - nMinValue;
		if(This.bottom > rectBase.bottom)
			This.bottom = rectBase.bottom - nMinValue;
		
		rect.CopyRect(This);
	}
	
	static void NormalizeRectEx(CRect& rect,CRect& rectBase,int nMinValue = 1)
	{
		CRect This;
		This.CopyRect(rect);
		
		if(This.left > This.right)
		{
			int i = This.left;
			This.left = This.right;
			This.right = i;
		}
		if(This.top > This.bottom)
		{
			int i = This.top;
			This.top = This.bottom;
			This.bottom = i;
		}
		
		if(This.left <= rectBase.left)
		{
			This.right += rectBase.left - This.left;
			This.left = nMinValue;
		}
		if(This.top <= rectBase.top)
		{
			This.bottom += rectBase.top - This.top;
			This.top = nMinValue;
		}
		if(This.right > rectBase.right)
		{
			This.left -= (This.right - rectBase.right);
			This.right = rectBase.right - nMinValue;
		}
		if(This.bottom > rectBase.bottom)
		{
			This.top -= (This.bottom - rectBase.bottom);
			This.bottom = rectBase.bottom - nMinValue;
		}
		
		rect.CopyRect(This);
	}
};

#endif // _CRECTEX_H
