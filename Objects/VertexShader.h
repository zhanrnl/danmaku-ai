#include "VertexShaderInstructions.h"

const UINT ShaderInstructionTypeCount = D3DSIO_BREAKP + 1;

struct ShaderInfoEntry;
class VShaderSimulator;
struct VertexData;
struct VShaderSimulatorHashInfo
{
    VShaderSimulatorHashInfo()
    {
        Prepare = NULL;
        Restore = NULL;
        Compiled = NULL;
    }
    VShaderSimulatorHashInfo(void (*_Prepare)(VShaderSimulator *ThisPtr),
                             void (*_Restore)(VShaderSimulator *ThisPtr),
                             void (*_Compiled)(VShaderSimulator *ThisPtr, const VertexData &VData))
    {
        Prepare = _Prepare;
        Restore = _Restore;
        Compiled = _Compiled;
    }

    __forceinline bool Valid() const
    {
        return (Prepare != NULL);
    }

    void (*Prepare)(VShaderSimulator *ThisPtr);
    void (*Restore)(VShaderSimulator *ThisPtr);
    void (*Compiled)(VShaderSimulator *ThisPtr, const VertexData &VData);
};

class VShader
{
public:
    VShader(const DWORD *pFunction, HANDLE Handle);

    __forceinline UINT Hash()
    {
        return _ComputedHash;
    }
    __forceinline HANDLE Handle()
    {
        return _Handle;
    }
    __forceinline const VShaderSimulatorHashInfo& HashInfo()
    {
        return _HashInfo;
    }
    __forceinline UINT MajorVersion()
    {
        return _MajorVersion;
    }
	bool DeclarationFound(D3DDECLUSAGE Usage) const;

    void ComputeShaderInstructionCounts(Vector<UINT> &InstructionCounts) const;
    void OutputDisassembly(ofstream &File);
    void OutputCSource(ofstream &SourceFile, ofstream &HeaderFile);
    void OutputCSourceInstruction(ShaderInstruction &Instruction, ofstream &File);
    void OutputCSourceResolution(SourceParameterToken SourceToken, SourceParameterToken RelativeToken, const String &TempName, const String &TargetName, ofstream &File);
    Vector<CompleteShaderInstruction> CompleteInstructions;
    Vector<ShaderInstruction> Instructions;
    Vector<ShaderInstruction> Definitions;
    Vector<ShaderInstruction> Declarations;

private:
    void RecomputeHash();

    UINT _MinorVersion, _MajorVersion;
    UINT _ComputedHash;
    VShaderSimulatorHashInfo _HashInfo;
    HANDLE _Handle;
};

class PShader
{
public:
    PShader(const DWORD *pFunction, HANDLE Handle);

    __forceinline UINT Hash()
    {
        return _ComputedHash;
    }

    void OutputDisassembly(ofstream &File);
    Vector<CompleteShaderInstruction> CompleteInstructions;
    Vector<ShaderInstruction> Instructions;
    Vector<ShaderInstruction> Definitions;
    Vector<ShaderInstruction> Declarations;
    
private:
    UINT _MinorVersion, _MajorVersion;
    UINT _ComputedHash;
    HANDLE _Handle;
};
