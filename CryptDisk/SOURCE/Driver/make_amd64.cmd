
pushd C:\WinDDK\7600.16385.1\bin\
if "%1" EQU "DBG" goto debug_

call setenv C:\WinDDK\7600.16385.1 fre WIN7 x64
set CRYPTO_LIB_BIN_DIR=..\..\..\CryptoLib\Release\amd64
goto build_

:debug_
call setenv C:\WinDDK\7600.16385.1 chk WIN7 x64
set CRYPTO_LIB_BIN_DIR=..\..\..\CryptoLib\Debug\amd64

:build_
popd
set CRYPTO_LIB_DIR=..\..\..\CryptoLib\Crypto
build -g -amd64

