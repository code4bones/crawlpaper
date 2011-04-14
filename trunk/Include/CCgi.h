/*
	CCgi.h
	Classe base per interfaccia CGI (deriva da CHtml per poter unificare in un unica classe quanto necessario).
	Luca Piergentili, 29/11/99
	l.pierge@teleline.es
*/
#ifndef _CCGI_H
#define _CCGI_H

#include "CHtml.h"

/*
	CGI_RESPONSE_TYPE
	tipo della risposta (Content, Location, header HTTP)
*/
enum CGI_RESPONSE_TYPE {
	CONTENT_RESPONSE,
	LOCATION_RESPONSE,
	HTTP_RESPONSE
};

/*
	CGI_SUBMISSION_TYPE
	tipo per modalita' invio dati
*/
enum CGI_SUBMISSION_TYPE {
	GET,
	POST,
	UNKNOWTYPE
};

/*
	CGI_ENV
	struttura per le variabili d'ambiente
*/
struct CGI_ENV {
	char* variable;
	char* value;
};

/*
	CGI_FORM
	struttura per i dati presenti nel form
*/
#define CGI_MAX_FORM_FIELD 32
#define CGI_MAX_FORM_VALUE 256
struct CGI_FORM {
	char field[CGI_MAX_FORM_FIELD + 1];
	char value[CGI_MAX_FORM_VALUE + 1];
};

// dimensioni buffers
#define CGI_MAX_ENVVAR_NAME	64		// nome della variabile
#define CGI_MAX_ENVVAR_VALUE	512		// contenuto della variabile
#define CGI_MAX_NPH_CODE		32		// codice HTTP per l'header NPH
#define CGI_MAX_NPH_EXTRA	1024		// codice extra
#define CGI_MAX_COOKIE_SIZE	1024		// cookie
#define CGI_MAX_COOKIE_NAME	128		// nome del cookie
#define CGI_MAX_COOKIE_VALUE	256		// valore del cookie

class CCgi : public CHtml
{
public:
	// costruttore/distruttore
	CCgi(CGI_RESPONSE_TYPE = CONTENT_RESPONSE,const char* = NULL);			// tipo risposta, nome file html output
	~CCgi();

void SendFile(const char* html_file);

	// header HTTP
	virtual void		Header			(const char* = NULL);
	void				SetNphHeader		(const int = 200,const char* = "OK",const char* = NULL);
	
	// dati ambiente
	const char*		GetEnvData		(char*,int);
	const char*		GetEnvValue		(const char*);
	
	// dati form
	const char*		GetFormData		(char*,int);
	const char*		GetFormField		(const char*);

	// dati
	const char*		GetData			(void)											{return(cgi_data);}
	const char*		GetDecodedData		(void)											{return(cgi_decoded_data);}
	int				GetDataLength		(void)											{return(submission_length);}
	int				GetDataPairs		(void)											{return(data_pairs);}

	// cookies
	const char*		SetCookie			(const char*,const char*,const char* = NULL,const char* = NULL,const char* = NULL,int = 0);
	const char*		GetCookie			(const char*);
	const char*		DeleteCookie		(const char*);
	
	// tipo richiesta
	CGI_RESPONSE_TYPE	GetResponseType	(void)											{return(response_type);}
	CGI_SUBMISSION_TYPE	GetSubmissionType	(void)											{return(submission_type);}

private:
	// conversioni
	char				ConvertHex		(char*);
	void				ConvertSpace		(char*);
	void				UnescapeUrl		(char*);
	void				DecodeData		(char*);
	
	// form
	int				CountDataPairs		(char*);
	void				LoadForm			(const char*);

	int				nph_code;
	char				nph_desc[CGI_MAX_NPH_CODE + 1];
	char				nph_extra[CGI_MAX_NPH_EXTRA + 1];
	CGI_RESPONSE_TYPE	response_type;
	int				env_size;
	CGI_ENV*			cgi_env;
	CGI_SUBMISSION_TYPE	submission_type;
	int				submission_length;
	char*			cgi_data;
	char*			cgi_decoded_data;
	int				data_pairs;
	CGI_FORM*			cgi_form;
	char				m_szFileName[_MAX_PATH+1];
};

#endif /* _CCGI_H */
