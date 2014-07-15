#include "Main.h"

void StateManager::Dump(ofstream &DumpFile)
{
    DumpFile << endl << "StateManager Dump\n";
    DumpFile << "Texture memory:\t" << TextureMemorySize() << " bytes\n";
    DumpFile << "Texture count:\t" << Textures.size() << endl;
}

void StateManager::Init()
{
    NullTexture = new Texture(NULL);

    CurVShader = NULL;
    CurPShader = NULL;
    CurIBuffer = NULL;

    D3DSURFACE_DESC Desc;
    Desc.Width = 1;
    Desc.Height = 1;

    Bitmap BlackBmp(1, 1);
    BlackBmp.Clear();

    NullTexture->Update(BlackBmp, Desc);
    
    for(UINT i = 0; i < MaxTextures; i++)
    {
        CurTexture[i] = NullTexture;
    }

    for(UINT i = 0; i < MaxVShaderFloatConstants; i++)
    {
        VShaderFloatConstants[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < MaxVShaderBoolConstants; i++)
    {
        VShaderBoolConstants[i] = FALSE;
    }
    for(UINT i = 0; i < MaxVShaderIntConstants; i++)
    {
        VShaderIntConstants[i] = Int4(0, 0, 0, 0);
    }
    VertexShaderSimulator.LoadConstantsFromDevice();
    VertexShaderSimulator.ResetRegisters();
    VertexShaderSimulator.InitalizeHashes();
    //VertexShaderSimulator.OutputNewShaderList();

    for(UINT i = 0; i < MaxPShaderFloatConstants; i++)
    {
        PShaderFloatConstants[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < MaxPShaderBoolConstants; i++)
    {
        PShaderBoolConstants[i] = FALSE;
    }
    for(UINT i = 0; i < MaxPShaderIntConstants; i++)
    {
        PShaderIntConstants[i] = Int4(0, 0, 0, 0);
    }
}

//
// Total bytes of texture memory resident in the state manager
//
UINT StateManager::TextureMemorySize()
{
    UINT Total = 0;
    TextureMap::const_iterator Iterator = Textures.begin();
    while(Iterator != Textures.end())
    {
        Total += Iterator->second->Bmp().Width() * Iterator->second->Bmp().Height() * 4;
        Iterator++;
    }
    return Total;
}

//
// Unlock event.  Records the texture in the associated handle either by creating
// a new texture or modifying the existing one.  Ignores small textures.
//
Texture* StateManager::UnlockTexture(D3DSURFACE_DESC &Desc, Bitmap &Bmp, HANDLE TextureHandle)
{
    TextureMap::iterator Iterator = Textures.find(TextureHandle);
    if(Iterator == Textures.end())
    {
        Texture *NewTexture = new Texture(TextureHandle);
        NewTexture->Update(Bmp, Desc);
        Textures[TextureHandle] = NewTexture;
        return NewTexture;
    }
    else
    {
        Iterator->second->Update(Bmp, Desc);
        return Iterator->second;
    }
}

//
// Unlock event.  Records the vertex buffer in the associated handle either by creating
// a new vertex buffer or modifying the existing one.
//
void StateManager::LockVBuffer(BufferLockData &Data, D3DVERTEXBUFFER_DESC &Desc)
{
    VBufferMap::iterator Iterator = VBuffers.find(Data.Handle);
    if(Iterator == VBuffers.end())
    {
        if(!Data.Create)
        {
            g_Context->Files.Assert << "VBuffer lock called before create\n";
        }
        VBuffer *NewBuffer = new VBuffer(Data.Handle);
        NewBuffer->Update(Data, Desc);
        VBuffers[Data.Handle] = NewBuffer;
    }
    else
    {
        if(Data.Create)
        {
            //g_Context->Files.Assert << "VBuffer reused\n";
        }
        //Assert(!Data.Create, "Create called twice on VBuffer");
        Iterator->second->Update(Data, Desc);
    }
}

//
// Unlock event.  Records the index buffer in the associated handle either by creating
// a new index buffer or modifying the existing one.
//
void StateManager::LockIBuffer(BufferLockData &Data, D3DINDEXBUFFER_DESC &Desc)
{
    IBufferMap::iterator Iterator = IBuffers.find(Data.Handle);
    if(Iterator == IBuffers.end())
    {
        if(!Data.Create)
        {
            g_Context->Files.Assert << "IBuffer lock called before create\n";
        }
        IBuffer *NewBuffer = new IBuffer(Data.Handle);
        NewBuffer->Update(Data, Desc);
        IBuffers[Data.Handle] = NewBuffer;
    }
    else
    {
        if(Data.Create)
        {
            //g_Context->Files.Assert << "IBuffer reused\n";
        }
        //Assert(!Data.Create, "Create called twice on IBuffer");
        Iterator->second->Update(Data, Desc);
    }
}

//
// Destroy event.  Frees associated memory.  Returns false if no texture with given handle exists.
//
bool StateManager::DestroyTexture(HANDLE Handle)
{
    TextureMap::iterator Iterator = Textures.find(Handle);
    if(Iterator == Textures.end())
    {
        return false;
    }
    else
    {
        //g_Context->Files.Assert << " Destroyed Texture: " << Handle << endl;
        for(UINT i = 0; i < MaxTextures; i++)
        {
            if(g_Context->Managers.State.CurTexture[i] == Iterator->second)
            {
                g_Context->Managers.State.CurTexture[i] = g_Context->Managers.State.NullTexture;
            }
        }
        delete Iterator->second;
        Textures.erase(Iterator);
        return true;
    }
}

//
// Destroy event.  Frees associated memory.  Returns false if no vertex buffer with given handle exists.
//
bool StateManager::DestroyVBuffer(HANDLE Handle)
{
    VBufferMap::iterator Iterator = VBuffers.find(Handle);
    if(Iterator == VBuffers.end())
    {
        return false;
    }
    else
    {
        g_Context->Files.Assert << " Destroyed VBuffer: " << Handle << endl;
        delete Iterator->second;
        VBuffers.erase(Iterator);
        return true;
    }
}

//
// Destroy event.  Frees associated memory.  Returns false if no index buffer with given handle exists.
//
bool StateManager::DestroyIBuffer(HANDLE Handle)
{
    IBufferMap::iterator Iterator = IBuffers.find(Handle);
    if(Iterator == IBuffers.end())
    {
        return false;
    }
    else
    {
        g_Context->Files.Assert << " Destroyed IBuffer: " << Handle << endl;
        delete Iterator->second;
        IBuffers.erase(Iterator);
        return true;
    }
}

//
// SetTransform event.  Updates the internal matrices.
//
void StateManager::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	Matrix4 world = Matrix4(*pMatrix);
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetTransform ";
    }
    switch(State)
    {
    case D3DTS_WORLD:
        //Matrices.World = Matrix4(*pMatrix);
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "World updated\n";
            g_Context->Files.CurrentFrameAllEvents << "World: " << world[3][0] << ' ' << world[3][1] << ' ' << world[3][2] << endl;
        }
        break;

    case D3DTS_VIEW:
        //Matrices.View = Matrix4(*pMatrix);
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "View\n";
        }
        break;

    case D3DTS_PROJECTION:
        //Matrices.Perspective = Matrix4(*pMatrix);
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Projection\n";
        }
        break;

    case D3DTS_TEXTURE0:
    case D3DTS_TEXTURE1:
    case D3DTS_TEXTURE2:
    case D3DTS_TEXTURE3:
    case D3DTS_TEXTURE4:
    case D3DTS_TEXTURE5:
    case D3DTS_TEXTURE6:
    case D3DTS_TEXTURE7:
        //TextureMatrices[UINT(State) - UINT(D3DTS_TEXTURE0)] = Matrix4(*pMatrix);
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture" << UINT(State) - UINT(D3DTS_TEXTURE0) << endl;
        }
        break;

    default:
        g_Context->Files.Assert << "Unknown Transform State: " << State << endl;
    }
    //TotalMatrix = Matrices.TotalMatrix();
}

