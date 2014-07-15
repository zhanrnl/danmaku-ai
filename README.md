# Danmaku no videojuego róbót (name subject to chænge)

## To setup

You need some flavor of Visual Studio that can compile C++, then open the d3d9CallbackSC2 project in Visual Studio. Copy all of the files in CopyMyContentsToTouhouDir into the directory with Touhou, the d3d9.dll is the hacked version that intercepts calls. Download Includes.zip, Libraries.zip, and DLLs.zip from http://graphics.stanford.edu/~mdfisher/BaseCode.html, install the DLLs in C:\Windows\System32, and add Includes and Libraries directories to your Visual Studio project (for me it's Properties window > VC++ Directories, add to Includes and Libraries respectively).

## To build and run

Build the project (F7) in Visual Studio. It should produce a d3d9Callback.dll in the DebugDLL directory. Copy this DLL into the folder with Touhou. Also make sure all the DLLs from mdfisher's DLL package are also in the same directory as Touhou. Then run Touhou, and if all goes well our program should be intercepting and processing the graphics calls.