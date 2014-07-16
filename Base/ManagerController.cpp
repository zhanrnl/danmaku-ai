//
// ManagerController.cpp
//
// Instance of global objects and the code and the start and end of each frame; calls all corresponding code
// in each of the various managers
// Written by Matthew Fisher
//

#include "Main.h"

//
// Anit-debugger reference
// http://www.symantec.com/connect/articles/windows-anti-debug-reference
//

void ManagerController::Init()
{
    _FrameStartTime = 0.0;
    _LastWindowResizeTime = 0.0;
    _FrameIndex = 0;
    _InsideFrameCapture = false;
    _AIEnabled = true;
	_ConsoleEnabled = true;
    _TexturesSavedThisFrame = 0;

    //
    // Fill ManagerList with entries
    //
    _ManagerList.PushEnd(&g_Context->Managers.State);
    _ManagerList.PushEnd(&g_Context->Managers.Render);
    _ManagerList.PushEnd(&g_Context->Managers.TextureEntry);
    //_ManagerList.PushEnd(&g_Context->Managers.Database);
    //_ManagerList.PushEnd(&g_Context->Managers.FrameUnit);
    //_ManagerList.PushEnd(&g_Context->Managers.FrameHUD);
    //_ManagerList.PushEnd(&g_Context->Managers.Minimap);
    _ManagerList.PushEnd(&g_Context->Managers.KeyboardMouse);
    //_ManagerList.PushEnd(&g_Context->Managers.Knowledge);
    //_ManagerList.PushEnd(&g_Context->Managers.ControlGroup);
    //_ManagerList.PushEnd(&g_Context->Managers.Thought);
    
    //
    // Initialize all managers
    //
    for(UINT i = 0; i < _ManagerList.Length(); i++)
    {
        _ManagerList[i]->Init();
    }
}

//
// Prepare objects for a new frame
//
void ManagerController::FrameStart()
{
    Debugger("ManagerController::FrameStart");
    
	if(Pressed(GetAsyncKeyState(KEY_F11)))
    {
#ifndef ULTRA_FAST
        g_ReportingEvents = true;
#endif
        _InsideFrameCapture = true;
        
		FileList &Files = g_Context->Files;
        Files.Events << "Capturing frame " << _FrameIndex << endl;

        Files.CurrentFrameAllEvents.close();
        Files.CurrentFrameRenderEvents.close();
		if(WebpageCaptureMode)
		{
			Files.CurrentFrameWebpageCapture.close();
		}
        
        Files.CurrentFrameAllEvents.clear();
        Files.CurrentFrameRenderEvents.clear();
		if(WebpageCaptureMode)
		{
			Files.CurrentFrameWebpageCapture.clear();
		}
        
        Files.CurrentFrameAllEvents.open((g_Context->Parameters.ScreenCaptureDirectory + String("FrameEvents") + String(_FrameIndex) + String(".txt")).CString());
        Files.CurrentFrameRenderEvents.open((g_Context->Parameters.ScreenCaptureDirectory + String("FrameRenderEvents") + String(_FrameIndex) + String(".txt")).CString());
		if(WebpageCaptureMode)
		{
			Files.CurrentFrameWebpageCapture.open((g_Context->Parameters.ScreenCaptureDirectory + String("Capture") + String(_FrameIndex) + String(".html")).CString());
			Files.CurrentFrameWebpageCapture << "<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"default.css\" /></head><body>\n";
			Files.CurrentFrameWebpageCapture << "<h4 style=\"margin-left: 10px;\">Final Render Target:</h4><br>\n";
			String FinalFrameFilename = String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Final.png");
			Files.CurrentFrameWebpageCapture << "<img style=\"margin-left: 10px;\" src=\"" << FinalFrameFilename << "\" /><br><br><h4 style=\"margin-left: 10px;\">All Render Events:</h4><br>\n";
			//Files.CurrentFrameWebpageCapture << "<table border=\"1\" style=\"text-align: center; margin-left: 8px;\"><tr><td>Index</td><td>Render Target</td><td>Texture 0, RGB</td><td>Texture 0, Alpha</td><td>Texture Name</td><td>Render Type</td><td>Primitive Count</td><td>Vertex Shader</td><td>Pixel Shader</td><td>Coordinates</td><td>Text</td></tr>\n";
            Files.CurrentFrameWebpageCapture << "<table border=\"1\" style=\"text-align: center; margin-left: 8px;\"><tr><td>Index</td><td>Render Target</td><td>Texture 0, RGB</td><td>Texture 0, Alpha</td><td>Texture Name</td><td>Render Type</td><td>Primitive Count</td><td>Vertex Shader</td><td>Pixel Shader</td><td>Coordinates</td></tr>\n";
		}
        
        Files.CurrentFrameRenderEvents << "Index\tTexture0 ID\tTexture0 Filename\tTexture1 ID\tTexture1 Filename\tType\tPrims\tVShader\tPShader\tCenter\tMin\tMax\tText\n";
        
		g_Context->WriteConsole(String("Capturing frame ") + String(_FrameIndex), RGBColor::Red, OverlayPanelSystem);
    }
    
	g_Context->Graphics.Overlay().ClearPanel(OverlayPanelStatus);
	if(_ConsoleEnabled)
	{
		g_Context->Graphics.Overlay().ClearPanel(OverlayPanelProduction);
	}

    //
    // Update frame time
    //
    double CurFrameTime = g_Context->Timer.Elapsed();
    _FrameStartTime = CurFrameTime;

    //
    // Advance frame timer
    //
    g_Context->FPSTimer.Frame();
    g_Context->WriteConsole(String("spf: ") + String(UINT(g_Context->FPSTimer.SPF() * 1000)) + String(" ms"), RGBColor::Yellow, OverlayPanelStatus);
    if(g_Context->Parameters.VideoCaptureMode)
    {
        _ConsoleEnabled = false;
        if(!_AIEnabled)
        {
            g_Context->WriteConsole(String("Capturing Video"), RGBColor::Yellow, OverlayPanelStatus);
        }
    }

    //
    // Reset all objects for this frame
    //
    for(UINT i = 0; i < _ManagerList.Length(); i++)
    {
        _ManagerList[i]->StartFrame();
    }
}

