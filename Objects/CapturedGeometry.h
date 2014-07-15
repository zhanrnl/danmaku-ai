//
// Information about a single render event that is saved between frames
//
struct PersistentRenderInfo
{
    UINT VShaderHash, PShaderHash;
};

OutputDataStream& operator << (OutputDataStream &File, const PersistentRenderInfo &Info);
InputDataStream& operator >> (InputDataStream &File, PersistentRenderInfo &Info);

class CapturedGeometry
{
public:
    CapturedGeometry()
    {
        Utility::ZeroMem(_Info);
    }

    void FreeMemory();

#if defined(D3DCALLBACK_DLL) || defined(D3DCALLBACK_APP)
    void LoadFromD3D(const RenderInfo &Data, const Vector<ProcessedVertex> &ProcessedVertices);
#endif
    
    void SaveToFile(const String &Filename) const;
    void LoadFromFile(const String &Filename);

    void SaveToMemory(Vector<BYTE> &Data) const;
    void LoadFromMemory(const Vector<BYTE> &Data);
    
    void SaveToMesh(BaseMesh &MeshOut) const;

    __forceinline const PersistentRenderInfo& Info() const
    {
        return _Info;
    }
    __forceinline PersistentRenderInfo& Info()
    {
        return _Info;
    }
    __forceinline const Vector<UINT>& Indices() const
    {
        return _Indices;
    }
    __forceinline const Vector<Vec4f>& Vertices() const
    {
        return _Vertices;
    }

#if defined(CAPTURE_VIEWER)
    void Prepare(GraphicsDevice &GD);
    __forceinline const D3D9Mesh& GetMesh()
    {
        return _Mesh;
    }
#endif

private:
    PersistentRenderInfo _Info;
    Vector<Vec4f> _Vertices;
    Vector<UINT> _Indices;

#if defined(CAPTURE_VIEWER)
    RGBColor _Color;
    D3D9Mesh _Mesh;
#endif
};
