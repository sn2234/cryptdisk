
pushd C:\WinDDK\6001.18002\bin\
if "%1" EQU "DBG" goto debug_

call setenv C:\WinDDK\6001.18002 fre WNET
goto build_

:debug_
call setenv C:\WinDDK\6001.18002 chk WNET

:build_
popd
build -g -386

