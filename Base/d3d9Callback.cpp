//
// d3dCallback.cpp
//
// Source for DLL exports.  Dispatches DirectX calls to the appropriate parts of the AI
// Written by Matthew Fisher
//

#include "Main.h"
#include "d3d9Callback.h"

//
// Dummy function needed by D3D9GraphicsDevice
//
extern "C" {
IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion)
{
    SignalError("Unexpected call to Direct3DCreate9");
    return NULL;
}
}

//
// DLL entry point
//
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            //g_Context = NULL;
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void SafeDLLInitialize()
{
    if(g_Context == NULL)
    {
        g_Context = new GlobalContext;
        g_Context->Init();
    }
}

D3D9CALLBACK_API void D3D9CallbackInitialize()
{
    
}

D3D9CALLBACK_API void D3D9CallbackFreeMemory()
{
    if(g_Context != NULL)
    {
        g_Context->Files.Events << "Freeing DLL memory\n";
        delete g_Context;
        g_Context = NULL;
    }
}

/*D3D9CALLBACK_API void BeginSingleFrameCapture()
{
    //
    // Open the single frame log file
    //
    g_Context->Files.CurrentFrameDump.open(FrameIndex());

#ifndef ULTRA_FAST
    g_ReportingEvents = true;
#endif
}

D3D9CALLBACK_API void EndSingleFrameCapture()
{
    //
    // Open the multi-frame log file
    //
    g_Context->Files.CurrentFrameAllEvents.close();
    g_Context->Files.CurrentFrameAllEvents.open((g_Context->Parameters.OutputFileDirectory + String("ReportedEventsAfterCapture.txt")).CString());

#ifndef ULTRA_FAST
    g_ReportingEvents = false;
#endif
}*/

D3D9CALLBACK_API bool ReportUnlockTexture(D3DSURFACE_DESC &Desc, Bitmap &Bmp, HANDLE TextureHandle)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "Unlock Texture " << TextureHandle << endl;
    }
    if(g_Context->Parameters.DebugTextureCreation)
    {
        g_Context->Files.Assert << "Unlock Texture " << TextureHandle << endl;
    }

    //
    // Tell the State Manager to create or update this texture and return said texture
    //
    Texture *ThisTexture = g_Context->Managers.State.UnlockTexture(Desc, Bmp, TextureHandle);

    return false;
}

D3D9CALLBACK_API void ReportLockVertexBuffer(BufferLockData &Data, D3DVERTEXBUFFER_DESC &Desc)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "LockVBuffer, Handle: " << Data.Handle << endl;
    }

    //
    // Tell the State Manager to create or update this vertex buffer
    //
    g_Context->Managers.State.LockVBuffer(Data, Desc);
}

D3D9CALLBACK_API void ReportLockIndexBuffer(BufferLockData &Data, D3DINDEXBUFFER_DESC &Desc)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "LockIBuffer, Handle: " << Data.Handle << endl;
    }

    //
    // Tell the State Manager to create or update this index buffer
    //
    g_Context->Managers.State.LockIBuffer(Data, Desc);
}

D3D9CALLBACK_API void ReportDestroy(HANDLE Handle)
{
    //if(!g_Context->Managers.State.DestroyVBuffer(Handle))
    {
        //if(!g_Context->Managers.State.DestroyIBuffer(Handle))
        {
            if(!g_Context->Managers.State.DestroyTexture(Handle))
            {
                //
                // We will recieve this a lot of unknown destroy handles since we are ignoring very small textures
                //
                //g_Context->Files.Assert << "Unknown destroy handle: " << Handle << endl;
            }
        }
    }
}

D3D9CALLBACK_API void ReportCreateDevice(D3D9Base::LPDIRECT3DDEVICE9 Device, ID3D9DeviceOverlay *Overlay)
{
    SafeDLLInitialize();

    g_Context->Graphics.SetDevice(Device);
    g_Context->Graphics.SetOverlay(Overlay);
}

D3D9CALLBACK_API void ReportFreeDevice()
{
    //g_Context->Files.Events << "Freeing device\n";
    g_Context->Managers.Render.ReleaseStorageObjects();
    //g_Context->Managers.State.VertexShaderSimulator.OutputNewShaderList();
}

D3D9CALLBACK_API void ReportSetTexture(DWORD Stage, HANDLE *SurfaceHandles, UINT SurfaceHandleCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetTexture Stage=" << Stage << " Handle=" << DWORD(SurfaceHandles[0]) << endl;
    }
    if(g_Context->Parameters.DebugTextureCreation)
    {
        g_Context->Files.Assert << "SetTexture " << SurfaceHandles[0] << endl;
    }
    g_Context->Managers.State.SetTexture(Stage, SurfaceHandles, SurfaceHandleCount);
}

