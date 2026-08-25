#!/usr/bin/env python3
"""
Compatibility check script for Desk-Emoji PC Client
Ensures Python 3.13 compatibility and required dependencies
"""

import sys
import subprocess
import importlib
import platform

def check_python_version():
    """Check if Python version is compatible"""
    print("Checking Python version...")
    version = sys.version_info
    print(f"Python version: {version.major}.{version.minor}.{version.micro}")
    
    if version.major == 3 and version.minor >= 11:
        print("✓ Python version is compatible")
        return True
    else:
        print("✗ Python version is not compatible. Please use Python 3.11 or higher.")
        return False

def check_package(package_name, import_name=None):
    """Check if a package is installed and importable"""
    if import_name is None:
        import_name = package_name
    
    try:
        importlib.import_module(import_name)
        print(f"✓ {package_name} is installed")
        return True
    except ImportError:
        print(f"✗ {package_name} is not installed")
        return False

def check_required_packages():
    """Check all required packages"""
    print("\nChecking required packages...")
    
    packages = [
        ("setuptools", "setuptools"),
        ("openai", "openai"),
        ("langchain", "langchain"),
        ("pyserial", "serial"),
        ("inquirer", "inquirer"),
        ("SpeechRecognition", "speech_recognition"),
        ("PyAudio", "pyaudio"),
        ("pygame", "pygame"),
        ("customtkinter", "customtkinter"),
        ("Pillow", "PIL"),
        ("bleak", "bleak"),
        ("esptool", "esptool"),
        ("httpx", "httpx"),
        ("typing-extensions", "typing_extensions"),
    ]
    
    missing_packages = []
    for package_name, import_name in packages:
        if not check_package(package_name, import_name):
            missing_packages.append(package_name)
    
    return missing_packages

def check_system_compatibility():
    """Check system-specific compatibility"""
    print("\nChecking system compatibility...")
    
    system = platform.system()
    print(f"Operating system: {system}")
    
    if system == "Windows":
        print("✓ Windows is supported")
    elif system == "Darwin":  # macOS
        print("✓ macOS is supported")
    elif system == "Linux":
        print("✓ Linux is supported")
    else:
        print(f"⚠ Unknown system: {system}")
    
    return True

def install_missing_packages(missing_packages):
    """Install missing packages"""
    if not missing_packages:
        return True
    
    print(f"\nInstalling missing packages: {', '.join(missing_packages)}")
    
    try:
        # Use pip to install packages
        for package in missing_packages:
            print(f"Installing {package}...")
            subprocess.check_call([sys.executable, "-m", "pip", "install", package])
            print(f"✓ {package} installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Failed to install packages: {e}")
        return False

def main():
    """Main compatibility check function"""
    print("Desk-Emoji PC Client - Python 3.13 Compatibility Check")
    print("=" * 60)
    
    # Check Python version
    if not check_python_version():
        sys.exit(1)
    
    # Check system compatibility
    check_system_compatibility()
    
    # Check required packages
    missing_packages = check_required_packages()
    
    if missing_packages:
        print(f"\nMissing packages: {', '.join(missing_packages)}")
        response = input("Would you like to install missing packages? (y/n): ")
        if response.lower() in ['y', 'yes']:
            if not install_missing_packages(missing_packages):
                print("Failed to install some packages. Please install them manually.")
                sys.exit(1)
        else:
            print("Please install missing packages manually using:")
            print(f"pip install {' '.join(missing_packages)}")
            sys.exit(1)
    
    print("\n" + "=" * 60)
    print("✓ All compatibility checks passed!")
    print("✓ Desk-Emoji PC Client is ready to run with Python 3.13")
    print("\nYou can now run the application with:")
    print("python main.py")

if __name__ == "__main__":
    main() 