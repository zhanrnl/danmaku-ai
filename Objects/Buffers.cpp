#include "Main.h"

void VBuffer::Update(BufferLockData &Data, D3DVERTEXBUFFER_DESC &_Desc)
{
    Desc = _Desc;
    if(Buffer.Length() != Desc.Size)
    {
        Buffer.Allocate(Desc.Size);
        Buffer.ZeroMem();
    }

    Assert(Handle == Data.Handle, "Invalid handle");
    Data.pRAMBuffer = (VOID*)(Buffer.CArray() + Data.OffsetToLock);
}

void IBuffer::Update(BufferLockData &Data, D3DINDEXBUFFER_DESC &_Desc)
{
    Desc = _Desc;
    if(Buffer.Length() != Desc.Size)
    {
        Buffer.Allocate(Desc.Size);
        Buffer.ZeroMem();
    }

    Assert(Handle == Data.Handle, "Invalid handle");
    Data.pRAMBuffer = (VOID*)(Buffer.CArray() + Data.OffsetToLock);
}

BufferHasher::BufferHasher()
{

}

ULONGLONG BufferHasher::Hash(const char *Buffer, ULONG BufferLength)
{
    ULONGLONG Result = BufferLength * 13 + BufferLength * BufferLength * 52 + BufferLength * BufferLength * BufferLength * 11;
    for(UINT i=0;i<BufferLength;i++)
    {
        Result += Buffer[i] * Buffer[i] * 452 + Buffer[i] * i * 531 + i * i * 9735 + Buffer[i] * Buffer[i] * Buffer[i] * i * i * 3;
    }
    return Result;
}

bool Texture::IsMovieBitmap(const Bitmap &Bmp)
{
	const Vec2i Dimensions = Bmp.Dimensions();
	if(Dimensions == Vec2i(200, 200) || Dimensions == Vec2i(300, 160))
	{
		return true;
	}
	return false;
}

bool Texture::IsProceduralBitmap(const Bitmap &Bmp)
{
    if(Bmp.Width() != Bmp.Height())
    {
        return false;
    }
    if(Bmp.Width() != 128 && Bmp.Width() != 256 && Bmp.Width() != 512)
    {
        return false;
    }
    const RGBColor Color0(10, 5, 5);
    const RGBColor Color1(21, 10, 10);
    UINT Color0Count = 0, Color1Count = 0;
    for(UINT y = 0; y < Bmp.Height(); y++)
    {
        for(UINT x = 0; x < Bmp.Width(); x++)
        {
            const RGBColor C = Bmp[y][x];
            if(C == Color0)
            {
                Color0Count++;
            }
            if(C == Color1)
            {
                Color1Count++;
            }
        }
    }
    return (Color0Count > 1000 || Color1Count > 1000);
}

bool Texture::IsFontBitmap(const Bitmap &Bmp)
{
    if(g_Context->Parameters.IgnoreAllFonts)
    {
        return false;
    }
    if(Bmp.Width() != Bmp.Height())
    {
        return false;
    }
    if(Bmp.Width() != 128 && Bmp.Width() != 256 && Bmp.Width() != 512 && Bmp.Width() != 1024)
    {
        //
        // all fonts are powers of two
        //
        return false;
    }
    const UINT HalfWidth = Bmp.Width() / 2;
    bool BlackPresent = false, WhitePresent = false, NonBlackPresent = false, AlphaVariation = false;
    for(UINT y = 0; y < Bmp.Height(); y++)
    {
        for(UINT x = 0; x < Bmp.Width(); x++)
        {
            const RGBColor C = Bmp[y][x];
            if((C.r != C.g || C.r != C.b) && x > HalfWidth + 1)
            {
                //
                // all fonts are grayscale on the right-hand side
                //
                return false;
            }
			if(C.r != 0 || C.g != 0 || C.b != 0 || C.a != 0)
			{
				int a = 5;
			}
            if(C == RGBColor::Black)
            {
                BlackPresent = true;
            }
            else if(C == RGBColor::White)
            {
                WhitePresent = true;
            }
            else
            {
                NonBlackPresent = true;
            }
            if(C.a != 0)
            {
                AlphaVariation = true;
            }
        }
    }
    return ((BlackPresent || WhitePresent) && (NonBlackPresent || AlphaVariation));
}

