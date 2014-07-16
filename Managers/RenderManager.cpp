#include "Main.h"
#include "Bullet.h"

RenderManager::RenderManager()
{
    _FrameRenderIndex = 0;
    for(UINT BufferIndex = 0; BufferIndex < ScratchVertexBufferCount; BufferIndex++)
    {
        _ScratchVertexBuffers[BufferIndex] = NULL;
    }
    for(UINT SurfaceIndex = 0; SurfaceIndex < ScratchSurfaceCount; SurfaceIndex++)
    {
        _ScratchSurfaces[SurfaceIndex] = NULL;
    }
	for(UINT TextureIndex = 0; TextureIndex < ScratchTextureCount; TextureIndex++)
    {
        _ScratchTextures[TextureIndex] = NULL;
    }
	_ProcessedVerticesDeclaration = NULL;
	_BmpCapturePrev = NULL;
	_BmpCaptureCurrent = NULL;
}

void RenderManager::StartFrame()
{
    _FrameRenderIndex = 0;
}

void RenderManager::EndFrame()
{
    if(g_Context->Parameters.VideoCaptureMode && !g_Context->Controller.AIEnabled())
	{
        CompressCurrentFrame();
	}
    if(g_ReportingEvents)
	{
		if (g_Context->Files.CurrentFrameAllEvents.is_open()) {
			Bitmap BmpScreen;
			CaptureCurrentRenderTarget(BmpScreen, 4096);
			BmpScreen.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Final.png"));
		}

        /*Bitmap BmpMinimapExplore, BmpMinimapArmy;
        g_Context->Managers.Minimap.MakeExplorationBitmap(BmpMinimapExplore);
        if(BmpMinimapExplore.Width() > 0 && BmpMinimapExplore.Height() > 0)
        {
            BmpMinimapExplore.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Explore.png"));
        }
        g_Context->Managers.Minimap.MakeArmyBitmap(BmpMinimapArmy);
        if(BmpMinimapArmy.Width() > 0 && BmpMinimapArmy.Height() > 0)
        {
            BmpMinimapArmy.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Army.png"));
        }
        
        const Bitmap &Minimap = g_Context->Managers.Minimap.Minimap();
        if(Minimap.Width() > 0 && Minimap.Height() > 0)
        {
            Minimap.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Minimap.png"));
        }*/
        
        //Vec2i MinimapDimensions = g_Context->Constants.GetVec2fConstant(ScreenConstantVec2fMinimapDimensions).RoundToVec2i();
        //Vec2i MinimapTopLeft = g_Context->Constants.GetVec2fConstant(ScreenConstantVec2fMinimapTopLeft).RoundToVec2i();
        //Bitmap Minimap(MinimapDimensions.x, MinimapDimensions.y);
        //Bmp.BltTo(Minimap, 0, 0, MinimapTopLeft.x, Bmp.Height() - 1 - MinimapTopLeft.y - MinimapDimensions.y, MinimapDimensions.x, MinimapDimensions.y);
        //Minimap.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Minimap.png"));
    }
}

void RenderManager::ReleaseStorageObjects()
{
    for(UINT BufferIndex = 0; BufferIndex < ScratchVertexBufferCount; BufferIndex++)
    {
        if(_ScratchVertexBuffers[BufferIndex])
        {
            _ScratchVertexBuffers[BufferIndex]->Release();
            _ScratchVertexBuffers[BufferIndex] = NULL;
            g_Context->Files.Events << "Releasing scratch vertex buffer " << BufferIndex << endl;
        }
    }
    for(UINT SurfaceIndex = 0; SurfaceIndex < ScratchSurfaceCount; SurfaceIndex++)
    {
        if(_ScratchSurfaces[SurfaceIndex])
        {
            _ScratchSurfaces[SurfaceIndex]->Release();
            _ScratchSurfaces[SurfaceIndex] = NULL;
            g_Context->Files.Events << "Releasing scratch surface " << SurfaceIndex << endl;
        }
    }
	for(UINT TextureIndex = 0; TextureIndex < ScratchTextureCount; TextureIndex++)
    {
        if(_ScratchTextures[TextureIndex])
        {
            _ScratchTextures[TextureIndex]->Release();
            _ScratchTextures[TextureIndex] = NULL;
            g_Context->Files.Events << "Releasing scratch texture " << TextureIndex << endl;
        }
    }
    if(_ProcessedVerticesDeclaration)
    {
        _ProcessedVerticesDeclaration->Release();
        _ProcessedVerticesDeclaration = NULL;
        g_Context->Files.Events << "Releasing processed vertex declaration\n";
    }
#ifdef USE_WMF
    _Compressor.FreeMemory();
#endif
}

void RenderManager::UpdateScratchTexture(UINT Index, const Bitmap &Bmp)
{
	if(_ScratchTextures[Index])
    {
        _ScratchTextures[Index]->Release();
        _ScratchTextures[Index] = NULL;
    }
    LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
	g_Context->Files.CurrentFrameAllEvents << "CreateTexture: Width=" << Bmp.Width() << ", Height=" << Bmp.Height() << endl;

	DWORD Usage = D3DUSAGE_AUTOGENMIPMAP;
	D3DPOOL Pool = D3DPOOL_MANAGED;

	HRESULT hr = Device->CreateTexture(Bmp.Width(), Bmp.Height(), 0, Usage, D3DFMT_A8R8G8B8, Pool, &_ScratchTextures[Index], NULL);
    PersistentAssert(SUCCEEDED(hr), "CreateTexture failed");

	D3DLOCKED_RECT Rect;
	hr = _ScratchTextures[Index]->LockRect(0, &Rect, NULL, 0);
	PersistentAssert(SUCCEEDED(hr), "LockRect failed");

    BYTE *Bytes = (BYTE *)Rect.pBits;
    for(UINT y = 0; y < Bmp.Height(); y++)
    {
        RGBColor *CurRow = (RGBColor *)(Bytes + y * Rect.Pitch);
        for(UINT x = 0; x < Bmp.Width(); x++)
        {
            RGBColor Color = Bmp[y][x];
            CurRow[x] = RGBColor(Color.b, Color.g, Color.r, Color.a);
        }
    }
    hr = _ScratchTextures[Index]->UnlockRect(0);
    PersistentAssert(SUCCEEDED(hr), "UnlockRect failed");
}

