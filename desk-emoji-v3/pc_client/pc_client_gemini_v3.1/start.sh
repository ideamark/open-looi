#!/bin/bash

echo "Desk-Emoji PC Client - Starting with Python 3.11 compatibility check..."

# Check if virtual environment exists
if [ ! -d ".venv" ]; then
    echo "Creating virtual environment..."
    python3.11 -m venv .venv
fi

# Activate virtual environment
source .venv/bin/activate

# Install/update dependencies
echo "Installing/updating dependencies..."
python -m pip install --upgrade pip
python -m pip install -r requirements.txt --trusted-host pypi.tuna.tsinghua.edu.cn -i https://pypi.tuna.tsinghua.edu.cn/simple

# Run compatibility check
echo "Running compatibility check..."
python compatibility_check.py

# Start the application
echo "Starting Desk-Emoji PC Client..."
python main.py