D3D9CALLBACK_API void ReportSetViewport(CONST D3DVIEWPORT9 *pViewport)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetViewport " << pViewport->X << ' ' << pViewport->Y << ' ' << pViewport->Width << ' ' << pViewport->Height << ' ' << pViewport->MinZ << ' ' << pViewport->MaxZ << endl;
    }
    g_Context->Managers.State.SetViewport(pViewport);
}

D3D9CALLBACK_API void ReportSetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
    g_Context->Managers.State.SetTransform(State, pMatrix);
}

D3D9CALLBACK_API void ReportSetVertexDeclaration(D3DVERTEXELEMENT9 *Elements, UINT ElementCount)
{
    if(g_ReportingEvents)
    {
        if(ElementCount <= 1 || Elements == NULL)
        {
            g_Context->Files.CurrentFrameAllEvents << "SetVertexDeclaration NULL\n";
        }
        else
        {
            g_Context->Files.CurrentFrameAllEvents << "SetVertexDeclaration Elements=" << ElementCount << "\tHash=" << Utility::Hash32((BYTE *)Elements, sizeof(D3DVERTEXELEMENT9) * (ElementCount - 1)) << endl;
        }
    }
    g_Context->Managers.State.SetVertexDeclaration(Elements, ElementCount);
}

D3D9CALLBACK_API void ReportSetFVF(DWORD FVF)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetFVF ";
        g_Context->Files.CurrentFrameAllEvents << "Unknown\n";
        g_Context->Files.Assert << "Unknown FVF " << FVF << endl;
    }
    g_Context->Managers.State.SetFVF(FVF);
}

D3D9CALLBACK_API void ReportSetStreamSource(UINT StreamNumber, HANDLE VBufferHandle, UINT OffsetInBytes, UINT Stride)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetStreamSource\t" << StreamNumber << '\t' << VBufferHandle << '\t' << OffsetInBytes << '\t' << Stride << endl;
    }
    g_Context->Managers.State.SetStreamSource(StreamNumber, VBufferHandle, OffsetInBytes, Stride);
}

D3D9CALLBACK_API void ReportSetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetStreamSourceFreq " << StreamNumber << " " << FrequencyParameter << endl;
    }
}

D3D9CALLBACK_API void ReportSetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetLight\n";
    }
    g_Context->Managers.State.SetLight(Index, pLight);
}

D3D9CALLBACK_API void ReportLightEnable(DWORD Index, BOOL Enable)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "LightEnable " << Index << ' ' << Enable << endl;
    }
}

D3D9CALLBACK_API void ReportSetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetMaterial\n";
    }
    g_Context->Managers.State.SetMaterial(pMaterial);
}

D3D9CALLBACK_API bool ReportDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "DrawPrimitive\n";
    }
    return g_Context->Managers.Render.DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

D3D9CALLBACK_API bool ReportDrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "DrawIndexedPrimitive Type=" << PrimitiveType << " BaseVertexIndex=" << BaseVertexIndex << " MinIndex=" << MinIndex;
        g_Context->Files.CurrentFrameAllEvents << " NumVertices=" << NumVertices << " StartIndex=" << StartIndex << " PrimitiveCount=" << PrimitiveCount << endl;
    }
    return g_Context->Managers.Render.DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
}

D3D9CALLBACK_API bool PreRenderQuery(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount)
{
    bool Result = g_Context->Managers.Render.PreRenderQuery(PrimitiveType, PrimitiveCount);
    if(g_ReportingEvents)
    {
        if(Result)
        {
            g_Context->Files.CurrentFrameAllEvents << "Skipping Render\n";
        }
    }
    return Result;
}

D3D9CALLBACK_API bool ReportDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "DrawPrimitiveUP\n";
    }
    return g_Context->Managers.Render.DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

D3D9CALLBACK_API bool ReportDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "DrawIndexedPrimitiveUP\n";
    }
    return g_Context->Managers.Render.DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

D3D9CALLBACK_API void ReportSetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetRenderState " << State << ' ' << Value << endl;
    }
    g_Context->Managers.State.SetRenderState(State, Value);
}

D3D9CALLBACK_API void ReportSetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetTextureStageState " << Stage << ' ' << Type << ' ' << Value << endl;
    }
    g_Context->Managers.State.SetTextureStageState(Stage, Type, Value);
}