bool Texture::IsScrapBitmap(const Bitmap &Bmp)
{
    if(Bmp.Width() != Bmp.Height())
    {
        return false;
    }
    if(Bmp.Width() != 128 && Bmp.Width() != 256 && Bmp.Width() != 512 && Bmp.Width() != 1024)
    {
        //
        // all scrap bitmaps are powers of two
        //
        return false;
    }
    bool GrayscalePresent = false, WhitePresent = false, BlackPresent = false;
    for(UINT y = 0; y < Bmp.Height(); y++)
    {
        for(UINT x = 0; x < Bmp.Width(); x++)
        {
            const RGBColor C = Bmp[y][x];
            if(C.r != C.g || C.r != C.b)
            {
                //
                // all scrap bitmaps are grayscale
                //
                return false;
            }
            if(C == RGBColor::White)
            {
                WhitePresent = true;
            }
            else if(C == RGBColor::Black)
            {
                WhitePresent = true;
            }
			else
            {
                GrayscalePresent = true;
            }
        }
    }
    return ((WhitePresent || BlackPresent) && GrayscalePresent);
}

void Texture::Update(const Bitmap &Bmp, const D3DSURFACE_DESC &Desc)
{
	/*Bitmap B;
	B.LoadPNG("C:\\SC2AICapture\\CapturedTextures\\3364950766.png");
	Utility::MessageBox("Break");
	bool Result0 = IsFontBitmap(B);
	bool Result1 = IsScrapBitmap(B);
	return;*/

    _BmpHash = Bmp.Hash32();
    _Filename = "*";
    _Unit = NULL;
    
    bool SaveThisBmp = !g_Context->Parameters.DiscardAllBitmaps;
    const TextureEntry *TextureMatch = g_Context->Managers.TextureEntry.FindTextureMatch(Bmp);

    RGBColor TopLeftColor = RGBColor::Black;
    if(Bmp.Width() >= 1 && Bmp.Height() >= 1)
    {
        TopLeftColor = Bmp[0][0];
    }
    
    if(Bmp.Width() * Bmp.Height() <= 1 * 1)
    {
        SaveThisBmp = false;
        _ID = "Pixel";
        _Type = RenderSpecial;
    }
    else if(Bmp.MonochromeIncludingAlpha(TopLeftColor))
    {
        SaveThisBmp = false;
        if(TopLeftColor == RGBColor::Magenta)
        {
            _ID = "Magenta";
        }
        else
        {
            _ID = String("r") + String(UINT(TopLeftColor.r)) +
                  String("g") + String(UINT(TopLeftColor.g)) +
                  String("b") + String(UINT(TopLeftColor.b)) +
                  String("a") + String(UINT(TopLeftColor.a));
        }
        _Type = RenderSpecial;
    }
    else if(TextureMatch != NULL)
    {
        _ID = TextureMatch->ID;
        _Filename = TextureMatch->Filename;
        _Type = TextureMatch->Type;
        SaveThisBmp = false;
        if(g_ReportingEvents)
        {
            //g_Context->Files.CurrentFrameAllEvents << "Texture::Update: match found: " << GetRenderTypeString(_Type) << " " << _ID << endl;
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: match found: " << _ID << endl;
        }
    }
    else if(IsFontBitmap(Bmp))
    {
        SaveThisBmp = false;
        _ID = "Font";
        _Type = RenderFont;
        _Bmp = Bmp;
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: Font\n";
        }    
    }
	else if(IsScrapBitmap(Bmp))
    {
        SaveThisBmp = false;
        _ID = "Scrap";
        _Type = RenderDecoration;
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: Scrap\n";
        }    
    }
    else if(IsProceduralBitmap(Bmp))
    {
        SaveThisBmp = false;
        _ID = "Procedural";
        _Type = RenderDecoration;
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: Procedural\n";
        }    
    }
	else if(IsMovieBitmap(Bmp))
	{
		SaveThisBmp = false;
        _ID = "Movie";
        _Type = RenderDecoration;
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: Movie\n";
        }
	}
    else
    {
        _ID = "Unmatched";
        _Type = RenderNoMatchFound;
        if(g_ReportingEvents)
        {
            g_Context->Files.CurrentFrameAllEvents << "Texture::Update: No match found\n";
        }
    }

    if(SaveThisBmp)
    {
        String Filename = g_Context->Parameters.TextureCaptureDirectory + String::ZeroPad(String(_BmpHash), 10) + String(".png");
        if(!Utility::FileExists(Filename))
        {
            BitmapSaveOptions Options;
            Options.SaveAlpha = true;
            Options.UseBGR = true;
            Bmp.SavePNG(Filename, Options);
            g_Context->Controller.TexturesSavedThisFrame()++;
        }
    }

    if(_Type == RenderUnit || _Type == RenderPortrait)
    {
        _Unit = g_Context->Managers.Database.GetUnitEntry(_ID);
    }

    if(WebpageCaptureMode)
	{
		_Bmp = Bmp;
	}
}