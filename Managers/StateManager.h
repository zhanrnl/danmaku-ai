//
// StateManager.h
//
// Header for StateManager class, which stores a local copy of all relevant
// data associated with the DirectX device
//

//
// Information for each stream source
//
struct StreamInfo
{
    StreamInfo()
    {
        StreamData = NULL;
        OffsetInBytes = 0;
        Stride = 0;
    }
    StreamInfo(VBuffer *_StreamData, UINT _OffsetInBytes, UINT _Stride)
    {
        StreamData = _StreamData;
        OffsetInBytes = _OffsetInBytes;
        Stride = _Stride;
    }
    VBuffer *StreamData;
    UINT OffsetInBytes;
    UINT Stride;
};

//
// definition of maps from DirectX handles to the software structures stored
// in this DLL
//
typedef hash_map<HANDLE, Texture*, HandleHash> TextureMap;
typedef hash_map<HANDLE, VShader*, HandleHash> VShaderMap;
typedef hash_map<HANDLE, PShader*, HandleHash> PShaderMap;
typedef hash_map<HANDLE, VBuffer*, HandleHash> VBufferMap;
typedef hash_map<HANDLE, IBuffer*, HandleHash> IBufferMap;

//
// Maximum values set by DirectX
//
const UINT MaxRenderStates = 210;
const UINT MaxTextureStageStates = 33;
const UINT MaxLights = 8;
const UINT MaxStreams = 16;
const UINT MaxTextureStaages = 4;
const UINT MaxTextures = 16;

const UINT MaxVShaderFloatConstants = VShaderConst::ConstFloatRegisterCount;
const UINT MaxVShaderBoolConstants = VShaderConst::ConstBoolRegisterCount;
const UINT MaxVShaderIntConstants = VShaderConst::ConstIntRegisterCount;

const UINT MaxPShaderFloatConstants = 256;
const UINT MaxPShaderBoolConstants = 32;
const UINT MaxPShaderIntConstants = 32;

//
// Stores a local copy of all relevant data on the DirectX device, such as
// all vertex / index buffers, textures, render states, etc.
// updated by calls from d3d8.DLL (the wrapper DLL) into d3dCallback.DLL (this
// project.)
//
class StateManager : public Manager
{
public:
    void Init();
    void Dump(ofstream &DumpFile);

    Texture* UnlockTexture(D3DSURFACE_DESC &Desc, Bitmap &Bmp, HANDLE TextureHandle);
    void LockVBuffer(BufferLockData &Data, D3DVERTEXBUFFER_DESC &Desc);
    void LockIBuffer(BufferLockData &Data, D3DINDEXBUFFER_DESC &Desc);
    
    void CreateVertexShader(CONST DWORD* pFunction, HANDLE Shader);
    void CreatePixelShader(CONST DWORD* pFunction, HANDLE Shader);

    void SetVertexShader(HANDLE Shader);
    void SetPixelShader(HANDLE Shader);

    bool DestroyTexture(HANDLE Handle);
    bool DestroyVBuffer(HANDLE Handle);
    bool DestroyIBuffer(HANDLE Handle);
    
    void SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
    void SetTexture(DWORD Stage, HANDLE *SurfaceHandles, UINT HandleCount);

    void LoadVertexColor(RGBColor &Color, UINT Index);
    void LoadVertexTexCoord0(Vec2f &TexCoord, UINT Index);
    void LoadVertexData(VertexData &V, UINT Index);
    void SetStreamSource(UINT StreamNumber, HANDLE VBufferHandle, UINT OffsetInBytes, UINT Stride);
    void SetIndices(HANDLE IBufferHandle);
    void SetVertexDeclaration(D3DVERTEXELEMENT9 *Elements, UINT ElementCount);
    void SetFVF(DWORD NewFVF);

    void SetMaterial(CONST D3DMATERIAL9* pMaterial);
    void SetLight(UINT Index, CONST D3DLIGHT9* pLight);
    void SetViewport(CONST D3DVIEWPORT9* pViewport);

