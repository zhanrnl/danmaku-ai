#include "Main.h"

#ifdef USE_COMPILED_SHADERS
#endif

#define AddVShaderHash(N) Assert(_HashedVShaders.find(N) == _HashedVShaders.end(), "Hash collision");                                                                                                    \
        _HashedVShaders[N] = VShaderSimulatorHashInfo(WrapperToVShaderPrepareHash##N, WrapperToVShaderRestoreHash##N, WrapperToVShaderCompiledHash##N);

void VShaderSimulator::InitalizeHashes()
{
    if(_HashedVShaders.size() == 0)
    {
        _HashedVShaders.clear();

#ifdef USE_COMPILED_SHADERS
#endif

    }
}

void VShaderSimulator::OutputNewShaderList()
{
    Directory Dir(g_Context->Parameters.ShaderCaptureDirectory);
    Vector<UINT> Hashes;
    for(UINT FileIndex = 0; FileIndex < Dir.Files().Length(); FileIndex++)
    {
        const String &CurFile = Dir.Files()[FileIndex];
        if(CurFile.StartsWith("VShader") && CurFile.EndsWith(".txt"))
        {
            String HashNumber = CurFile;
            for(UINT Index = 0; Index < 7; Index++)
            {
                HashNumber.PopFront();
            }
            for(UINT Index = 0; Index < 4; Index++)
            {
                HashNumber.PopEnd();
            }
            UINT CurHash = HashNumber.ConvertToUnsignedInteger();
            Hashes.PushEnd(CurHash);
        }
    }

    ofstream File((g_Context->Parameters.OutputFileDirectory + String("NewShaderList.txt")).CString());

    if(Hashes.Length() > 0)
    {
        for(UINT Index = 0; Index < Hashes.Length(); Index++)
        {
            File << "#include \"" << g_Context->Parameters.FinalShaderCodeDirectory << "VShader" << String::ZeroPad(String(Hashes[Index]), 10) << ".cpp\"\n";
        }
        File << endl;

        for(UINT Index = 0; Index < Hashes.Length(); Index++)
        {
            File << "#include \"" << g_Context->Parameters.FinalShaderCodeDirectory << "VShader" << String::ZeroPad(String(Hashes[Index]), 10) << ".h\"\n";
        }
        File << endl;

        for(UINT Index = 0; Index < Hashes.Length(); Index++)
        {
            File << "AddVShaderHash(" << Hashes[Index] << ");\n";
        }
        File << endl;
    }
}

bool VShaderSimulator::ContainsHash(DWORD Hash)
{
    return (_HashedVShaders.find(Hash) != _HashedVShaders.end());
}

VShaderSimulatorHashInfo VShaderSimulator::GetHashInfoFromHash(DWORD Hash)
{
    Assert(ContainsHash(Hash), "Hash not found in VShaderSimulator::GetHashInfoFromHash");
    return (_HashedVShaders.find(Hash)->second);
}

Vec4f& VShaderSimulator::GetRegister(D3DSHADER_PARAM_REGISTER_TYPE Type, UINT Offset)
{
    switch(Type)
    {
    case D3DSPR_TEMP:
        return TempRegisters[Offset];
    case D3DSPR_INPUT:
        return InputRegisters[Offset];
    case D3DSPR_ADDR:
        return AddressRegisters[Offset];
    case D3DSPR_CONST:
        return ConstFloatRegisters[Offset];
    case D3DSPR_RASTOUT:
        return OutputRasterRegisters[Offset];
    case D3DSPR_ATTROUT:
        return OutputAttributeRegisters[Offset];
    case D3DSPR_TEXCRDOUT:
        return OutputTextureRegisters[Offset];
    default:
        SignalError("Invalid register");
        return TempRegisters[0];
    }
}

__forceinline void ResolveSourceModifier(Vec4f &Result, SourceParameterToken Token)
{
    if(Token.SourceModifier() == D3DSPSM_NEG)
    {
        Result = -Result;
    }
    else if(Token.SourceModifier() == D3DSPSM_ABS)
    {
        Result = Vec4f(Math::Abs(Result.x), Math::Abs(Result.y), Math::Abs(Result.z), Math::Abs(Result.w));
    }
    else if(Token.SourceModifier() == D3DSPSM_ABSNEG)
    {
        Result = Vec4f(-Math::Abs(Result.x), -Math::Abs(Result.y), -Math::Abs(Result.z), -Math::Abs(Result.w));
    }
}

__forceinline void ResolveSwizzle(Vec4f &Result, const Vec4f &Input, SourceParameterToken Token)
{
    for(UINT i = 0; i < 4; i++)
    {
        Result.Element(i) = Input.Element(Token.SwizzleChannels(i));
    }
    ResolveSourceModifier(Result, Token);
}

__forceinline void ResolveRelativeSwizzle(Vec4f &Result, const Vec4f Inputs[4], SourceParameterToken Token)
{
    for(UINT i = 0; i < 4; i++)
    {
        Result.Element(i) = Inputs[i].Element(Token.SwizzleChannels(i));
    }
    ResolveSourceModifier(Result, Token);
}

Vec4f VShaderSimulator::ResolveSourceToken(SourceParameterToken SourceToken, SourceParameterToken RelativeToken)
{
    Vec4f Result;
    int FixedOffset = int(SourceToken.RegisterOffset());
    if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_ABSOLUTE)
    {
        ResolveSwizzle(Result, GetRegister(SourceToken.RegisterType(), FixedOffset), SourceToken);
    }
    else
    {
        Vec4f Input = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
        int FinalOffset = FixedOffset + Math::Round(AddressRegisters[0].Element(UINT(RelativeToken.SwizzleChannels(0))));
        if(between(FinalOffset, 0, VShaderConst::ConstFloatRegisterCount - 1))
        {
            Input = GetRegister(SourceToken.RegisterType(), FinalOffset);
        }
        ResolveSwizzle(Result, Input, SourceToken);
        /*Vec4f Inputs[4];
        for(UINT i = 0; i < 4; i++)
        {
            int FinalOffset = FixedOffset + Round(AddressRegisters[0].Element(i));
            if(between(FinalOffset, 0, ConstFloatRegisterCount - 1))
            {
                Inputs[i] = GetRegister(SourceToken.RegisterType(), FinalOffset);
            }
            else
            {
                Inputs[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
        ResolveRelativeSwizzle(Result, Inputs, SourceToken);*/
    }
    return Result;
}