D3D9CALLBACK_API void ReportSetIndices(HANDLE IBufferHandle)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetIndices\t" << IBufferHandle << "\n";
    }
    g_Context->Managers.State.SetIndices(IBufferHandle);
}

D3D9CALLBACK_API void ReportCreateVertexShader(CONST DWORD* pFunction, HANDLE Shader)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "CreateVertexShader " << Shader << endl;
    }
    g_Context->Managers.State.CreateVertexShader(pFunction, Shader);
}

D3D9CALLBACK_API void ReportSetVertexShader(HANDLE Shader)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetVertexShader " << Shader << endl;
    }
    g_Context->Managers.State.SetVertexShader(Shader);
}

D3D9CALLBACK_API void ReportCreatePixelShader(CONST DWORD* pFunction, HANDLE Shader)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "CreatePixelShader " << Shader << endl;
    }
    g_Context->Managers.State.CreatePixelShader(pFunction, Shader);
}

D3D9CALLBACK_API void ReportSetPixelShader(HANDLE Shader)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetPixelShader " << Shader << endl;
    }
    g_Context->Managers.State.SetPixelShader(Shader);
}

D3D9CALLBACK_API void ReportSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetVertexShaderConstantF Start=" << StartRegister << ", Count=" << Vector4fCount << endl;
        for(UINT i = 0; i < Vector4fCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i * 4 + 0] << '\t' 
                        << pConstantData[i * 4 + 1] << '\t' 
                        << pConstantData[i * 4 + 2] << '\t' 
                        << pConstantData[i * 4 + 3] << endl;
        }
    }
    g_Context->Managers.State.SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

D3D9CALLBACK_API void ReportSetVertexShaderConstantI(UINT StartRegister, CONST UINT* pConstantData, UINT Vector4iCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetVertexShaderConstantI " << Vector4iCount << endl;
        for(UINT i = 0; i < Vector4iCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i * 4 + 0] << '\t' 
                        << pConstantData[i * 4 + 1] << '\t' 
                        << pConstantData[i * 4 + 2] << '\t' 
                        << pConstantData[i * 4 + 3] << endl;
        }
    }
    g_Context->Managers.State.SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

D3D9CALLBACK_API void ReportSetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetVertexShaderConstantB " << BoolCount << endl;
        for(UINT i = 0; i < BoolCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i] << endl;
        }
    }
    g_Context->Managers.State.SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

D3D9CALLBACK_API void ReportSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetPixelShaderConstantF Start=" << StartRegister << " Count=" << Vector4fCount << endl;
        for(UINT i = 0; i < Vector4fCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i * 4 + 0] << '\t' 
                        << pConstantData[i * 4 + 1] << '\t' 
                        << pConstantData[i * 4 + 2] << '\t' 
                        << pConstantData[i * 4 + 3] << endl;
        }
    }
    g_Context->Managers.State.SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

D3D9CALLBACK_API void ReportSetPixelShaderConstantI(UINT StartRegister, CONST UINT* pConstantData, UINT Vector4iCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetPixelShaderConstantI " << Vector4iCount << endl;
        for(UINT i = 0; i < Vector4iCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i * 4 + 0] << '\t' 
                        << pConstantData[i * 4 + 1] << '\t' 
                        << pConstantData[i * 4 + 2] << '\t' 
                        << pConstantData[i * 4 + 3] << endl;
        }
    }
    g_Context->Managers.State.SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

D3D9CALLBACK_API void ReportSetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetPixelShaderConstantB " << BoolCount << endl;
        for(UINT i = 0; i < BoolCount; i++)
        {
            g_Context->Files.CurrentFrameAllEvents << pConstantData[i] << endl;
        }
    }
    g_Context->Managers.State.SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

D3D9CALLBACK_API void ReportSetRenderTarget(DWORD RenderTargetIndex, HANDLE Surface)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "SetRenderTarget " << Surface << endl;
    }
}

D3D9CALLBACK_API void ReportPresent(CONST RECT* pSourceRect,CONST RECT* pDestRect)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "Present\n";
    }
}

D3D9CALLBACK_API void ReportClear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "Clear\n";
    }
}

D3D9CALLBACK_API void ReportBeginScene()
{
    g_Context->Controller.FrameStart();
    if(g_ReportingEvents)
    {
        g_Context->Files.CurrentFrameAllEvents << "BeginScene\n";
    }
}

D3D9CALLBACK_API void ReportEndScene()
{
    g_Context->Controller.FrameEnd();
}