    //
    // The most high-frequency functions are inlined
    //
    __forceinline void SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
    {
        DebugOnlyAssert(StartRegister + BoolCount < MaxVShaderBoolConstants, "SetShaderConstant out of range");
        memcpy(&(VShaderBoolConstants[StartRegister]), pConstantData, sizeof(BOOL) * BoolCount);
        memcpy(&(VertexShaderSimulator.ConstBoolRegisters[StartRegister]), pConstantData, sizeof(BOOL) * BoolCount);
    }
    __forceinline void SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
    {
        DebugOnlyAssert(StartRegister + Vector4fCount <= MaxVShaderFloatConstants, "SetShaderConstant out of range");
        /*if(StartRegister == 0 && Vector4fCount >= 3)
        {
            float *TargetPtr = (float *)&(VShaderFloatConstants[0]);
            for(UINT Index = 0; Index < 12; Index++)
            {
                TargetPtr[Index] = pConstantData[Index];
            }
        }*/
        //
        // These are both high frequency and unnecessary for the SC2 AI
        //
        memcpy(&(VShaderFloatConstants[StartRegister]), pConstantData, sizeof(float) * 4 * Vector4fCount);
        memcpy(&(VertexShaderSimulator.ConstFloatRegisters[StartRegister]), pConstantData, sizeof(float) * 4 * Vector4fCount);
    }
    __forceinline void SetVertexShaderConstantI(UINT StartRegister, CONST UINT* pConstantData, UINT Vector4iCount)
    {
        DebugOnlyAssert(StartRegister + Vector4iCount < MaxVShaderIntConstants, "SetShaderConstant out of range");
        memcpy(&(VShaderIntConstants[StartRegister]), pConstantData, sizeof(UINT) * 4 * Vector4iCount);
        memcpy(&(VertexShaderSimulator.ConstIntRegisters[StartRegister]), pConstantData, sizeof(UINT) * 4 * Vector4iCount);
    }

    __forceinline void SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
    {
        DebugOnlyAssert(StartRegister + BoolCount < MaxPShaderBoolConstants, "SetShaderConstant out of range");
        memcpy(&(PShaderBoolConstants[StartRegister]), pConstantData, sizeof(BOOL) * BoolCount);
    }
    __forceinline void SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
    {
        DebugOnlyAssert(StartRegister + Vector4fCount < MaxPShaderFloatConstants, "SetShaderConstant out of range");
        memcpy(&(PShaderFloatConstants[StartRegister]), pConstantData, sizeof(float) * 4 * Vector4fCount);
    }
    __forceinline void SetPixelShaderConstantI(UINT StartRegister, CONST UINT* pConstantData, UINT Vector4iCount)
    {
        DebugOnlyAssert(StartRegister + Vector4iCount < MaxPShaderIntConstants, "SetShaderConstant out of range");
        memcpy(&(PShaderIntConstants[StartRegister]), pConstantData, sizeof(UINT) * 4 * Vector4iCount);
    }

    __forceinline void SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
    {
        RenderStates[State] = Value;
    }
    __forceinline void SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
    {
        if(Stage < MaxTextureStaages)
        {
            TextureStageStates[Type][MaxTextureStaages] = Value;
        }
    }

    UINT TextureMemorySize();

    Vec4f VShaderFloatConstants[MaxVShaderFloatConstants];
    BOOL VShaderBoolConstants[MaxVShaderBoolConstants];
    Int4 VShaderIntConstants[MaxVShaderIntConstants];
    VShaderSimulator VertexShaderSimulator;

    Vec4f PShaderFloatConstants[MaxPShaderFloatConstants];
    BOOL PShaderBoolConstants[MaxPShaderBoolConstants];
    Int4 PShaderIntConstants[MaxPShaderIntConstants];

    Texture *NullTexture;   // Texture used as a stub when SetTexture(NULL) is called

    VShader *CurVShader;
    PShader *CurPShader;
    Texture *CurTexture[MaxTextures];          // Currently set textures
    StreamInfo VBufferStreams[MaxStreams];    // Current vertex buffers for each stream
    IBuffer *CurIBuffer;
    Vector<D3DVERTEXELEMENT9> VertexDeclaration; // Current vertex declaration (programmable shaders)
    
    UINT VBufferStride;     // Current vertex buffer stride
    
    //DWORD CurFVF;         // Current flexible-vertex-format flags (not updating correctly)

    DWORD RenderStates[MaxRenderStates];                // all render state variables
    DWORD TextureStageStates[MaxTextureStageStates][MaxTextureStaages];

    D3DLIGHT9 CurLight[MaxLights];  // Current lights
    D3DMATERIAL9 CurMaterial;       // Current material
    D3DVIEWPORT9 CurViewport;       // Current viewport

    TextureMap Textures;    // Map to all allocated textures
    VShaderMap VShaders;    // Map to all allocated vertex shaders
    PShaderMap PShaders;    // Map to all allocated pixel shaders
    VBufferMap VBuffers;    // Map to all allocated vertex buffers
    IBufferMap IBuffers;    // Map to all allocated index buffers
    //MatrixController Matrices;  // Storage for world, view, and perspective matrices (not updating correctly...maybe MultiplyTransform?)
    //Matrix4 TextureMatrices[8];  // Texture stage matrices
    //Matrix4 TotalMatrix;         // Storage for combined world, view, and perspective matrices

    BufferHasher Hasher;    // Hashes buffers for fast identification
};
