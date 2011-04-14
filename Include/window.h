/*
	window.h
	Header per l'ambiente Win32.
	Grazie al disegno a cazzo di cane degli headers relativi, MFC genera un errore se viene incluso <windows.h>,
	per cui ricava il tipo di ambiente utilizzato ed include quanto necessario.
	Luca Piergentili, 29/08/98
	lpiergentili@yahoo.com
*/
#ifndef _WINDOW_H
#define _WINDOW_H 1

#include "macro.h"
#include "typedef.h"

#if defined(_WINDOWS)
	/*
	Using the SDK Headers (MSDN)
	=============================================================
	Minimum system required Macros to define:
	Windows Server 2003 family		_WIN32_WINNT	>=	0x0502 
	Windows XP					_WIN32_WINNT	>=	0x0501 
	Windows 2000					_WIN32_WINNT	>=	0x0500 
	Windows Me					_WIN32_WINDOWS	=	0x0490 
	Windows 98					_WIN32_WINDOWS	>=	0x0410 
	Windows NT 4.0					_WIN32_WINNT	>=	0x0400 
	Internet Explorer 6.0			_WIN32_IE		>=	0x0600 
	Internet Explorer 5.01, 5.5		_WIN32_IE		>=	0x0501 
	Internet Explorer 5.0, 5.0a, 5.0b	_WIN32_IE		>=	0x0500 
	Internet Explorer 4.01			_WIN32_IE		>=	0x0401 
	Internet Explorer 4.0			_WIN32_IE		>=	0x0400 
	Internet Explorer 3.0, 3.01, 3.02	_WIN32_IE		>=	0x0300 
	*/

	//#define NTDDI_VERSION NTDDI_WIN2K		// 0x0500 W2000
	//#define NTDDI_VERSION NTDDI_LONGHORN		// 0x0600 Vista
	
	//#define _WIN95 1
	//#define _WIN98 1
	#define _WINNT 1

	#if defined(_WIN95) || defined(_WIN98) || defined(_WINNT)
	  
	  #ifndef WIN32
	    #define WIN32
	  #endif
	  #ifndef _WIN32
	    #define _WIN32
	  #endif
	  
	  //#define _WIN32_IE 0x0400
	  #define _WIN32_IE 0x0500
	  //#define _WIN32_IE 0x0600
#ifdef PRAGMA_MESSAGE_VERBOSE
       #pragma message("\t\t\t_WIN32_IE has been defined to " STR(_WIN32_IE))
#endif
	  
	  #ifdef _WIN95
#ifdef PRAGMA_MESSAGE_VERBOSE
	  #pragma message("\t\t\tcompiling for _WIN95")
#endif
	  #endif
	  
	  #ifdef _WIN98
#ifdef PRAGMA_MESSAGE_VERBOSE
	    #pragma message("\t\t\tcompiling for _WIN98")
#endif
	    #define _WIN32_WINDOWS 0x0410
#ifdef PRAGMA_MESSAGE_VERBOSE
	    #pragma message("\t\t\t_WIN32_WINDOWS has been defined to " STR(_WIN32_WINDOWS))
#endif
	  #endif
	  
	  #ifdef _WINNT
#ifdef PRAGMA_MESSAGE_VERBOSE
	    #pragma message("\t\t\tcompiling for _WINNT")
#endif
	    //#define _WIN32_WINNT 0x0400
	    #define _WIN32_WINNT 0x0500
	    //#define _WIN32_WINNT 0x0600
#ifdef PRAGMA_MESSAGE_VERBOSE
	    #pragma message("\t\t\t_WIN32_WINNT has been defined to " STR(_WIN32_WINNT))
#endif
	  #endif
	
	#endif

#ifdef PRAGMA_MESSAGE_VERBOSE
	#ifndef _WIN32_WINNT
	  #pragma message("\t\t\t_WIN32_WINNT not defined")
	#endif
	#ifndef _WIN32_WINDOWS
	  #pragma message("\t\t\t_WIN32_WINDOWS not defined")
	#endif
	#ifndef _WIN32_IE
	  #pragma message("\t\t\t_WIN32_IE not defined")
	#endif
