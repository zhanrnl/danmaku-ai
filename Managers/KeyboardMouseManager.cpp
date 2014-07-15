//
// KeyboardMouseManager.cpp
//
// Interface for sending input to g_Context->Managers.KeyboardMouse progreams
// Written by Matthew Fisher
//

#include "Main.h"

void KeyboardMouseManager::SendInputAlterKey(WORD Key, bool Up)
{
    INPUT Input;
    Input.type = INPUT_KEYBOARD;
    Input.ki.wVk = Key;
    Input.ki.wScan = 0;
    if(Up)
    {
        Input.ki.dwFlags = KEYEVENTF_KEYUP;
    }
    else
    {
        Input.ki.dwFlags = 0;
    }
    Input.ki.time = 0;
    Input.ki.dwExtraInfo = 0;
    SendInput(1, &Input, sizeof(INPUT));
}

void KeyboardMouseManager::StartFrame()
{

}

/*
enum KeyboardMouseEventType
{
    KeyboardMouseEventKeyDown,
    KeyboardMouseEventKeyPress,
    KeyboardMouseEventKeyUp,
    KeyboardMouseEventMouseMove,
    KeyboardMouseEventMouseDown,
    KeyboardMouseEventMouseUp,
};
*/

void KeyboardMouseManager::PreemptiveMouseMove(const Vec2i &Pos)
{
    POINT ScreenPoint;
    ScreenPoint.x = Pos.x;
    ScreenPoint.y = Pos.y;
    ClientToScreen(g_Context->Graphics.CreationParameters().hFocusWindow, &ScreenPoint);
    SetCursorPos(ScreenPoint.x, ScreenPoint.y);
}

Vec2i KeyboardMouseManager::GetMousePos()
{
    POINT CurPos;
    GetCursorPos(&CurPos);
    return Vec2i(CurPos.x, CurPos.y);
}

__forceinline void PostKeyMessage(HWND Window, UINT Msg, WORD Key)
{
    UINT ScanCode = MapVirtualKey(Key, MAPVK_VK_TO_VSC);
    LPARAM lParam = (LPARAM)(ScanCode << 16);
    DWORD Flags = 0x00000001;
    if(Msg == WM_KEYUP)
    {
        Flags = 0xC0000001;
    }
    PostMessage(Window, Msg, Key, lParam | Flags);
}

void KeyboardMouseManager::ExecuteEvent(const KeyboardMouseEvent &Event)
{
    HWND Window = g_Context->Graphics.CreationParameters().hFocusWindow;
    if(Event.Type == KeyboardMouseEventKeyDown || Event.Type == KeyboardMouseEventKeyUp || Event.Type == KeyboardMouseEventKeyPress)
    {
        if(Event.Type == KeyboardMouseEventKeyDown || Event.Type == KeyboardMouseEventKeyPress)
        {
            PostKeyMessage(Window, WM_KEYDOWN, Event.Key);//PostMessage(Window, WM_KEYDOWN, Event.Key, lParam | 0x00000001);
        }
        if(Event.Type == KeyboardMouseEventKeyUp || Event.Type == KeyboardMouseEventKeyPress)
        {
            PostKeyMessage(Window, WM_KEYUP, Event.Key);
        }
    }
    else if(Event.Type == KeyboardMouseEventMouseMove || Event.Type == KeyboardMouseEventMouseDown || Event.Type == KeyboardMouseEventMouseUp)
    {
        //Vec2i CurMousePos = GetMousePos();
        //LPARAM lParam = MAKELPARAM(_CurMousePos.x, _CurMousePos.y);
        LPARAM lParam = MAKELPARAM(Event.MousePos.x, Event.MousePos.y);

        //PostMessage(Window, WM_NCHITTEST, 0, lParam); // ignores shift & ctrl key state
        //PostMessage(Window, WM_MOUSEMOVE, 0, lParam); // ignores shift & ctrl key state
        
        //RECT WindowRect;
        //GetWindowRect(Window, &WindowRect);

        POINT ScreenPoint;
        ScreenPoint.x = Event.MousePos.x;
        ScreenPoint.y = Event.MousePos.y;
        ClientToScreen(Window, &ScreenPoint);

        SetCursorPos(ScreenPoint.x, ScreenPoint.y);
        
        DWORD ModifierFlags = 0;
        if(Event.Modifier == ModifierShift)
        {
             ModifierFlags = MK_SHIFT;
             if(Event.Type == KeyboardMouseEventMouseMove)
             {
                SendInputAlterKey(VK_LSHIFT, false);
             }
             //PostKeyMessage(Window, WM_KEYDOWN, VK_LSHIFT);
        }
        if(Event.Modifier == ModifierCtrl)
        {
             ModifierFlags = MK_CONTROL;
             if(Event.Type == KeyboardMouseEventMouseMove)
             {
                SendInputAlterKey(VK_LCONTROL, false);
             }
        }

        if(Event.Button == MouseButtonLeft)
        {
            if(Event.Type == KeyboardMouseEventMouseDown)
            {
                PostMessage(Window, WM_LBUTTONDOWN, MK_LBUTTON | ModifierFlags, lParam);
            }
            if(Event.Type == KeyboardMouseEventMouseUp)
            {
                PostMessage(Window, WM_LBUTTONUP, MK_LBUTTON | ModifierFlags, lParam);
            }
        }
        else if(Event.Button == MouseButtonRight)
        {
            if(Event.Type == KeyboardMouseEventMouseDown)
            {
                PostMessage(Window, WM_RBUTTONDOWN, MK_RBUTTON | ModifierFlags, lParam);
            }
            if(Event.Type == KeyboardMouseEventMouseUp)
            {
                PostMessage(Window, WM_RBUTTONUP, MK_RBUTTON | ModifierFlags, lParam);
            }
        }

        if(Event.Modifier == ModifierShift)
        {
            //PostKeyMessage(Window, WM_KEYUP, VK_LSHIFT);
            if(Event.Type == KeyboardMouseEventMouseUp)
            {
                SendInputAlterKey(VK_LSHIFT, true);
            }
        }
        if(Event.Modifier == ModifierCtrl)
        {
            if(Event.Type == KeyboardMouseEventMouseUp)
            {
                SendInputAlterKey(VK_LCONTROL, true);
            }
        }
    }
}

