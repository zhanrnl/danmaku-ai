//
// Buffers.h
//
// Vertex buffers, index buffers, and textures, and the BufferHasher class which converts
// an array of bytes into a hash and looks it up in a map structure.
//

struct UnitEntry;

struct ProcessedVertex
{
    Vec4f TransformedProjectionPos;
    Vec2f TexCoord;
    RGBColor Diffuse;
};
//const DWORD ProcessedFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
const DWORD ProcessedFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);

//
// Software version of a DirectX vertex buffer
//
class VBuffer
{
public:
    VBuffer(HANDLE _Handle)
    {
        Handle = _Handle;
    }

    void Update(BufferLockData &Data, D3DVERTEXBUFFER_DESC &_Desc);

    D3DVERTEXBUFFER_DESC Desc;  //DirectX description of buffer
    Vector<BYTE> Buffer;        //Raw data of buffer
    HANDLE Handle;              //DirectX handle to the object
};

//
// Software version of a DirectX index buffer
//
class IBuffer
{
public:
    IBuffer(HANDLE _Handle)
    {
        Handle = _Handle;
    }
    __forceinline DWORD GetIndex(UINT Index) const
    {
        if(Desc.Format == D3DFMT_INDEX16)
        {
            return ((WORD *)Buffer.CArray())[Index];
        }
        else
        {
            return ((DWORD *)Buffer.CArray())[Index];
        }
    }
    __forceinline DWORD Length() const
    {
        if(Desc.Format == D3DFMT_INDEX16)
        {
            return Buffer.Length() / sizeof(WORD);
        }
        else
        {
            return Buffer.Length() / sizeof(DWORD);
        }
    }

    void Update(BufferLockData &Data, D3DINDEXBUFFER_DESC &_Desc);

    D3DINDEXBUFFER_DESC Desc;   //DirectX description of buffer
    Vector<BYTE> Buffer;        //Raw data of buffer
    HANDLE Handle;              //DirectX handle to the object
};

typedef map<ULONGLONG, UINT> BufferHashMap;

//
// Hashes an array of bytes and looks up the appropriate hash in a local map
// Used for identifying static structures (such as material colors.)
// Textures have their own hashing function.
//
class BufferHasher
{
public:
    BufferHasher();

    ULONGLONG Hash(const char *Buffer, ULONG BufferLength);

    //
    // Map from hash to known material
    //
    BufferHashMap MaterialMap;
};

//
// Information on a specific texture surface in the running application.  Textures
// are identified as they are loaded.
//
class Texture
{
public:
    Texture(HANDLE Handle)
    {
        _Handle = Handle;
        _BmpHash = 0;
        _Type = RenderUninitialized;
    }

    void Update(const Bitmap &Bmp, const D3DSURFACE_DESC &Desc);

    __forceinline const String& ID() const
    {
        return _ID;
    }
    __forceinline const String& Filename() const
    {
        return _Filename;
    }
    __forceinline const Bitmap& Bmp() const
    {
        return _Bmp;
    }
    __forceinline UINT BmpHash() const
    {
        return _BmpHash;
    }
    __forceinline RenderType Type() const
    {
        return _Type;
    }
    __forceinline UnitEntry* Unit() const
    {
        return _Unit;
    }

private:
    static bool IsFontBitmap(const Bitmap &Bmp);
    static bool IsProceduralBitmap(const Bitmap &Bmp);
	static bool IsScrapBitmap(const Bitmap &Bmp);
	static bool IsMovieBitmap(const Bitmap &Bmp);

    String _ID;              // Name of texture
    String _Filename;        // Filename of texture
    RenderType _Type;        // Type of texture
    D3DSURFACE_DESC _Desc;   // DirectX description of texture
    HANDLE _Handle;          // DirectX handle of texture
    Bitmap _Bmp;             // Data for the top level surface of texture
    UINT _BmpHash;           // Hash of bitmap'
    UnitEntry* _Unit;        // Unit entry if this texture corresponds to a unit
};
