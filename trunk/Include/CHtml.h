/*
	CHtml.h
	Classe base per interfaccia HTML.
	Luca Piergentili, 29/11/99
	lpiergentili@yahoo.com
*/
#ifndef _CHTML_H
#define _CHTML_H

#include <stdio.h>
#include <stdlib.h>

enum HTMLOUTPUT_TYPE {
	OUTPUTFILE = 1,
	STDOUT = 2
};

struct HTMLFONT_TYPE {
	char*	type;
	int		size;
	char*	color;
};

enum HTMLALIGN_TYPE {
	HTMLALIGN_NONE = 0,
	HTMLALIGN_CENTER,
	HTMLALIGN_LEFT,
	HTMLALIGN_RIGHT
};

enum HTMLSTYLE_TYPE {
	HTMLSTYLE_NONE = 0,
	HTMLSTYLE_NORMAL,
	HTMLSTYLE_BOLD
};

struct HTMLLAYOUT {
	HTMLFONT_TYPE	font;
	HTMLALIGN_TYPE	align;
	HTMLSTYLE_TYPE	style;
	int			border;
	int			width;
};

struct HTMLTABLELAYOUT {
	HTMLLAYOUT	htmllayout;
	char*		title;
	int			border;
	int			cellpadding;
	int			cellspacing;
	int			width;
	char			classname[32];
};

struct CSSLAYOUT {
	char			tagname[32];
	char			classname[32];
	HTMLALIGN_TYPE	align;
	int			width;
};

/*
	CHtml
*/
class CHtml
{
public:
	CHtml();
	virtual ~CHtml();

	int				Open				(const char* file = NULL);
	int				Close			(void);

	virtual void		Header			(const char* title = NULL,const char* doctype = NULL,const char* meta = NULL);
	void				Footer			(void);
	
	void				Text				(const char*,HTMLLAYOUT* layout = NULL);
	void				FormattedText		(const char*,...);
	void				Line				(const char*,HTMLLAYOUT* layout = NULL);
	void				FormattedLine		(const char*,...);
	inline void		NewLine			(void) {fwrite("<br>\r\n",sizeof(char),6,m_pStream);}

	inline void		BoldOn			(void) {fwrite("<b>",sizeof(char),3,m_pStream);}
	inline void		BoldOff			(void) {fwrite("</b>",sizeof(char),4,m_pStream);}

	void				TableOpen			(HTMLTABLELAYOUT* tablelayout = NULL);
	inline void		TableOpenRow		(void) {fwrite("<tr>\r\n",sizeof(char),6,m_pStream);}
	
	void				TablePutCol		(const char*,HTMLLAYOUT* layout = NULL,const char* extra = NULL);
	void				TablePutCol		(const char*,CSSLAYOUT* layout = NULL,const char* extra = NULL);
	inline void		TableOpenCol		(void) {fwrite("<td>",sizeof(char),4,m_pStream);}
	void				TablePutColText	(const char*,HTMLLAYOUT* layout = NULL);
	inline void		TableCloseCol		(void) {fwrite("</td>\r\n",sizeof(char),7,m_pStream);}
	
	inline void		TableCloseRow		(void) {fwrite("</tr>\r\n",sizeof(char),7,m_pStream);}
	inline void		TableClose		(void) {fwrite("\r\n</table>\r\n",sizeof(char),12,m_pStream);}

	inline HTMLOUTPUT_TYPE GetOutputType	(void) {return(m_OutputType);}
	inline const char*	GetOutputFileName	(void) {return(m_OutputType==STDOUT ? NULL : m_szFileName);}

private:
	FILE*			m_pStream;
	HTMLOUTPUT_TYPE	m_OutputType;
	char				m_szFileName[_MAX_PATH+1];
};

#endif // _CHTML_H
