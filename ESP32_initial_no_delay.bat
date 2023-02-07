@echo off
d:
cd 001.Programs\Auto test r394\Auto Test
set /p Comport=Input ESP32_comport:
:loop
echo Type 1~4 as control menu
echo 1.On relay
echo 2.Off relay
echo 3.Press servo
echo 4.Release servo

set /p Command=The menu is:
If "%Command%"=="1" (goto OnRly)
If "%Command%"=="2" (goto OffRly)
If "%Command%"=="3" (goto PressSvo)
If "%Command%"=="4" (goto ReleaseSvo)

:OnRly
echo On Relay
python ESP32_control.py --ESP32_comport %Comport% --ESP32_command RLY01001
goto loop

:OffRly
echo Off Relay
python ESP32_control.py --ESP32_comport %Comport% --ESP32_command RLY01002
goto loop

:PressSvo
echo Press servo
python ESP32_control.py --ESP32_comport %Comport% --ESP32_command SVO01001
goto loop

:ReleaseSvo
echo Release servo
python ESP32_control.py --ESP32_comport %Comport% --ESP32_command SVO01002
goto loop