void RenderManager::ResizeScratchSurface(UINT Index, const D3DSURFACE_DESC &Desc)
{
    bool MustResize = (_ScratchSurfaces[Index] == NULL);
    if(!MustResize)
    {
        D3DSURFACE_DESC CurDesc;
        _ScratchSurfaces[Index]->GetDesc(&CurDesc);
        if(Desc.Width != CurDesc.Width || Desc.Height != CurDesc.Height || Desc.Format != CurDesc.Format)
        {
            MustResize = true;
        }
    }
    if(MustResize)
    {
        if(_ScratchSurfaces[Index])
        {
            _ScratchSurfaces[Index]->Release();
            _ScratchSurfaces[Index] = NULL;
        }
        LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
        HRESULT hr = Device->CreateOffscreenPlainSurface(Desc.Width, Desc.Height, Desc.Format, D3DPOOL_SYSTEMMEM, &_ScratchSurfaces[Index], NULL);
        g_Context->Files.CurrentFrameAllEvents << "CreateOffscreenPlainSurface: Width=" << Desc.Width << ", Height=" << Desc.Height << " Format=" << Desc.Format << " Success=" << String(bool(SUCCEEDED(hr))) << endl;
        PersistentAssert(SUCCEEDED(hr), "CreateOffscreenPlainSurface failed");
    }
}

void RenderManager::ResizeScratchVertexBuffer(UINT Index, UINT VertexSize, UINT VertexCount, DWORD FVF)
{
    bool MustResize = (_ScratchVertexBuffers[Index] == NULL);
    if(!MustResize)
    {
        D3D9Base::D3DVERTEXBUFFER_DESC Desc;
        HRESULT hr = _ScratchVertexBuffers[Index]->GetDesc(&Desc);
        Assert(SUCCEEDED(hr), "GetDesc failed");
        if(Desc.Size / VertexSize < VertexCount)
        {
            MustResize = true;
        }
    }
    if(MustResize)
    {
        HRESULT hr = g_Context->Graphics.Device()->CreateVertexBuffer(VertexSize * VertexCount, D3DUSAGE_SOFTWAREPROCESSING, FVF, D3DPOOL_SYSTEMMEM, &_ScratchVertexBuffers[Index], NULL);
        Assert(SUCCEEDED(hr), "CreateVertexBuffer failed");
    }
}

void RenderManager::CompressCurrentFrame()
{
    LPDIRECT3DSURFACE9 RenderTarget = NULL;
    LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
    
    HRESULT hr = Device->GetRenderTarget(0, &RenderTarget);
    PersistentAssert(SUCCEEDED(hr), "GetRenderTarget failed");
    
    if(RenderTarget == NULL)
    {
		g_Context->Files.Assert << "No render target\n";
        return;
    }

    D3DSURFACE_DESC Desc;
    RenderTarget->GetDesc(&Desc);
    ResizeScratchSurface(0, Desc);

    hr = Device->GetRenderTargetData(RenderTarget, _ScratchSurfaces[0]);
    if(FAILED(hr))
	{
        g_Context->Files.Assert << "GetRenderTargetData failed\n";
		return;
	}

    RenderTarget->Release();

#ifdef USE_WMF
    if(!_Compressor.Capturing())
    {
        _Compressor.OpenFile(g_Context->Parameters.ScreenCaptureDirectory + String("Capture.mp4"), Desc.Width, Desc.Height, 20000000, 30, true, &(g_Context->Timer));
    }

    _Compressor.AddFrame(_ScratchSurfaces[0], Vec2i(0, 0), g_Context->Timer.Elapsed());
#endif
}

void RenderManager::CaptureRenderTargetSurface(LPDIRECT3DSURFACE9 RenderTarget, Bitmap &Bmp)
{
    LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
    
    if(RenderTarget == NULL)
    {
        Bmp.Allocate(1, 1);
        Bmp.Clear(RGBColor::Magenta);
        return;
    }

    D3DSURFACE_DESC Desc;
    RenderTarget->GetDesc(&Desc);
    Vec2i InitialDimensions(Desc.Width, Desc.Height);
    ResizeScratchSurface(0, Desc);

    HRESULT hr = Device->GetRenderTargetData(RenderTarget, _ScratchSurfaces[0]);
    if(SUCCEEDED(hr))
    {
        Desc.Format = D3DFMT_A8R8G8B8;
        float ScaleFactor = 1.0f;
        while(Math::Max(Desc.Width * ScaleFactor, Desc.Height * ScaleFactor) > 2048)
        {
            ScaleFactor *= 0.995f;
        }
        Desc.Width = UINT(Desc.Width * ScaleFactor);
        Desc.Height = UINT(Desc.Height * ScaleFactor);
        ResizeScratchSurface(1, Desc);

        g_Context->Files.CurrentFrameAllEvents << "Loading render target scratch surface from surface...";
        hr = D3DXLoadSurfaceFromSurface(_ScratchSurfaces[1], NULL, NULL, _ScratchSurfaces[0], NULL, NULL, D3DX_FILTER_LINEAR, 0);
        //PersistentAssert(SUCCEEDED(hr), "D3DXLoadSurfaceFromSurface failed");
        if(SUCCEEDED(hr))
        {
            g_Context->Files.CurrentFrameAllEvents << "success...";
            Bmp.LoadFromSurface(_ScratchSurfaces[1]);
            g_Context->Files.CurrentFrameAllEvents << "bitmap loaded.\n";
        }
        else
        {
            g_Context->Files.CurrentFrameAllEvents << "failed.\n";
            g_Context->Files.Assert << "D3DXLoadSurfaceFromSurface failed\n";
            Bmp.Allocate(1, 1);
            Bmp.Clear(RGBColor::Blue);
        }
    }
    else
    {
        g_Context->Files.Assert << "GetRenderTargetData failed (multisampled frame buffers are not supported)\n";
        Bmp.Allocate(1, 1);
        Bmp.Clear(RGBColor::Magenta);
    }
}

void RenderManager::CaptureCurrentRenderTarget(Bitmap &Bmp, UINT MaxDimension)
{
    LPDIRECT3DSURFACE9 RenderTarget = NULL;
    LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
    
    HRESULT hr = Device->GetRenderTarget(0, &RenderTarget);
    PersistentAssert(SUCCEEDED(hr), "GetRenderTarget failed");
    
    if(RenderTarget == NULL)
    {
        Bmp.Allocate(1, 1);
        Bmp.Clear(RGBColor::Magenta);
        return;
    }

    D3DSURFACE_DESC Desc;
    RenderTarget->GetDesc(&Desc);
    Vec2i InitialDimensions(Desc.Width, Desc.Height);
    ResizeScratchSurface(0, Desc);

    hr = Device->GetRenderTargetData(RenderTarget, _ScratchSurfaces[0]);
    if(SUCCEEDED(hr))
    {
        RenderTarget->Release();

        Desc.Format = D3DFMT_A8R8G8B8;
        float ScaleFactor = 1.0f;
        while(Math::Max(Desc.Width * ScaleFactor, Desc.Height * ScaleFactor) > MaxDimension)
        {
            ScaleFactor *= 0.995f;
        }
        Desc.Width = UINT(Desc.Width * ScaleFactor);
        Desc.Height = UINT(Desc.Height * ScaleFactor);
        ResizeScratchSurface(1, Desc);

        hr = D3DXLoadSurfaceFromSurface(_ScratchSurfaces[1], NULL, NULL, _ScratchSurfaces[0], NULL, NULL, D3DX_FILTER_LINEAR, 0);
        PersistentAssert(SUCCEEDED(hr), "D3DXLoadSurfaceFromSurface failed");

        Bmp.LoadFromSurface(_ScratchSurfaces[1]);
        Bmp.FlipBlueAndRed();
    }
    else
    {
        g_Context->Files.Assert << "GetRenderTargetData failed (multisampled frame buffers are not supported)\n";
        Bmp.Allocate(1, 1);
        Bmp.Clear(RGBColor::Magenta);
    }
}