void VShaderSimulator::ApplyInstruction(ShaderInstruction &I)
{
    D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode = I._InstructionToken.OpCode();
    UINT Size = GetVSInstructionSize(OpCode);
    //String ErrorString = String("Unsupported instruction: ") + GetInstructionOpCodeString(I._InstructionToken.OpCode()) +
    //                     String(", Size ") + String(I._InstructionToken.Size());
    String ErrorString;
    DestinationParameterToken Destination = I.GetDestinationParameterToken();
    if(Size == 0)
    {
        switch(OpCode)
        {
        case D3DSIO_NOP:
            break;
        case D3DSIO_BREAK:
        case D3DSIO_ENDIF:
        case D3DSIO_ELSE:
        case D3DSIO_ENDREP:
        case D3DSIO_ENDLOOP:
        case D3DSIO_RET:
        default:
            SignalError(ErrorString);
            break;
        }
    }
    else if(Size == 1)
    {
        switch(OpCode)
        {
        case D3DSIO_CALL:
        case D3DSIO_LABEL:
        case D3DSIO_REP:
        case D3DSIO_BREAKP:
        default:
            SignalError(ErrorString);
            break;
        }
    }
    else if(Size == 2)
    {
        Vec4f Result, Source;
        SourceParameterToken SourceToken = I.GetSourceParameterToken(0);
        if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_ABSOLUTE)
        {
            Source = ResolveSourceToken(SourceToken, 0);
        }
        else
        {
            Source = ResolveSourceToken(SourceToken, I.GetRelativeSourceParameterToken(0));
        }
        switch(OpCode)
        {
        case D3DSIO_MOV:
        case D3DSIO_MOVA:
            Result = Source;
            break;
        case D3DSIO_RCP:
            if(Source.x == 0.0f)
            {
                Result.w = FLT_MAX;
            }
            else
            {
                Result.w = 1.0f / Source.x;
            }
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_RSQ:
            if(Source.x == 0.0f)
            {
                Result.w = FLT_MAX;
            }
            else
            {
                Result.w = 1.0f / sqrtf(fabsf(Source.x));
            }
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_EXP:
        case D3DSIO_EXPP:
            Result.w = powf(2.0f, Source.x);
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_LOG:
        case D3DSIO_LOGP:
            if(Source.x == 0.0f)
            {
                Result.w = FLT_MAX;
            }
            else
            {
                Result.w = logf(fabsf(Source.x)) / logf(2.0f);
            }
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_FRC:
            Result.x = Source.x - floorf(Source.x);
            Result.y = Source.y - floorf(Source.y);
            Result.z = Source.z - floorf(Source.z);
            Result.w = Source.w - floorf(Source.w);
            break;
        case D3DSIO_ABS:
            Result.x = abs(Source.x);
            Result.y = abs(Source.y);
            Result.z = abs(Source.z);
            Result.w = abs(Source.w);
            break;
        case D3DSIO_NRM:
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
            break;
        case D3DSIO_CALLNZ:
        case D3DSIO_LOOP:
        case D3DSIO_LIT:
        case D3DSIO_BREAKC:
        case D3DSIO_DSX:
        case D3DSIO_DSY:
        default:
            SignalError(ErrorString);
        }
        Vec4f &Target = GetRegister(Destination.RegisterType(), Destination.RegisterOffset());
        for(UINT i = 0; i < 4; i++)
        {
            if(Destination.WriteMask(i) == 1)
            {
                Target.Element(i) = Result.Element(i);
            }
        }
    }
    else if(Size == 3)
    {
        Vec4f Result, Sources[2];
        for(UINT i = 0; i < 2; i++)
        {
            SourceParameterToken SourceToken = I.GetSourceParameterToken(i);
            if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_ABSOLUTE)
            {
                Sources[i] = ResolveSourceToken(SourceToken, 0);
            }
            else
            {
                Sources[i] = ResolveSourceToken(SourceToken, I.GetRelativeSourceParameterToken(i));
            }
        }
        switch(OpCode)
        {
        case D3DSIO_ADD:
            Result = Sources[0] + Sources[1];
            break;
        case D3DSIO_SUB:
            Result = Sources[0] - Sources[1];
            break;
        case D3DSIO_MUL:
            Result = Vec4f(Sources[0].x * Sources[1].x,
                          Sources[0].y * Sources[1].y, 
                          Sources[0].z * Sources[1].z, 
                          Sources[0].w * Sources[1].w);
            break;
        case D3DSIO_DP3:
            Result.w = Sources[0].x * Sources[1].x + 
                       Sources[0].y * Sources[1].y + 
                       Sources[0].z * Sources[1].z;
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_DP4:
            Result.w = Sources[0].x * Sources[1].x + 
                       Sources[0].y * Sources[1].y + 
                       Sources[0].z * Sources[1].z +
                       Sources[0].w * Sources[1].w;
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_MIN:
            Result = Vec4f::Minimize(Sources[0], Sources[1]);
            break;
        case D3DSIO_MAX:
            Result = Vec4f::Maximize(Sources[0], Sources[1]);
            break;
        case D3DSIO_SLT:
            Result.x = (Sources[0].x < Sources[1].x) ? 1.0f : 0.0f;
            Result.y = (Sources[0].y < Sources[1].y) ? 1.0f : 0.0f;
            Result.z = (Sources[0].z < Sources[1].z) ? 1.0f : 0.0f;
            Result.w = (Sources[0].w < Sources[1].w) ? 1.0f : 0.0f;
            break;
        case D3DSIO_SGE:
            Result.x = (Sources[0].x >= Sources[1].x) ? 1.0f : 0.0f;
            Result.y = (Sources[0].y >= Sources[1].y) ? 1.0f : 0.0f;
            Result.z = (Sources[0].z >= Sources[1].z) ? 1.0f : 0.0f;
            Result.w = (Sources[0].w >= Sources[1].w) ? 1.0f : 0.0f;
            break;
        case D3DSIO_DST:
            Result.x = 1;
            Result.y = Sources[0].y * Sources[1].y;
            Result.z = Sources[0].z;
            Result.w = Sources[1].w;
            break;
        case D3DSIO_POW:
            Result.w = powf(fabsf(Sources[0].x), Sources[1].x);
            Result.x = Result.y = Result.z = Result.w;
            break;
        case D3DSIO_CRS:
            Result.x = Sources[0].y * Sources[1].z - Sources[0].z * Sources[1].y;
            Result.y = Sources[0].z * Sources[1].x - Sources[0].x * Sources[1].z;
            Result.z = Sources[0].x * Sources[1].y - Sources[0].y * Sources[1].x;
            break;
        default:
            SignalError(ErrorString);
        }
        Vec4f &Target = GetRegister(Destination.RegisterType(), Destination.RegisterOffset());
        for(UINT i = 0; i < 4; i++)
        {
            if(Destination.WriteMask(i) == 1)
            {
                Target.Element(i) = Result.Element(i);
            }
        }
    }
    else if(Size == 4)
    {
        Vec4f Result, Sources[3];
        for(UINT i = 0; i < 3; i++)
        {
            SourceParameterToken SourceToken = I.GetSourceParameterToken(i);
            if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_ABSOLUTE)
            {
                Sources[i] = ResolveSourceToken(SourceToken, 0);
            }
            else
            {
                Sources[i] = ResolveSourceToken(SourceToken, I.GetRelativeSourceParameterToken(i));
            }
        }
        switch(OpCode)
        {
        case D3DSIO_MAD:
            Result.x = Sources[0].x * Sources[1].x + Sources[2].x;
            Result.y = Sources[0].y * Sources[1].y + Sources[2].y;
            Result.z = Sources[0].z * Sources[1].z + Sources[2].z;
            Result.w = Sources[0].w * Sources[1].w + Sources[2].w;
            break;
        case D3DSIO_LRP:
            Result.x = Sources[0].x * (Sources[1].x - Sources[2].x) + Sources[2].x;
            Result.y = Sources[0].y * (Sources[1].y - Sources[2].y) + Sources[2].y;
            Result.z = Sources[0].z * (Sources[1].z - Sources[2].z) + Sources[2].z;
            Result.w = Sources[0].w * (Sources[1].w - Sources[2].w) + Sources[2].w;
            break;
        case D3DSIO_SGN:
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
            break;
        case D3DSIO_SINCOS:
            Result.x = cosf(Sources[0].x);
            Result.y = sinf(Sources[0].x);
            break;
        default:
            SignalError(ErrorString);
        }
        Vec4f &Target = GetRegister(Destination.RegisterType(), Destination.RegisterOffset());
        for(UINT i = 0; i < 4; i++)
        {
            if(Destination.WriteMask(i) == 1)
            {
                Target.Element(i) = Result.Element(i);
            }
        }
    }
    else
    {
        SignalError("Invalid instruction size");
    }
}

