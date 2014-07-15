//
// comparison function for HANDLE needed by STL map structure
//
struct HandleHash
{
    size_t operator()( const HANDLE& h ) const
    {
        return size_t(h);
    }
    bool operator()(HANDLE h1, HANDLE h2) const
    {
        return DWORD(h1) < DWORD(h2);
    }
    const static size_t bucket_size = 4;
    const static size_t min_buckets = 8;
};

//
// Integer-quad used by shaders
//
struct Int4
{
    Int4() {}
    Int4(UINT _x, UINT _y, UINT _z, UINT _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
    UINT x, y, z, w;
};

#ifdef USE_FUNCTION_DEBUGGER

extern ofstream g_Context->Files.Assert;
struct DebugHelper
{
    DebugHelper(const String &_Name)
    {
        Name = _Name;
        g_Context->Files.Assert << "Enter " << Name << endl;
    }
    ~DebugHelper()
    {
        g_Context->Files.Assert << "Exit " << Name << endl;
    }
    String Name;
};
#define Debugger(x) DebugHelper D(x)

#else

#define Debugger(x)

#endif