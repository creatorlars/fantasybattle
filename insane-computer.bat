@echo off
REM Launch the PARANOIA "insane computer" app.
REM Forwards any extra arguments through to insane_computer.py.

setlocal
set "SCRIPT_DIR=%~dp0paranoia\insane-computer"
set "PYTHON=%SCRIPT_DIR%\.venv\Scripts\python.exe"

if not exist "%PYTHON%" (
    echo [insane-computer] Could not find venv Python at "%PYTHON%".
    echo Create it with:  py -m venv "%SCRIPT_DIR%\.venv"  ^&^&  "%PYTHON%" -m pip install -r "%SCRIPT_DIR%\requirements.txt"
    exit /b 1
)

pushd "%SCRIPT_DIR%"
"%PYTHON%" insane_computer.py %*
set "EXITCODE=%ERRORLEVEL%"
popd
endlocal & exit /b %EXITCODE%