void VShaderSimulator::OutputRegisters(ofstream &File)
{
    for(UINT i = 0; i < VShaderConst::RegisterCanidateCount; i++)
    {
        Vector<UINT> &RegisterUsedList = RegistersUsed[i];
        for(UINT i2 = 0; i2 < RegisterUsedList.Length(); i2++)
        {
            if(RegisterUsedList[i2])
            {
                Vec4f RegisterValue = GetRegister(VShaderConst::RegisterCanidates[i], i2);
                if(VShaderConst::RegisterCanidates[i] == D3DSPR_ADDR)
                {
                    File << Math::Round(RegisterValue.x) << ", " <<
                            Math::Round(RegisterValue.y) << ", " <<
                            Math::Round(RegisterValue.z) << ", " <<
                            Math::Round(RegisterValue.w) << '\t';
                }
                else
                {
                    File << RegisterValue.x << ", " <<
                            RegisterValue.y << ", " <<
                            RegisterValue.z << ", " <<
                            RegisterValue.w << '\t';
                }
            }
        }
    }
}

void VShaderSimulator::LoadConstantsFromDevice()
{
    memcpy(ConstFloatRegisters, g_Context->Managers.State.VShaderFloatConstants, sizeof(Vec4f) * VShaderConst::ConstFloatRegisterCount);
    memcpy(ConstIntRegisters, g_Context->Managers.State.VShaderIntConstants, sizeof(Int4) * VShaderConst::ConstIntRegisterCount);
    memcpy(ConstBoolRegisters, g_Context->Managers.State.VShaderBoolConstants, sizeof(bool) * VShaderConst::ConstBoolRegisterCount);
}

