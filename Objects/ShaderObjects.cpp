#include "Main.h"

String InstructionToken::ToString()
{
    String Result = GetInstructionOpCodeString(OpCode());
    if(Predicated())
    {
        Result = Result + " Predicated";
    }
    return Result;
}

String SourceParameterToken::ToString(SourceParameterToken RelativeToken)
{
    String SwizzleString = ".xxxx";
    String AddressString;
    for(UINT i = 0; i < 4; i++)
    {
        SwizzleString[i + 1] = GetSwizzleChannelString(SwizzleChannels(i))[0];
    }
    if(SwizzleString == ".xyzw")
    {
        SwizzleString.FreeMemory();
    }
    else if(SwizzleString == ".xxxx")
    {
        SwizzleString = ".x";
    }
    else if(SwizzleString == ".yyyy")
    {
        SwizzleString = ".y";
    }
    else if(SwizzleString == ".zzzz")
    {
        SwizzleString = ".z";
    }
    else if(SwizzleString == ".wwww")
    {
        SwizzleString = ".w";
    }

    String SourceModifierStartString, SourceModifierEndString;
    if(SourceModifier() == D3DSPSM_NONE)
    {
        
    }
    else if(SourceModifier() == D3DSPSM_NEG)
    {
        SourceModifierStartString = "-";
    }
    else if(SourceModifier() == D3DSPSM_ABS)
    {
        SourceModifierStartString = "abs(";
        SourceModifierEndString = ")";
    }
    else if(SourceModifier() == D3DSPSM_ABSNEG)
    {
        SourceModifierStartString = "-abs(";
        SourceModifierEndString = ")";
    }
    else
    {
        g_Context->Files.Assert << "Unexpected SourceModifier: " << SourceModifier() << endl;
        SourceModifierStartString = "ERROR";
    }
    if(AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
    {
        AddressString = String("[") + RelativeToken.ToString() + String("]");
    }
    String Result = SourceModifierStartString +
                    GetRegisterTypeString(RegisterType(), RegisterOffset()) +
                    AddressString +
                    SwizzleString +
                    SourceModifierEndString;
    return Result;
}

String DestinationParameterToken::ToString()
{
    String MaskString = ".";
    for(UINT i = 0; i < 4; i++)
    {
        if(WriteMask(i) == 1)
        {
            MaskString.PushEnd(GetSwizzleChannelString(D3DSHADER_SWIZZLE_CHANNEL(i))[0]);
        }
    }
    if(MaskString == ".xyzw")
    {
        MaskString.FreeMemory();
    }
    String Result = GetRegisterTypeString(RegisterType(), RegisterOffset()) +
                    MaskString;
    if(AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
    {
        Result = Result + String(" Relative");
    }
    return Result;
}

String CompleteShaderInstruction::ToString()
{
    if(_Instruction._InstructionToken.OpCode() == D3DSIO_COMMENT)
    {
        if(Comment.StartsWith("CTAB"))
        {
            return "<Constant Table>";
        }
        else if(Comment.StartsWith("DBUG"))
        {
            return "<Debug Info>";
        }
        else
        {
            return Comment;
        }
    }
    else
    {
        return _Instruction.ToString();
    }
}

String ShaderInstruction::ToString()
{
    if(_InstructionToken.OpCode() == D3DSIO_DEF)
    {
        String Result = _InstructionToken.ToString().MakeLowercase();
        UINT InstructionSize = _InstructionToken.Size();
        for(UINT i = 0; i < InstructionSize; i++)
        {
            if(i == 0)
            {
                Result = Result + " " + GetDestinationParameterToken().ToString();
            }
            else
            {
                float *FloatCast = (float *)(&_ParamaterTokens[i]);
                Result = Result + String(", ") + String(*FloatCast);
            }
        }
        return Result;
    }
    else if(_InstructionToken.OpCode() == D3DSIO_DCL)
    {
        DestinationParameterToken Destination(_ParamaterTokens[1]);
        D3DDECLUSAGE Usage = D3DDECLUSAGE(GetInteger(_ParamaterTokens[0], 0, 4));
        String DeclString = GetDeclUsageString(Usage).MakeLowercase();
        if(Usage == D3DDECLUSAGE_TEXCOORD)
        {
            DeclString = DeclString + String(GetInteger(_ParamaterTokens[0], 16, 19));
        }
        String Result = _InstructionToken.ToString().MakeLowercase() + "_" +
                        DeclString + " " +
                        Destination.ToString();
        return Result;
    }
    else
    {
        String Result = _InstructionToken.ToString().MakeLowercase();
        UINT InstructionSize = GetVSInstructionSize(_InstructionToken.OpCode());
        for(UINT i = 0; i < InstructionSize; i++)
        {
            if(i == 0)
            {
                Result += " " + GetDestinationParameterToken().ToString();
            }
            else
            {
                SourceParameterToken SourceToken = GetSourceParameterToken(i - 1);
                SourceParameterToken RelativeToken(0);
                if(SourceToken.AddressMode() == D3DSHADER_ADDRMODE_RELATIVE)
                {
                    RelativeToken = GetRelativeSourceParameterToken(i - 1);
                }
                Result += ", " + SourceToken.ToString(RelativeToken);
            }
        }
        return Result;
    }
}
