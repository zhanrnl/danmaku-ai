
namespace VShaderConst
{
const UINT InputRegisterCount = 16;
const UINT TempRegisterCount = 12;
const UINT ConstFloatRegisterCount = 256;
const UINT ConstBoolRegisterCount = 64;
const UINT ConstIntRegisterCount = 64;
const UINT AddressRegisterCount = 1;
const UINT LoopCounterRegisterCount = 1;
const UINT OutputRasterRegisterCount = 2;
const UINT OutputAttributeRegisterCount = 2;
const UINT OutputTextureRegisterCount = 8;
const UINT RegisterCanidateCount = 10;
const D3DSHADER_PARAM_REGISTER_TYPE RegisterCanidates[RegisterCanidateCount] = {D3DSPR_TEMP,
                                                                                D3DSPR_INPUT,
                                                                                D3DSPR_ADDR,
                                                                                D3DSPR_LOOP,
                                                                                D3DSPR_CONST,
                                                                                D3DSPR_CONSTINT,
                                                                                D3DSPR_CONSTBOOL,
                                                                                D3DSPR_TEXCRDOUT,
                                                                                D3DSPR_RASTOUT,
                                                                                D3DSPR_ATTROUT};
};

class VShaderSimulator
{
public:
    VShaderSimulator() { _UnhashedRenderReported = false; }

    void InitalizeHashes();
    void OutputNewShaderList();

    void Prepare(VShader &VS);
    void Restore(VShader &VS);
    void LoadConstantsFromDevice();
    void ResetRegisters();
    
    void Simulate(VShader &VS, VertexData &V, ofstream &SimulationFile, bool UseFile);
    void LoadRegistersFromStream(VShader &VS, VertexData &V);

    void OutputRegisters(ofstream &File);
    void ApplyInstruction(ShaderInstruction &I);
    Vec4f& GetRegister(D3DSHADER_PARAM_REGISTER_TYPE Type, UINT Offset);
    Vec4f ResolveSourceToken(SourceParameterToken Source, SourceParameterToken RelativeToken);
    bool ContainsHash(DWORD Hash);
    VShaderSimulatorHashInfo GetHashInfoFromHash(DWORD Hash);

    Vec4f TempRegisters[VShaderConst::TempRegisterCount];
    Vec4f InputRegisters[VShaderConst::InputRegisterCount];
    Vec4f AddressRegisters[VShaderConst::AddressRegisterCount];
    int LoopCounterRegisters[VShaderConst::LoopCounterRegisterCount];
    Vec4f ConstFloatRegisters[VShaderConst::ConstFloatRegisterCount];
    Int4 ConstIntRegisters[VShaderConst::ConstIntRegisterCount];
    bool ConstBoolRegisters[VShaderConst::ConstBoolRegisterCount];
    Vec4f OutputTextureRegisters[VShaderConst::OutputTextureRegisterCount];
    Vec4f OutputRasterRegisters[VShaderConst::OutputRasterRegisterCount];
    Vec4f OutputAttributeRegisters[VShaderConst::OutputAttributeRegisterCount];
    Vector<UINT> RegistersUsed[VShaderConst::RegisterCanidateCount];

private:
    void ReportUnhashedRender();

    bool _UnhashedRenderReported;
    map<DWORD, VShaderSimulatorHashInfo> _HashedVShaders;
};