bool RenderManager::CaptureCurrentRenderTargetSubset(Bitmap &Bmp, const Rectangle2i &Region)
{
    LPDIRECT3DSURFACE9 RenderTarget = NULL;
    LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();
    
    HRESULT hr = Device->GetRenderTarget(0, &RenderTarget);
    
    if(RenderTarget == NULL || FAILED(hr))
    {
        g_Context->Files.Assert << "GetRenderTarget failed\n";
        return false;
    }

    D3DSURFACE_DESC Desc;
    RenderTarget->GetDesc(&Desc);
    Vec2i InitialDimensions(Desc.Width, Desc.Height);
    
    ResizeScratchSurface(0, Desc);

    hr = Device->GetRenderTargetData(RenderTarget, _ScratchSurfaces[0]);
    RenderTarget->Release();

    if(SUCCEEDED(hr))
    {
        Desc.Format = D3DFMT_A8R8G8B8;
        Desc.Width = Region.Width();
        Desc.Height = Region.Height();
        ResizeScratchSurface(1, Desc);

        RECT SourceRect;
        SourceRect.left = Region.Min.x;
        SourceRect.right = Region.Max.x;
        SourceRect.top = Region.Min.y;
        SourceRect.bottom = Region.Max.y;

        hr = D3DXLoadSurfaceFromSurface(_ScratchSurfaces[1], NULL, NULL, _ScratchSurfaces[0], NULL, &SourceRect, D3DX_FILTER_POINT, 0);
        PersistentAssert(SUCCEEDED(hr), "D3DXLoadSurfaceFromSurface failed");

        Bmp.LoadFromSurface(_ScratchSurfaces[1]);
        Bmp.FlipBlueAndRed();
        return true;
    }
    else
    {
        g_Context->Files.Assert << "GetRenderTargetData failed\n";
        return false;
    }
}

void WebpageTextureSave(const Bitmap &TextureBmp, const String &RGBFilename, const String &AlphaFilename)
{
	if(!Utility::FileExists(g_Context->Parameters.ScreenCaptureDirectory + RGBFilename))
	{
		Bitmap Bmp = TextureBmp;
		Bmp.FlipBlueAndRed();
		if(Bmp.Width() == 0 || Bmp.Height() == 0)
		{
			Bmp.Allocate(1, 1);
			Bmp[0][0] = RGBColor::Magenta;
		}

		const UINT FinalSize = 256;
		Bitmap FinalRGB(FinalSize, FinalSize), FinalAlpha;
		FinalRGB.Clear(RGBColor(255, 255, 255, 255));
		
		int XOffset = 0, YOffset = 0;
		float AspectRatio = float(Bmp.Height()) / float(Bmp.Width());
		if(Bmp.Width() < Bmp.Height())
		{
			float ExpectedWidth = float(FinalSize) / AspectRatio;
			XOffset = Math::Round((FinalSize - ExpectedWidth) * 0.5f);
			if(XOffset <= 2)
			{
				XOffset = 0;
			}
		}
		if(Bmp.Height() < Bmp.Width())
		{
			float ExpectedHeight = float(FinalSize) * AspectRatio;
			YOffset = Math::Round((FinalSize - ExpectedHeight) * 0.5f);
			if(YOffset <= 2)
			{
				YOffset = 0;
			}
		}
		
		Bmp.StretchBltTo(FinalRGB, XOffset, YOffset, FinalRGB.Width() - 2 * XOffset, FinalRGB.Height() - 2 * YOffset, 0, 0, Bmp.Width(), Bmp.Height(), Bitmap::SamplingPoint);
		FinalAlpha = FinalRGB;

		for(UINT y = 0; y < FinalRGB.Height(); y++)
		{
			for(UINT x = 0; x < FinalRGB.Width(); x++)
			{
				FinalRGB[y][x].a = 0;
				BYTE Value = FinalAlpha[y][x].a;
				FinalAlpha[y][x] = RGBColor(Value, Value, Value, 0);
			}
		}

		FinalRGB.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + RGBFilename);
		FinalAlpha.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + AlphaFilename);
	}
}

