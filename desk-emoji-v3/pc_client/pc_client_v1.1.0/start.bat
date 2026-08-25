python -m venv .venv
call .venv\Scripts\activate
pip install -r requirements.txt --trusted-host pypi.tuna.tsinghua.edu.cn -i https://pypi.tuna.tsinghua.edu.cn/simple
python main.py