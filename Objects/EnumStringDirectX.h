
enum D3DSHADER_SWIZZLE_CHANNEL
{
    D3DSWC_X = 0,
    D3DSWC_Y = 1,
    D3DSWC_Z = 2,
    D3DSWC_W = 3,
};

String GetDeclUsageString(D3DDECLUSAGE Usage);
String GetSwizzleChannelString(D3DSHADER_SWIZZLE_CHANNEL Channel);
String GetInstructionOpCodeString(D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode);
String GetRegisterTypeString(D3DSHADER_PARAM_REGISTER_TYPE RegisterType);
String GetRegisterTypeString(D3DSHADER_PARAM_REGISTER_TYPE RegisterType, UINT Offset);
String GetInstructionOpCodeString(D3DSHADER_INSTRUCTION_OPCODE_TYPE OpCode);
String GetPrimitiveTypeString(D3DPRIMITIVETYPE Type);