void RenderManager::ReportRenderWebpage(const RenderInfo &Info)
{
	ofstream &os = g_Context->Files.CurrentFrameWebpageCapture;

	os << "<tr>";
	os << "<td>" << _FrameRenderIndex << "</td>";

	String RenderTargetFilename = String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Obj") + String::ZeroPad(String(_FrameRenderIndex), 5) + String(".png");
	os << "<td><img src=\"" << RenderTargetFilename << "\"/></td>";

	String Texture0ImageRGB = String::ZeroPad(String(Info.Texture0->BmpHash()), 10) + String("RGB.png");
	String Texture0ImageAlpha = String::ZeroPad(String(Info.Texture0->BmpHash()), 10) + String("Alpha.png");
	//String Texture1ImageRGB = String::ZeroPad(String(Info.Texture1->BmpHash()), 10) + String("RGB.png");
	//String Texture1ImageAlpha = String::ZeroPad(String(Info.Texture1->BmpHash()), 10) + String("Alpha.png");

    D3D9Base::LPDIRECT3DBASETEXTURE9 curBaseTexture = NULL;
    auto device = g_Context->Graphics.Device();
    device->GetTexture(0, &curBaseTexture);

    if(Info.Texture0->ID() == "Magenta" && curBaseTexture != NULL && curBaseTexture->GetType() == D3DRTYPE_TEXTURE)
    {
        Texture0ImageRGB = String("Dynamic") + String(rand() % 10000) + String("RGB.png");
        Texture0ImageAlpha = String("Dynamic") + String(rand() % 10000) + String("Alpha.png");

        D3D9Base::LPDIRECT3DTEXTURE9 curTexture = (D3D9Base::LPDIRECT3DTEXTURE9)curBaseTexture;
        
        LPDIRECT3DSURFACE9 sourceSurface;
        curTexture->GetSurfaceLevel(0, &sourceSurface);
        
        Bitmap bmp;
        
        g_Context->Files.CurrentFrameAllEvents << "Begin capturing render target surface\n"; g_Context->Files.CurrentFrameAllEvents.flush();
        CaptureRenderTargetSurface(sourceSurface, bmp);
        g_Context->Files.CurrentFrameAllEvents << "Done capturing render target surface\n";
        sourceSurface->Release();

        if(bmp.Width() > 0 && bmp.Height() > 0)
        {
            g_Context->Files.CurrentFrameAllEvents << "Saving webpage texture: " << Texture0ImageRGB << endl;
            WebpageTextureSave(bmp, Texture0ImageRGB, Texture0ImageAlpha);
        }
    }
    else
    {
	    WebpageTextureSave(Info.Texture0->Bmp(), Texture0ImageRGB, Texture0ImageAlpha);
    }

    if(curBaseTexture != NULL)
    {
        curBaseTexture->Release();
    }
	//WebpageTextureSave(*Info.Texture1, Texture1ImageRGB, Texture1ImageAlpha);

	String Texture0Description;
	if(Info.Texture0->ID().Length() == 1)
	{
		Texture0Description = "Unclassified";
	}
	else
	{
		if(Info.Texture0->Filename().Length() == 1)
		{
			Texture0Description = Info.Texture0->ID();
		}
		else
		{
			Texture0Description = Info.Texture0->Filename();
		}
	}
	//String Texture1Description = Info.Texture1->ID();

	String VShaderTargetFilename = String::ZeroPad(String::UnsignedIntAsHex(Info.VShaderHash), 8) + String(".txt");
	String VShaderTargetPath = g_Context->Parameters.ScreenCaptureDirectory + VShaderTargetFilename;
	String VShaderSourceFilename = g_Context->Parameters.ShaderCaptureDirectory + String("VShader") + String::ZeroPad(String(Info.VShaderHash), 10) + ".txt";
	if(!Utility::FileExists(VShaderTargetPath))
	{
		if(Utility::FileExists(VShaderSourceFilename))
		{
			Utility::CopyFile(VShaderSourceFilename, VShaderTargetPath);
		}
		else
		{
			ofstream File(VShaderTargetPath.CString());
			File << "Vertex shader capture failed\n";
		}
	}

	String PShaderTargetFilename = String::ZeroPad(String::UnsignedIntAsHex(Info.PShaderHash), 8) + String(".txt");
	String PShaderTargetPath = g_Context->Parameters.ScreenCaptureDirectory + PShaderTargetFilename;
	String PShaderSourceFilename = g_Context->Parameters.ShaderCaptureDirectory + String("PShader") + String::ZeroPad(String(Info.PShaderHash), 10) + ".txt";
	if(!Utility::FileExists(PShaderTargetPath))
	{
		if(Utility::FileExists(PShaderSourceFilename))
		{
			Utility::CopyFile(PShaderSourceFilename, PShaderTargetPath);
		}
		else
		{
			ofstream File(PShaderTargetPath.CString());
			File << "Pixel shader capture failed\n";
		}
	}

	os << "<td><img src=\"" << Texture0ImageRGB << "\"/></td>";
	os << "<td><img src=\"" << Texture0ImageAlpha << "\"/></td>";
	//os << "<td><img src=\"" << Texture1ImageRGB << "\"/> <img src=\"" << Texture1ImageAlpha << "\"/><br/>" << Texture1Description << "</td>";
	os << "<td>" << Texture0Description << "</td>";
	os << "<td>" << GetPrimitiveTypeString(Info.PrimitiveType) << "</td>";
	os << "<td>" << Info.PrimitiveCount << "</td>";
	os << "<td><a href=\"" << VShaderTargetFilename << "\">" << String::ZeroPad(String::UnsignedIntAsHex(Info.VShaderHash), 8) << "</a></td>";
	os << "<td><a href=\"" << PShaderTargetFilename << "\">" << String::ZeroPad(String::UnsignedIntAsHex(Info.PShaderHash), 8) << "</a></td>";
	os << "<td>" << Math::Round(Info.ScreenBound.Center().x) << ", " << Math::Round(Info.ScreenBound.Center().y) << "</td>";
    /*if(Info.Text.Length() == 0)
	{
		os << "<td> </td>";
	}
	else
	{
		os << "<td>" << Info.Text << "</td>";
	}*/
	os << "</tr>\n";
    os.flush();
    
	/*os << Info.Texture0->ID() << "\t";
    os << Info.Texture0->Filename() << "\t";
    os << Info.Texture1->ID() << "\t";
    os << Info.Texture1->Filename() << "\t";*/
}

void RenderManager::ReportRender(const RenderInfo &Info, ostream &os)
{
    os << _FrameRenderIndex << "\t";
    os << Info.Texture0->ID() << "\t";
    os << Info.Texture0->Filename() << "\t";
    os << Info.Texture1->ID() << "\t";
    os << Info.Texture1->Filename() << "\t";
    os << GetPrimitiveTypeString(Info.PrimitiveType) << "\t";
    os << Info.PrimitiveCount << "\t";

    VShader *CurVShader = g_Context->Managers.State.CurVShader;
    if(CurVShader == NULL)
    {
        os << Info.VShaderHash << " (fixed function)\t";
    }
    else
    {
        os << Info.VShaderHash << "\t";
    }

    os << Info.PShaderHash << "\t";
    os << Info.ScreenBound.Center().CommaSeparatedString() << "\t";
    os << Info.ScreenBound.Min.CommaSeparatedString() << "\t";
    os << Info.ScreenBound.Max.CommaSeparatedString() << "\t";
	//os << Info.ScreenBoundTest.Center().CommaSeparatedString() << "\t";
    //os << Info.ScreenBoundTest.Min.CommaSeparatedString() << "\t";
    //os << Info.ScreenBoundTest.Max.CommaSeparatedString() << "\t";
    os << Info.Text << endl;
}

ProcessedVertex* RenderManager::GetVertexData(RenderInfo &Info, UINT Index, ProcessedVertex *VBuffer)
{
    if(Info.IsIndexed)
    {
        Index = g_Context->Managers.State.CurIBuffer->GetIndex(Index);
    }
    return (VBuffer + Index);
}

