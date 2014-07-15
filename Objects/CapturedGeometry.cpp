#include "Main.h"

OutputDataStream& operator << (OutputDataStream &File, const PersistentRenderInfo &Info)
{
    File.WriteData(Info);
    return File;
}

InputDataStream& operator >> (InputDataStream &File, PersistentRenderInfo &Info)
{
    File.ReadData(Info);
    return File;
}

void CapturedGeometry::FreeMemory()
{
    _Info.PShaderHash = 0;
    _Info.VShaderHash = 0;
    _Vertices.FreeMemory();
    _Indices.FreeMemory();
}

#if defined(D3DCALLBACK_DLL) || defined(D3DCALLBACK_APP)
void CapturedGeometry::LoadFromD3D(const RenderInfo &Data, const Vector<ProcessedVertex> &ProcessedVertices)
{
    FreeMemory();

    SignalError("Fix this");
    //_Info. = Data.P;

    if(Data.PrimitiveType != D3DPT_TRIANGLELIST && Data.PrimitiveType != D3DPT_TRIANGLESTRIP)
    {
        g_Context->Files.Assert << "Unexpected CapturedGeometry render parameters\n";
        return;
    }
    if(g_Context->Managers.State.CurIBuffer == NULL)
    {
        D3D9Base::LPDIRECT3DINDEXBUFFER9 IndexBuffer = NULL;
        HRESULT hr = g_Context->Graphics.Device()->GetIndices(&IndexBuffer);
        if(FAILED(hr))
        {
            g_Context->Files.Assert << "GetIndices failed in CapturedGeometry::Load\n";
            return;
        }
        if(IndexBuffer != NULL)
        {
            IndexBuffer->Release();
            g_Context->Files.Assert << "CurIBuffer NULL but GetIndices is not in CapturedGeometry::Load\n";
            return;
        }
    }
    
    _Vertices.Allocate(Data.NumVertices);
    for(UINT VertexIndex = 0; VertexIndex < Data.NumVertices; VertexIndex++)
    {
        const Vec4f &TPos = ProcessedVertices[VertexIndex].TransformedProjectionPos;
        _Vertices[VertexIndex] = Vec4f(TPos.x, TPos.y, TPos.z, TPos.w);
    }

    const UINT IndexCount = Data.PrimitiveCount * 3;
    _Indices.Allocate(IndexCount);

    if(Data.PrimitiveType == D3DPT_TRIANGLELIST)
    {
        if(Data.IsIndexed)
        {
            if(Data.IsUserPointer)
            {
                if(Data.Is32bitIndices)
                {
                    const DWORD *UserIndices = (DWORD*)Data.UserIndexData;
                    for(UINT IndexIndex = 0; IndexIndex < IndexCount; IndexIndex++)
                    {
                        _Indices[IndexIndex] = UserIndices[IndexIndex];
                    }
                }
                else
                {
                    const WORD *UserIndices = (WORD*)Data.UserIndexData;
                    for(UINT IndexIndex = 0; IndexIndex < IndexCount; IndexIndex++)
                    {
                        _Indices[IndexIndex] = UserIndices[IndexIndex];
                    }
                }
            }
            else
            {
                IBuffer &IndexBuffer = *g_Context->Managers.State.CurIBuffer;
                for(UINT IndexIndex = 0; IndexIndex < IndexCount; IndexIndex++)
                {
                    UINT CurIndex = IndexBuffer.GetIndex(Data.StartIndex + IndexIndex) - Data.MinIndex;
                    _Indices[IndexIndex] = CurIndex;
                }
            }
        }
        else
        {
            for(UINT IndexIndex = 0; IndexIndex < IndexCount; IndexIndex++)
            {
                _Indices[IndexIndex] = IndexIndex;
            }
        }
    }
    else if(Data.PrimitiveType == D3DPT_TRIANGLESTRIP)
    {
        if(Data.IsIndexed)
        {
            const UINT TriangleStripIndexCount = Data.PrimitiveCount + 2;
            Vector<DWORD> TriangleStripIndices(TriangleStripIndexCount);
            if(Data.IsUserPointer)
            {
                if(Data.Is32bitIndices)
                {
                    const DWORD *UserIndices = (DWORD*)Data.UserIndexData;
                    for(UINT IndexIndex = 0; IndexIndex < TriangleStripIndexCount; IndexIndex++)
                    {
                        TriangleStripIndices[IndexIndex] = UserIndices[IndexIndex];
                    }
                }
                else
                {
                    const WORD *UserIndices = (WORD*)Data.UserIndexData;
                    for(UINT IndexIndex = 0; IndexIndex < TriangleStripIndexCount; IndexIndex++)
                    {
                        TriangleStripIndices[IndexIndex] = UserIndices[IndexIndex];
                    }
                }
            }
            else
            {
                IBuffer &IndexBuffer = *g_Context->Managers.State.CurIBuffer;
                for(UINT IndexIndex = 0; IndexIndex < TriangleStripIndexCount; IndexIndex++)
                {
                    UINT CurIndex = IndexBuffer.GetIndex(Data.StartIndex + IndexIndex) - Data.MinIndex;
                    TriangleStripIndices[IndexIndex] = CurIndex;
                }
            }
            bool OddPrimitive = false;
            for(UINT PrimitiveIndex = 0; PrimitiveIndex < Data.PrimitiveCount; PrimitiveIndex++)
            {
                if(OddPrimitive)
                {
                    _Indices[PrimitiveIndex * 3 + 0] = TriangleStripIndices[PrimitiveIndex + 0];
                    _Indices[PrimitiveIndex * 3 + 1] = TriangleStripIndices[PrimitiveIndex + 2];
                    _Indices[PrimitiveIndex * 3 + 2] = TriangleStripIndices[PrimitiveIndex + 1];
                }
                else
                {
                    _Indices[PrimitiveIndex * 3 + 0] = TriangleStripIndices[PrimitiveIndex + 0];
                    _Indices[PrimitiveIndex * 3 + 1] = TriangleStripIndices[PrimitiveIndex + 1];
                    _Indices[PrimitiveIndex * 3 + 2] = TriangleStripIndices[PrimitiveIndex + 2];
                }
                OddPrimitive = !OddPrimitive;
            }
        }
        else
        {
            bool OddPrimitive = false;
            for(UINT PrimitiveIndex = 0; PrimitiveIndex < Data.PrimitiveCount; PrimitiveIndex++)
            {
                if(OddPrimitive)
                {
                    _Indices[PrimitiveIndex * 3 + 0] = PrimitiveIndex + 0;
                    _Indices[PrimitiveIndex * 3 + 1] = PrimitiveIndex + 2;
                    _Indices[PrimitiveIndex * 3 + 2] = PrimitiveIndex + 1;
                }
                else
                {
                    _Indices[PrimitiveIndex * 3 + 0] = PrimitiveIndex + 0;
                    _Indices[PrimitiveIndex * 3 + 1] = PrimitiveIndex + 1;
                    _Indices[PrimitiveIndex * 3 + 2] = PrimitiveIndex + 2;
                }
                OddPrimitive = !OddPrimitive;
            }
        }
    }
}
#endif

