#define Vec4Swizzle(Target, Source, XTerm, YTerm, ZTerm, WTerm) Target.x = Source.XTerm; Target.y = Source.YTerm; Target.z = Source.ZTerm; Target.w = Source.WTerm;

struct InstructionToken
{
    InstructionToken() {}
    InstructionToken(DWORD _W)
    {
        W = _W;
    }
    __forceinline D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode() const
    {
        //MaskDebug << "OpCode: " << GetInteger(W, 0, 15) << ' ' << W << endl;
        return D3DSHADER_INSTRUCTION_OPCODE_TYPE(GetInteger(W, 0, 15));
    }
    __forceinline UINT Size() const
    {
        return GetInteger(W, 24, 27);
    }
    __forceinline bool Predicated() const
    {
        return (GetBit(W, 28) == 1);
    }
    String ToString();
    
    DWORD W;
};

struct SourceParameterToken
{
    SourceParameterToken() {}
    SourceParameterToken(DWORD _W)
    {
        W = _W;
    }
    __forceinline UINT RegisterOffset()
    {
        return GetInteger(W, 0, 10);
    }
    __forceinline D3DSHADER_PARAM_REGISTER_TYPE RegisterType()
    {
        UINT Value = (GetBit(W, 28) << 0) + 
                     (GetBit(W, 29) << 1) + 
                     (GetBit(W, 30) << 2) + 
                     (GetBit(W, 11) << 3) + 
                     (GetBit(W, 12) << 4);
        Assert(Value <= D3DSPR_PREDICATE, "Invalid register type");
        return D3DSHADER_PARAM_REGISTER_TYPE(Value);
    }
    __forceinline D3DSHADER_PARAM_SRCMOD_TYPE SourceModifier()
    {
        return D3DSHADER_PARAM_SRCMOD_TYPE(W & D3DSP_SRCMOD_MASK);
    }
    __forceinline D3DSHADER_ADDRESSMODE_TYPE AddressMode()
    {
        return D3DSHADER_ADDRESSMODE_TYPE(W & D3DSHADER_ADDRESSMODE_MASK);
    }
    __forceinline D3DSHADER_SWIZZLE_CHANNEL SwizzleChannels(UINT ChannelIndex)
    {
        Assert(ChannelIndex < 4, "Invalid channel index");
        return D3DSHADER_SWIZZLE_CHANNEL((GetBit(W, 16 + ChannelIndex * 2 + 0) << 0) + 
                                         (GetBit(W, 16 + ChannelIndex * 2 + 1) << 1));
    }
    __forceinline bool IdentitySwizzle()
    {
        return (SwizzleChannels(0) == D3DSWC_X &&
                SwizzleChannels(1) == D3DSWC_Y &&
                SwizzleChannels(2) == D3DSWC_Z &&
                SwizzleChannels(3) == D3DSWC_W);
    }

    String ToString(SourceParameterToken RelativeToken);
    String ToString()
    {
        return ToString(0);
    }

    DWORD W;
};

struct DestinationParameterToken
{
    DestinationParameterToken() {}
    DestinationParameterToken(DWORD _W)
    {
        W = _W;
    }
    __forceinline UINT RegisterOffset()
    {
        return GetInteger(W, 0, 10);
    }
    __forceinline D3DSHADER_PARAM_REGISTER_TYPE RegisterType()
    {
        UINT Value = (GetBit(W, 28) << 0) + 
                     (GetBit(W, 29) << 1) + 
                     (GetBit(W, 30) << 2) + 
                     (GetBit(W, 11) << 3) + 
                     (GetBit(W, 12) << 4);
        Assert(Value <= D3DSPR_PREDICATE, "Invalid register type");
        return D3DSHADER_PARAM_REGISTER_TYPE(Value);
    }
    __forceinline UINT WriteMask(UINT ComponentIndex)
    {
        return GetBit(W, 16 + ComponentIndex);
    }
    __forceinline D3DSHADER_ADDRESSMODE_TYPE AddressMode()
    {
        return D3DSHADER_ADDRESSMODE_TYPE(W & D3DSHADER_ADDRESSMODE_MASK);
    }
    String ToString();

    DWORD W;
};

const UINT MaxParameterTokens = 8;
struct ShaderInstruction
{
    String ToString();
    __forceinline DestinationParameterToken GetDestinationParameterToken()
    {
        return DestinationParameterToken(_ParamaterTokens[0]);
    }
    __forceinline DestinationParameterToken GetRelativeDestinationParameterToken()
    {
        return DestinationParameterToken(_ParamaterTokens[1]);
    }
    SourceParameterToken GetSourceParameterToken(UINT Index)
    {
        UINT ActualTokensProcessed = 0;
        for(UINT i = 0; i < MaxParameterTokens; i++)
        {
            if(ActualTokensProcessed == Index + 1)
            {
                return SourceParameterToken(_ParamaterTokens[i]);
            }
            if(SourceParameterToken(_ParamaterTokens[i]).AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
            {
                i++;
            }
            ActualTokensProcessed++;
        }
        return SourceParameterToken(0);
    }
    SourceParameterToken GetRelativeSourceParameterToken(UINT Index)
    {
        UINT ActualTokensProcessed = 0;
        for(UINT i = 0; i < MaxParameterTokens; i++)
        {
            if(ActualTokensProcessed == Index + 1)
            {
                return SourceParameterToken(_ParamaterTokens[i + 1]);
            }
            if(SourceParameterToken(_ParamaterTokens[i]).AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
            {
                i++;
            }
            ActualTokensProcessed++;
        }
        return SourceParameterToken(0);
    }

    InstructionToken _InstructionToken;
    DWORD _ParamaterTokens[MaxParameterTokens];
};

struct CompleteShaderInstruction
{
    String ToString();

    String Comment;
    ShaderInstruction _Instruction;
};