void RenderManager::ProcessVerticesSoftware(RenderInfo &Info, const VertexProcessingOptions &Options)
{
	LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();

    ResizeScratchVertexBuffer(0, sizeof(ProcessedVertex), Info.NumVertices, ProcessedFVF);

    DWORD CurFVF = 0;
    HRESULT hr = Device->GetFVF(&CurFVF);
    Assert(SUCCEEDED(hr), "GetFVF failed");

    if(Info.IsUserPointer)
    {
        g_Context->Files.Assert << "UserPointer\n";
    }
    /*bool StreamSourceUsed = Info.IsUserPointer;
    D3DMATRIX WorldBefore, ViewBefore, ProjBefore;
    if(StreamSourceUsed)
    {
        ResizeScratchVertexBuffer(1, Info.UserVertexDataStride, Info.NumVertices, CurFVF);

        D3D9Base::LPDIRECT3DVERTEXBUFFER9 CurrentStream = NULL;
        UINT StreamOffset, StreamStride;
        Device->GetStreamSource(0, &CurrentStream, &StreamOffset, &StreamStride);
        if(CurrentStream == NULL)
        {
            BYTE *NewStreamData;
            hr = _ScratchVertexBuffers[1]->Lock(0, 0, (void **)&NewStreamData, 0);
            Assert(SUCCEEDED(hr), "Lock failed");
            
            memcpy(NewStreamData, Info.UserVertexData, Info.UserVertexDataStride * Info.NumVertices);
            
            hr = _ScratchVertexBuffers[1]->Unlock();
            Assert(SUCCEEDED(hr), "Unlock failed");
            
            hr = Device->SetStreamSource(0, _ScratchVertexBuffers[1], 0, Info.UserVertexDataStride);
            Assert(SUCCEEDED(hr), "SetStreamSource failed");
        }
        else
        {
            g_Context->Files.Assert << "RenderPrimitiveUP called while StreamSource non-NULL\n";
            CurrentStream->Release();
        }

        Device->GetTransform(D3DTS_WORLDMATRIX(0), &WorldBefore);
        Device->GetTransform(D3DTS_VIEW, &ViewBefore);
        Device->GetTransform(D3DTS_PROJECTION, &ProjBefore);
    }*/
    
    //
    // Starcraft 2 Uses both VS2.0 and VS 3.0 shaders
    //
    bool UseVertexDeclaration = false;
    D3D9Base::LPDIRECT3DVERTEXSHADER9 CurrentVertexShader = NULL;
    hr = Device->GetVertexShader(&CurrentVertexShader);
    Assert(SUCCEEDED(hr), "GetVertexShader failed");
    if(CurrentVertexShader != NULL)
    {
        UINT SizeNeeded = 0;
        CurrentVertexShader->GetFunction(NULL, &SizeNeeded);
        Vector<BYTE> Function(SizeNeeded);
        CurrentVertexShader->GetFunction(Function.CArray(), &SizeNeeded);

        UINT MajorVersion = D3DSHADER_VERSION_MAJOR( ((DWORD*)Function.CArray())[0] );

        if(MajorVersion >= 3)
        {
            UseVertexDeclaration = true;
        }
        CurrentVertexShader->Release();
    }
    
    if(UseVertexDeclaration)
    {
        if(_ProcessedVerticesDeclaration == NULL)
        {
            D3D9Base::D3DVERTEXELEMENT9 Elements[] =
                {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},
                 {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                 {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
                 D3DDECL_END()};
            HRESULT hr = Device->CreateVertexDeclaration(Elements, &_ProcessedVerticesDeclaration);
            Assert(SUCCEEDED(hr), "CreateVertexDeclaration failed");
        }
        hr = Device->ProcessVertices(Info.BaseVertexIndex, 0, Info.NumVertices, _ScratchVertexBuffers[0], _ProcessedVerticesDeclaration, 0);
    }
    else
    {
        hr = Device->ProcessVertices(Info.BaseVertexIndex, 0, Info.NumVertices, _ScratchVertexBuffers[0], NULL, 0);
    }

    bool ProcessVerticesFailed = FAILED(hr);
    if(ProcessVerticesFailed)
    {
        g_Context->Files.Assert << "Process vertices failed";
    }

    /*if(StreamSourceUsed)
    {
        g_Context->Graphics.Device()->SetStreamSource(0, NULL, 0, 0);
        g_Context->Graphics.Device()->SetFVF(CurFVF);
        g_Context->Graphics.Device()->SetVertexShader(NULL);

        Device->SetTransform(D3DTS_WORLDMATRIX(0), &WorldBefore);
        Device->SetTransform(D3DTS_VIEW, &ViewBefore);
        Device->SetTransform(D3DTS_PROJECTION, &ProjBefore);
    }*/

    _HardwareProcessedVertices.ReSize(Info.NumVertices);
    ProcessedVertex *ProcessedVertices = _HardwareProcessedVertices.CArray();
    
    if(ProcessVerticesFailed)
    {
        for(UINT VertexIndex = 0; VertexIndex < Info.NumVertices; VertexIndex++)
        {
            ProcessedVertices[VertexIndex].TransformedProjectionPos = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else
    {
        ProcessedVertex *LockedProcessedVertices = NULL;
        hr = _ScratchVertexBuffers[0]->Lock(0, 0, (void **)&LockedProcessedVertices, 0);
        Assert(SUCCEEDED(hr), "Processed vertex buffer lock failed");
        
        for(UINT VertexIndex = 0; VertexIndex < Info.NumVertices; VertexIndex++)
        {
            const ProcessedVertex &LockedVertex = LockedProcessedVertices[VertexIndex];
            ProcessedVertices[VertexIndex] = LockedVertex;
        }

        hr = _ScratchVertexBuffers[0]->Unlock();
        Assert(SUCCEEDED(hr), "Processed vertex buffer unlock failed");
    }
}

void RenderManager::ProcessVerticesHardware(RenderInfo &Info, const VertexProcessingOptions &Options)
{
	StateManager &State = g_Context->Managers.State;
	LPDIRECT3DDEVICE9 Device = g_Context->Graphics.Device();

	_HardwareProcessedVertices.ReSize(Info.NumVertices);
    ProcessedVertex *ProcessedVertices = _HardwareProcessedVertices.CArray();

    D3DVIEWPORT9 CurViewport;
    Device->GetViewport(&CurViewport);
    Matrix4 Viewport = Matrix4::Scaling(Vec3f(1.0f, -1.0f, 1.0f)) *
		               Matrix4::Translation(Vec3f(1.0f, 1.0f, 0.0f)) *
					   Matrix4::Scaling(Vec3f(CurViewport.Width / 2.0f, CurViewport.Height / 2.0f, 1.0f)) *
					   Matrix4::Translation(Vec3f(float(CurViewport.X), float(CurViewport.Y), 0.0f));
	
	VShader *CurVShader = g_Context->Managers.State.CurVShader;
    if(CurVShader == NULL)
    {
		g_Context->Files.Assert << "No VShader\n";
        return;
    }
    
	Matrix4 ObjectToScreen;
	if(CurVShader->DeclarationFound(D3DDECLUSAGE_NORMAL))
	{
		Matrix4 ObjectToCamera;
		Matrix4 CameraToProjection;

		ObjectToCamera.SetColumn(0, State.VShaderFloatConstants[0]);
		ObjectToCamera.SetColumn(1, State.VShaderFloatConstants[1]);
		ObjectToCamera.SetColumn(2, State.VShaderFloatConstants[2]);
		ObjectToCamera.SetColumn(3, Vec4f(0.0f, 0.0f, 0.0f, 1.0f));

		CameraToProjection.SetRow(0, State.VShaderFloatConstants[198]);
		CameraToProjection.SetRow(1, State.VShaderFloatConstants[199]);
		CameraToProjection.SetRow(2, State.VShaderFloatConstants[200]);
		CameraToProjection.SetRow(3, State.VShaderFloatConstants[201]);
		ObjectToScreen = ObjectToCamera * CameraToProjection * Viewport;
	}
	else
	{
		Matrix4 ObjectToProjection;

		ObjectToProjection.SetRow(0, State.VShaderFloatConstants[0]);
		ObjectToProjection.SetRow(1, State.VShaderFloatConstants[1]);
		ObjectToProjection.SetRow(2, State.VShaderFloatConstants[2]);
		ObjectToProjection.SetRow(3, State.VShaderFloatConstants[3]);
		ObjectToScreen = ObjectToProjection * Viewport;
	}

	if(g_ReportingEvents)
	{
		g_Context->Files.CurrentFrameAllEvents << "ObjectToScreen: " << ObjectToScreen.CommaSeparatedStringSingleLine() << endl;
		g_Context->Files.CurrentFrameAllEvents << "Viewport: " << Viewport.CommaSeparatedStringSingleLine() << endl;
	}
	
	BYTE *PositionStreamStart = NULL;
	UINT PositionStride = 0;

	BYTE *Texture0StreamStart = NULL;
	UINT Texture0Stride = 0;

	BYTE *ColorStreamStart = NULL;
	UINT ColorStride = 0;

	for(UINT i = 0; i < State.VertexDeclaration.Length(); i++)
    {
        const D3D9Base::D3DVERTEXELEMENT9 &Decl = State.VertexDeclaration[i];
		StreamInfo &Stream = State.VBufferStreams[Decl.Stream];
		if(Stream.StreamData == NULL)
		{
			g_Context->Files.Assert << "Reference to unbound stream\n";
			return;
		}
		if(Decl.Usage == D3DDECLUSAGE_POSITION)
		{
			PositionStreamStart = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Decl.Offset);
			PositionStride = Stream.Stride;
		}
		if(Decl.Usage == D3DDECLUSAGE_TEXCOORD && Decl.UsageIndex == 0)
		{
			Texture0StreamStart = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Decl.Offset);
			Texture0Stride = Stream.Stride;
		}
		if(Decl.Usage == D3DDECLUSAGE_COLOR && Decl.UsageIndex == 0)
		{
			ColorStreamStart = (BYTE *)(Stream.StreamData->Buffer.CArray() + Stream.OffsetInBytes + Decl.Offset);
			ColorStride = Stream.Stride;
		}
    }

	if(PositionStreamStart == NULL)
	{
		g_Context->Files.Assert << "Call has no position\n";
		return;
	}

	for(UINT VertexIndex = 0; VertexIndex < Info.NumVertices; VertexIndex++)
	{
		ProcessedVertex &CurVertex = ProcessedVertices[VertexIndex];

		float *PositionStream = (float *)(PositionStreamStart + VertexIndex * PositionStride);
		Vec3f Position(PositionStream[0], PositionStream[1], PositionStream[2]);
		CurVertex.TransformedProjectionPos = Vec4f(ObjectToScreen.TransformPoint(Position), 1.0f);
		if(Texture0StreamStart != NULL)
		{
			float *Texture0Stream = (float *)(Texture0StreamStart + VertexIndex * Texture0Stride);
			CurVertex.TexCoord = Vec2f(Texture0Stream[0], Texture0Stream[1]);
		}
		if(ColorStreamStart != NULL)
		{
			RGBColor *ColorStream = (RGBColor *)(ColorStreamStart + VertexIndex * ColorStride);
			CurVertex.Diffuse = ColorStream[0];
		}
		if(VertexIndex == 0 && g_ReportingEvents)
		{
			g_Context->Files.CurrentFrameAllEvents << "Position: " << Position.CommaSeparatedString() << endl;
		}
	}
}

void RenderManager::ProcessVertices(RenderInfo &Info, const VertexProcessingOptions &Options)
{
    //ProcessVerticesSoftware(Info, Options);
	ProcessVerticesHardware(Info, Options);
}

//
// Performs transforms on Info to fill in several fields
//
void RenderManager::DoTransforms(RenderInfo &Info, const VertexProcessingOptions &Options)
{
    Info.ScreenBound.Min = Vec2f::Origin;
    Info.ScreenBound.Max = Vec2f::Origin;
    
    if(!Options.FullSimulation)
    {
        return;
    }

    ProcessVertices(Info, Options);

    const ProcessedVertex *ProcessedVertices = _HardwareProcessedVertices.CArray();

    Rectangle2f ScreenBound(Vec2f::Origin, Vec2f::Origin);
    for(UINT VertexIndex = 0; VertexIndex < Info.NumVertices; VertexIndex++)
    {
        const ProcessedVertex &CurVertex = ProcessedVertices[VertexIndex];
        const Vec4f &TransformedProjectionPos = CurVertex.TransformedProjectionPos;
        Vec2f ScreenPos = Vec2f(CurVertex.TransformedProjectionPos.x, CurVertex.TransformedProjectionPos.y);

        if(VertexIndex == 0)
        {
            ScreenBound.Min = ScreenPos;
            ScreenBound.Max = ScreenPos;
        }
        else
        {
            ScreenBound.Min = Vec2f::Minimize(ScreenBound.Min, ScreenPos);
            ScreenBound.Max = Vec2f::Maximize(ScreenBound.Max, ScreenPos);
        }
    }
    Info.ScreenBound = ScreenBound;
}

__forceinline UINT VertexCountFromPrimitiveCount(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount)
{
    switch(PrimitiveType)
    {
    case D3DPT_POINTLIST:
        return PrimitiveCount;
    case D3DPT_LINELIST:
        return PrimitiveCount * 2;
    case D3DPT_LINESTRIP:
        return PrimitiveCount + 1;
    case D3DPT_TRIANGLELIST:
        return PrimitiveCount * 3;
    case D3DPT_TRIANGLESTRIP:
        return PrimitiveCount + 2;
    case D3DPT_TRIANGLEFAN:
        return PrimitiveCount + 2;
    default:
        SignalError("Invalid primitive type");
        return 0;
    }
}

bool RenderManager::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    RenderInfo Info;
    Info.PrimitiveType = PrimitiveType;
    Info.NumVertices = VertexCountFromPrimitiveCount(PrimitiveType, PrimitiveCount);
    Info.MinIndex = 0;
    Info.PrimitiveCount = PrimitiveCount;
    Info.StartIndex = 0;
    Info.BaseVertexIndex = StartVertex;
    Info.IsIndexed = false;
    Info.IsUserPointer = false;
    Info.UserIndexData = NULL;
    Info.UserVertexData = NULL;
    Info.UserVertexDataStride = 0;
    return Draw(Info);
}

