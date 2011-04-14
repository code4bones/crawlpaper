//LPI

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if !defined(AFX_STDAFX_H__0A781DD9_5C37_49E2_A4F5_E517F5B8A621__INCLUDED_)
#define AFX_STDAFX_H__0A781DD9_5C37_49E2_A4F5_E517F5B8A621__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Use target Windows version for compatibility
// with CPropertyPageEx, CPropertySheetEx
//#define _WIN32_IE 0x0400

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxcmn.h>			// MFC support for Windows Common Controls

#include "macro.h"
#include "window.h"

#ifndef WS_EX_LAYOUTRTL
   #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: WS_EX_LAYOUTRTL not defined by the current SDK")
   #define WS_EX_LAYOUTRTL 0x00400000
#endif

#ifndef WC_BUTTON
   #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: WC_BUTTON, etc. not defined by the current SDK")
   #define WC_BUTTON		_T("Button")
   #define WC_STATIC		_T("Static")
   #define WC_EDIT			_T("Edit")
   #define WC_LISTBOX		_T("ListBox")
   #define WC_COMBOBOX		_T("ComboBox")
   #define WC_SCROLLBAR		_T("ScrollBar")
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0A781DD9_5C37_49E2_A4F5_E517F5B8A621__INCLUDED_)