void KeyboardMouseManager::Step()
{
    if(_EventQueue.size() == 0)
    {
        g_Context->Files.Assert << "KeyboardMouseManager::Step called when no events in queue\n";
        return;
    }
    if(_EventQueue.front().Type == KeyboardMouseEventMouseMove)
    {
        POINT CurMousePos, TargetMousePos;
        GetCursorPos(&CurMousePos);

        TargetMousePos.x = _EventQueue.front().MousePos.x;
        TargetMousePos.y = _EventQueue.front().MousePos.y;
        ClientToScreen(g_Context->Graphics.CreationParameters().hFocusWindow, &TargetMousePos);

        if(CurMousePos.x == TargetMousePos.x && CurMousePos.y == TargetMousePos.y)
        {
            //g_Context->Files.Thread << "Skipping MouseMove\n";
            _EventQueue.pop();
            if(_EventQueue.size() == 0)
            {
                return;
            }
        }
    }
    const KeyboardMouseEvent &FirstEvent = _EventQueue.front();
    if(FirstEvent.Type == KeyboardMouseEventFrameBarrier)
    {
        _EventQueue.pop();
    }
    else if(FirstEvent.IsMouseEvent())
    {
        bool CurEventIsMouseDown = (FirstEvent.Type == KeyboardMouseEventMouseDown);

        ExecuteEvent(FirstEvent);
        _EventQueue.pop();

        if(CurEventIsMouseDown && _EventQueue.size() > 0 && _EventQueue.front().Type == KeyboardMouseEventMouseUp)
        {
            ExecuteEvent(_EventQueue.front());
            _EventQueue.pop();
        }
    }
    else
    {
        bool Done = false;
        while(!Done)
        {
            const KeyboardMouseEvent &CurrentEvent = _EventQueue.front();
            ExecuteEvent(CurrentEvent);
            _EventQueue.pop();

            
            if(_EventQueue.size() == 0)
            {
                Done = true;
            }
            else
            {
                KeyboardMouseEventType NewType = _EventQueue.front().Type;
                if(NewType == KeyboardMouseEventMouseDown || NewType == KeyboardMouseEventMouseUp || NewType == KeyboardMouseEventFrameBarrier)
                {
                    Done = true;
                }
            }
        }
    }
}

void KeyboardMouseManager::WaitFrames(UINT Count)
{
    /*BYTE KeyStates[256];
    GetKeyboardState((PBYTE)&KeyStates);
    KeyStates[KEY_G] = 255;
    SetKeyboardState((PBYTE)&KeyStates);*/

    KeyboardMouseEvent BarrierEvent(KeyboardMouseEventFrameBarrier);
    for(UINT Index = 0; Index < Count; Index++)
    {
        _EventQueue.push(BarrierEvent);
    }
}

void KeyboardMouseManager::SendKey(WORD Key, bool SupressDebug)
{

    KeyboardMouseEvent KeyPressEvent(KeyboardMouseEventKeyPress);
    KeyPressEvent.Key = Key;
    _EventQueue.push(KeyPressEvent);
}

void KeyboardMouseManager::SendKeyDown(WORD Key, bool SupressDebug)
{
    KeyboardMouseEvent KeyDownEvent(KeyboardMouseEventKeyDown);
    KeyDownEvent.Key = Key;
    _EventQueue.push(KeyDownEvent);
}

void KeyboardMouseManager::SendKeyUp(WORD Key, bool SupressDebug)
{
    KeyboardMouseEvent KeyUpEvent(KeyboardMouseEventKeyUp);
    KeyUpEvent.Key = Key;
    _EventQueue.push(KeyUpEvent);
}

