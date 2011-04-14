@echo off
rem
rem	da includere nell'installatore:
rem
rem		wallpaper-fullbin.n.n.n.zip:
rem	
rem		- BerkeleyDB.dll
rem		- BTNexgenIPL32.dll
rem		- changelog.txt
rem		- credits.txt
rem		- license.txt
rem		- maddll.dll
rem		- mfc90.dll
rem		- msvcm90.dll
rem		- msvcp90.dll
rem		- msvcr90.dll
rem		- readme.txt
rem		- srdll.dll
rem		- unrar.dll
rem		- unzip32.dll
rem		- WallBrowser.exe
rem		- WallPaper.exe
rem		- WallPaperUni.exe
rem		- zLibdll.dll
rem	
rem	da includere nella distribuzione per il codice sorgente:
rem
rem		libraries-src-n.n.n.zip:
rem
rem		- BerkeleyDB.zip
rem		- libid3tag.zip
rem		- libjpeg.zip
rem		- libmad.zip
rem		- libtiff.zip
rem		- libz.zip
rem		- maddll.zip
rem		- NexgenIPL.zip
rem		- srdll.zip
rem		- unrar.zip
rem		- unzip550.zip
rem		- zLibdll.zip
rem
rem		wallpaper-src.n.n.n.zip:
rem
rem		- makeall.bat
rem		- changelog.txt
rem		- credits.txt
rem		- license.txt
rem		- readme.txt
rem		- SelfExtractor.zip
rem		- WallBrowser.zip
rem		- WallPaper.zip
rem		- WallPaperSFX.zip
rem		- WallPaperUni.zip
rem

cls
echo.
echo HEY DUDE, READ THIS!
echo This script assumes that you have the MSVC 2008 C++ compiler
echo installed into the following directory:
echo.
echo   C:\Program Files\Microsoft Visual Studio 9.0\VC\bin
echo.
echo if not, hit Ctrl+C and change the pathname.
echo.
echo If you do not want to build libraries use the NOLIB option
echo.
echo.
pause
c:
cd\
cd "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin"
call vcvars32.bat
set VCVARS32CALLED=YES
if "%1"=="NOLIB" goto nolib

rem
rem	librerie/dll
rem

l:
cd\BerkeleyDB\build_win32
call make.bat

l:
cd\zLibDll
call make.bat

l:
cd\libtiff
call make.bat

l:
cd\libjpeg
call make.bat

l:
cd\libz
call make.bat

l:
cd\libid3tag\msvc++
call make.bat

l:
cd\libmad\msvc++
call make.bat

l:
cd\maddll
call make.bat

l:
cd\srdll
call make.bat

l:
cd\unzip550\windll\vc6
call make.bat

rem
rem	per i progetti di cui non viene distribuito il codice sorgente
rem

l:
copy \NexgenIPL\BTNexgenIPL32.lib \Lib
copy \NexgenIPL\BTNexgenIPL32.dll \Lib
copy \unrar\unrar.lib \Lib
copy \unrar\unrar.dll \Lib

rem
rem	copia le dll nella directory per la distribuzione
rem

l:
copy \Lib\BerkeleyDB.dll \WallPaper\Distrib
copy \Lib\zLibDll.dll \WallPaper\Distrib
copy \Lib\maddll.dll \WallPaper\Distrib
copy \Lib\srdll.dll \WallPaper\Distrib
copy \Lib\unzip32.dll \WallPaper\Distrib
copy \Lib\BTNexgenIPL32.dll \WallPaper\Distrib
copy \Lib\unrar.dll \WallPaper\Distrib

rem
rem	ricostruzione dei progetti per i programmi e copia nella directory per la distribuzione
rem
:nolib

l:
cd\SelfExtractor
devenv SelfExtractor.sln /rebuild Release
if errorlevel 1 goto error
copy .\Release\SelfExtractor.exe \WallPaper\Distrib

l:
cd\WallPaperSFX
devenv WallPaperSFX.sln /rebuild Release
if errorlevel 1 goto error
copy .\Release\WallPaperSFX.exe \WallPaper\Distrib

l:
cd\WallPaperUni
devenv WallPaperUni.sln /rebuild Release
if errorlevel 1 goto error
copy .\Release\WallPaperUni.exe \WallPaper\Distrib

l:
cd\WallBrowser
devenv WallBrowser.sln /rebuild Release
if errorlevel 1 goto error
copy .\Release\WallBrowser.exe \WallPaper\Distrib

l:
cd\WallPaper
devenv WallPaper.sln /rebuild Release
if errorlevel 1 goto error
copy .\Release\WallPaper.exe \WallPaper\Distrib

goto done
:error
echo ERROR building projects
pause
:done
L:
cd\WallPaper\distrib
