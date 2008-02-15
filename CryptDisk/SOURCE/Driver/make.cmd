
pushd I:\WinDDk\6000\bin
if "%1" EQU "DBG" goto debug_

call setenv I:\WinDDk\6000 fre WNET
goto build_

:debug_
call setenv I:\WinDDk\6000 chk WNET

:build_
popd
build -g -386

