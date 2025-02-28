Download python 3.13.2 from https://www.python.org/ftp/python/3.13.2/

### Install python 3.13.2

When the installation is complete, open the command prompt and run the following command to install the `scons` package:
```
python -m pip install scons
```

This project uses wxWidgets 3.2.6 and Visual Studio 2022. The following steps are for building wxWidgets 3.2.6 with Visual Studio 2022.

###  Download wxWidgets 3.2.6
1. Download the source code from the [wxWidgets website](https://www.wxwidgets.org/downloads/).
![Alt text](docs/download-wxwidgets-source-code.png?raw=true "wxwidgets downloading page")

Afterwards, extract the zip file to a folder of your choice. For example, `C:\Users\tinhnong\Downloads\wxWidgets-3.2.6`.

###  Open the Correct Command Prompt**
Before running `nmake`, open the **x64 Native Tools Command Prompt**:  
1. Press **Win + S**, type **x64 Native Tools Command Prompt for VS 2022**, and run it.  
2. Navigate to the wxWidgets build folder:  
   ```sh
   cd C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\build\msw
   ```

3. Run `nmake` again:  
   ```sh
    nmake /f makefile.vc BUILD=debug SHARED=0 RUNTIME_LIBS=static TARGET_CPU=X64
   ```

---

###  Build the minimal example project**

1. Press **Win + S**, type **x64 Native Tools Command Prompt for VS 2022**, and run it.  

2. Navigate to the minimal example project folder:  
   ```sh
   cd C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\samples\minimal
   ```

3. Run `nmake` again:  
   ```sh
    set INCLUDE=C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\include;C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\lib\vc_lib;%INCLUDE%
    set LIB=C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\lib\vc_lib;%LIB%
    nmake -f makefile.vc BUILD=debug RUNTIME_LIBS=static TARGET_CPU=X64 
   ```
4. Run the executable:  
   ```sh
   cd C:\Users\tinhnong\Downloads\wxWidgets-3.2.6\samples\minimal\vc_x64_mswud
   .\minimal.exe
   ```
---
