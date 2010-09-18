
pushd C:\WinDDK\7600.16385.1\bin\
if "%1" EQU "DBG" goto debug_

call setenv C:\WinDDK\7600.16385.1 fre WNET
goto build_

:debug_
call setenv C:\WinDDK\7600.16385.1 chk WNET

:build_
popd
set CRYPTO_LIB_DIR=..\..\..\CryptoLib\Crypto
build -g -386

