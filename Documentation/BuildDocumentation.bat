cd \ARP\XBeamRate\Documentation

doxygen Doxygen.dox

\ARP\BridgeLink\RegFreeCOM\x64\Release\MakeDocMap XBRate
rmdir /S /Q \ARP\BridgeLink\Docs\XBRate\%1\
mkdir \ARP\BridgeLink\Docs\XBRate\%1\
copy \ARP\XBeamRate\Documentation\doc\html\* \ARP\BridgeLink\Docs\XBRate\%1\
copy \ARP\XBeamRate\Documentation\XBRate.dm \ARP\BridgeLink\Docs\XBRate\%1\

