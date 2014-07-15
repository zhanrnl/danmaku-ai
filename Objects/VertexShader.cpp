#include "Main.h"

PShader::PShader(const DWORD *pFunction, HANDLE Handle)
{
    _Handle = Handle;
    VShader TempShader(pFunction, Handle);
    _ComputedHash = TempShader.Hash();
}

void VShader::RecomputeHash()
{
    //We cannot use CompleteInstructions.Hash32() because it contains a String
    _ComputedHash = CompleteInstructions.Length() * 7856;
    for(UINT i = 0; i < CompleteInstructions.Length(); i++)
    {
        if(CompleteInstructions[i]._Instruction._InstructionToken.OpCode() != D3DSIO_COMMENT)
        {
            _ComputedHash += i * CompleteInstructions[i]._Instruction._InstructionToken.W * 67;
            for(UINT i2 = 0; i2 < MaxParameterTokens; i2++)
            {
                _ComputedHash += CompleteInstructions[i]._Instruction._ParamaterTokens[i2] * i * i2 * 963;
            }
        }
    }
    _ComputedHash = _ComputedHash & 0x0FFFFFFF;
    if(g_Context->Managers.State.VertexShaderSimulator.ContainsHash(_ComputedHash))
    {
        _HashInfo = g_Context->Managers.State.VertexShaderSimulator.GetHashInfoFromHash(_ComputedHash);
    }
}

