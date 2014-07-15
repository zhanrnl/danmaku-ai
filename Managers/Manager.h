//
// Manager.h
//
// Absctract class for all global objects that affect the AI state
// Written by Matthew Fisher
//

class Manager
{
public:
    virtual void Init()          {}  // Called once on DLL load
    virtual void ResetNewGame()  {}  // Called once each time a new map loads
    virtual void StartFrame()    {}  // Called once at beginning of each frame
    virtual void EndFrame()      {}  // Called once at end of each frame
    virtual void Dump(ofstream &DumpFile) {}  // Called on user request to report state to DumpFile
};
