@echo off
echo Creating virtual environment...
python -m venv .venv


echo Installing dependencies...
@echo on
.venv\Scripts\python.exe -m pip install -r requirements.txt --trusted-host pypi.tuna.tsinghua.edu.cn -i https://pypi.tuna.tsinghua.edu.cn/simple

@echo off
echo Starting application...
.venv\Scripts\python.exe main.py

pause