pwd:=="show defa"
clear:=="set term/wi=132"
cls:=="set term/wi=80"
cd:=="set defa "
eedit:=="edit/edt"
redit:=="edit/read"
prisrv:==$DKB100:[PROMIQA.SSP.PRISRV]prisrv 9
tests:==$DKB100:[PROMIQA.SSP.PRISRV]testsend.exe 
ticksrv:==$DKB100:[PROMIQA.SSP.PRISRV]ticksrv.exe 180 "MCS" 9


define mcsint DISK$PROMREL_DISK:[PROMREL.SSP.F235MCS.MCSINT]
prisrv:==$mcsint:prisrv.exe 9
