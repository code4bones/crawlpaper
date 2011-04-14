/*
	CPropertyPageList.h
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CPROPERTYPAGEARRAY_H
#define _CPROPERTYPAGEARRAY_H 1

#include "window.h"
#include "CNodeList.h"

struct PROPERTYPAGE {
public:
	PROPERTYPAGE(int idd = -1,void* page = (void*)NULL,CRuntimeClass* runtimeclass = (CRuntimeClass*)NULL)
	{
		this->idd = idd;
		this->page = page;
		this->runtimeclass = runtimeclass;
		this->status = 0;
	}
	
	~PROPERTYPAGE() {}
	
	int idd;
	void* page;
	CRuntimeClass* runtimeclass;
	int status;
};

class CPropertyPageList : public CNodeList
{
public:
	CPropertyPageList() : CNodeList() {}
	virtual ~CPropertyPageList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((PROPERTYPAGE*)iter->data)
		{
			delete ((PROPERTYPAGE*)iter->data);
			iter->data = (PROPERTYPAGE*)NULL;
		}
		return(TRUE);
	}
#ifdef _DEBUG
	const char* Signature(void) {return("CPropertyPageList");}
#endif
	int Size(void) {return(-1);}
};

#endif // _CPROPERTYPAGEARRAY_H
