/*
	CHtml.cpp
	Classe base per interfaccia HTML.
	Luca Piergentili, 29/11/99
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include "CHtml.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
	CHtml()
*/
CHtml::CHtml()
{
	m_pStream = (FILE*)NULL;
	m_OutputType = (HTMLOUTPUT_TYPE)0;
}

/*
	~CHtml()
*/
CHtml::~CHtml()
{
	Close();
}

/*
	Open()

	Apre il file, se gia' esiste lo azzera.
	Passare NULL per usare stdout.
*/
int CHtml::Open(const char* file/*=NULL*/)
{
	if(m_pStream)
		return(0);

	_fmode = _O_BINARY;

	HTMLOUTPUT_TYPE type = (file==NULL ? STDOUT : OUTPUTFILE);

//check_type:

	switch(type)
	{
#if 1
		case OUTPUTFILE:
			strncpy(m_szFileName,file,sizeof(m_szFileName)-1);
			m_pStream = fopen(m_szFileName,"w+");
			m_OutputType = OUTPUTFILE;
			break;
#else
		case OUTPUTFILE:
			strncpy(m_szFileName,file,sizeof(m_szFileName)-1);
			if((m_pStream = fopen(m_szFileName,"wb"))==(FILE*)NULL)
			{
				type = STDOUT;
				goto check_type;
			}
			m_OutputType = OUTPUTFILE;
			break;
#endif

		case STDOUT:
			memset(m_szFileName,'\0',sizeof(m_szFileName));
			m_pStream = stdout;
			m_OutputType = STDOUT;
			break;
	}

	return(m_pStream!=(FILE*)NULL ? 1 : 0);
}

/*
	Close()

	Chiude il file (o svuota lo stream).
*/
int CHtml::Close(void)
{
	if(m_pStream)
	{
		if(m_OutputType==OUTPUTFILE)
			fclose(m_pStream);
		else if(m_OutputType==STDOUT)
			fflush(m_pStream);
		
		m_pStream = (FILE*)NULL;
	}
	
	return(1);
}

/*
	Header()

	Imposta l'header.
*/
void CHtml::Header(const char* title/*=NULL */,const char* doctype/*=NULL */,const char* meta/*=NULL */)
{
	if(doctype)
		fprintf(m_pStream,"%s\r\n",doctype);
	
	fwrite(	"<html>\r\n<head>\r\n",
			sizeof(char),
			16,
			m_pStream
			);

	if(title)
		fprintf(m_pStream,"<title>%s</title>\r\n",title);

	if(meta)
		fprintf(m_pStream,"%s\r\n",meta);

	fwrite(	"</head>\r\n<body>\r\n",
			sizeof(char),
			17,
			m_pStream
			);
}

/*
	Footer()

	Imposta il footer.
*/
void CHtml::Footer(void)
{
	fwrite("\r\n</body>\r\n</html>",sizeof(char),18,m_pStream);
}

/*
	Text()

	Trascrive il testo nel file.
	Accetta il layout.
*/
void CHtml::Text(const char* string,HTMLLAYOUT* layout/*=NULL*/)
{
	if(layout)
	{
		if(layout->font.type)
			fprintf(m_pStream,"<font face=\"%s\" size=\"%d\">",layout->font.type,layout->font.size);
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("<b>",sizeof(char),3,m_pStream);
	}

	fwrite(string,sizeof(char),strlen(string),m_pStream);

	if(layout)
	{
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("</b>",sizeof(char),4,m_pStream);
		if(layout->font.type)
			fwrite("</font>",sizeof(char),7,m_pStream);
	}
}

/*
	FormattedText()
	
	Formatta il testo trascrivendolo nel file.
	Non accetta il layout.
*/
void CHtml::FormattedText(const char* fmt,...)
{
	char* arg = (char*)&fmt + sizeof(fmt);
	vfprintf(m_pStream,fmt,arg);
}

/*
	Line()

	Trascrive la linea di testo nel file.
	Accetta il layout.
*/
void CHtml::Line(const char* string,HTMLLAYOUT* layout/*=NULL*/)
{
	CHtml::Text(string,layout);
	CHtml::NewLine();
}

/*
	FormattedLine()
	
	Formatta la linea di testo trascrivendola nel file.
	Non accetta il layout.
*/
void CHtml::FormattedLine(const char* fmt,...)
{
	char* arg = (char*)&fmt + sizeof(fmt);
	vfprintf(m_pStream,fmt,arg);
	CHtml::NewLine();
}

