
FASTEN YOUR BELT AND READ BEFORE FLY (v.4.2.4, 10/04/2011):
===========================================================
It seems that the MCI driver has changed its default behaviour, at least on the Windows Vista.
The old code used to play .wav files had to be changed because a new, undocumented behaviour of
the MCI driver under Vista and now I noticed there are some differences too when playing .cda 
files (audio CD tracks). The code has been fixed but I dont know if that may cause troubles on
other Windows cartoons when playing .wav and .cda files, if you notice something wrong just let
me know (lpiergentili@yahoo.com).


BUILD NOTES (for developers only):
==================================
WallPaper has been built on (almost) all available Win32 environments, starting from the 
Microsoft Visual C++ 1.5 on Windows 95. The current version has been built on Windows Vista 
SP1 using the Microsoft Visual C++ 9.0 compiler from the Microsoft Visual Studio 2008.

To build the current version:

- Download the source code for WallPaper and for the external libraries from
  http://sourceforge.net/projects/crawlpaper/ and uncompress the distribution files.

- Create a logical drive from the command prompt using the following command:
  subst L: C:\DEV
  The subst'ed directory (C:\DEV is what I use) must exists.

- Go to the root of the subst'ed drive (L:\) and create a \Lib directory. The \Lib directory
  (where all the projects output the .lib/.dll files used by WallPaper), must be in your path
  and must be added to the directories used by the VC++ environment for executables, libraries
  and include files.

- Uncompress all the zip files contained into the the distribution files (see above) on the
  root of the subst'ed drive (L:\), preserving the original pathnames. You will get the following
  directories structure:
  
  \BerkeleyDB
  \libid3tag
  \libjpeg
  \libmad
  \libtiff
  \libz
  \maddll
  \NexgenIPL
  \SelfExtractor
  \srdll
  \unrar
  \unzip550
  \WallBrowser
  \WallPaper
  \WallPaperSFX
  \WallPaperUni
  \zLibdll

  To build the WallPaper program from the command prompt use the makeall.bat script located 
  into the \WallPaper\Distrib directory.

- The source code for the unrar dll is available at http://www.rarlabs.com and is not
  distributed here, so you don't need to build anything, just copy the related .lib/.dll
  files into the \Lib directory of the subst'ed drive (L:).

- There is no source code available for NexgenIPL, so you don't need to build anything,
  just copy the related .lib/.dll files into the \Lib directory of the subst'ed drive (L:).

- Do not use the Microsoft Visual C++ 6.0 to build the source code. There is a known M$ bug
  ("BUG: Modal Dialogs in MFC Regular DLL Cause ASSERT in AfxWndProc") which causes an exception
  when in Release mode. The Microsoft Visual C++ .NET is the only environment which contains a
  version of MFC which does not suck with the message handling.

- WallPaper does not uses C++ templates and all the lists are handled through the CNodeList
  class. The only template implementation you can see appears into third party code snippets.
  Templates are good from a theoretical point of view, but in the real life they suck big time.
  If you disagree, try to debug them and next we can talk about it.

- WallPaper does not uses Unicode and I have no intention to support it, the only Unicode
  references may appears into third party code snippets.

- All the comments (in my code) are in italian, but the code is clear enough, so *don't*
  bother me about this. If you can't understand some code because the language used for the
  comments, probably the best thing you can do is go write some Excel macros.

- The crawler, when running in multithreads mode under the M$ Visual C++ debugger, may hang.
  Such troubles does not happen when running the same debug build under external debuggers,
  like BoundsChecker, or when running the release build.

- With the 4.1.4d version, when defining the _WIN32_WINNT macro to 0x0500 the code used for
  adding a file to the playlist hangs, generating an exception in kernel32.dll.
  At the moment I've no fix for this.

- Expect bugs, expect bugs and expect bugs. If this is not clear enough, keep in mind the
  following: expect bugs.