OutputDataStream& operator << (OutputDataStream &File, const Vec3f &V)
{
    File.WriteData(V);
    return File;
}

InputDataStream& operator >> (InputDataStream &File, const Vec3f &V)
{
    File.ReadData(V);
    return File;
}

OutputDataStream& operator << (OutputDataStream &File, const Vec4f &V)
{
    File.WriteData(V);
    return File;
}

InputDataStream& operator >> (InputDataStream &File, const Vec4f &V)
{
    File.ReadData(V);
    return File;
}

void CapturedGeometry::SaveToFile(const String &Filename) const
{
    if(!Utility::FileExists(Filename))
    {
        OutputDataStream File;
        File.WriteData(_Info);
        File << _Vertices;
        File << _Indices;
        File.SaveToFile(Filename);
    }
}

void CapturedGeometry::LoadFromFile(const String &Filename)
{
    InputDataStream File;
    File.LoadFromFile(Filename);
    File.ReadData(_Info);
    File >> _Vertices;
    File >> _Indices;
}

void CapturedGeometry::SaveToMemory(Vector<BYTE> &Data) const
{
    OutputDataStream File;
    File.WriteData(_Info);
    File << _Vertices;
    File << _Indices;
    Data = File.Data();
}

void CapturedGeometry::LoadFromMemory(const Vector<BYTE> &Data)
{
    InputDataStream File;
    File.WrapMemory(Data);
    File.ReadData(_Info);
    File >> _Vertices;
    File >> _Indices;
}

void CapturedGeometry::SaveToMesh(BaseMesh &MeshOut) const
{
    const UINT VertexCount = _Vertices.Length();
    MeshOut.Allocate(VertexCount, _Indices.Length() / 3);

    if(VertexCount > 0)
    {
        Vector<BYTE> VertexTest(VertexCount);
        VertexTest.Clear(1);

        for(UINT VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
        {
            /*Vec4f TPos = _Vertices[VertexIndex];
            if(TPos.x < 0.0f || TPos.x > _Info.Viewport.Width ||
               TPos.y < 0.0f || TPos.y > _Info.Viewport.Height )
            {
                VertexTest[VertexIndex] = 0;
            }
            TPos.x = Utility::Bound(TPos.x, 0.0f, float(_Info.Viewport.Width));
            TPos.y = Utility::Bound(TPos.y, 0.0f, float(_Info.Viewport.Height));
            TPos.z = Utility::Bound(TPos.y, 0.0f, float(_Info.Viewport.MaxZ));
            MeshOut.Vertices()[VertexIndex].Pos = Vec3f(TPos.x, TPos.y, TPos.z);*/
        }

        const UINT FaceCount = _Indices.Length() / 3; 
        Vector<BYTE> FaceTest(FaceCount);
        FaceTest.Clear(1);
        for(UINT FaceIndex = 0; FaceIndex < FaceCount; FaceIndex++)
        {
            for(UINT VertexIndex = 0; VertexIndex < 3; VertexIndex++)
            {
                UINT CurIndex = _Indices[FaceIndex * 3 + VertexIndex];
                MeshOut.Indices()[FaceIndex * 3 + VertexIndex] = CurIndex;
                if(VertexTest[CurIndex] == 0)
                {
                    FaceTest[FaceIndex] = 0;
                }
            }
        }

        if(MeshOut.FaceCount() > 2)
        {
            MeshOut.CullFaces(FaceTest.CArray());
        }
    }
}

#if defined(CAPTURE_VIEWER)
void CapturedGeometry::Prepare(GraphicsDevice &GD)
{
    _Mesh.SetGD(GD);
    SaveToMesh(_Mesh);
    _Color = RGBColor::RandomColor();
    _Mesh.SetColor(RGBColor(240, 50, 50));
    //_Mesh.UnIndex();
    //_Mesh.GCNormals();
}
#endif