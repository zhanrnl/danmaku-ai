#include "Main.h"

String GetPrimitiveTypeString(D3DPRIMITIVETYPE Type)
{
    switch(Type)
    {
    case D3DPT_POINTLIST:
        return "PointList";
    case D3DPT_LINELIST:
        return "LineList";
    case D3DPT_LINESTRIP:
        return "LineStrip";
    case D3DPT_TRIANGLELIST:
        return "TriangleList";
    case D3DPT_TRIANGLESTRIP:
        return "TriangleStrip";
    case D3DPT_TRIANGLEFAN:
        return "TriangleFan";
    default:
        return "Invalid";
    }
}

String GetDeclUsageString(D3DDECLUSAGE Usage)
{
    switch(Usage)
    {
    case D3DDECLUSAGE_POSITION:
        return "POSITION";
    case D3DDECLUSAGE_BLENDWEIGHT:
        return "BLENDWEIGHT";
    case D3DDECLUSAGE_BLENDINDICES:
        return "BLENDINDICES";
    case D3DDECLUSAGE_NORMAL:
        return "NORMAL";
    case D3DDECLUSAGE_PSIZE:
        return "PSIZE";
    case D3DDECLUSAGE_TEXCOORD:
        return "TEXCOORD";
    case D3DDECLUSAGE_TANGENT:
        return "TANGENT";
    case D3DDECLUSAGE_BINORMAL:
        return "BINORMAL";
    case D3DDECLUSAGE_TESSFACTOR:
        return "TESSFACTOR";
    case D3DDECLUSAGE_POSITIONT:
        return "POSITIONT";
    case D3DDECLUSAGE_COLOR:
        return "COLOR";
    case D3DDECLUSAGE_FOG:
        return "FOG";
    case D3DDECLUSAGE_DEPTH:
        return "DEPTH";
    case D3DDECLUSAGE_SAMPLE:
        return "SAMPLE";
    default:
        return "ERROR";
    }
}

String GetSwizzleChannelString(D3DSHADER_SWIZZLE_CHANNEL Channel)
{
    switch(Channel)
    {
    case D3DSWC_X:
        return "x";
    case D3DSWC_Y:
        return "y";
    case D3DSWC_Z:
        return "z";
    case D3DSWC_W:
        return "w";
    default:
        return "ERROR";
    }
}

String GetRegisterTypeString(D3DSHADER_PARAM_REGISTER_TYPE RegisterType)
{
    switch(RegisterType)
    {
    case D3DSPR_TEMP:
        return "r";
    case D3DSPR_INPUT:
        return "v";
    case D3DSPR_CONST:
        return "c";
    case D3DSPR_ADDR:
        return "a";
    //case D3DSPR_TEXTURE:      //pixel shader only
    //    return "TEXTURE";
    case D3DSPR_RASTOUT:
        return "RASTOUT";
    case D3DSPR_ATTROUT:
        return "oColor";
    case D3DSPR_TEXCRDOUT:
        return "oT";
    //case D3DSPR_OUTPUT:
    //    return "o";
    case D3DSPR_CONSTINT:
        return "ci";
    case D3DSPR_COLOROUT:
        return "co";
    case D3DSPR_DEPTHOUT:
        return "do";
    case D3DSPR_SAMPLER:
        return "SAMPLER";
    case D3DSPR_CONST2:
        return "CONST2";
    case D3DSPR_CONST3:
        return "CONST3";
    case D3DSPR_CONST4:
        return "CONST4";
    case D3DSPR_CONSTBOOL:
        return "cb";
    case D3DSPR_LOOP:
        return "aL";
    case D3DSPR_TEMPFLOAT16:
        return "TEMPFLOAT16";
    case D3DSPR_MISCTYPE:
        return "MISCTYPE";
    case D3DSPR_LABEL:
        return "LABEL";
    case D3DSPR_PREDICATE:
        return "PREDICATE";
    default:
        return "ERROR";
    }
}

String GetRegisterTypeString(D3DSHADER_PARAM_REGISTER_TYPE RegisterType, UINT Offset)
{
    if(RegisterType == D3DSPR_RASTOUT)
    {
        if(Offset == 0)
        {
            return "oPos";
        }
        else if(Offset == 1)
        {
            return "oFog";
        }
    }
    return GetRegisterTypeString(RegisterType) + String(Offset);
}

