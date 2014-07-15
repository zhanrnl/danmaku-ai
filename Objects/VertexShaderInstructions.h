__forceinline void INST_VS_RCP(Vec4f &Result, const Vec4f &Source)
{
    if(Source.x == 0.0f)
    {
        Result.w = FLT_MAX;
    }
    else
    {
        Result.w = 1.0f / Source.x;
    }
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_RSQ(Vec4f &Result, const Vec4f &Source)
{
    if(Source.x == 0.0f)
    {
        Result.w = FLT_MAX;
    }
    else
    {
        Result.w = 1.0f / sqrtf(fabsf(Source.x));
    }
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_EXP(Vec4f &Result, const Vec4f &Source)
{
    Result.w = powf(2.0f, Source.x);
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_LOG(Vec4f &Result, const Vec4f &Source)
{
    if(Source.x == 0.0f)
    {
        Result.w = FLT_MAX;
    }
    else
    {
        Result.w = logf(fabsf(Source.x)) / logf(2.0f);
    }
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_FRC(Vec4f &Result, const Vec4f &Source)
{
    Result.x = Source.x - floorf(Source.x);
    Result.y = Source.y - floorf(Source.y);
    Result.z = Source.z - floorf(Source.z);
    Result.w = Source.w - floorf(Source.w);
}

__forceinline void INST_VS_ABS(Vec4f &Result, const Vec4f &Source)
{
    Result.x = abs(Source.x);
    Result.y = abs(Source.y);
    Result.z = abs(Source.z);
    Result.w = abs(Source.w);
}

__forceinline void INST_VS_NRM(Vec4f &Result, const Vec4f &Source)
{
    float f = Source.x * Source.x + Source.y * Source.y + Source.z * Source.z;
    if (f != 0.0f)
    {
        f = 1.0f / sqrtf(f);
    }
    else
    {
        f = FLT_MAX;
    }
    Result.x = Source.x * f;
    Result.y = Source.y * f;
    Result.z = Source.z * f;
    Result.w = Source.w * f;
}

__forceinline void INST_VS_MUL(Vec4f &Result, const Vec4f Sources[2])
{
    Result = Vec4f(Sources[0].x * Sources[1].x,
                  Sources[0].y * Sources[1].y, 
                  Sources[0].z * Sources[1].z, 
                  Sources[0].w * Sources[1].w);
}

__forceinline void INST_VS_DP3(Vec4f &Result, const Vec4f Sources[2])
{
    Result.w = Sources[0].x * Sources[1].x + 
               Sources[0].y * Sources[1].y + 
               Sources[0].z * Sources[1].z;
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_DP4(Vec4f &Result, const Vec4f Sources[2])
{
    Result.w = Sources[0].x * Sources[1].x + 
               Sources[0].y * Sources[1].y + 
               Sources[0].z * Sources[1].z +
               Sources[0].w * Sources[1].w;
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_SLT(Vec4f &Result, const Vec4f Sources[2])
{
    Result.x = (Sources[0].x < Sources[1].x) ? 1.0f : 0.0f;
    Result.y = (Sources[0].y < Sources[1].y) ? 1.0f : 0.0f;
    Result.z = (Sources[0].z < Sources[1].z) ? 1.0f : 0.0f;
    Result.w = (Sources[0].w < Sources[1].w) ? 1.0f : 0.0f;
}

__forceinline void INST_VS_SGE(Vec4f &Result, const Vec4f Sources[2])
{
    Result.x = (Sources[0].x >= Sources[1].x) ? 1.0f : 0.0f;
    Result.y = (Sources[0].y >= Sources[1].y) ? 1.0f : 0.0f;
    Result.z = (Sources[0].z >= Sources[1].z) ? 1.0f : 0.0f;
    Result.w = (Sources[0].w >= Sources[1].w) ? 1.0f : 0.0f;
}

__forceinline void INST_VS_DST(Vec4f &Result, const Vec4f Sources[2])
{
    Result.x = 1;
    Result.y = Sources[0].y * Sources[1].y;
    Result.z = Sources[0].z;
    Result.w = Sources[1].w;
}

__forceinline void INST_VS_POW(Vec4f &Result, const Vec4f Sources[2])
{
    Result.w = powf(fabsf(Sources[0].x), Sources[1].x);
    Result.x = Result.y = Result.z = Result.w;
}

__forceinline void INST_VS_CRS(Vec4f &Result, const Vec4f Sources[2])
{
    Result.x = Sources[0].y * Sources[1].z - Sources[0].z * Sources[1].y;
    Result.y = Sources[0].z * Sources[1].x - Sources[0].x * Sources[1].z;
    Result.z = Sources[0].x * Sources[1].y - Sources[0].y * Sources[1].x;
}

__forceinline void INST_VS_MAD(Vec4f &Result, const Vec4f Sources[3])
{
    Result.x = Sources[0].x * Sources[1].x + Sources[2].x;
    Result.y = Sources[0].y * Sources[1].y + Sources[2].y;
    Result.z = Sources[0].z * Sources[1].z + Sources[2].z;
    Result.w = Sources[0].w * Sources[1].w + Sources[2].w;
}

__forceinline void INST_VS_LRP(Vec4f &Result, const Vec4f Sources[3])
{
    Result.x = Sources[0].x * (Sources[1].x - Sources[2].x) + Sources[2].x;
    Result.y = Sources[0].y * (Sources[1].y - Sources[2].y) + Sources[2].y;
    Result.z = Sources[0].z * (Sources[1].z - Sources[2].z) + Sources[2].z;
    Result.w = Sources[0].w * (Sources[1].w - Sources[2].w) + Sources[2].w;
}

__forceinline void INST_VS_SGN(Vec4f &Result, const Vec4f Sources[3])
{
    for(UINT i = 0; i < 4; i++)
    {
        if(Sources[0].Element(i) < 0.0f)
        {
            Result.Element(i) = -1.0f;
        }
        else if(Sources[0].Element(i) > 0.0f)
        {
            Result.Element(i) = 1.0f;
        }
        else
        {
            Result.Element(i) = 0.0f;
        }
    }
}

__forceinline void INST_VS_SINCOS(Vec4f &Result, const Vec4f Sources[3])
{
    Result.x = cosf(Sources[0].x);
    Result.y = sinf(Sources[0].x);
}