/*
	TableOpen()

	Apre la tabella.
*/
void CHtml::TableOpen(HTMLTABLELAYOUT* tablelayout/*=NULL*/)
{
	fwrite("\r\n<table",sizeof(char),8,m_pStream);
	
	if(tablelayout)
	{
		char width[16];
		if(tablelayout->width > 100)
			sprintf(width,"%ld",tablelayout->width);
		else
			sprintf(width,"\"%d%%\"",tablelayout->width);
		
		fprintf(	m_pStream,
				" border=%d cellpadding=%d cellspacing=%d width=%s",
				tablelayout->border,
				tablelayout->cellpadding,
				tablelayout->cellspacing,
				width
				);
		if(tablelayout->classname[0]!='\0' && strcmp(tablelayout->classname,"")!=0)
			fprintf(	m_pStream,
					" class=\"%s\"",
					tablelayout->classname
					);
	}
	
	fwrite(">\r\n",sizeof(char),3,m_pStream);

	if(tablelayout)
	{
		if(tablelayout->title)
		{
			char font_begin[32] = {""};
			char font_end[8] = {""};
			if(tablelayout->htmllayout.font.type)
			{
				_snprintf(font_begin,sizeof(font_begin)-1,"<font face=\"%s\" size=\"%d\">",tablelayout->htmllayout.font.type,tablelayout->htmllayout.font.size);
				strcpy(font_end,"</font>");
			}
			
			fprintf(	m_pStream,
					"\r\n<caption align=center><b>%s%s%s</b></caption>\r\n",
					font_begin,
					tablelayout->title,
					font_end
					);
		}
	}
}

/*
	TablePutCol()

	Inserisce la colonna nella tabella.
*/
void CHtml::TablePutCol(const char* text,HTMLLAYOUT* layout/*=NULL*/,const char* extra/*=NULL*/)
{
	fwrite("<td",sizeof(char),3,m_pStream);

	char* align = NULL;
	
	if(layout)
		switch(layout->align)
		{
			case HTMLALIGN_LEFT:
				align = "left";
				break;
			case HTMLALIGN_RIGHT:
				align = "right";
				break;
			case HTMLALIGN_CENTER:
				align = "center";
				break;
			case HTMLALIGN_NONE:
			default:
				align = NULL;
				break;
		}
	
	if(align)
		fprintf(m_pStream," align=%s",align);

	if(layout)
		if(layout->width > 0)
			fprintf(m_pStream," width=%d%%",layout->width);

	fwrite(">",sizeof(char),1,m_pStream);

	if(layout)
		if(layout->font.type)
		{
			fprintf(m_pStream,"<font face=\"%s\"",layout->font.type);
			fprintf(m_pStream," size=\"%d\"",layout->font.size);
			if(layout->font.color)
				fprintf(m_pStream," color=\"%s\"",layout->font.color);
			fwrite(">",sizeof(char),1,m_pStream);
		}

	if(layout)
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("<b>",sizeof(char),3,m_pStream);

	fwrite(text,sizeof(char),strlen(text),m_pStream);
	
	if(extra)
		fwrite(extra,sizeof(char),strlen(extra),m_pStream);

	if(layout)
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("</b>",sizeof(char),4,m_pStream);

	if(layout)
		if(layout->font.type)
			fwrite("</font>",sizeof(char),7,m_pStream);
	
	fwrite("</td>\r\n",sizeof(char),7,m_pStream);
}
void CHtml::TablePutCol(const char* text,CSSLAYOUT* layout/*=NULL*/,const char* extra/*=NULL*/)
{
	fwrite("<td",sizeof(char),3,m_pStream);

	char* align = NULL;
	
	if(layout)
		switch(layout->align)
		{
			case HTMLALIGN_LEFT:
				align = "left";
				break;
			case HTMLALIGN_RIGHT:
				align = "right";
				break;
			case HTMLALIGN_CENTER:
				align = "center";
				break;
			case HTMLALIGN_NONE:
			default:
				align = NULL;
				break;
		}
	
	if(align)
		fprintf(m_pStream," align=%s",align);

	if(layout)
		if(layout->width > 0)
			fprintf(m_pStream," width=%d%%",layout->width);

	fwrite(">",sizeof(char),1,m_pStream);

	if(layout)
		if(layout->tagname)
		{
			fprintf(m_pStream,"<%s",layout->tagname);
			if(strcmp(layout->classname,"")!=0)
				fprintf(m_pStream," class=\"%s\"",layout->classname);
			fwrite(">",sizeof(char),1,m_pStream);
		}

//	if(layout)
//		if(layout->style==HTMLSTYLE_BOLD)
//			fwrite("<b>",sizeof(char),3,m_pStream);

	fwrite(text,sizeof(char),strlen(text),m_pStream);
	
	if(extra)
		fwrite(extra,sizeof(char),strlen(extra),m_pStream);

//	if(layout)
//		if(layout->style==HTMLSTYLE_BOLD)
//			fwrite("</b>",sizeof(char),4,m_pStream);

	if(layout)
		if(layout->tagname)
			fprintf(m_pStream,"</%s>",layout->tagname);
	
	fwrite("</td>\r\n",sizeof(char),7,m_pStream);
}

/*
	TablePutColText()

	Trascrive il testo nella colonna.
*/
void CHtml::TablePutColText(const char* text,HTMLLAYOUT* layout/*=NULL*/)
{
	if(layout)
		if(layout->font.type)
			fprintf(m_pStream,"<font face=\"%s\" size=\"%d\">",layout->font.type,layout->font.size);

	if(layout)
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("<b>",sizeof(char),3,m_pStream);

	fwrite(text,sizeof(char),strlen(text),m_pStream);

	if(layout)
		if(layout->style==HTMLSTYLE_BOLD)
			fwrite("</b>",sizeof(char),4,m_pStream);

	if(layout)
		if(layout->font.type)
			fwrite("</font>",sizeof(char),7,m_pStream);
}
