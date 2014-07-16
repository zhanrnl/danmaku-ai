//
// GlobalContext.cpp
//
// Definition of global context.  The AI must use a global context because it is embeeded inside a DLL
// Written by Matthew Fisher
//

#include "Main.h"

#ifndef ULTRA_FAST
bool g_ReportingEvents = false;
#endif

GlobalContext *g_Context;

void GraphicsInfo::Init()
{
    _Device = NULL;
    _Overlay = NULL;
}

void GraphicsInfo::SetDevice(LPDIRECT3DDEVICE9 Device)
{
    Assert(Device != NULL, "Device == NULL");

    D3D9Base::IDirect3DSwapChain9* pSwapChain;
    
    HRESULT hr = Device->GetSwapChain(0, &pSwapChain);
    Assert(SUCCEEDED(hr), "GetSwapChain failed");
    
    hr = pSwapChain->GetPresentParameters(&_PresentParameters);
    Assert(SUCCEEDED(hr), "GetPresentParameters failed");
    
    pSwapChain->Release();

    CheckWindowSize();

    hr = Device->GetCreationParameters(&_CreationParameters);
    Assert(SUCCEEDED(hr), "GetCreationParameters failed");

    _Device = Device;
}

void GraphicsInfo::CheckWindowSize()
{
    BOOL Success = GetClientRect(_PresentParameters.hDeviceWindow, &_ClientRect);
    Assert(Success != 0, "GetClientRect failed");

    Success = GetWindowRect(_PresentParameters.hDeviceWindow, &_WindowRect);
    Assert(Success != 0, "GetWindowRect failed");

    _WindowDimensions = Vec2i(_ClientRect.right, _ClientRect.bottom);
    g_Context->Constants.Resize(_WindowDimensions);
}

void GraphicsInfo::SetOverlay(ID3D9DeviceOverlay *Overlay)
{
    _Overlay = Overlay;
}

void FileList::Init(const String &BaseDirectory)
{
#if !defined(ULTRA_FAST) && defined(WRITING_TO_FILES)
    Events.open((BaseDirectory + String("Events.txt")).CString());
    Assert.open((BaseDirectory + String("Assert.txt")).CString());
    Thread.open((BaseDirectory + String("Threads.txt")).CString());
    Thought.open((BaseDirectory + String("Thoughts.txt")).CString());
    Actions.open((BaseDirectory + String("Actions.txt")).CString());
#endif
}

void GlobalContext::WriteConsole(const String &Text, RGBColor Color, OverlayPanelType Panel)
{
    if(Graphics.OverlayValid())
    {
        Graphics.Overlay().WriteLine(Text, Color, UINT(Panel));
    }
}

void GlobalContext::ReportAction(const String &Description, RGBColor Color)
{
	if(g_Context->Controller.ConsoleEnabled())
	{
		WriteConsole(Description, Color, OverlayPanelActions);
	}
    g_Context->Files.Actions << Description << endl;
}

void GlobalContext::Init()
{
    srand(UINT(time(NULL)));
    FPSTimer.Start(30.0f);
    Timer.Start();
    Graphics.Init();
    Parameters.LoadFromFile("AIParameters.txt");
    Files.Init(Parameters.OutputFileDirectory);
    Controller.Init();
}
