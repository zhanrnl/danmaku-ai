//
// RenderManager.h
//
// Structures for handling all DrawIndexedPrimitive calls
//

const UINT MaxTexCoords = 8;
const UINT MaxColorCoords = 2;

const UINT BULLETS1_HASH = 3924195017;
const UINT BULLETS2_HASH = 2731789217;
const UINT REIMU_HASH = 2973107762;
const UINT MARISA_HASH = 755481399;
const UINT POWERUP_HASH = 334748864;
const UINT SCORE_HASH = 4102754665;
const UINT ENEMY_HASH = 325298167;

// Bosses
const UINT SHIZUHA_MINORIKO_HASH	= 820914528;
const UINT HINA_HASH				= 867588864;
const UINT NITORI1_HASH				= 1029603703;
const UINT NITORI2_HASH				= 3352186118;
const UINT MOMIZI_HASH				= 3474212404; // also contains birds
const UINT AYA_HASH					= 1111547272;
const UINT SANAE_HASH				= 4261938554;
const UINT KANAKO_HASH				= 2183085055;
const UINT SUWAKO_HASH				= 300481864;

//
// All information about a single render event
//
struct RenderInfo
{
    UINT VShaderHash, PShaderHash;

    Texture *Texture0;
    Texture *Texture1;
    
    String Text;

    Rectangle2f ScreenBound;
	//Rectangle2f ScreenBoundTest;
    
    D3DPRIMITIVETYPE PrimitiveType;
    UINT NumVertices;
    UINT MinIndex;
    UINT PrimitiveCount;
    UINT StartIndex;
    UINT BaseVertexIndex;
    bool IsIndexed;
    bool Is32bitIndices;
    bool IsUserPointer;
    const void* UserIndexData;
    const void* UserVertexData;
    UINT UserVertexDataStride;
};

struct VertexData
{
    Vec4f Position;
    Vec4f Normal;
    Vec4f BlendWeight;
    Vec4f BlendIndices;
    Vec4f BiNormal, Tangent;
    Vec4f Color[MaxColorCoords];
    Vec4f TexCoord[MaxTexCoords];
};

struct VertexProcessingOptions
{
	bool FullSimulation;
};

struct TriListVertex {
	Vec4f p;
	D3DCOLOR c;
	Vec2f uv;
};

class RenderManager : public Manager
{
public:
    RenderManager();

    void StartFrame();
    void EndFrame();

    bool PreRenderQuery(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount);
    bool DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    bool DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);
    bool DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
    bool DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
    
    void CaptureRenderTargetSurface(LPDIRECT3DSURFACE9 RenderTarget, Bitmap &Bmp);
	void CaptureCurrentRenderTarget(Bitmap &Bmp, UINT MaxDimension);
    bool CaptureCurrentRenderTargetSubset(Bitmap &Bmp, const Rectangle2i &Region);

    void ReleaseStorageObjects();
    
private:
    void CompressCurrentFrame();

    void ResizeScratchSurface(UINT Index, const D3DSURFACE_DESC &Desc);
	void UpdateScratchTexture(UINT Index, const Bitmap &Bmp);
    void ResizeScratchVertexBuffer(UINT Index, UINT VertexSize, UINT VertexCount, DWORD FVF);
    
    void LoadBitmapSubset(const Bitmap &Src, Bitmap &Dest, const Rectangle2f &Rect);
    void ReportRender(const RenderInfo &Info, ostream &os);
	void ReportRenderWebpage(const RenderInfo &Info);
    bool Draw(RenderInfo &Info);
    void ProcessFont(RenderInfo &Info);
	void DoTransforms(RenderInfo &Info, const VertexProcessingOptions &Options);
    
    void ProcessVertices(RenderInfo &Info, const VertexProcessingOptions &Options);
    void ProcessVerticesSoftware(RenderInfo &Info, const VertexProcessingOptions &Options);
	void ProcessVerticesHardware(RenderInfo &Info, const VertexProcessingOptions &Options);
    
    ProcessedVertex* GetVertexData(RenderInfo &Info, UINT Index, ProcessedVertex *VBuffer);

    static bool LettersAdjacent(const Rectangle2f &Rect0, const Rectangle2f &Rect1);

    static const UINT ScratchVertexBufferCount = 2;
    D3D9Base::LPDIRECT3DVERTEXBUFFER9 _ScratchVertexBuffers[ScratchVertexBufferCount];

    static const UINT ScratchSurfaceCount = 2;
    D3D9Base::LPDIRECT3DSURFACE9 _ScratchSurfaces[ScratchSurfaceCount];

	static const UINT ScratchTextureCount = 1;
    D3D9Base::LPDIRECT3DTEXTURE9 _ScratchTextures[ScratchTextureCount];

    Vector<ProcessedVertex> _HardwareProcessedVertices;
	Vector<ProcessedVertex> _HardwareProcessedVerticesTest;

    D3D9Base::LPDIRECT3DVERTEXDECLARATION9 _ProcessedVerticesDeclaration;

#ifdef USE_WMF
    VideoCompressor _Compressor;
#endif
	Bitmap *_BmpCapturePrev, *_BmpCaptureCurrent;
    UINT _FrameRenderIndex;
    //FrameCaptureFile _CaptureFile;
};