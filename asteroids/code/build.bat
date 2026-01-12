@echo off

set CommonCompilerFlags=-MT -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4244 -wd4996 -wd4456 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref /FORCE:MULTIPLE raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib kernel32.lib msvcrt.lib /NODEFAULTLIB:LIBCMT

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM delete pdb
del *.pdb > NUL 2> NUL

REM Build shared static library
REM cl %CommonCompilerFlags% -c ..\s3mail\code\s3mail_shared.cpp
REM lib -nologo s3mail_shared.obj /OUT:s3mail_shared.lib

REM Optimzation switches /02
REM echo WAITING FOR PDB > lock.tmp
REM echo Building asteroids.dll...
REM cl %CommonCompilerFlags% ..\s3mail\code\s3mail.cpp s3mail_shared.lib /link /DLL -EXPORT:GameUpdateAndRender -EXPORT:GameHandleKeyPress -EXPORT:GameInitializeUI opengl32.lib -Out:s3mail.dll
REM del lock.tmp

REM echo Building s3mail.exe
cl %CommonCompilerFlags% ..\asteroids\code\win32_asteroids.cpp -Fmwin32_asteroids.map /link  %CommonLinkerFlags%

popd
