
pushd C:\WINDDK\3790\bin
if "%1" EQU "DBG" goto debug_
call setenv.bat C:\WINDDK\3790 fre
goto build_

:debug_
call setenv.bat C:\WINDDK\3790 chk

:build_
popd
build -386

