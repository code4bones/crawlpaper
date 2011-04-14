#ifndef _CITEMDATA_H
#define _CITEMDATA_H 1

#include "window.h"
#include "CItemSheet.h"

class CItemData  
{
public:
	LPARAM m_lParam;
	CItemSheet * m_pSheet;
	CItemData();
	virtual ~CItemData();

};

#endif // _CITEMDATA_H
