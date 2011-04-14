/*
	CWildCards.cpp
	Classe base per gestione wildcards (?*).
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com

	Incapsulato il codice originale di Florian Schintke (vedi sotto) e aggiunto
	il codice per gestire piu' wildcards in un unica stringa (separate da spazio).
	
	Implementation of the UN*X wildcards in C. So they are available in a portable way and can be used
	whereever	needed.
	Author(s):
	Florian Schintke (schintke@cs.tu-berlin.de)
	Dates:
	First editing: unknown, but before 04/02/1997
	Last Change  : 11/10/1998
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#ifdef _WINDOWS
  #include "window.h"
#endif
#include <stdio.h>
#include <string.h>
#include "strings.h"
#include <stdlib.h>
#include "CNodeList.h"
#include "CWildCards.h"

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
	CWildCard
*/
CWildCard::CWildCard(char* pString)
{
	int nLen = strlen(pString);
	nLen = nLen <= 0 ? 1 : nLen;
	m_pString = new char[nLen+1];
	strcpyn(m_pString,pString,nLen+1);
}

/*
	~CWildCard()
*/
CWildCard::~CWildCard()
{
	if(m_pString)
		delete [] m_pString,m_pString = NULL;
}

/*
	Match()
*/
BOOL CWildCards::Match(const char* wildcard,const char* test)
{
	BOOL bMatch = FALSE;

	if(m_bIgnoreSpaces)
	{
		int nLen = 0;
		char* pWildcard = NULL;
		char* pTest = NULL;
		
		if(m_bIgnoreCase)
		{
			nLen = strlen(wildcard)+1;
			pWildcard = new char[nLen];
			if(pWildcard)
			{
				strcpyn(pWildcard,wildcard,nLen);
				strupr(pWildcard);
			}
			nLen = strlen(test)+1;
			pTest = new char[nLen];
			if(pTest)
			{
				strcpyn(pTest,test,nLen);
				strupr(pTest);
			}
		}
		else
		{
			pWildcard = (char*)wildcard;
			pTest = (char*)test;
		}

		bMatch = (BOOL)match(pWildcard,pTest);

		if(nLen > 0)
		{
			if(pWildcard)
				delete [] pWildcard,pWildcard = NULL;
			if(pTest)
				delete [] pTest,pTest = NULL;
		}
	}
	else
	{		
		m_listWildcards.DeleteAll();

		int nLen = strlen(wildcard);
		nLen = nLen <= 0 ? 1 : nLen;
		char* pString = new char[nLen+1];

		int nTestLen = 0;
		char* pTest;

		if(m_bIgnoreCase)
		{
			nTestLen = strlen(test)+1;
			pTest = new char[nTestLen];
			if(pTest)
			{
				strcpyn(pTest,test,nTestLen);
				strupr(pTest);
			}
			else
				nTestLen = 0;
		}
		else
			pTest = (char*)test;

		if(pString && pTest)
		{
			CWildCard* pWildCard;
			ITERATOR iter;
			
			memset(pString,'\0',nLen+1);

			int i,n;
			for(i=0,n=0; wildcard[i]; i++)
			{
				if(wildcard[i]!=' ')
					pString[n++] = wildcard[i];
				else
				{
					pString[n] = '\0';
					n = 0;
					pWildCard = new CWildCard(pString);
					if(pWildCard)
						m_listWildcards.Add(pWildCard);
					memset(pString,'\0',nLen+1);
				}
			}

			if(n > 0 && pString[0]!='\0')
			{
				pWildCard = new CWildCard(pString);
				if(pWildCard)
					m_listWildcards.Add(pWildCard);
				memset(pString,'\0',nLen+1);
			}

			if((iter = m_listWildcards.First())!=(ITERATOR)NULL)
			{
				do
				{
					pWildCard = (CWildCard*)iter->data;
					if(pWildCard)
					{
						strcpyn(pString,pWildCard->GetString(),nLen+1);
						if(m_bIgnoreCase)
							strupr(pString);
						if(match(pString,pTest))
						{
							bMatch = TRUE;
							break;
						}
					}

					iter = m_listWildcards.Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			}
			
			delete [] pString,pString = NULL;
		}

		if(nTestLen > 0 && pTest)
			delete [] pTest,pTest = NULL;

		m_listWildcards.DeleteAll();
	}

	return(bMatch);
}

/*
	match()
	
	0 if wildcard does not match test
	1 if wildcard matches test
*/
int CWildCards::match(const char* wildcard,const char* test)
{
	if(strcmp(wildcard,test)==0)
		return(1);

  int fit = 1;
  
  for (; ('\000' != *wildcard) && (1 == fit) && ('\000' != *test); wildcard++)
    {
      switch (*wildcard)
        {
        case '[':
	  wildcard++; /* leave out the opening square bracket */ 
          fit = set (&wildcard, &test);
	  /* we don't need to decrement the wildcard as in case */
	  /* of asterisk because the closing ] is still there */
          break;
        case '?':
          test++;
          break;
        case '*':
          fit = asterisk (&wildcard, &test);
	  /* the asterisk was skipped by asterisk() but the loop will */
	  /* increment by itself. So we have to decrement */
	  wildcard--;
          break;
        default:
          fit = (int) (*wildcard == *test);
          test++;
        }
    }
  while ((*wildcard == '*') && (1 == fit)) 
    /* here the teststring is empty otherwise you cannot */
    /* leave the previous loop */ 
    wildcard++;
  return (int) ((1 == fit) && ('\0' == *test) && ('\0' == *wildcard));
}

/*
	set()
	
	scans a set of characters and returns 0 if the set mismatches at this
	position in the teststring and 1 if it is matching wildcard is set to
	the closing ] and test is unmodified if mismatched and otherwise the
	char pointer is pointing to the next character
*/
int CWildCards::set(const char** wildcard,const char** test)
{
  int fit = 0;
  int negation = 0;
  int at_beginning = 1;

  if ('!' == **wildcard)
    {
      negation = 1;
      (*wildcard)++;
    }
  while ((']' != **wildcard) || (1 == at_beginning))
    {
      if (0 == fit)
        {
          if (('-' == **wildcard) 
              && ((*(*wildcard - 1)) < (*(*wildcard + 1)))
              && (']' != *(*wildcard + 1))
	      && (0 == at_beginning))
            {
              if (((**test) >= (*(*wildcard - 1)))
                  && ((**test) <= (*(*wildcard + 1))))
                {
                  fit = 1;
                  (*wildcard)++;
                }
            }
          else if ((**wildcard) == (**test))
            {
              fit = 1;
            }
        }
      (*wildcard)++;
      at_beginning = 0;
    }
  if (1 == negation)
    /* change from zero to one and vice versa */
    fit = 1 - fit;
  if (1 == fit) 
    (*test)++;

  return (fit);
}

/*
	asterisk()
	
	scans an asterisk
*/
int CWildCards::asterisk(const char** wildcard,const char** test)
{
  /* Warning: uses multiple returns */

  int fit = 1;
  char *oldwildcard, *oldtest;

  /* erase the leading asterisk */
  (*wildcard)++; 
  while (('\000' != (**test))
	 && (('?' == **wildcard) 
	     || ('*' == **wildcard)))
    {
      if ('?' == **wildcard) 
	(*test)++;
      (*wildcard)++;
    }
  /* Now it could be that test is empty and wildcard contains */
  /* aterisks. Then we delete them to get a proper state */
  while ('*' == (**wildcard))
    (*wildcard)++;

  if (('\0' == (**test)) && ('\0' != (**wildcard)))
    return (fit = 0);
  if (('\0' == (**test)) && ('\0' == (**wildcard)))
    return (fit = 1); 
  else
    {
      /* Neither test nor wildcard are empty!          */
      /* the first character of wildcard isn't in [*?] */
      oldwildcard = (char*)*wildcard;
      oldtest = (char*)*test;
      do 
	{
	  if (0 == match(*wildcard, (*test)))
	    oldtest++;
	  *wildcard = oldwildcard;
	  *test = oldtest;
	  /* skip as much characters as possible in the teststring */
	  /* stop if a character match occurs */
	  while (((**wildcard) != (**test)) 
		 && ('['  != (**wildcard))
		 && ('\0' != (**test)))
	    (*test)++;
	  oldwildcard = (char*)*wildcard;
	  oldtest = (char*)*test;
	}
      while ((('\0' != **test))? 
	     (0 == match (*wildcard, (*test))) 
	     : (0 != (fit = 0)));
      if (('\0' == **test) && ('\0' == **wildcard))
	fit = 1;
      return (fit);
    }
}

/*
testwildcards.main
==================
#! /bin/sh

# The simplest input
./testwildcards "" "" "t"

# Get a simple error
./testwildcards "" "a" "f"

# Single character match
./testwildcards "a" "a" "t"

# Single character mismatch
./testwildcards "a" "b" "f"

# Simple question mark test
./testwildcards "?" "b" "t"

# Doubled question mark test
./testwildcards "??" "bc" "t"

# question mark followed by a character
# matches
./testwildcards "?c" "bc" "t"

# question mark follows a character
# matches
./testwildcards "b?" "bc" "t"

# Simple set test that matches
./testwildcards "[a-z]" "b" "t"

# Simple set test that mismatches
./testwildcards "[A-Z]" "b" "f"

# Simple asterisk test that matches
./testwildcards "*" "a" "t"

# Simple asterisk test that matches
./testwildcards "**" "a" "t"

# asterisk that finishes the wildcard while the 
# remaining teststring is empty
./testwildcards "*" "" "t"

# several asterisks 
# matches
./testwildcards "*bc*hij" "abcdfghij" "t"

# several asterisks 
# mismatches
./testwildcards "*b*a*" "b" "f"

# several asterisks 
# mismatches
./testwildcards "*bc*hik" "abcdfghij" "f"

# asterisk that follows a string that would match the teststring 
# without the asterisk too.
# matches
./testwildcards "abc*" "abc" "t"

# doubled asterisk that follows a string that would match the 
# teststring without the asterisks.
# matches
./testwildcards "abc**" "abc" "t"

# Simple negated set test that matches
# The following set would be a syntax error: [!] because you don't need
# to quote the ! in a single character set, we can assume that after 
# an ! in the first position an element of the set comes. In this case
# the ]. The program would search for the ] that closes the set now.
# If you want to have an exclamation mark in a set do not write it in
# the beginning.
./testwildcards "[!]]" "!" "t"
./testwildcards "[!]]" "]" "f"

# Simple negated set test that matches
./testwildcards "[!abc]" "d" "t"

# Simple negated set test that mismatches
./testwildcards "[!abc]" "b" "f"

# asterisk with following question marks that matches
./testwildcards "*???" "abc" "t"

# asterisk with following question marks that mismatches
./testwildcards "*???" "ab" "f"

# asterisk with following question marks that mismatches
./testwildcards "*???" "abcd" "t"

# asterisk with following question mark and asterisk that mismatches
./testwildcards "*?*" "abcd" "t"

# asterisk with following characters that matches 
./testwildcards "*bc" "abc" "t"

# asterisk with following characters that mismatches 
./testwildcards "*cc" "abc" "f"

# asterisk that finishes the wildcard that does not match
./testwildcards "[a-c]*" "d" "f"

# asterisk followed by a set that matches
./testwildcards "*[a-e]" "d" "t"

# asterisk followed by a symbol that does not match followed by a 
# second asterisk 
# mismatches
./testwildcards "*a*" "de" "f"

# asterisk followed by a set that mismatches
./testwildcards "*[a-c]" "d" "f"

# range that mismatches because
# the character in the teststring is too big 
./testwildcards "[a-c]" "d" "f"

# range that mismatches because
# the character in the teststring is to too small 
./testwildcards "[b-d]" "a" "f"

# set with a ] in it as the first element that matches 
./testwildcards "[]abc]" "b" "t"

# set with a ] in it as the first element that mismatches 
./testwildcards "[]abc]" "d" "f"

# set with a pseudo range in it that isn't one because the
# first character in the range is greater than the last one (of the range) 
# matching
./testwildcards "[z-a]" "-" "t"

# set with a pseudo range in it that isn't one because the
# first character in the range is greater than the last one (of the range) 
# mismatching
./testwildcards "[z-a]" "b" "f"

# set with a - that could be a range but after that the closing ]
# matching
./testwildcards "[A-]" "-" "t"

# set with a - that could be a range but after that the closing ]
# mismatching
./testwildcards "[A-]" "]" "f"

# set with a - as the first element that matches
# Note: theoretically [-a could be a range from [ to a
./testwildcards "[-a]" "-" "t"

# set with a - as the first element that matches
# Note: theoretically [-[ could not be a range from [ to [
#       because the beginning [ doesn't have a smaller code than the [
./testwildcards "[-[]" "-" "t"

# set with a - as the first element that matches
./testwildcards "[-]" "-" "t"

# negated set with a - as the first element that matches
./testwildcards "[!-b]" "a" "t"

# negated set with a - as the first element that matches
./testwildcards "[!-b]" "-" "f"

# set with a - as the first element that mismatches
./testwildcards "[-b]" "a" "f"

# set followed by normal characters
# matches
./testwildcards "[a-g]lorian" "florian" "t"

# set followed by normal characters, but not there in the teststring
# mismatches
./testwildcards "[a-g]*rorian" "f" "f"

# Jenny Sowden <SowdenJ@deluxe-data.co.uk> found a bug in 
# version 1.0 of the routine while processing this pattern. 
# Thank you. (fixed since 10.11.1998 in version 1.1 of wildcards.c)
./testwildcards "*???*" "123" "t"

testwildcards.report
====================
(tt) ['', '']
(ff) ['', 'a']
(tt) ['a', 'a']
(ff) ['a', 'b']
(tt) ['?', 'b']
(tt) ['??', 'bc']
(tt) ['?c', 'bc']
(tt) ['b?', 'bc']
(tt) ['[a-z]', 'b']
(ff) ['[A-Z]', 'b']
(tt) ['*', 'a']
(tt) ['**', 'a']
(tt) ['*', '']
(tt) ['*bc*hij', 'abcdfghij']
(ff) ['*b*a*', 'b']
(ff) ['*bc*hik', 'abcdfghij']
(tt) ['abc*', 'abc']
(tt) ['abc**', 'abc']
(tt) ['[!]]', '!']
(ff) ['[!]]', ']']
(tt) ['[!abc]', 'd']
(ff) ['[!abc]', 'b']
(tt) ['*???', 'abc']
(ff) ['*???', 'ab']
(tt) ['*???', 'abcd']
(tt) ['*?*', 'abcd']
(tt) ['*bc', 'abc']
(ff) ['*cc', 'abc']
(ff) ['[a-c]*', 'd']
(tt) ['*[a-e]', 'd']
(ff) ['*a*', 'de']
(ff) ['*[a-c]', 'd']
(ff) ['[a-c]', 'd']
(ff) ['[b-d]', 'a']
(tt) ['[]abc]', 'b']
(ff) ['[]abc]', 'd']
(tt) ['[z-a]', '-']
(ff) ['[z-a]', 'b']
(tt) ['[A-]', '-']
(ff) ['[A-]', ']']
(tt) ['[-a]', '-']
(tt) ['[-[]', '-']
(tt) ['[-]', '-']
(tt) ['[!-b]', 'a']
(ff) ['[!-b]', '-']
(ff) ['[-b]', 'a']
(tt) ['[a-g]lorian', 'florian']
(ff) ['[a-g]*rorian', 'f']
(tt) ['*???*', '123']
*/