bool RenderManager::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    RenderInfo Info;
    Info.PrimitiveType = PrimitiveType;
    Info.NumVertices = VertexCountFromPrimitiveCount(PrimitiveType, PrimitiveCount);
    Info.MinIndex = 0;
    Info.PrimitiveCount = PrimitiveCount;
    Info.StartIndex = 0;
    Info.BaseVertexIndex = 0;
    Info.IsIndexed = false;
    Info.IsUserPointer = true;
    Info.UserIndexData = NULL;
    Info.UserVertexData = pVertexStreamZeroData;
    Info.UserVertexDataStride = VertexStreamZeroStride;
    return Draw(Info);
}

bool RenderManager::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
    RenderInfo Info;
    Info.PrimitiveType = PrimitiveType;
    Info.NumVertices = NumVertices;
    Info.MinIndex = MinIndex;
    Info.PrimitiveCount = PrimitiveCount;
    Info.StartIndex = StartIndex;
    Info.BaseVertexIndex = BaseVertexIndex;
    Info.IsIndexed = true;
    Info.IsUserPointer = false;
    Info.UserIndexData = NULL;
    Info.UserVertexData = NULL;
    Info.UserVertexDataStride = 0;
    return Draw(Info);
}

bool RenderManager::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    RenderInfo Info;
    Info.PrimitiveType = PrimitiveType;
    Info.NumVertices = NumVertices;
    Info.MinIndex = MinVertexIndex;
    Info.PrimitiveCount = PrimitiveCount;
    Info.StartIndex = 0;
    Info.BaseVertexIndex = 0;
    Info.IsIndexed = true;
    Info.IsUserPointer = true;
    Info.UserIndexData = (const WORD *)pIndexData;
    Info.UserVertexData = pVertexStreamZeroData;
    Info.UserVertexDataStride = VertexStreamZeroStride;
    Info.Is32bitIndices = (IndexDataFormat == D3DFMT_INDEX32);
    return Draw(Info);
}