void VShaderSimulator::ReportUnhashedRender()
{
    if(!_UnhashedRenderReported)
    {
        _UnhashedRenderReported = true;
        g_Context->Files.Assert << "Unhashed prepare\n";
    }
}

void VShaderSimulator::Prepare(VShader &VS)
{
    if(VS.HashInfo().Valid())
    {
        VS.HashInfo().Prepare(this);
    }
    else
    {
        ReportUnhashedRender();
        for(UINT i = 0; i < VS.Definitions.Length(); i++)
        {
            ShaderInstruction &CurInstruction = VS.Definitions[i];
            if(CurInstruction.GetDestinationParameterToken().RegisterType() == D3DSPR_CONST)
            {
                Vec4f Value;
                for(UINT i = 0; i < 4; i++)
                {
                    float *FloatCast = (float *)(&CurInstruction._ParamaterTokens[i + 1]);
                    Value.Element(i) = *FloatCast;
                }
                ConstFloatRegisters[CurInstruction.GetDestinationParameterToken().RegisterOffset()] = Value;
            }
        }
    }
}

void VShaderSimulator::Restore(VShader &VS)
{
    if(VS.HashInfo().Valid())
    {
        VS.HashInfo().Restore(this);
    }
    else
    {
        if(!_UnhashedRenderReported)
        {
            _UnhashedRenderReported = true;
            g_Context->Files.Assert << "Unhashed restore\n";
        }
        for(UINT i = 0; i < VS.Definitions.Length(); i++)
        {
            ShaderInstruction &CurInstruction = VS.Definitions[i];
            UINT Offset = CurInstruction.GetDestinationParameterToken().RegisterOffset();
            ConstFloatRegisters[Offset] = g_Context->Managers.State.VShaderFloatConstants[Offset];
        }
    }
}

