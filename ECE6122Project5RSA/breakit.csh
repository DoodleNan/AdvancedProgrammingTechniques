#!/bin/csh
set user = $1
set Expected = HelloFrom.xgu64.GgnHzpNAQVUnJTldWoNldHPaXptEEvFjgCtfAamcnDQP
set Actual = `./BreakRSA  853818337273529323 619113929010929957  679791583000874950 610304293794895010 174741494750572568 790703099242563460 685576056164502755 443940865675272221 552959579882364896 515145625803360982 112915674500491799 611279428395536181`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