void RenderManager::LoadBitmapSubset(const Bitmap &Src, Bitmap &Dest, const Rectangle2f &Rect)
{
    Vec2i Start, End, Dim;
    
    Start.x = Math::Round(Rect.Min.x * Src.Width());
    Start.y = Math::Round(Rect.Min.y * Src.Height());

    End.x = Math::Round(Rect.Max.x * Src.Width());
    End.y = Math::Round(Rect.Max.y * Src.Height());
    
    Dim = End - Start;
    Dest.Allocate(Dim.x, Dim.y);
    Src.BltTo(Dest, 0, 0, Start.x, Start.y, Dim.x, Dim.y);
}

bool RenderManager::LettersAdjacent(const Rectangle2f &Rect0, const Rectangle2f &Rect1)
{
    if(Math::Abs(Rect1.Min.x - Rect0.Max.x) > 5.0f)
    {
        return false;
    }
    if(Math::Abs(Rect1.Min.y - Rect0.Min.y) > 10.0f)
    {
        return false;
    }
    if(Math::Abs(Rect1.Max.y - Rect0.Max.y) > 10.0f)
    {
        return false;
    }
    return true;
}

void RenderManager::ProcessFont(RenderInfo &Info)
{
    if(Info.PrimitiveType != D3DPT_TRIANGLELIST)
    {
        g_Context->Files.Assert << "Unexpected font render parameters\n";
        return;
    }

    if(_HardwareProcessedVertices.Length() != Info.PrimitiveCount * 2)
    {
        g_Context->Files.Assert << "Unexpected number of vertices in font render\n";
        return;
    }

	UINT LettersToCapture = Info.PrimitiveCount / 4;

	const Rectangle2i &UnitInfoRegion = g_Context->Constants.GetRectangle2iConstant(ScreenConstantRectangle2iUnitInfoRegion);
	const Rectangle2i &MenuRegion = g_Context->Constants.GetRectangle2iConstant(ScreenConstantRectangle2iMenuRegion);
    const Rectangle2i &WarpGateRegion = g_Context->Constants.GetRectangle2iConstant(ScreenConstantRectangle2iWarpGateRegion);

    if(ScreenBoundInsideRegion(Info.ScreenBound, MenuRegion))
    {
        if(g_ReportingEvents)
		{
			g_Context->Files.CurrentFrameAllEvents << "Menu string\n";
			Info.Text = "Menu";
		}
		return;
    }
    if(ScreenBoundInsideRegion(Info.ScreenBound, UnitInfoRegion) ||
       ScreenBoundInsideRegion(Info.ScreenBound, WarpGateRegion))
    {
		LettersToCapture = Info.PrimitiveCount / 2;
    }
    
    const Bitmap &FontBmp = g_Context->Managers.State.CurTexture[0]->Bmp();
    
	Rectangle2f PrevScreenPosBounds;
    for(UINT LetterIndex = 0; LetterIndex < LettersToCapture; LetterIndex++)
    {         
        Rectangle2f TexCoordBounds;
        Rectangle2f ScreenPosBounds;
        for(UINT VertexIndex = 0; VertexIndex < 3; VertexIndex++)
        {
            const ProcessedVertex &CurVertex = _HardwareProcessedVertices[LetterIndex * 4 + VertexIndex];
            const Vec2f CurScreenPos = Vec2f(CurVertex.TransformedProjectionPos.x, CurVertex.TransformedProjectionPos.y);
            const Vec2f CurTexCoord = Vec2f(CurVertex.TexCoord.x, 1.0f - CurVertex.TexCoord.y);
            if(VertexIndex == 0)
            {
                TexCoordBounds.Min = CurTexCoord;
                TexCoordBounds.Max = CurTexCoord;
                ScreenPosBounds.Min = CurScreenPos;
                ScreenPosBounds.Max = CurScreenPos;
            }
            else
            {
                TexCoordBounds.ExpandBoundingBox(CurTexCoord);
                ScreenPosBounds.ExpandBoundingBox(CurScreenPos);
            }
        }
        
        Bitmap LetterBmp;
        LoadBitmapSubset(FontBmp, LetterBmp, TexCoordBounds);

        if(LetterIndex != 0 && !LettersAdjacent(PrevScreenPosBounds, ScreenPosBounds))
        {
            Info.Text.PushEnd('@');
        }

        const TextureEntry* FontMatch = g_Context->Managers.TextureEntry.FindFontMatch(LetterBmp);
        if(FontMatch == NULL)
        {
            const bool CaptureLetters = false;
            if(CaptureLetters)
            {
                UINT Hash = LetterBmp.Hash32();
                g_Context->Files.Events << "Saving letter " << Info.PrimitiveCount << "\n";
                String Filename = g_Context->Parameters.LetterCaptureDirectory + String::ZeroPad(String(Hash), 9) + String(".png");
                if(LetterBmp.Width() > 0 && LetterBmp.Height() > 0 && !Utility::FileExists(Filename))
                {
                    BitmapSaveOptions Options;
                    Options.SaveAlpha = true;
                    Options.UseBGR = true;

                    LetterBmp.SavePNG(Filename, Options);
                    /*LetterBmp.SavePNG(g_Context->Parameters.LetterCaptureDirectory + 
                        String("Frame") + String(g_Context->Controller.FrameIndex()) +
                        String("Obj") + String::ZeroPad(String(_FrameRenderIndex), 4) + 
                        String("Letter") + String::ZeroPad(String(LetterIndex), 4) +
                        String(".png"), Options);*/
                }
            }
            Info.Text.PushEnd('*');
        }
        else
        {
            if(g_ReportingEvents && LettersToCapture <= 4)
            {
                g_Context->Files.CurrentFrameAllEvents << FontMatch->FirstCharacter << ": " << FontMatch->Filename << endl;
            }
            Info.Text.PushEnd(FontMatch->FirstCharacter);
        }
        PrevScreenPosBounds = ScreenPosBounds;
    }
    //g_Context->Managers.FrameHUD.ReportFont(Info);
}