void KeyboardMouseManager::SendCtrlKey(WORD Key, bool SupressDebug)
{
    KeyboardMouseEvent CtrlDownEvent(KeyboardMouseEventKeyDown);
    CtrlDownEvent.Key = KEY_CTRL;
    _EventQueue.push(CtrlDownEvent);

    KeyboardMouseEvent KeyPressEvent(KeyboardMouseEventKeyPress);
    KeyPressEvent.Key = Key;
    _EventQueue.push(KeyPressEvent);

    KeyboardMouseEvent CtrlUpEvent(KeyboardMouseEventKeyUp);
    CtrlUpEvent.Key = KEY_CTRL;
    _EventQueue.push(CtrlUpEvent);
}

void KeyboardMouseManager::SendShiftKey(WORD Key, bool SupressDebug)
{
    KeyboardMouseEvent ShiftDownEvent(KeyboardMouseEventKeyDown);
    ShiftDownEvent.Key = KEY_SHIFT;
    _EventQueue.push(ShiftDownEvent);

    KeyboardMouseEvent KeyPressEvent(KeyboardMouseEventKeyPress);
    KeyPressEvent.Key = Key;
    _EventQueue.push(KeyPressEvent);

    KeyboardMouseEvent ShiftUpEvent(KeyboardMouseEventKeyUp);
    ShiftUpEvent.Key = KEY_SHIFT;
    _EventQueue.push(ShiftUpEvent);
}

void KeyboardMouseManager::SendString(const String &S)
{
    g_Context->Files.Thread << "Send String: " << S << endl;
    UINT Len = S.Length();
    for(UINT LetterIndex = 0; LetterIndex < Len; LetterIndex++)
    {
        char C = S[LetterIndex];
        if(C >= 'A' && C <= 'Z')
        {
            SendShiftKey(C - 'A' + KEY_A, true);
        }
        if(C >= 'a' && C <= 'z')
        {
            SendKey(C - 'a' + KEY_A, true);
        }
        if(C >= '0' && C <= '9')
        {
            SendKey(C - '0' + KEY_0, true);
        }
        if(C == '(')
        {
            SendShiftKey(KEY_9, true);
        }
        if(C == ')')
        {
            SendShiftKey(KEY_0, true);
        }
        if(C == '\"')
        {
            SendShiftKey(VK_OEM_7, true);
        }
        if(C == '\'')
        {
            SendKey(VK_OEM_7, true);
        }
        if(C == ';')
        {
            SendKey(VK_OEM_1, true);
        }
        if(C == ':')
        {
            SendShiftKey(VK_OEM_1, true);
        }
        if(C == ',')
        {
            SendKey(VK_OEM_COMMA, true);
        }
        if(C == '.')
        {
            SendKey(VK_OEM_PERIOD, true);
        }
        if(C == ' ')
        {
            SendKey(KEY_SPACE, true);
        }
        if(C == '\n')
        {
            SendKey(KEY_ENTER, true);
        }
        if(C == '/')
        {
            SendKey(VK_DIVIDE, true);
        }
        if(C == '-')
        {
            SendKey(VK_SUBTRACT, true);
        }
    }
}

void KeyboardMouseManager::Click(const Vec2i &Pos, MouseButtonType Button, ModifierType Modifier)
{
    KeyboardMouseEvent MouseMoveEvent(KeyboardMouseEventMouseMove);
    MouseMoveEvent.MousePos = Pos;
    MouseMoveEvent.Modifier = Modifier;
    _EventQueue.push(MouseMoveEvent);

    KeyboardMouseEvent MouseDownEvent(KeyboardMouseEventMouseDown);
    MouseDownEvent.Button = Button;
    MouseDownEvent.MousePos = Pos;
    MouseDownEvent.Modifier = Modifier;
    _EventQueue.push(MouseDownEvent);

    KeyboardMouseEvent MouseUpEvent(KeyboardMouseEventMouseUp);
    MouseUpEvent.Button = Button;
    MouseUpEvent.MousePos = Pos;
    MouseUpEvent.Modifier = Modifier;
    _EventQueue.push(MouseUpEvent);

    //KeyboardMouseEvent FrameBarrierEvent(KeyboardMouseEventFrameBarrier);
    //_EventQueue.push(FrameBarrierEvent);
}

void KeyboardMouseManager::Drag(const Vec2i &StartPos, const Vec2i &EndPos, MouseButtonType Button)
{
    KeyboardMouseEvent MouseMoveEvent(KeyboardMouseEventMouseMove);
    MouseMoveEvent.MousePos = StartPos;
    _EventQueue.push(MouseMoveEvent);

    KeyboardMouseEvent MouseDownEvent(KeyboardMouseEventMouseDown);
    MouseDownEvent.Button = Button;
    MouseDownEvent.MousePos = StartPos;
    _EventQueue.push(MouseDownEvent);

    //KeyboardMouseEvent MouseMoveEvent(KeyboardMouseEventMouseMove);
    //MouseMoveEvent.MousePos = Pos;
    //_EventQueue.push(MouseMoveEvent);

    KeyboardMouseEvent MouseUpEvent(KeyboardMouseEventMouseUp);
    MouseUpEvent.Button = Button;
    MouseUpEvent.MousePos = EndPos;
    _EventQueue.push(MouseUpEvent);
}