#if 0
	 Change the color of fonts...   kevindu  4:12 22 Aug '03  
 
	  We can do this by adding blow codes into the class CDialogHeader:

	a:Add two member variables
	protected:
	.
	.
	COLORREF m_cText;
	COLORREF m_cDisc;

	b:Add a member function:
	void CDialogHeader::SetFontColors(const COLORREF Text, const COLORREF Disc)
	{
	m_cText=Text;
	m_cDisc=Disc;

	//Force update of fonts in the OnPaint handler
	m_bFonts=FALSE;

	//Repaint the header
	RepaintHeader();
	}

	c: add tow lows in "void CDialogHeader::OnPaint()" 
	{
	.
	. 
	SetTextColor(dc.m_hDC,m_cText); //Add
	DrawText(dc.m_hDC,m_sTitle,m_sTitle.GetLength(),text,
	m_uAlign | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL); 

	.
	. 
	SetTextColor(dc.m_hDC,m_cDisc); //Add
	DrawText(dc.m_hDC,m_sDesc,m_sDesc.GetLength(),text,
	m_uAlign | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL);
	.
	.
	}

	d:Now you can set text color by call the function "SetFontColors(RGB(..),RGB(..))" in you program.

	MayBe there are more easy ways to do this ,pls post to me.Thanks! 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Dialog Footer   mohdalwi  13:49 12 Jan '03  
 
  thank for this code !.

this dialog header could be dialog footer but add some code;

modify Init(void* ptrWnd) --> Init(void* ptrWnd, bool isTop)

just modify a little bit code:

if(isTop)
{
pWnd->SetWindowPos(NULL,0,0,rect.Width(),rect.Height() + m_nHeaderHeight,
SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

//Create the header window
CreateEx (NULL,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 
-1,-1,rect.Width(),m_nHeaderHeight,pWnd->m_hWnd,0,0);
}
else
{
pWnd->SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),
SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);


CreateEx(NULL,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 
-1,rect.bottom - m_nHeaderHeight ,rect.Width(),m_nHeaderHeight,pWnd->m_hWnd,0,0);
}




then call this function
Init(this,false); //false for footer and true for header

for Dialog Footer do not call this function
MoveCtrls(this)


Mohd Alwi
 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 Good class - Suggestions   John O'Byrne  4:39 24 Jul '02  
 
  Hi, I'm going to use your class in my code, but I added this piece of code which makes it much more good looking:

Instead of drawing a plain rectangle, I draw a gradient rectangle: (code taken from a sample in the msdn)

TRIVERTEX vert[2] ;
GRADIENT_RECT gRect;
vert [0] .x = rect.left;
vert [0] .y = rect.top;
vert [0] .Red = 0x0000;
vert [0] .Green = 0x0000;
vert [0] .Blue = 0x6600;
vert [0] .Alpha = 0x0000;

vert [1] .x = rect.right;
vert [1] .y = rect.bottom; 
vert [1] .Red = 0xff00;
vert [1] .Green = 0xff00;
vert [1] .Blue = 0xff00;
vert [1] .Alpha = 0x0000;

gRect.UpperLeft = 0;
gRect.LowerRight = 1;
dc.GradientFill(vert,2,&gRect,1,GRADIENT_FILL_RECT_H); 
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Re: Good class - Suggestions - TextBackground color (Is theree any way to make transperent)   Anonymous  10:33 24 Jul '02  
 
  HOW to make text background transperent.
When text is getting displayed,it has gray background if I don't call FillSolidrect with white color.
And if call FillSoildRect with white color befor your suggested code, text has white background.

Is there any better solution is there.

By the way.
In order to use GradientFill, we need to include following two files.

Wingdi.h, Windows.h

and also msimg32.lib need to include in link option.

Thanks in advance.
 
-------------

  Re: Good class - Suggestions - TextBackground color (Is theree any way to make transperent)   jmgurgel  11:00 24 Jul '02  
 
  
Try adding these two lines in the CDialogHeader OnPaint handler.
Add them right before the dc.FillSolidRect.


//New code
dc.SetBkColor(m_cBackground);
dc.SetBkMode(TRANSPARENT);
//New code

//Old code
//Fill the rect with solid color
//Leaving space for line at bottom of rect
dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height() - m_nxEdge,
m_cBackground);

I'm using win XP with themes and all so I don't have this prob. but I'm sure other people will.

So, Try this and please let me know if that works. Thanks 

-------------------------
 Re: Good class - Suggestions - TextBackground color (Is theree any way to make transperent)   John O'Byrne  16:18 24 Jul '02  
 
  No need to include anything specific nor to link with any library,
