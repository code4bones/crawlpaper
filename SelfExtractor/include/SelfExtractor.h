// Self Extractor.h : main header file for the SELF EXTRACTOR application
//

#if !defined(AFX_SELFEXTRACTOR_H__5C3D7753_497B_11D3_A8BC_0050043A01C0__INCLUDED_)
#define AFX_SELFEXTRACTOR_H__5C3D7753_497B_11D3_A8BC_0050043A01C0__INCLUDED_

#include "window.h"

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSelfExtractorApp:
// See Self Extractor.cpp for the implementation of this class
//

class CSelfExtractorApp : public CWinApp
{
public:
	CSelfExtractorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelfExtractorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSelfExtractorApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELFEXTRACTOR_H__5C3D7753_497B_11D3_A8BC_0050043A01C0__INCLUDED_)