VShader::VShader(const DWORD *pFunction, HANDLE Handle)
{
    _Handle = Handle;

    Assert(pFunction != NULL, "Invalid function tokens");
    UINT TokenIndex = 0;
    DWORD VersionToken = pFunction[TokenIndex++];
    _MinorVersion = D3DSHADER_VERSION_MINOR(VersionToken);
    _MajorVersion = D3DSHADER_VERSION_MAJOR(VersionToken);
    bool Done = false;

    if(_MajorVersion == 1)
    {
        g_Context->Files.Assert << "Shader version 1 not supported\n";
        Done = true;
    }

    while(!Done)
    {
        DWORD CurrentToken = pFunction[TokenIndex++];
        if(CurrentToken == D3DVS_END())
        {
            Done = true;
        }
        else if((CurrentToken & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
        {
            CompleteInstructions.PushEnd();
            CompleteShaderInstruction &CurInstruction = CompleteInstructions.Last();
            CurInstruction._Instruction._InstructionToken = InstructionToken(CurrentToken);
            UINT CommentLength = (CurrentToken & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
            for(UINT i = 0; i < CommentLength; i++)
            {
                DWORD CurrentComment = pFunction[TokenIndex++];
                UCHAR *CommentStream = (UCHAR *)&CurrentComment;
                for(UINT i2 = 0; i2 < 4; i2++)
                {
                    CurInstruction.Comment.PushEnd(CommentStream[i2]);
                }
            }
        }
        else
        {
            CompleteInstructions.PushEnd();
            CompleteShaderInstruction &CurInstruction = CompleteInstructions.Last();
            CurInstruction._Instruction._InstructionToken = InstructionToken(CurrentToken);
            UINT InstructionLength = CurInstruction._Instruction._InstructionToken.Size();
            Assert(InstructionLength <= MaxParameterTokens, "Maximum instruction length exceeded.");
            for(UINT i = 0; i < MaxParameterTokens; i++)
            {
                if(i < InstructionLength)
                {
                    CurInstruction._Instruction._ParamaterTokens[i] = pFunction[TokenIndex++];
                }
                else
                {
                    CurInstruction._Instruction._ParamaterTokens[i] = 0;
                }
            }
        }
    }
    for(UINT i = 0; i < CompleteInstructions.Length(); i++)
    {
        CompleteShaderInstruction &CurCompleteInstruction = CompleteInstructions[i];
        switch(CurCompleteInstruction._Instruction._InstructionToken.OpCode())
        {
        case D3DSIO_COMMENT:
            break;
        case D3DSIO_DEF:
            Definitions.PushEnd(CurCompleteInstruction._Instruction);
            break;
        case D3DSIO_DCL:
            Declarations.PushEnd(CurCompleteInstruction._Instruction);
            break;
        default:
            Instructions.PushEnd(CurCompleteInstruction._Instruction);
            break;
        }
    }
    RecomputeHash();
}

bool VShader::DeclarationFound(D3DDECLUSAGE Usage) const
{
	for(UINT Index = 0; Index < Declarations.Length(); Index++)
	{
		const ShaderInstruction &CurDeclaration = Declarations[Index];

		D3DDECLUSAGE CurUsage = D3DDECLUSAGE(GetInteger(CurDeclaration._ParamaterTokens[0], 0, 4));
        if(Usage == CurUsage)
		{
			return true;
		}
	}
	return false;
}
void VShader::ComputeShaderInstructionCounts(Vector<UINT> &InstructionCounts) const
{
    InstructionCounts.Allocate(ShaderInstructionTypeCount);
    InstructionCounts.Clear(0);
    for(UINT InstructionIndex = 0; InstructionIndex < Instructions.Length(); InstructionIndex++)
    {
        InstructionCounts[Instructions[InstructionIndex]._InstructionToken.OpCode()]++;
    }
}

void VShader::OutputDisassembly(ofstream &File)
{
    File << "vs_" << _MajorVersion << "_" << _MinorVersion << endl;
    File << "# Shader Hash " << _ComputedHash << endl;
    for(UINT i = 0; i < CompleteInstructions.Length(); i++)
    {
        File << CompleteInstructions[i].ToString() << endl;
    }
}

void VShader::OutputCSource(ofstream &File, ofstream &HeaderFile)
{
    UINT MyHash = Hash();

    HeaderFile << "static void WrapperToVShaderPrepareHash" << MyHash << "(VShaderSimulator *ThisPtr);\n";
    HeaderFile << "static void WrapperToVShaderRestoreHash" << MyHash << "(VShaderSimulator *ThisPtr);\n";
    HeaderFile << "static void WrapperToVShaderCompiledHash" << MyHash << "(VShaderSimulator *ThisPtr, const VertexData &VData);\n";
    HeaderFile << "void VShaderPrepareHash" << MyHash << "();\n";
    HeaderFile << "void VShaderRestoreHash" << MyHash << "();\n";
    HeaderFile << "void VShaderCompiledHash" << MyHash << "(const VertexData &VData);\n";

    File << "/*\n";
    File << "HashedVShaders.PushEnd(VShaderSimulatorHashInfo("
         << MyHash << ", "
         << "WrapperToVShaderPrepareHash" << MyHash << ", "
         << "WrapperToVShaderRestoreHash" << MyHash << ", "
         << "WrapperToVShaderCompiledHash" << MyHash << ");\n";
    File << "*/\n\n";

    File << "void VShaderSimulator::WrapperToVShaderPrepareHash" << MyHash << "(VShaderSimulator *ThisPtr)\n";
    File << "{\n";
    File << "\tThisPtr->VShaderPrepareHash" << MyHash << "();\n";
    File << "}\n\n";

    File << "void VShaderSimulator::WrapperToVShaderRestoreHash" << MyHash << "(VShaderSimulator *ThisPtr)\n";
    File << "{\n";
    File << "\tThisPtr->VShaderRestoreHash" << MyHash << "();\n";
    File << "}\n\n";

    File << "void VShaderSimulator::WrapperToVShaderCompiledHash" << MyHash << "(VShaderSimulator *ThisPtr, const VertexData &VData)\n";
    File << "{\n";
    File << "\tThisPtr->VShaderCompiledHash" << MyHash << "(VData);\n";
    File << "}\n\n";

    File << "void VShaderSimulator::VShaderPrepareHash" << MyHash << "()\n";
    File << "{\n";
    File << "\t//\n";
    File << "\t// Definitions\n";
    File << "\t//\n";
    for(UINT i = 0; i < Definitions.Length(); i++)
    {
        ShaderInstruction &CurInstruction = Definitions[i];
        if(CurInstruction.GetDestinationParameterToken().RegisterType() == D3DSPR_CONST)
        {
            Vec4f Value;
            for(UINT i = 0; i < 4; i++)
            {
                float *FloatCast = (float *)(&CurInstruction._ParamaterTokens[i + 1]);
                Value.Element(i) = *FloatCast;
            }
            File << "\tConstFloatRegisters[" << CurInstruction.GetDestinationParameterToken().RegisterOffset() << "]"
                 << " = Vec4f(" << Value.x << ", " << Value.y << ", " << Value.z << ", " << Value.w << ");\n";
        }
    }
    File << "}\n\n";

    File << "void VShaderSimulator::VShaderRestoreHash" << MyHash << "()\n";
    File << "{\n";
    File << "\t//\n";
    File << "\t// Definitions\n";
    File << "\t//\n";
    for(UINT i = 0; i < Definitions.Length(); i++)
    {
        ShaderInstruction &CurInstruction = Definitions[i];
        if(CurInstruction.GetDestinationParameterToken().RegisterType() == D3DSPR_CONST)
        {
            File << "\tConstFloatRegisters[" << CurInstruction.GetDestinationParameterToken().RegisterOffset() << "]"
                 << " = g_Context->Managers.State.VShaderFloatConstants[" << CurInstruction.GetDestinationParameterToken().RegisterOffset() << "];\n";
        }
    }
    File << "}\n\n";

    File << "void VShaderSimulator::VShaderCompiledHash" << MyHash << "(const VertexData &VData)\n";
    File << "{\n";
    File << "\t//\n";
    File << "\t// Declarations\n";
    File << "\t//\n";
    for(UINT i = 0; i < Declarations.Length(); i++)
    {
        ShaderInstruction &CurDeclaration = Declarations[i];
        DestinationParameterToken Destination(CurDeclaration._ParamaterTokens[1]);
        D3DDECLUSAGE Usage = D3DDECLUSAGE(GetInteger(CurDeclaration._ParamaterTokens[0], 0, 4));
        UINT UsageIndex = GetInteger(CurDeclaration._ParamaterTokens[0], 16, 19);
        String StreamSource;
        switch(Usage)
        {
        case D3DDECLUSAGE_POSITION:
            StreamSource = "VData.Position";
            break;
        case D3DDECLUSAGE_NORMAL:
            StreamSource = "VData.Normal";
            break;
        case D3DDECLUSAGE_BLENDWEIGHT:
            StreamSource = "VData.BlendWeight";
            break;
        case D3DDECLUSAGE_BLENDINDICES:
            StreamSource = "VData.BlendIndices";
            break;
        case D3DDECLUSAGE_TEXCOORD:
            //Assert(UsageIndex < MaxTexCoords, "Texture coordinate index out of range");
            if(UsageIndex < MaxTexCoords)
            {
                UsageIndex = MaxTexCoords - 1;
            }
            StreamSource = String("VData.TexCoord[") + String(UsageIndex) + String("]");
            break;
        case D3DDECLUSAGE_COLOR:
            Assert(UsageIndex < MaxColorCoords, "Color coordinate index out of range");
            StreamSource = String("VData.Color[") + String(UsageIndex) + String("]");
            break;
        case D3DDECLUSAGE_FOG:
            //This is incorrect; however I have no real desire to support fog since it's deprecated in PS 3.0+.
            StreamSource = "VData.Position";
            break;
        case D3DDECLUSAGE_BINORMAL:
            StreamSource = "VData.BiNormal";
            break;
        case D3DDECLUSAGE_TANGENT:
            StreamSource = "VData.Tangent";
            break;
        default:
            g_Context->Files.Assert << "Invalid vertex declaration in vertex shader: " << UINT(Usage) << endl;
            SignalError("Invalid vertex declaration in vertex shader");
        }
        File << "\t" << GetSimulatorRegisterName(Destination.RegisterType()) << "[" << Destination.RegisterOffset()
             << "] = " << StreamSource << ";\n";
    }
    File << "\t\n";
    File << "\t//\n";
    File << "\t// Instructions\n";
    File << "\t//\n\n";
    for(UINT i = 0; i < Instructions.Length(); i++)
    {
        OutputCSourceInstruction(Instructions[i], File);
    }
    File << "}\n";
}

void VShader::OutputCSourceResolution(SourceParameterToken SourceToken, SourceParameterToken RelativeToken, const String &TempName, const String &TargetName, ofstream &File)
{
    String OffsetString;
    if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_ABSOLUTE)
    {
        OffsetString = String(SourceToken.RegisterOffset());
        //ResolveSwizzle(Result, GetRegister(SourceToken.RegisterType(), FixedOffset), SourceToken);
    }
    else
    {
        OffsetString = String("Utility::Bound(") +
                       String(SourceToken.RegisterOffset()) +
                       String(" + ") +
                       String("Math::Round(AddressRegisters[0].") +
                       GetSwizzleChannelString(RelativeToken.SwizzleChannels(0)) +
                       String("), 0, ") +
                       String(GetSimulatorRegisterCount(SourceToken.RegisterType()) - 1) +
                       String(")");
    }

    String Modifier;
    if(SourceToken.SourceModifier() == D3DSPSM_NEG)
    {
        Modifier = "-";
    }
    else if(SourceToken.SourceModifier() == D3DSPSM_ABS)
    {
        Modifier = "ERROR";
    }
    else if(SourceToken.SourceModifier() == D3DSPSM_ABSNEG)
    {
        Modifier = "ERROR";
    }

    if(SourceToken.IdentitySwizzle())
    {
        File << "\t\t" << TargetName << " = " << Modifier << GetSimulatorRegisterName(SourceToken.RegisterType()) << "[" << OffsetString << "];\n";
    }
    else
    {
        File << "\t\t" << TempName << " = " << Modifier << GetSimulatorRegisterName(SourceToken.RegisterType()) << "[" << OffsetString << "];\n";
        File << "\t\tVec4Swizzle(" << TargetName << ", " << TempName << ", ";
        for(UINT i = 0; i < 4; i++)
        {
            File << GetSwizzleChannelString(SourceToken.SwizzleChannels(0));
            if(i < 3)
            {
                File << ", ";
            }
            else
            {
                File << ");\n";
            }
        }
    }
}

void VShader::OutputCSourceInstruction(ShaderInstruction &I, ofstream &File)
{
    File << "\t//\n\t// " << I.ToString() << "\n\t//\n\t{\n";
    D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode = I._InstructionToken.OpCode();
    UINT Size = GetVSInstructionSize(OpCode);
    String ErrorString = String("Unsupported instruction: ") + GetInstructionOpCodeString(I._InstructionToken.OpCode()) +
                         String(", Size ") + String(I._InstructionToken.Size());
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
        File << "\t\tVec4f Result, Source, SourcePreSwizzle;\n";
        SourceParameterToken SourceToken = I.GetSourceParameterToken(0);
        SourceParameterToken RelativeToken = 0;
        if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
        {
            RelativeToken = I.GetRelativeSourceParameterToken(0);
        }
        OutputCSourceResolution(SourceToken, RelativeToken, "SourcePreSwizzle", "Source", File);

        switch(OpCode)
        {
        case D3DSIO_MOV:
        case D3DSIO_MOVA:
            File << "\t\tResult = Source;\n";
            break;
        case D3DSIO_RCP:
            File << "\t\tINST_VS_RCP(Result, Source);\n";
            break;
        case D3DSIO_RSQ:
            File << "\t\tINST_VS_RSQ(Result, Source);\n";
            break;
        case D3DSIO_EXP:
        case D3DSIO_EXPP:
            File << "\t\tINST_VS_EXP(Result, Source);\n";
            break;
        case D3DSIO_LOG:
        case D3DSIO_LOGP:
            File << "\t\tINST_VS_LOG(Result, Source);\n";
            break;
        case D3DSIO_FRC:
            File << "\t\tINST_VS_FRC(Result, Source);\n";
            break;
        case D3DSIO_ABS:
            File << "\t\tINST_VS_ABS(Result, Source);\n";
            break;
        case D3DSIO_NRM:
            File << "\t\tINST_VS_NRM(Result, Source);\n";
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
    }
    else if(Size == 3)
    {
        File << "\t\tVec4f Result, Sources[2], SourcesPreSwizzle[2];\n";
        for(UINT i = 0; i < 2; i++)
        {
            SourceParameterToken SourceToken = I.GetSourceParameterToken(i);
            SourceParameterToken RelativeToken = 0;
            if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
            {
                RelativeToken = I.GetRelativeSourceParameterToken(i);
            }
            OutputCSourceResolution(SourceToken,
                                    RelativeToken,
                                    String("SourcesPreSwizzle[") + String(i) + String("]"),
                                    String("Sources[") + String(i) + String("]"),
                                    File);
        }

        switch(OpCode)
        {
        case D3DSIO_ADD:
            File << "\t\tResult = Sources[0] + Sources[1];\n";
            break;
        case D3DSIO_SUB:
            File << "\t\tResult = Sources[0] - Sources[1];\n";
            break;
        case D3DSIO_MUL:
            File << "\t\tINST_VS_MUL(Result, Sources);\n";
            break;
        case D3DSIO_DP3:
            File << "\t\tINST_VS_DP3(Result, Sources);\n";
            break;
        case D3DSIO_DP4:
            File << "\t\tINST_VS_DP4(Result, Sources);\n";
            break;
        case D3DSIO_MIN:
            File << "\t\tResult = Vec4f::Minimize(Sources[0], Sources[1]);\n";
            break;
        case D3DSIO_MAX:
            File << "\t\tResult = Vec4f::Maximize(Sources[0], Sources[1]);\n";
            break;
        case D3DSIO_SLT:
            File << "\t\tINST_VS_SLT(Result, Sources);\n";
            break;
        case D3DSIO_SGE:
            File << "\t\tINST_VS_SGE(Result, Sources);\n";
            break;
        case D3DSIO_DST:
            File << "\t\tINST_VS_DST(Result, Sources);\n";
            break;
        case D3DSIO_POW:
            File << "\t\tINST_VS_POW(Result, Sources);\n";
            break;
        case D3DSIO_CRS:
            File << "\t\tINST_VS_CRS(Result, Sources);\n";
            break;
        default:
            SignalError(ErrorString);
        }
    }
    else if(Size == 4)
    {
        File << "\t\tVec4f Result, Sources[3], SourcesPreSwizzle[3];\n";
        for(UINT i = 0; i < 3; i++)
        {
            SourceParameterToken SourceToken = I.GetSourceParameterToken(i);
            SourceParameterToken RelativeToken = 0;
            if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
            {
                RelativeToken = I.GetRelativeSourceParameterToken(i);
            }
            OutputCSourceResolution(SourceToken,
                                    RelativeToken,
                                    String("SourcesPreSwizzle[") + String(i) + String("]"),
                                    String("Sources[") + String(i) + String("]"),
                                    File);
        }

        switch(OpCode)
        {
        case D3DSIO_MAD:
            File << "\t\tINST_VS_MAD(Result, Sources);\n";
            break;
        case D3DSIO_LRP:
            File << "\t\tINST_VS_LRP(Result, Sources);\n";
            break;
        case D3DSIO_SGN:
            File << "\t\tINST_VS_SGN(Result, Sources);\n";
            break;
        case D3DSIO_SINCOS:
            File << "\t\tINST_VS_SINCOS(Result, Sources);\n";
            break;
        default:
            SignalError(ErrorString);
        }
    }
    else
    {
        SignalError("Invalid instruction size.");
    }
    if(Size >= 2)
    {
        File << "\t\tVec4f &Destination = " << GetSimulatorRegisterName(Destination.RegisterType()) << "[" << Destination.RegisterOffset() << "];\n";
        if(Destination.WriteMask(0) == 1 && 
           Destination.WriteMask(1) == 1 && 
           Destination.WriteMask(2) == 1 && 
           Destination.WriteMask(3) == 1)
        {
            File << "\t\tDestination = Result;\n";
        }
        else
        {
            for(UINT i = 0; i < 4; i++)
            {
                if(Destination.WriteMask(i) == 1)
                {
                    File << "\t\tDestination." << GetSwizzleChannelString(D3DSHADER_SWIZZLE_CHANNEL(i))
                         << " = Result." << GetSwizzleChannelString(D3DSHADER_SWIZZLE_CHANNEL(i)) << ";\n";
                }
            }
        }
    }
    File << "\t}\n\t\n";
}