Just call GradientFill 


#endif



#if !defined(AFX_DIALOGHEADER_H__D47F665F_FEB3_4AA7_B746_D385FC7956ED__INCLUDED_)
#define AFX_DIALOGHEADER_H__D47F665F_FEB3_4AA7_B746_D385FC7956ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// DialogHeader.h : header file
//******************************************************************************
//Author : Jason Gurgel
//EMail  : jmgurgel@attbi.com
//(C)    : 7-14-2002
//
//Purpose: Provide a professional looking header area for dialog controls
//       : that will display an icon, a title phrase in bold, and a
//       : description phrase. And also provide a custom background color
//       : and custom header height.
//		  
//Note   : This code was insipired by Mustafa Demirhan's article about hacking
//		 : the CPropterySheet. See link below. 
//		 : http://www.codeproject.com/property/hacking_the_cpropertyshee.asp#xx162207xx
//
//Disclaimer: This code is free to use as long as this information
//			: is included. The author takes no responsibility for
//			: the corretness of the code or any damage incured from it's use. 
//			: It is free so use at your own risk, and enjoy!
//			: Any comments, suggestions, or additions would be appricated.
//******************************************************************************



// How to use this class
//==============================================================================
//1. Include the files "DialogHeader.h" & "DialogHeader.cpp" in your project.
//
//2. Create a member variable in your dialog class e.g.(m_HeaderCtrl) and don't
//   forget to include the DialogHeader.h file.
//
//3. In your dialogs OnInit function do the following.
//   You can set these options anywhere you have access to the header ctrl
//   but the OnInit function is probably the eaisest.
//	  a. Set the headers display data:
//              m_HeaderCtrl.SetHeaderData(hIcon,"Title","Description");
//		 Or you can set/change each memeber individually using:
//              m_HeaderCtrl.SetIconHandle(hIcon);
//              m_HeaderCtrl.SetTitleText("Title");
//              m_HeaderCtrl.SetDescText("Description.");
//      
//
// Note: The above functions will be the most commonly used.
//
//	  b. Set the font sizes:  m_HeaderCtrl.SetFontSizes(TitleSize,DescSize);
//
//	  c. Set the text alignment: m_HeaderCtrl.SetTextAlign(DT_LEFT);
//                  Options are: {DT_LEFT,DT_CENTER,DT_RIGHT}
//
//    d. Set the background color: 
//                            m_HeaderCtrl.SetBackgroundColor(RGB(255,255,255));
//
//    e. Set blank icon mode: m_HeaderCtrl.SetBlankIcon(TRUE);
//                            FALSE=If no icon then no space allocated for icon     
//                            TRUE=If no icon space is allocated for one
//
//    f. Set the drawing offsets:
//         m_HeaderCtrl.SetIconOffset(5); Defines top left corner of icon
//                                        and border around header ctrl
//         m_HeaderCtrl.SetTitleOffset(10); Defines space between title text
//                                          and icon
//         m_HeaderCtrl.SetDescOffset(10); Defines the amount of indent for
//                                         desc text from the title text
//
//    g. Set the header height: m_HeaderCtrl.SetHeaderHeight(x);
//           -Where x=Height of header, if you choose x to be too small
//            to display the icon, the title, and one line of desc text
//            then x will be automaticly set to this min height.
//
//           -Also if you choose x=0, then the height will also be
//            automaticly calculated to disp the icon, the title, and 
//            one line of the desc text.
//   
//           -This calculation is based on the font sizes and offsets
//            so you must set these options before calling SetHeaderHeight
//            and the Init function.
//
// Note: The above options all have default values. So change what you want.
// Note: All the above options can be changed at runtime and the ctrl will
//       repaint itself with the new options. However the header height 
//       may not be enough to accomodate the new changes and you may have
//       to adjust the header height and move dialog controls and resize
//       the parent window yourself!
//    
// 	  h. Create the header ctrl and resize the dialog window to accomodate
//       the header ctrl by calling:  m_HeaderCtrl.Init(this);
//
//	  i. Move all ctrls on the dialog down to accomodate the header ctrl
//       by calling: m_HeaderCtrl.MoveCtrls(this);