//
// SetMaterial event.  Updates the internal material.
//
void StateManager::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    if(pMaterial == NULL)
    {
        g_Context->Files.Assert << "SetMaterial pMaterial is NULL\n";
    }
    else
    {
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Material Hash: " << Hasher.Hash((const char *)pMaterial, sizeof(D3DMATERIAL9))
                        << " Diffuse: " << pMaterial->Diffuse.r << ' ' << pMaterial->Diffuse.g << ' ' << pMaterial->Diffuse.b << ' ' << pMaterial->Diffuse.a << ' '
                        << " Ambient: " << pMaterial->Ambient.r << ' ' << pMaterial->Ambient.g << ' ' << pMaterial->Ambient.b << ' ' << pMaterial->Ambient.a << ' '
                        << " Specular: " << pMaterial->Specular.r << ' ' << pMaterial->Specular.g << ' ' << pMaterial->Specular.b << ' ' << pMaterial->Specular.a << ' '
                        << " Emissive: " << pMaterial->Emissive.r << ' ' << pMaterial->Emissive.g << ' ' << pMaterial->Emissive.b << ' ' << pMaterial->Emissive.a << endl;
        }
        CurMaterial = *pMaterial;
    }
}

//
// SetLight event.  Updates the internal light copy.
//
void StateManager::SetLight(UINT Index, CONST D3DLIGHT9* pLight)
{
    if(Index < MaxLights)
    {
        CurLight[Index] = *pLight;
    }
    else
    {
        g_Context->Files.Assert << "SetLight Index > MaxLights\n";
    }
}

