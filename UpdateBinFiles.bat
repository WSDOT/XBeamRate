REM - Script to prepare for Release

SET BINTARGET=bin
SET REGFREECOM=\ARP\BridgeLink\RegFreeCOM


copy /Y %REGFREECOM%\Win32\Release\XBeamRate*.dll	%BINTARGET%\Win32\
copy /Y %REGFREECOM%\x64\Release\XBeamRate*.dll		%BINTARGET%\x64\
copy /Y \ARP\BridgeLink\XBeamRate.chm	              	%BINTARGET%\

copy /Y Images\*.png					%BINTARGET%\Images\