// *Note*: MoveCtls works for most dialogs, however if you use this with a 
//		   CPropertySheet or a complicated ctrl you may need to bypass
//         the MoveCtrls function and/or perform your own control
//         placement in the InitDialog function.
//
//
//Example:
/*
BOOL CDlgFileTypes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	//Set inital header data
	m_HeaderCtrl.SetTitleText("Registered File Types on this computer");
	m_HeaderCtrl.SetDescText("Displaying all registered file types for this computer below.");
	m_HeaderCtrl.SetIconHandle(AfxGetApp()->LoadIcon(IDI_HARDDRIVE));

	//Create the header ctrl
	m_HeaderCtrl.Init(this);
	m_HeaderCtrl.MoveCtrls(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
*/
//==============================================================================


//Defines: All default values are defined here
//-------: If you wish to change any default values change them here
#define DEFAULTICONOFFSET		5
#define DEFAULTTITLEOFFSET		10
#define DEFAULTDESCOFFSET		10
#define DEFAULTTEXTALIGN			DT_LEFT
#define DEFAULTTITLEFONTSIZE		13
#define DEFAULTDESCFONTSIZE		13
#define DEFAULTBLANKICON			TRUE
#define DEFAULTHEADERHEIGHT		55


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader window

class CDialogHeader : public CWnd
{
// Construction
public:
	CDialogHeader();	//Default Constructor


// Attributes
public:


// Operations
public:
	void Init(void* ptrWnd);
	void MoveCtrls(void* ptrWnd);


// Access Functions
	COLORREF	GetBackgroundColor() const	{return m_cBackground;}
	CString		GetDescStr() const			{return m_sDesc;}
	int			GetTitleFontSize() const	{return m_nTitleFontSize;}
	int			GetDescFontSize() const		{return m_nDescFontSize;}
	int			GetHeaderHeight() const		{return m_nHeaderHeight;}
	HICON		GetIconHandle()	const		{return m_hIcon;}
	int			GetIconOffset() const		{return m_nIconOffset;}
	int			GetTitleOffset() const		{return m_nTitleOffset;}
	int			GetDescOffset() const		{return m_nDescOffset;}
	UINT		GetAlign() const			{return m_uAlign;}
	BOOL		GetBlankIcon() const		{return m_bBlankIcon;}

	void SetIconHandle(const HICON icon=NULL);
	void SetTitleText(const LPCTSTR text="");
	void SetDescText(const LPCTSTR text="");
	void SetHeaderData(const HICON hIcon=NULL,
		               const LPCTSTR strTitle="",
					   const LPCTSTR strDesc="");
	void SetBackgroundColor(const COLORREF color);
	void SetHeaderHeight(const int height=0);
	void SetFontSizes(const int TitleSize=DEFAULTTITLEFONTSIZE,
		              const int DescSize=DEFAULTDESCFONTSIZE);
	void SetIconOffset(const int offset=DEFAULTICONOFFSET);
	void SetTitleOffset(const int offset=DEFAULTTITLEOFFSET);
	void SetDescOffset(const int offset=DEFAULTDESCOFFSET);
	void SetTextAlign(const UINT align=DEFAULTTEXTALIGN);
	void SetBlankIcon(const BOOL blank=DEFAULTBLANKICON);

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogHeader)
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CDialogHeader();


	// Generated message map functions
protected:
	//{{AFX_MSG(CDialogHeader)
	afx_msg BOOL OnEraseBkgnd(CDC* /*pDC*/);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	// Internal class functions
private:
	void UpdateFont(CFont& font,const LONG lfWeight=FW_NORMAL,const int Size=13);
	void RepaintHeader();


	// Member Variables
protected:
	int			m_nTitleFontSize;	//Size of font used to draw title text
	int			m_nDescFontSize;	//Size of font used to draw desc text
	int			m_nHeaderHeight;	//Height of dialog header
	int			m_nIconWidth;		//System icon width
	int			m_nIconHeight;		//System icon height
	int			m_nxEdge;			//System edge width
	int			m_nyEdge;			//System edge height
	int			m_nIconOffset;		//Icon offset from top left client edge
	int			m_nTitleOffset;		//Title offset from icon right
	int			m_nDescOffset;		//Desc offset from left of title text
	UINT		m_uAlign;			//Text alignment
	BOOL		m_bFonts;			//Boolean to track if fonts have been created
	BOOL		m_bBlankIcon;		//Always leave space for icon
	HICON		m_hIcon;			//Handle to header icon (NULL if none)
	CFont		m_BoldFont;			//Bold font for title
	CFont		m_NormalFont;		//Normal font for body
	CString		m_sTitle;			//Title text
	CString		m_sDesc;			//Description text
	COLORREF	m_cBackground;		//Background fill color
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGHEADER_H__D47F665F_FEB3_4AA7_B746_D385FC7956ED__INCLUDED_)
