//
// ManagerController.h
//
// Instance of global objects and the code and the start and end of each frame; calls all corresponding code
// in each of the various managers
// Written by Matthew Fisher
//

class ManagerController
{
public:
    void Init();
    void FrameStart();
    void FrameEnd();
    
    __forceinline double FrameStartTime()
    {
        return _FrameStartTime;
    }
    __forceinline double LastUpdate()
    {
        return _LastUpdate;
    }
    __forceinline UINT FrameIndex()
    {
        return _FrameIndex;
    }
    __forceinline bool AIEnabled()
    {
        return _AIEnabled;
    }
	__forceinline bool ConsoleEnabled()
    {
        return _ConsoleEnabled;
    }
    __forceinline bool InsideFrameCapture()
    {
        return _InsideFrameCapture;
    }
    __forceinline UINT& TexturesSavedThisFrame()
    {
        return _TexturesSavedThisFrame;
    }

private:
	bool	_ConsoleEnabled;
    bool    _AIEnabled;
    UINT    _FrameIndex;             // Number of frames seen so far
    UINT    _TexturesSavedThisFrame;
    double  _FrameStartTime;
    double  _LastUpdate;
    bool    _InsideFrameCapture;
    double  _LastWindowResizeTime;
    Vector<Manager *> _ManagerList;  // List of all managers
};
