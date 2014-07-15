//
// KeyboardMouseManager.h
//
// Interface for sending input to g_Context->Managers.KeyboardMouse progreams
// Written by Matthew Fisher
//

struct KeyboardMouseEvent
{
    KeyboardMouseEvent()
    {
        Modifier = ModifierNone;
    }
    KeyboardMouseEvent(KeyboardMouseEventType _Type)
    {
        Modifier = ModifierNone;
        Type = _Type;
    }
    __forceinline bool IsMouseEvent() const
    {
        return (Type == KeyboardMouseEventMouseMove ||
                Type == KeyboardMouseEventMouseDown ||
                Type == KeyboardMouseEventMouseUp);
    }

    KeyboardMouseEventType Type;

    ModifierType Modifier;
    WORD Key;
    Vec2i MousePos;
    MouseButtonType Button;
};

//
// Interface for sending keyboard and mouse commands to g_Context->Managers.KeyboardMouse programs.  This
// class can handle an infinite number of keys simultaneously, but clicks must be
// seralized and a 2nd click command issued will overwrite the existing one.
//
class KeyboardMouseManager : public Manager
{
public:
    KeyboardMouseManager() {}

    void StartFrame();

    //
    // Keyboard
    //
    void SendInputAlterKey(WORD Key, bool Up);
    void SendKey(WORD Key, bool SupressDebug = false);
    void SendKeyDown(WORD Key, bool SupressDebug = false);
    void SendKeyUp(WORD Key, bool SupressDebug = false);
    void SendCtrlKey(WORD Key, bool SupressDebug = false);
    void SendShiftKey(WORD Key, bool SupressDebug = false);
    void SendString(const String &S);
    
    //
    // Mouse
    //
    void PreemptiveMouseMove(const Vec2i &Pos);
    void Click(const Vec2i &Pos, MouseButtonType Button, ModifierType Modifier);
    void Drag(const Vec2i &StartPos, const Vec2i &EndPos, MouseButtonType Button);
    Vec2i GetMousePos();

    //
    // Wait
    //
    void WaitFrames(UINT Count);
    
    void Step();
    void ExecuteEvent(const KeyboardMouseEvent &Event);

    __forceinline bool Done()
    {
        return (_EventQueue.size() == 0);
    }

private:
    void AlterKey(WORD Key, bool Up);
    queue<KeyboardMouseEvent> _EventQueue;
};
