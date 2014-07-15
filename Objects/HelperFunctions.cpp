#include "Main.h"

double GameTime()
{
    return g_Context->Controller.FrameStartTime();
}

String SupplyNameFromRace(RaceType Race)
{
    switch(Race)
    {
    case RaceZerg:
        return "Overlord";
    case RaceTerran:
        return "SupplyDepot";
    case RaceProtoss:
        return "Pylon";
    default:
        return "Error";
    }
}

String ScoutNameFromRace(RaceType Race)
{
    switch(Race)
    {
    case RaceZerg:
        return "Zergling";
    case RaceTerran:
        return "Marine";
    case RaceProtoss:
        return "Zealot";
    default:
        return "Error";
    }
}

String ExtractorNameFromRace(RaceType Race)
{
    switch(Race)
    {
    case RaceZerg:
        return "Extractor";
    case RaceTerran:
        return "Refinery";
    case RaceProtoss:
        return "Assimilator";
    default:
        return "Error";
    }
}

String BaseNameFromRace(RaceType Race)
{
	switch(Race)
    {
    case RaceZerg:
        return "Hatchery";
    case RaceTerran:
        return "CommandCenter";
    case RaceProtoss:
        return "Nexus";
    default:
        return "Error";
    }
}

String WorkerNameFromRace(RaceType Race)
{
    switch(Race)
    {
    case RaceZerg:
        return "Drone";
    case RaceTerran:
        return "SCV";
    case RaceProtoss:
        return "Probe";
    default:
        return "Error";
    }
}

bool IsNumeric(char c)
{
    return (c >= '0' && c <= '9');
}

bool Pressed(SHORT Key)
{
    return (Key == -32767 || Key == -32768);
}

UINT GetVSInstructionSize(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
        case D3DSIO_NOP : return 0;
        case D3DSIO_MOV : return 2;
        case D3DSIO_ADD : return 3;
        case D3DSIO_SUB : return 3;
        case D3DSIO_MAD : return 4;
        case D3DSIO_MUL : return 3;
        case D3DSIO_RCP : return 2;
        case D3DSIO_RSQ : return 2;
        case D3DSIO_DP3 : return 3;
        case D3DSIO_DP4 : return 3;
        case D3DSIO_MIN : return 3;
        case D3DSIO_MAX : return 3;
        case D3DSIO_SLT : return 3;
        case D3DSIO_SGE : return 3;
        case D3DSIO_EXP : return 2;
        case D3DSIO_LOG : return 2;
        case D3DSIO_LIT : return 2;
        case D3DSIO_DST : return 3;
        case D3DSIO_LRP : return 4;
        case D3DSIO_FRC : return 2;
        case D3DSIO_M4x4 : return 3;
        case D3DSIO_M4x3 : return 3;
        case D3DSIO_M3x4 : return 3;
        case D3DSIO_M3x3 : return 3;
        case D3DSIO_M3x2 : return 3;
        case D3DSIO_CALL : return 1;
        case D3DSIO_CALLNZ : return 2;
        case D3DSIO_LOOP : return 2;
        case D3DSIO_RET : return 0;
        case D3DSIO_ENDLOOP : return 0;
        case D3DSIO_LABEL : return 1;
        case D3DSIO_DCL : return 2;
        case D3DSIO_POW : return 3;
        case D3DSIO_CRS : return 3;
        case D3DSIO_SGN : return 4;
        case D3DSIO_ABS : return 2;
        case D3DSIO_NRM : return 2;
        case D3DSIO_SINCOS : return 4;
        case D3DSIO_REP : return 1;
        case D3DSIO_ENDREP : return 0;
        case D3DSIO_IF : return 1;
        case D3DSIO_IFC : return 2;
        case D3DSIO_ELSE : return 0;
        case D3DSIO_ENDIF : return 0;
        case D3DSIO_BREAK : return 0;
        case D3DSIO_BREAKC : return 2;
        case D3DSIO_MOVA : return 2;
        case D3DSIO_DEFB : return 2;
        case D3DSIO_DEFI : return 5;
        case D3DSIO_EXPP : return 2;
        case D3DSIO_LOGP : return 2;
        case D3DSIO_CND : return 4;
        case D3DSIO_DEF : return 5;
        case D3DSIO_CMP : return 4;
        case D3DSIO_BEM : return 3;
        case D3DSIO_DP2ADD : return 4;
        case D3DSIO_DSX : return 2;
        case D3DSIO_DSY : return 2;
        case D3DSIO_BREAKP : return 1;
        case D3DSIO_TEX : return 3;
        case D3DSIO_TEXLDD: return 5;
        case D3DSIO_TEXLDL: return 3;
        case D3DSIO_TEXKILL: return 1;
        default: 
            DebugOnlyAssert(NULL, "Invalid shader instruction.");
            return 0;
    }
}

String GetSimulatorRegisterName(D3DSHADER_PARAM_REGISTER_TYPE Type)
{
    switch(Type)
    {
    case D3DSPR_TEMP:
        return "TempRegisters";
    case D3DSPR_INPUT:
        return "InputRegisters";
    case D3DSPR_ADDR:
        return "AddressRegisters";
    case D3DSPR_CONST:
        return "ConstFloatRegisters";
    case D3DSPR_RASTOUT:
        return "OutputRasterRegisters";
    case D3DSPR_ATTROUT:
        return "OutputAttributeRegisters";
    case D3DSPR_TEXCRDOUT:
        return "OutputTextureRegisters";
    default:
        return "Invalid Register";
    }
}

UINT GetSimulatorRegisterCount(D3DSHADER_PARAM_REGISTER_TYPE Type)
{
    switch(Type)
    {
    case D3DSPR_TEMP:
        return VShaderConst::TempRegisterCount;
    case D3DSPR_INPUT:
        return VShaderConst::InputRegisterCount;
    case D3DSPR_ADDR:
        return VShaderConst::AddressRegisterCount;
    case D3DSPR_CONST:
        return VShaderConst::ConstFloatRegisterCount;
    case D3DSPR_RASTOUT:
        return VShaderConst::OutputRasterRegisterCount;
    case D3DSPR_ATTROUT:
        return VShaderConst::OutputAttributeRegisterCount;
    case D3DSPR_TEXCRDOUT:
        return VShaderConst::OutputTextureRegisterCount;
    default:
        SignalError("Invalid register type.");
        return 0;
    }
}
