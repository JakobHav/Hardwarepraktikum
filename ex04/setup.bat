@echo off
echo Creating virtual environment...
python -m venv venv

echo Activating virtual environment...
call venv\Scripts\activate.bat

echo Installing required packages...
pip install -r requirements.txt

echo.
echo Setup complete! Virtual environment created and packages installed.
echo.
echo To activate the environment in future sessions, run:
echo   venv\Scripts\activate.bat
echo.
echo To run the visualization:
echo   python plot.py
echo.
pause