bool RenderManager::PreRenderQuery(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount)
{

	const Texture *Texture0 = g_Context->Managers.State.CurTexture[0];
    const RenderType Type0 = Texture0->Type();
    const String &ID0 = Texture0->ID();

	return false;
    if((!g_Context->Controller.AIEnabled() && !g_ReportingEvents) || WebpageCaptureMode)
    {
        return false;
    }

	//
	// TODO: add ID0 == "Various", but make sure not to eliminate selection circles
	//
    if(ID0 == "Procedural" || ID0 == "SystemFont" || ID0 == "GeyserBorder" || ID0 == "ProtossHUD" || ID0 == "Sky" || ID0 == "Glow" || ID0 == "Terrain" || ID0 == "Foliage" || ID0 == "Unknown")
    {
        return true;
    }

    return false;
}

void PrintVertex(TriListVertex &v) {
	g_Context->Files.Thought << "    Position {" << v.p.x << ", " << v.p.y << ", " << v.p.z << "}  UV {" << v.uv.x << ", " << v.uv.y << "}" << endl;
}

bool IsBulletDrawCall(RenderInfo &Info) {
	UINT bmpHash = Info.Texture0->BmpHash();
	/*
	g_Context->Files.CurrentFrameAllEvents << "Is bullet draw call?" << endl;
	g_Context->Files.CurrentFrameAllEvents << (Info.PrimitiveType == D3DPT_TRIANGLELIST) << endl;
	g_Context->Files.CurrentFrameAllEvents << (Info.PrimitiveType % 2 == 0) << endl;
	g_Context->Files.CurrentFrameAllEvents << bmpHash << " " << BULLETS1_HASH << " " << BULLETS2_HASH << endl;
	*/
	return Info.PrimitiveType == D3DPT_TRIANGLELIST && Info.PrimitiveCount % 2 == 0 && 
		(bmpHash == BULLETS1_HASH || bmpHash == BULLETS2_HASH);
}

bool RenderManager::Draw(RenderInfo &Info)
{
	bool RenderHighlighted = false;
    
    Info.Texture0 = g_Context->Managers.State.CurTexture[0];
    Info.Texture1 = g_Context->Managers.State.CurTexture[1];
    VShader *CurVShader = g_Context->Managers.State.CurVShader;
    if(CurVShader == NULL)
    {
        Info.VShaderHash = 0;
    }
    else
    {
        Info.VShaderHash = CurVShader->Hash();
    }

    PShader *CurPShader = g_Context->Managers.State.CurPShader;
    if(CurPShader == NULL)
    {
        Info.PShaderHash = 0;
    }
    else
    {
        Info.PShaderHash = CurPShader->Hash();
    }
    
    const RenderType Type0 = Info.Texture0->Type();
    const RenderType Type1 = Info.Texture1->Type();
    const String &ID0 = Info.Texture0->ID();
    const String &ID1 = Info.Texture1->ID();

    VertexProcessingOptions Options;
    Options.FullSimulation = g_ReportingEvents;

    if(Type0 == RenderFont)
    {
        Options.FullSimulation = true;
    }
    
    DoTransforms(Info, Options);

	if (IsBulletDrawCall(Info)) {
		int prim_count = Info.PrimitiveCount;
		TriListVertex *vs = (TriListVertex *)Info.UserVertexData;
		g_Context->Managers.Bullet.LoadBulletsFromCall(prim_count, vs, Info);
		if (g_ReportingEvents) {
			g_Context->Files.Thought << "BULLETS IN FRAME " << g_Context->Controller.FrameIndex() << endl;
			g_Context->Managers.Bullet.PrintAllBullets(g_Context->Files.Thought);
		}
	}

    //
    // Report this render event if appropriate
    //
	if (g_ReportingEvents && g_Context->Files.CurrentFrameAllEvents.is_open())
    {
        ReportRender(Info, g_Context->Files.CurrentFrameRenderEvents);
        ReportRender(Info, g_Context->Files.CurrentFrameAllEvents);
		if(WebpageCaptureMode)
		{
			ReportRenderWebpage(Info);
		}

        Bitmap Bmp;
		UINT MaxDimension = 512;
		if(WebpageCaptureMode)
		{
			MaxDimension = 342;
		}
        CaptureCurrentRenderTarget(Bmp, MaxDimension);
		Bmp.SavePNG(g_Context->Parameters.ScreenCaptureDirectory + String("Frame") + String(g_Context->Controller.FrameIndex()) + String("Obj") + String::ZeroPad(String(_FrameRenderIndex), 5) + String(".png"));
    }

    _FrameRenderIndex++;

    
    return RenderHighlighted;
}