void VShaderSimulator::LoadRegistersFromStream(VShader &VS, VertexData &VData)
{
    for(UINT i = 0; i < VS.Declarations.Length(); i++)
    {
        ShaderInstruction &CurDeclaration = VS.Declarations[i];
        DestinationParameterToken Destination(CurDeclaration._ParamaterTokens[1]);
        D3DDECLUSAGE Usage = D3DDECLUSAGE(GetInteger(CurDeclaration._ParamaterTokens[0], 0, 4));
        UINT UsageIndex = GetInteger(CurDeclaration._ParamaterTokens[0], 16, 19);
        Vec4f StreamData = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
        switch(Usage)
        {
        case D3DDECLUSAGE_POSITION:
            StreamData = VData.Position;
            break;
        case D3DDECLUSAGE_NORMAL:
            StreamData = VData.Normal;
            break;
        case D3DDECLUSAGE_BLENDWEIGHT:
            StreamData = VData.BlendWeight;
            break;
        case D3DDECLUSAGE_BLENDINDICES:
            StreamData = VData.BlendIndices;
            break;
        case D3DDECLUSAGE_TEXCOORD:
            Assert(UsageIndex < MaxTexCoords, "Texture coordinate index out of range");
            StreamData = VData.TexCoord[UsageIndex];
            break;
        case D3DDECLUSAGE_COLOR:
            Assert(UsageIndex < MaxColorCoords, "Color coordinate index out of range");
            StreamData = VData.Color[UsageIndex];
            break;
        case D3DDECLUSAGE_FOG:
            //fog is probably not important
            break;
        case D3DDECLUSAGE_BINORMAL:
            StreamData = VData.BiNormal;
            break;
        case D3DDECLUSAGE_TANGENT:
            StreamData = VData.Tangent;
            break;
        default:
            g_Context->Files.Assert << "Invalid vertex declaration in vertex shader: " << UINT(Usage) << endl;
            SignalError("Invalid vertex declaration in vertex shader");
        }
        GetRegister(Destination.RegisterType(), Destination.RegisterOffset()) = StreamData;
    }
}