String GetInstructionOpCodeString(D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode)
{
    switch(OpCode)
    {
    case D3DSIO_NOP:
        return "NOP";
    case D3DSIO_MOV:
        return "MOV";
    case D3DSIO_ADD:
        return "ADD";
    case D3DSIO_SUB:
        return "SUB";
    case D3DSIO_MAD:
        return "MAD";
    case D3DSIO_MUL:
        return "MUL";
    case D3DSIO_RCP:
        return "RCP";
    case D3DSIO_RSQ:
        return "RSQ";
    case D3DSIO_DP3:
        return "DP3";
    case D3DSIO_DP4:
        return "DP4";
    case D3DSIO_MIN:
        return "MIN";
    case D3DSIO_MAX:
        return "MAX";
    case D3DSIO_SLT:
        return "SLT";
    case D3DSIO_SGE:
        return "SGE";
    case D3DSIO_EXP:
        return "EXP";
    case D3DSIO_LOG:
        return "LOG";
    case D3DSIO_LIT:
        return "LIT";
    case D3DSIO_DST:
        return "DST";
    case D3DSIO_LRP:
        return "LRP";
    case D3DSIO_FRC:
        return "FRC";
    case D3DSIO_M4x4:
        return "M4x4";
    case D3DSIO_M4x3:
        return "M4x3";
    case D3DSIO_M3x4:
        return "M3x4";
    case D3DSIO_M3x3:
        return "M3x3";
    case D3DSIO_M3x2:
        return "M3x2";
    case D3DSIO_CALL:
        return "CALL";
    case D3DSIO_CALLNZ:
        return "CALLNZ";
    case D3DSIO_LOOP:
        return "LOOP";
    case D3DSIO_RET:
        return "RET";
    case D3DSIO_ENDLOOP:
        return "ENDLOOP";
    case D3DSIO_LABEL:
        return "LABEL";
    case D3DSIO_DCL:
        return "DCL";
    case D3DSIO_POW:
        return "POW";
    case D3DSIO_CRS:
        return "CRS";
    case D3DSIO_SGN:
        return "SGN";
    case D3DSIO_ABS:
        return "ABS";
    case D3DSIO_NRM:
        return "NRM";
    case D3DSIO_SINCOS:
        return "SINCOS";
    case D3DSIO_REP:
        return "REP";
    case D3DSIO_ENDREP:
        return "ENDREP";
    case D3DSIO_IF:
        return "IF";
    case D3DSIO_IFC:
        return "IFC";
    case D3DSIO_ELSE:
        return "ELSE";
    case D3DSIO_ENDIF:
        return "ENDIF";
    case D3DSIO_BREAK:
        return "BREAK";
    case D3DSIO_BREAKC:
        return "BREAKC";
    case D3DSIO_MOVA:
        return "MOVA";
    case D3DSIO_DEFB:
        return "DEFB";
    case D3DSIO_DEFI:
        return "DEFI";
    case D3DSIO_TEXCOORD:
        return "TEXCOORD";
    case D3DSIO_TEXKILL:
        return "TEXKILL";
    case D3DSIO_TEX:
        return "TEX";
    case D3DSIO_TEXBEM:
        return "TEXBEM";
    case D3DSIO_TEXBEML:
        return "TEXBEML";
    case D3DSIO_TEXREG2AR:
        return "TEXREG2AR";
    case D3DSIO_TEXREG2GB:
        return "TEXREG2GB";
    case D3DSIO_TEXM3x2PAD:
        return "TEXM3x2PAD";
    case D3DSIO_TEXM3x2TEX:
        return "TEXM3x2TEX";
    case D3DSIO_TEXM3x3PAD:
        return "TEXM3x3PAD";
    case D3DSIO_TEXM3x3TEX:
        return "TEXM3x3TEX";
    case D3DSIO_RESERVED0:
        return "RESERVED0";
    case D3DSIO_TEXM3x3SPEC:
        return "TEXM3x3SPEC";
    case D3DSIO_TEXM3x3VSPEC:
        return "TEXM3x3VSPEC";
    case D3DSIO_EXPP:
        return "EXPP";
    case D3DSIO_LOGP:
        return "LOGP";
    case D3DSIO_CND:
        return "CND";
    case D3DSIO_DEF:
        return "DEF";
    case D3DSIO_TEXREG2RGB:
        return "TEXREG2RGB";
    case D3DSIO_TEXDP3TEX:
        return "TEXDP3TEX";
    case D3DSIO_TEXM3x2DEPTH:
        return "TEXM3x2DEPTH";
    case D3DSIO_TEXDP3:
        return "TEXDP3";
    case D3DSIO_TEXM3x3:
        return "TEXM3x3";
    case D3DSIO_TEXDEPTH:
        return "TEXDEPTH";
    case D3DSIO_CMP:
        return "CMP";
    case D3DSIO_BEM:
        return "BEM";
    case D3DSIO_DP2ADD:
        return "DP2ADD";
    case D3DSIO_DSX:
        return "DSX";
    case D3DSIO_DSY:
        return "DSY";
    case D3DSIO_TEXLDD:
        return "TEXLDD";
    case D3DSIO_SETP:
        return "SETP";
    case D3DSIO_TEXLDL:
        return "TEXLDL";
    case D3DSIO_BREAKP:
        return "BREAKP";
    case D3DSIO_PHASE:
        return "PHASE";
    case D3DSIO_COMMENT:
        return "COMMENT";
    case D3DSIO_END:
        return "END";
    default:
        return "ERROR";
    }
}