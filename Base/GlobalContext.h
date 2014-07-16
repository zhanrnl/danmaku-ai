//
// GlobalContext.h
//
// Definition of global context.  The AI must use a global context because it is embeeded inside a DLL
// Written by Matthew Fisher
//

#ifdef ULTRA_FAST
#define g_ReportingEvents 0
#else
extern bool g_ReportingEvents;    // Currently spewing all reported DirectX calls to files
#endif

class GraphicsInfo
{
public:
    void Init();
    void SetDevice(LPDIRECT3DDEVICE9 Device);
    void CheckWindowSize();
    void SetOverlay(ID3D9DeviceOverlay *Overlay);
    bool OverlayValid()
    {
        return (_Overlay != NULL);
    }
    __forceinline ID3D9DeviceOverlay& Overlay()
    {
        Assert(_Overlay != NULL, "Overlay NULL");
        return *_Overlay;
    }

    //
    // Accessors
    //
    __forceinline LPDIRECT3DDEVICE9 Device()
    {
        Assert(_Device != NULL, "Device == NULL");
        return _Device;
    }
    __forceinline const D3DDEVICE_CREATION_PARAMETERS& CreationParameters()
    {
        return _CreationParameters;
    }
    __forceinline const D3DPRESENT_PARAMETERS& PresentParameters()
    {
        return _PresentParameters;
    }
    __forceinline const Vec2i& WindowDimensions()
    {
        return _WindowDimensions;
    }
    __forceinline const RECT& WindowRect()
    {
        return _WindowRect;
    }

private:
    ID3D9DeviceOverlay              *_Overlay;
    LPDIRECT3DDEVICE9               _Device;
    D3DPRESENT_PARAMETERS           _PresentParameters;
    D3DDEVICE_CREATION_PARAMETERS   _CreationParameters;
    RECT                            _ClientRect, _WindowRect;
    Vec2i                           _WindowDimensions;
};

struct ManagerList
{
    ManagerList() {}

    StateManager            State;
    RenderManager           Render;
    TextureEntryManager     TextureEntry;
    //FrameUnitManager        FrameUnit;
    //FrameHUDManager         FrameHUD;
    //MinimapManager          Minimap;
    //DatabaseManager         Database;
    //ThoughtManager          Thought;
    KeyboardMouseManager    KeyboardMouse;
    //KnowledgeManager        Knowledge;
    //ControlGroupManager     ControlGroup;
};

struct FileList
{
    void Init(const String &BaseDirectory);

    ofstream Events;
    ofstream CurrentFrameRenderEvents;
    ofstream CurrentFrameAllEvents;
	ofstream CurrentFrameWebpageCapture;
    ofstream Assert;
    ofstream Thread;
    ofstream Thought;
    ofstream Actions;
};

struct GlobalContext
{
    GlobalContext(){}
    void Init();
    void WriteConsole(const String &Text, RGBColor Color, OverlayPanelType Panel);
    void ReportAction(const String &Description, RGBColor Color);

    FileList            Files;
    ManagerList         Managers;
    GraphicsInfo        Graphics;
    ManagerController   Controller;
    AIParameters        Parameters;
    FrameTimer          FPSTimer;
    Clock               Timer;
    ScreenConstants     Constants;
};

extern GlobalContext *g_Context;