//
// SetViewport event.  Updates the internal viewport.
//
void StateManager::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    CurViewport = *pViewport;
}

//
// SetTexture event.  SurfaceHandles is the vector of handles for all mipmap surfaces of the texture
// (we consider only the topmost to be of interest.)  HandleCount is the length of SurfaceHandles.
//
void StateManager::SetTexture(DWORD Stage, HANDLE *SurfaceHandles, UINT HandleCount)
{
    if(Stage >= MaxTextures && SurfaceHandles[0] != NULL)
    {
        g_Context->Files.Assert << "SetTexture Stage >= MaxTextures (" << Stage << ")\n";
        return;
    }

    if(SurfaceHandles[0] == NULL)
    {
        CurTexture[Stage] = NullTexture;
        return;
    }

    TextureMap::const_iterator TMapIterator;
    TMapIterator = Textures.find(SurfaceHandles[0]);
    if(TMapIterator == Textures.end())
    {
        CurTexture[Stage] = NullTexture;
        g_Context->Files.Assert << "SetTexture TopLevelSurfaceHandle not in list: " << SurfaceHandles[0] << endl;
    }
    else
    {
        CurTexture[Stage] = TMapIterator->second;
        if(g_ReportingEvents)
        {
            if(CurTexture[Stage]->ID().Length() == 0)
            {
                g_Context->Files.CurrentFrameAllEvents << "Texture unmatched\n";
            }
            else
            {
                g_Context->Files.CurrentFrameAllEvents << "Texture Name: " << CurTexture[Stage]->ID() << endl;
            }
        }
    }
}

//
// SetStreamSource event.  Updates the current vertex buffer.
//
void StateManager::SetStreamSource(UINT StreamNumber, HANDLE VBufferHandle, UINT OffsetInBytes, UINT Stride)
{
    if(VBufferHandle == NULL)
    {
        g_Context->Files.Assert << "SetStreamSource VBufferHandle NULL\n";
    }
    else if(VBuffers.find(VBufferHandle) == VBuffers.end())
    {
        g_Context->Files.Assert << "SetStreamSource VBufferHandle not in list: " << VBufferHandle << endl;
    }
    else if(StreamNumber >= MaxStreams)
    {
        g_Context->Files.Assert << "StreamNumber >= MaxStreams\n";
    }
    else
    {
        VBufferStreams[StreamNumber] = StreamInfo(VBuffers[VBufferHandle], OffsetInBytes, Stride);
    }
}

