
pushd C:\WinDDK\7600.16385.1\bin\
if "%1" EQU "DBG" goto debug_

call setenv C:\WinDDK\7600.16385.1 fre WNET
set CRYPTO_LIB_BIN_DIR=..\..\..\CryptoLib\Release\x86
goto build_

:debug_
call setenv C:\WinDDK\7600.16385.1 chk WNET
set CRYPTO_LIB_BIN_DIR=..\..\..\CryptoLib\Debug\x86

:build_
popd
set CRYPTO_LIB_DIR=..\..\..\CryptoLib\Crypto
build -g -x86

