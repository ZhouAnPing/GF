pwd:=="show defa"
clear:=="set term/wi=132"
cls:=="set term/wi=80"
cd:=="set defa "
eedit:=="edit/edt"
redit:=="edit/read"
mcsint:==$PROMPRI1:[PROMF3.SSP.F235MCS.MCSINT]mcsintsrv 9
tests:== $PROMPRI1:[PROMF3.SSP.F235MCS.MCSINT]testsend.exe 