//
// SetStreamSource event.  Updates the current index buffer.
//
void StateManager::SetIndices(HANDLE IBufferHandle)
{
    if(IBufferHandle == NULL)
    {
        CurIBuffer = NULL;
    }
    else if(IBuffers.find(IBufferHandle) == IBuffers.end())
    {
        g_Context->Files.Assert << "SetIndices IBufferHandle not in list\n";
    }
    else
    {
        CurIBuffer = IBuffers[IBufferHandle];
    }
}

//
// SetVertexDeclaration from d3d9.dll.  Updates the current vertex declaration
//
void StateManager::SetVertexDeclaration(D3DVERTEXELEMENT9 *Elements, UINT ElementCount)
{
    VertexDeclaration.ReSize(ElementCount - 1);
    for(UINT i = 0; i < VertexDeclaration.Length(); i++)
    {
        VertexDeclaration[i] = Elements[i];
    }
}

void StateManager::LoadVertexTexCoord0(Vec2f &TexCoord, UINT Index)
{
    for(UINT i = 0; i < VertexDeclaration.Length(); i++)
    {
        const D3D9Base::D3DVERTEXELEMENT9 &Decl = VertexDeclaration[i];
        if(Decl.Usage == D3DDECLUSAGE_TEXCOORD && Decl.UsageIndex == 0)
        {
            StreamInfo &Stream = VBufferStreams[Decl.Stream];
            Assert(Stream.StreamData != NULL, "Reference to unbound stream");
            BYTE *ByteStream = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Index * Stream.Stride + Decl.Offset);
            float *FloatStream = (float *)ByteStream;
            Vec4f Result = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
            switch(Decl.Type)
            {
            case D3DDECLTYPE_FLOAT1:
                Result = Vec4f(FloatStream[0], 0.0f, 0.0f, 1.0f);
                break;
            case D3DDECLTYPE_FLOAT2:
                Result = Vec4f(FloatStream[0], FloatStream[1], 0.0f, 1.0f);
                break;
            case D3DDECLTYPE_FLOAT3:
                Result = Vec4f(FloatStream[0], FloatStream[1], FloatStream[2], 1.0f);
                break;
            case D3DDECLTYPE_FLOAT4:
                Result = Vec4f(FloatStream[0], FloatStream[1], FloatStream[2], FloatStream[3]);
                break;
            default:
                SignalError(String("Unsupported type: ") + String(UINT(Decl.Type)));
            }
            Assert(Decl.UsageIndex < MaxTexCoords, "Texture coordinate index out of range");
            TexCoord = Vec2f(Result.x, Result.y);
            return;
        }
    }
}

void StateManager::LoadVertexColor(RGBColor &C, UINT Index)
{
    for(UINT i = 0; i < VertexDeclaration.Length(); i++)
    {
        const D3D9Base::D3DVERTEXELEMENT9 &Decl = VertexDeclaration[i];
        if(Decl.Usage == D3DDECLUSAGE_COLOR && Decl.UsageIndex == 0)
        {
            StreamInfo &Stream = VBufferStreams[Decl.Stream];
            Assert(Stream.StreamData != NULL, "Reference to unbound stream");
            BYTE *ByteStream = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Index * Stream.Stride + Decl.Offset);
            float *FloatStream = (float *)ByteStream;
            Vec4f Result = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
            if(Decl.Type == D3DDECLTYPE_D3DCOLOR)
            {
                C = RGBColor(ByteStream[0], ByteStream[1], ByteStream[2], ByteStream[3]);
                return;
            }
            else
            {
                SignalError(String("Unsupported type: ") + String(UINT(Decl.Type)));
            }
        }
    }
}

