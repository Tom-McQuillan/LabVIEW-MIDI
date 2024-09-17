@echo off

REM Change to current directory
cd /D "%~dp0"

REM Setup the MSVC Build Environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Define output DLL name
set DLL_NAME=MIDItoLabVIEW.dll

echo ************************************************************
echo Building the file: MIDItoLabVIEW.cpp
echo ************************************************************

REM Build
REM Specify Linking against LabVIEW's labviewv.lib and user32.lib (adjust as needed)
cl.exe /Zi /LD /EHsc /I "C:\Program Files\National Instruments\LabVIEW 2020\cintools" /I "3rd-party" /I "include" /Fe:%DLL_NAME% "MIDItoLabVIEW.cpp" /link /LIBPATH "C:\Program Files\National Instruments\LabVIEW 2020\cintools\labviewv.lib" "user32.lib"

IF %ERRORLEVEL% NEQ 0 (
    echo Error occurred during the build process.
    exit /b %ERRORLEVEL%
)

echo Build successful. Output: %DLL_NAME%

exit
