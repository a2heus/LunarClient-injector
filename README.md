# Hyperion Free Injector

## Description
Hyperion Free Injector is a program that injects a DLL into the `javaw.exe` process (used by Lunar Client).  
This project automatically downloads the DLL from a URL and injects it into the target process.

## Features
✅ Automatic DLL download  
✅ `javaw.exe` process detection (lunar client processà 
✅ Secure DLL injection  
✅ User-friendly interface with colored messages  

## Requirements
- Windows (tested on Windows 10 and 11)
- A compatible C++ compiler (MinGW, MSVC)
- An internet connection to download the DLL

## Compilation
### With MinGW (g++)
```sh
g++ -o injector.exe injector.cpp -lurlmon -static-libgcc -static-libstdc++
```

### With MSVC (cl.exe)
```sh
cl /EHsc injector.cpp /link urlmon.lib
```

## Usage
1. Run `injector.exe`
2. Wait for the DLL to download
3. Ensure Lunar Client is running
4. The injection happens automatically

## Disclaimer
🚨 **This program is provided for educational purposes only.**  
Using modification software in online games may violate their terms of service.  
**The author takes no responsibility for any misuse.**