void StateManager::LoadVertexData(VertexData &V, UINT Index)
{
    for(UINT i = 0; i < VertexDeclaration.Length(); i++)
    {
        const D3D9Base::D3DVERTEXELEMENT9 &Decl = VertexDeclaration[i];
        StreamInfo &Stream = VBufferStreams[Decl.Stream];
        Assert(Stream.StreamData != NULL, "Reference to unbound stream");
        const BYTE *ByteStream = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Index * Stream.Stride + Decl.Offset);
        const float *FloatStream = (float *)ByteStream;
        const SHORT *ShortStream = (SHORT *)ByteStream;
        Vec4f Result = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
        switch(Decl.Type)
        {
        case D3DDECLTYPE_FLOAT1:
            Result = Vec4f(FloatStream[0], 0.0f, 0.0f, 1.0f);
            break;
        case D3DDECLTYPE_FLOAT2:
            Result = Vec4f(FloatStream[0], FloatStream[1], 0.0f, 1.0f);
            break;
        case D3DDECLTYPE_FLOAT3:
            Result = Vec4f(FloatStream[0], FloatStream[1], FloatStream[2], 1.0f);
            break;
        case D3DDECLTYPE_FLOAT4:
            Result = Vec4f(FloatStream[0], FloatStream[1], FloatStream[2], FloatStream[3]);
            break;
        case D3DDECLTYPE_D3DCOLOR:
            Result = Vec4f(float(ByteStream[0]) / 255.0f, float(ByteStream[1]) / 255.0f, float(ByteStream[2]) / 255.0f, float(ByteStream[3]) / 255.0f);
            break;
        case D3DDECLTYPE_UBYTE4N:
            Result = Vec4f(float(ByteStream[0]) / 255.0f, float(ByteStream[1]) / 255.0f, float(ByteStream[2]) / 255.0f, float(ByteStream[3]) / 255.0f);
            break;
        case D3DDECLTYPE_UBYTE4:
            Result = Vec4f(float(ByteStream[0]), float(ByteStream[1]), float(ByteStream[2]), float(ByteStream[3]));
            break;
        case D3DDECLTYPE_SHORT2:
            Result = Vec4f(float(ShortStream[0]), float(ShortStream[1]), 0.0f, 1.0f);
            break;
        case D3DDECLTYPE_SHORT4:
            Result = Vec4f(float(ShortStream[0]), float(ShortStream[1]), float(ShortStream[2]), float(ShortStream[3]));
            break;
        case D3DDECLTYPE_FLOAT16_2:
            //NOTE: this is currently broken pending implementing 16-bit floating point conversion
            Result = Vec4f(float(ShortStream[0]), float(ShortStream[1]), 0.0f, 1.0f);
            break;
        case D3DDECLTYPE_FLOAT16_4:
            //NOTE: this is currently broken pending implementing 16-bit floating point conversion
            Result = Vec4f(float(ShortStream[0]), float(ShortStream[1]), float(ShortStream[2]), float(ShortStream[3]));
            break;
            
        default:
            SignalError(String("Unsupported type: ") + String(UINT(Decl.Type)));
        }
        if(Decl.Usage == D3DDECLUSAGE_POSITION)
        {
            V.Position = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_BLENDWEIGHT)
        {
            V.BlendWeight = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_BLENDINDICES)
        {
            V.BlendIndices = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_NORMAL)
        {
            V.Normal = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_BINORMAL)
        {
            V.BiNormal = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_TANGENT)
        {
            V.Tangent = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_COLOR)
        {
            Assert(Decl.UsageIndex < MaxColorCoords, "Color coordinate index out of range");
            V.Color[Decl.UsageIndex] = Result;
        }
        else if(Decl.Usage == D3DDECLUSAGE_TEXCOORD)
        {
            Assert(Decl.UsageIndex < MaxTexCoords, "Texture coordinate index out of range");
            V.TexCoord[Decl.UsageIndex] = Result;
        }
    }
}

//
// SetFVF event from d3d9.dll.  Updates the current FVF.
//
void StateManager::SetFVF(DWORD NewFVF)
{
    //CurFVF = NewFVF;
}

