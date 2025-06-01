@echo off


echo 1. Compile and load the test project
echo 2. Flash the test firmware
echo Enter 1 or 2 to select:

set /p choice=Choice: 

if "%choice%"=="1" (
    echo Compiling and loading the main project...
    cd "./test"
    
    rmdir /S /Q "./build"
    
    idf.py -p COM14 set-target esp32s3 build flash monitor 
) else if "%choice%"=="2" (
    cd "./test"
    
    idf.py -p COM14 flash monitor
) else (
    echo Invalid choice. Please enter 1 or 2.
)

pause