#endif

	#if defined(_AFX) || defined(_AFXDLL)
		#if _MSC_VER >= 1300
			#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
			#define _AFX_ALL_WARNINGS
		#endif

		#ifdef WIN32_SDK
			#undef WIN32_SDK
		#endif

		#define WIN32_MFC 1
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\tWIN32_MFC defined")
#endif
		
		#ifndef _AFX_H_INCLUDED
			#define _AFX_H_INCLUDED 1
			#include <afx.h>
		#endif

		#ifndef _AFXWIN_H_INCLUDED
			#define _AFXWIN_H_INCLUDED 1
			#include <afxwin.h>		// componenti MFC di base e standard
		#endif

		#ifndef _AFXEXT_H_INCLUDED
			#define _AFXEXT_H_INCLUDED 1
			#include <afxext.h>		// estensioni MFC
		#endif

		#ifndef _AFXDISP_H_INCLUDED
			#define _AFXDISP_H_INCLUDED 1
			#include <afxdisp.h>	// classi di automazione MFC
		#endif

		#ifndef _AFXCMN_H_INCLUDED
			#define _AFXCMN_H_INCLUDED 1
			#include <afxcmn.h>		// supporto MFC per controlli comuni di Windows
		#endif

		#ifndef _AFXDLGS_H_INCLUDED
			#define _AFXDLGS_H_INCLUDED 1
			#include <afxdlgs.h>
		#endif

		#ifndef _AFXMT_H_INCLUDED
			#define _AFXMT_H_INCLUDED 1
			#include <afxmt.h>
		#endif

		#ifndef _AFXOLE_H_INCLUDED
			#define _AFXOLE_H_INCLUDED 1
			#include <afxole.h>
		#endif

		#ifndef _AFXADV_H_INCLUDED
			#define _AFXADV_H_INCLUDED 1
			#include <afxadv.h>
		#endif

		#ifndef _AFXDTCTL_H_INCLUDED
			#define _AFXDTCTL_H_INCLUDED 1
			#include <afxdtctl.h>	// Supporto MFC per controlli comuni di Internet Explorer 4
		#endif

	#else
		#ifdef WIN32_MFC
			#undef WIN32_MFC
		#endif
		
		#define WIN32_SDK 1
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\tWIN32_SDK defined")
#endif
		
		#define STRICT 1
		#include <windows.h>
		#include <windowsx.h>
	#endif

	/*
	Shell and Common Controls Versions - DLL Version Numbers
	========================================================
	MSDN July 2000

	All but a handful of the programming elements discussed in the Shell and Common Controls documentation are contained in three DLLs:
	Comctl32.dll, Shell32.dll, and Shlwapi.dll. Because of ongoing enhancements, different versions of these DLLs implement different
	features. Throughout this document, programming elements are marked with a version number. This version number indicates that the
	programming element was first implemented in that version and will also be found in all subsequent versions of the DLL. If no version
	number is specified, the programming element is implemented in all versions. The following table outlines the different DLL versions,
	and how they were distributed.

	Version DLL Distribution Platform 
	4.00 All Microsoft® Windows® 95/Windows NT® 4.0. 
	4.70 All Microsoft® Internet Explorer 3.x. 
	4.71 All Microsoft® Internet Explorer 4.0 (see note 2). 
	4.72 All Microsoft® Internet Explorer 4.01 and Windows® 98 (see note 2). 
	5.00 Shlwapi.dll Microsoft® Internet Explorer 5 (see note 3). 
	5.00 Shell32.dll Microsoft® Windows® 2000. (see note 3). 
	5.80 Comctl32.dll Microsoft® Internet Explorer 5 (see note 3). 
	5.81 Comctl32.dll Microsoft® Windows 2000(see note 3). 

	Note 1: The 4.00 versions of Shell32.dll and Comctl32.dll are found on the original versions of Windows 95 and Windows NT 4. New
	versions of Commctl.dll were shipped with all Internet Explorer releases. Shlwapi.dll first shipped with Internet Explorer 4.0, so
	its first version number is 4.71. The Shell was not updated with the Internet Explorer 3.0 release, so Shell32.dll does not have a
	version 4.70. While Shell32.dll versions 4.71 and 4.72 were shipped with the corresponding Internet Explorer releases, they were
	not necessarily installed (see Note 2). For subsequent releases, the version numbers for the three DLLs are not identical. In general,
	you should assume that all three DLLs may have different version numbers, and test each one separately.

	Note 2: All systems with Internet Explorer 4.0 or 4.01 will have the associated version of Comctl32.dll and Shlwapi.dll (4.71 or 4.72,
	respectively). However, for systems prior to Windows 98, Internet Explorer 4.0 and 4.01 can be installed with or without the integrated
	shell. If they are installed with the integrated shell, the associated version of Shell32.dll will be installed. If they are installed
	without the integrated shell, Shell32.dll is not updated. In other words, the presence of version 4.71 or 4.72 of Comctl32.dll or
	Shlwapi.dll on a system does not guarantee that Shell32.dll has the same version number. All Windows 98 systems have version 4.72 of
	Shell32.dll.

	Note 3:Version 5.80 of Comctl32.dll and version 5.0 of Shlwapi.dll are distributed with Internet Explorer 5. They will be found on all
	systems on which Internet Explorer 5 is installed, except Windows 2000. Internet Explorer 5 does not update the Shell, so version 5.0
	of Shell32.dll will not be found on Windows NT, Windows 95, or Windows 98 systems. Version 5.0 of Shell32.dll will be distributed with
	Windows 2000, along with version 5.0 of Shlwapi.dll, and version 5.81 of Comctl32.dll.
	
	
	Project Versions
	================
	MSDN July 2000

	To ensure that your application is compatible with different targeted versions of Comctl32.dll and Shell32.dll, a version macro was
	added to the header files. This macro is used to define, exclude, or redefine certain definitions for different versions of the DLL.
	The macro name is _WIN32_IE and you, the developer, are responsible for defining the macro as a hexadecimal number. This version number
	defines the target version of the application that is using the DLL. The following are the currently available version numbers and the
	effect each has on your application. 

	Version Description:
	0x0200	The application will be compatible with Comctl32.dll and Shell32.dll version 4.00 and later.
			The application will not be able to implement features that were added after version 4.00 of Comctl32.dll. 
	0x0300	The application will be compatible with Comctl32.dll and Shell32.dll version 4.70 and later.
			The application will not be able to implement features that were added after version 4.70 of Comctl32.dll. 
	0x0400	The application will be compatible with Comctl32.dll and Shell32.dll version 4.71 and later.
			The application will not be able to implement features that were added after version 4.71 of Comctl32.dll. 
	0x0401	The application will be compatible with Comctl32.dll and Shell32.dll version 4.72 and later.
			The application will not be able to implement features that were added after version 4.72 of Comctl32.dll. 
	0x0500	The application will be compatible with Comctl32.dll version 5.80 and later, and Shell32.dll and Shlwapi.dll
			version 5.0 and later.
			The application will not be able to implement features that were added after version 5.80 of Comctl32.dll or
			version 5.0 of Shell32.dll and Shlwapi.dll. 
	0x0501	The application will be compatible with Comctl32.dll version 5.81 and later and Shell32.dll and Shlwapi.dll
			version 5.0 and later. The application will not be able to implement features that were added after version
			5.81 of Comctl32.dll or version 5.0 of Shell32.dll and Shlwapi.dll. 

	If you do not define this macro in your project, it will automatically be defined as 0x0500. To define a different value, you can add
	the following to the compiler directives in your make file (substitute the desired version number for 0x0400): 
	/D _WIN32_IE=0x0400 
	*/
	#include <commctrl.h>
	#include <shlobj.h>
	
	// CDateTime.h
	#undef GetDateFormat
	#undef GetTimeFormat

	#include <shlobj.h>
#else
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): "__FILE__" included into a non-SDK/MFC environment")
	#error not an SDK/MFC environment
#endif

#endif // _WINDOW_H