//
// Performs actions based upon this frame's render calls
//
void ManagerController::FrameEnd()
{
    Debugger("ManagerController::FrameEnd");

    g_Context->Files.Thread << "End Scene, Frame=" << _FrameIndex << endl;
    
    //g_Context->Managers.FrameUnit.MatchHealthBars();
    //g_Context->Managers.FrameUnit.UpdateBattlePosition();

    //
    // Finalize all objects for this frame
    //
    for(UINT i = 0; i < _ManagerList.Length(); i++)
    {
        _ManagerList[i]->EndFrame();
    }

    if(!g_Context->Managers.KeyboardMouse.Done())
    {
        //
        // Perform the next step in a click
        //
        g_Context->Managers.KeyboardMouse.Step();
    }

    if(_InsideFrameCapture) //|| Pressed(GetAsyncKeyState(VK_F10))
    {
        //
        // Have each manager report its state to the dump file
        //
        /*ofstream CurrentFrameDump;
        CurrentFrameDump.open((g_Context->Parameters.ScreenCaptureDirectory + String("FrameDump ") + String(_FrameIndex) + String(".txt")).CString());
        for(UINT i = 0; i < _ManagerList.Length(); i++)
        {
            _ManagerList[i]->Dump(CurrentFrameDump);
        }*/

        if(_InsideFrameCapture)
        {
#ifndef ULTRA_FAST
            g_ReportingEvents = false;
#endif
            _InsideFrameCapture = false;
            g_Context->Files.CurrentFrameAllEvents.close();
            g_Context->Files.CurrentFrameRenderEvents.close();
			g_Context->Files.CurrentFrameWebpageCapture.close();

            /*Bitmap Bmp;
            DWORD Temp;
            g_Context->Managers.Render.CaptureCurrentRenderTarget(Bmp, Temp, 4096, false);
            Bmp.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(_FrameIndex) + String("Final.png"));*/
        }
    }

	if(Pressed(GetAsyncKeyState(KEY_F6)))
    {
		g_Context->Graphics.Overlay().ClearPanel(0);
		g_Context->Graphics.Overlay().ClearPanel(1);
		g_Context->Graphics.Overlay().ClearPanel(2);
		g_Context->Graphics.Overlay().ClearPanel(3);
		g_Context->Graphics.Overlay().ClearPanel(4);
		_ConsoleEnabled = false;
    }
    if(Pressed(GetAsyncKeyState(KEY_NUMPADMULTIPLY)))
    {
		_ConsoleEnabled = true;
        if(!_AIEnabled)
        {
            _AIEnabled = true;
            if(!g_Context->Parameters.VideoCaptureMode)
            {
                g_Context->WriteConsole("AI Enabled", RGBColor::Green, OverlayPanelSystem);
            }
        }
    }
    if(Pressed(GetAsyncKeyState(KEY_NUMPADDIVIDE)))
    {
		_ConsoleEnabled = true;
        if(_AIEnabled)
        {
            _AIEnabled = false;
            if(!g_Context->Parameters.VideoCaptureMode)
            {
                g_Context->WriteConsole("AI Disabled", RGBColor::Red, OverlayPanelSystem);
            }
        }
    }

    if(_TexturesSavedThisFrame != 0)
    {
        g_Context->WriteConsole(String("Saved textures: ") + String(_TexturesSavedThisFrame), RGBColor::Yellow, OverlayPanelSystem);
        _TexturesSavedThisFrame = 0;
    }

    if(_FrameStartTime - _LastWindowResizeTime >= 5.0f)
    {
        _LastWindowResizeTime = _FrameStartTime;
        g_Context->Graphics.CheckWindowSize();
		const Vec2i &CurrentSize = g_Context->Graphics.WindowDimensions();
		bool FullScreenSize = (CurrentSize == Vec2i(1920, 1200) || CurrentSize == Vec2i(2560, 1600) || CurrentSize == Vec2i(1920, 1080) || CurrentSize == Vec2i(1920, 1080) || CurrentSize == Vec2i(1280, 720));
        if(!FullScreenSize && !g_Context->Constants.ValidSize() && g_Context->Parameters.AllowWindowResizing)
        {
            RECT ClientRect, ScreenRect;
            HWND Window = g_Context->Graphics.PresentParameters().hDeviceWindow;
            GetClientRect(Window, &ClientRect);
            GetWindowRect(Window, &ScreenRect);
            int BorderWidth = ScreenRect.right - ScreenRect.left - ClientRect.right;
            int BorderHeight = ScreenRect.bottom - ScreenRect.top - ClientRect.bottom;
            
            UINT NewWindowWidth = 1024;
            UINT NewWindowHeight = 768;

            if(g_Context->Parameters.VideoCaptureMode)
            {
                NewWindowWidth = 1280;
                NewWindowHeight = 720;
            }
            MoveWindow(Window, ScreenRect.left, ScreenRect.top, NewWindowWidth + BorderWidth, NewWindowHeight + BorderHeight, TRUE);
        }
    }
    _FrameIndex++;
}
