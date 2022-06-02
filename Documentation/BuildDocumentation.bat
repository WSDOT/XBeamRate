cd %ARPDIR%\XBeamRate\Documentation

rmdir /S /Q doc
doxygen Doxygen.dox

%ARPDIR%\BridgeLink\RegFreeCOM\x64\Release\MakeDocMap XBRate
rmdir /S /Q %ARPDIR%\BridgeLink\Docs\XBRate\%1\
mkdir %ARPDIR%\BridgeLink\Docs\XBRate\%1\
copy %ARPDIR%\XBeamRate\Documentation\doc\html\* %ARPDIR%\BridgeLink\Docs\XBRate\%1\
copy %ARPDIR%\XBeamRate\Documentation\XBRate.dm %ARPDIR%\BridgeLink\Docs\XBRate\%1\