void VShaderSimulator::Simulate(VShader &VS, VertexData &VData, ofstream &SimulationFile, bool UseFile)
{
    bool Found = false;
    if(!UseFile)
    {
        // report declaration creation

        if(VS.HashInfo().Valid())
        {
            Found = true;
            VS.HashInfo().Compiled(this, VData);
        }
        else
        {
            ReportUnhashedRender();
        }
    }
    if(!Found)
    {
        LoadRegistersFromStream(VS, VData);
        if(UseFile)
        {
            for(UINT i = 0; i < VShaderConst::RegisterCanidateCount; i++)
            {
                Vector<UINT> &RegisterUsedList = RegistersUsed[i];
                RegisterUsedList.Allocate(VShaderConst::ConstFloatRegisterCount);
                for(UINT i2 = 0; i2 < RegisterUsedList.Length(); i2++)
                {
                    RegisterUsedList[i2] = 0;
                }
            }
            for(UINT i = 0; i < VS.Instructions.Length(); i++)
            {
                ShaderInstruction &CurInstruction = VS.Instructions[i];
                UINT Size = GetVSInstructionSize(CurInstruction._InstructionToken.OpCode());
                for(UINT i2 = 0; i2 < Size; i2++)
                {
                    D3DSHADER_PARAM_REGISTER_TYPE RegisterType;
                    UINT RegisterOffset;
                    if(i2 == 0)
                    {
                        RegisterType = CurInstruction.GetDestinationParameterToken().RegisterType();
                        RegisterOffset = CurInstruction.GetDestinationParameterToken().RegisterOffset();
                    }
                    else
                    {
                        RegisterType = CurInstruction.GetSourceParameterToken(i2 - 1).RegisterType();
                        RegisterOffset = CurInstruction.GetSourceParameterToken(i2 - 1).RegisterOffset();
                    }
                    for(UINT i3 = 0; i3 < VShaderConst::RegisterCanidateCount; i3++)
                    {
                        if(RegisterType == VShaderConst::RegisterCanidates[i3])
                        {
                            RegistersUsed[i3][RegisterOffset] = true;
                        }
                    }
                }
            }
            SimulationFile << "Hash " << VS.Hash() << endl;
            SimulationFile << "Instruction\t";

            for(UINT i = 0; i < VShaderConst::RegisterCanidateCount; i++)
            {
                Vector<UINT> &RegisterUsedList = RegistersUsed[i];
                for(UINT i2 = 0; i2 < RegisterUsedList.Length(); i2++)
                {
                    if(RegisterUsedList[i2])
                    {
                        SimulationFile << GetRegisterTypeString(VShaderConst::RegisterCanidates[i], i2) << '\t';
                    }
                }
            }
            SimulationFile << endl;

            SimulationFile << '\t';
            OutputRegisters(SimulationFile);
            SimulationFile << endl;
        }

        UINT InstructionIndex = 0;
        bool Done = false;
        while(!Done)
        {
            if(InstructionIndex == VS.Instructions.Length())
            {
                Done = true;
            }
            else
            {
                if(UseFile)
                {
                    SimulationFile << VS.Instructions[InstructionIndex].ToString() << '\t';
                }
                ApplyInstruction(VS.Instructions[InstructionIndex++]);
                if(UseFile)
                {
                    OutputRegisters(SimulationFile);
                    SimulationFile << endl;
                }
            }
        }
    }
}

void VShaderSimulator::ResetRegisters()
{
    for(UINT i = 0; i < VShaderConst::TempRegisterCount; i++)
    {
        TempRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < VShaderConst::InputRegisterCount; i++)
    {
        InputRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
    }
    for(UINT i = 0; i < VShaderConst::AddressRegisterCount; i++)
    {
        AddressRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < VShaderConst::OutputTextureRegisterCount; i++)
    {
        OutputTextureRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < VShaderConst::OutputRasterRegisterCount; i++)
    {
        OutputRasterRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
    for(UINT i = 0; i < VShaderConst::OutputAttributeRegisterCount; i++)
    {
        OutputAttributeRegisters[i] = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
    }
}