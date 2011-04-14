/*
	win32api.h
	Implementazione di quanto omesso dall' API (SDK/MFC).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#ifndef _WIN32API_H
#define _WIN32API_H 1

#include "window.h"
#include <stdlib.h>

#define _MAX_FILEPATH (_MAX_DIR+_MAX_FNAME)

#define PACKVERSION(major,minor) MAKELONG(minor,major)

enum OSVERSIONTYPE {
	WINDOWS_31,
	WINDOWS_95,
	WINDOWS_98,
	WINDOWS_MILLENNIUM,
	WINDOWS_NT,
	WINDOWS_2000,
	WINDOWS_XP,
	WINDOWS_VISTA,
	WINDOWS_SEVEN,
	UNKNOW_WINDOWS_VERSION
};

typedef struct _iconheader {
	WORD nReserved;	// Reserved. Should always be 0.
	WORD nImageType;	// Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
	WORD nImageCount;	// Specifies number of images in the file.
} ICONHEADER;

typedef struct _iconimage {
	BYTE	nWidth;		// Specifies image width in pixels. Can be any number between 0 to 255. Value 0 means image width is 256 pixels.
	BYTE	nHeight;		// Specifies image height in pixels. Can be any number between 0 to 255. Value 0 means image height is 256 pixels.
	BYTE	nColors;		// Specifies number of colors in the color palette. Should be 0 if the image is truecolor.
	BYTE	nReserved;	// Reserved. Should be 0.[Notes 1]
	WORD	nColorPlanes;	// In .ICO format: Specifies color planes. Should be 0 or 1.[Notes 2]
					// In .CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left.
	WORD	nBitesxPixel;	// In .ICO format: Specifies bits per pixel. [Notes 3]
					// In .CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top.
	DWORD nBitmapSize;	// Specifies the size of the bitmap data in bytes
	DWORD nBitmapOfs;	// Specifies the offset of bitmap data address in the file
} ICONIMAGE;

int			GetBiggerIconSize		(LPCSTR lpcszIconFile,int nPreferredSize = 0);

BOOL			WritePrivateProfileInt	(LPCSTR lpcszSectioneName,LPCSTR lpcszKeyName,int iValue,LPCSTR lpcszIniFile);

LPSTR		GetThisModuleFileName	(LPSTR lpszFileName,UINT nSize);
OSVERSIONTYPE	GetWindowsVersion		(LPSTR lpszWindowsPlatform,UINT nSize,DWORD* dwMajorVersion = NULL,DWORD* dwMinorVersion = NULL);
DWORD		GetDllVersion			(LPCTSTR lpszDllName);

void			GetLastErrorString		(void);
LPVOID		GetLastErrorString		(DWORD dwError);

int			MessageBoxResource		(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID);
int			MessageBoxResourceEx	(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID,...);
int			FormatResourceString	(LPSTR buffer,UINT nSize,UINT nID);
int			FormatResourceStringEx	(LPSTR buffer,UINT nSize,UINT nID,...);

BOOL			ExtractResource		(UINT resource,LPCSTR lpcszResName,LPCSTR lpszFileName);
BOOL			ExtractResourceIntoBuffer(UINT nID,LPCSTR lpcszResName,LPSTR lpBuffer,UINT nBufferSize);

BOOL			CreateShortcut			(LPCSTR Target,LPCSTR Arguments,LPCSTR LinkFileName,LPCSTR LinkLocation,LPCSTR WorkingDir,UINT nIconIndex);
	
BOOL			CreateRegistryKey		(LPCSTR lpcszKey,LPCSTR lpcszName,LPCSTR lpcszValue);
BOOL			DeleteRegistryKey		(LPCSTR lpcszKey,LPCSTR lpcszName);

void			Delay				(int delay);
BOOL			PeekAndPump			(void);

BOOL			CopyFileTo			(HWND hWnd,LPCSTR lpcszSourceFile,LPCSTR lpcszDestFile,BOOL bMoveInsteadCopy = FALSE,BOOL bShowDialog = TRUE);
BOOL			DeleteFileToRecycleBin	(HWND hWnd,LPCSTR lpcszFileName,BOOL bShowDialog = TRUE,BOOL bAllowUndo = TRUE);
BOOL			FileExist				(LPCSTR lpcszFileName);
BOOL			GetFileSizeExt			(HANDLE hFile,PLARGE_INTEGER lpFileSize);
QWORD		GetFileSizeExt			(HANDLE hFile);
QWORD		GetFileSizeExt			(LPCSTR lpcszFileName);

LPCSTR		StripPathFromFile		(LPCSTR lpcszFileName);
LPSTR		StripFileFromPath		(LPCSTR lpcszFileName,LPSTR pPath,UINT nPathSize,BOOL bRemoveBackslash);
LPSTR		EnsureBackslash		(LPSTR lpszFileName,UINT nFileSize);
LPSTR		RemoveBackslash		(LPSTR lpszFileName);

struct TASKBARPOS {
	HWND	hWnd;
	RECT rc;
	int nTaskbarPlacement;
	int nTaskbarWidth;
	int nTaskbarHeight;
	int nScreenWidth;
	int nScreenHeight;
};

// per l'aggiustamento sulle coordinate della taskbar
#define TASKBAR_X_TOLERANCE	40
#define TASKBAR_Y_TOLERANCE	40

BOOL			GetTaskBarPos			(TASKBARPOS* tbi);

void			SetForegroundWindowEx	(HWND hWnd,BOOL bInvalidate = TRUE);

#endif // _WIN32API_H