void StateManager::CreateVertexShader(CONST DWORD* pFunction, HANDLE Shader)
{
    VShaderMap::iterator Iterator = VShaders.find(Shader);
    if(Iterator == VShaders.end())
    {
        VShader *NewShader = new VShader(pFunction, Shader);
        Assert(NewShader != NULL, "Out of memory in StateManager::CreateVertexShader");

        if(CaptureAllShaders && !VertexShaderSimulator.ContainsHash(NewShader->Hash()))
        {
            String AssemblyFilename = g_Context->Parameters.ShaderCaptureDirectory + String("VShader") + String::ZeroPad(String(NewShader->Hash()), 10) + ".txt";
            String SourceFilename   = g_Context->Parameters.ShaderCaptureDirectory + String("VShader") + String::ZeroPad(String(NewShader->Hash()), 10) + ".cpp";
            String HeaderFilename   = g_Context->Parameters.ShaderCaptureDirectory + String("VShader") + String::ZeroPad(String(NewShader->Hash()), 10) + ".h";
            ofstream AssemblyFile(AssemblyFilename.CString());
            NewShader->OutputDisassembly(AssemblyFile);
            if(g_Context->Parameters.OutputCSource)
            {
                ofstream SourceFile(SourceFilename.CString());
                ofstream HeaderFile(HeaderFilename.CString());
                NewShader->OutputCSource(SourceFile, HeaderFile);
            }
        }
        /*g_Context->Files.ShaderInfo << String::ZeroPad(String(NewShader->Hash()), 10) << '\t' << NewShader->Instructions.Length() << '\t';
        Vector<UINT> InstructionCounts;
        NewShader->ComputeShaderInstructionCounts(InstructionCounts);
        for(UINT InstructionIndex = 0; InstructionIndex < InstructionCounts.Length(); InstructionIndex++)
        {
            g_Context->Files.ShaderInfo << InstructionCounts[InstructionIndex];
            if(InstructionIndex != InstructionCounts.Length() - 1)
            {
                 g_Context->Files.ShaderInfo << '\t';
            }
        }
        g_Context->Files.ShaderInfo << endl;*/
        VShaders[Shader] = NewShader;
    }
    else
    {
        //g_Context->Files.Assert << "Double-create on same VShader handle\n";
    }
}

void StateManager::CreatePixelShader(CONST DWORD* pFunction, HANDLE Shader)
{
    PShaderMap::iterator Iterator = PShaders.find(Shader);
    if(Iterator == PShaders.end())
    {
        PShader *NewShader = new PShader(pFunction, Shader);
        Assert(NewShader != NULL, "Out of memory in StateManager::CreatePixelShader");
        if(CaptureAllShaders)
        {
            VShader TempShader(pFunction, NULL);
            String AssemblyFilename = String(g_Context->Parameters.ShaderCaptureDirectory) + String("PShader") + String::ZeroPad(String(TempShader.Hash()), 10) + ".txt";
            ofstream DisassemblyFile(AssemblyFilename.CString());
            TempShader.OutputDisassembly(DisassemblyFile);
            /*g_Context->Files.ShaderInfo << String::ZeroPad(String(TempShader.Hash()), 10) << '\t' << TempShader.Instructions.Length() << '\t';
            Vector<UINT> InstructionCounts;
            TempShader.ComputeShaderInstructionCounts(InstructionCounts);
            for(UINT InstructionIndex = 0; InstructionIndex < InstructionCounts.Length(); InstructionIndex++)
            {
                g_Context->Files.ShaderInfo << InstructionCounts[InstructionIndex];
                if(InstructionIndex != InstructionCounts.Length() - 1)
                {
                     g_Context->Files.ShaderInfo << '\t';
                }
            }
            g_Context->Files.ShaderInfo << endl;*/
        }
        PShaders[Shader] = NewShader;
    }
    else
    {
        //g_Context->Files.Assert << "Double-create on same PShader handle\n";
    }
}

void StateManager::SetVertexShader(HANDLE Shader)
{
    if(Shader == NULL)
    {
        CurVShader = NULL;
    }
    else if(VShaders.find(Shader) == VShaders.end())
    {
        g_Context->Files.Assert << "SetVertexShader handle not in list\n";
    }
    else
    {
        CurVShader = VShaders[Shader];
    }
}

void StateManager::SetPixelShader(HANDLE Shader)
{
    if(Shader == NULL)
    {
        CurPShader = NULL;
    }
    else if(PShaders.find(Shader) == PShaders.end())
    {
        g_Context->Files.Assert << "SetPixelShader handle not in list\n";
    }
    else
    {
        CurPShader = PShaders[Shader];
    }
}
