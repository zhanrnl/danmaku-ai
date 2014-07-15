//
// HelperFunctions.h
//
// Various useful functions at global scope
//

//
// Misc functions
//
__forceinline bool ScreenBoundInsideRegion(const Rectangle2f &ScreenBound, const Rectangle2i &ScreenRegion)
{
    return (ScreenBound.Min.x >= ScreenRegion.Min.x &&
            ScreenBound.Min.y >= ScreenRegion.Min.y &&
            ScreenBound.Max.x <= ScreenRegion.Max.x &&
            ScreenBound.Max.y <= ScreenRegion.Max.y);
}

//
// Database helpers
//
String BaseNameFromRace(RaceType Race);
String WorkerNameFromRace(RaceType Race);
String ExtractorNameFromRace(RaceType Race);
String ScoutNameFromRace(RaceType Race);
String SupplyNameFromRace(RaceType Race);
__forceinline bool IsWorkerUnit(const String &UnitName)
{
    return (UnitName == "Drone" || UnitName == "Probe" || UnitName == "SCV");
}
__forceinline bool IsBaseBuilding(const String &UnitName)
{
    return (UnitName == "Hatchery" || UnitName == "Lair" || UnitName == "Hive" ||
            UnitName == "Nexus" ||
            UnitName == "Command Center" || UnitName == "Orbital Command" || UnitName == "Planetary Fortress");
}
__forceinline bool IsSupplyUnit(const String &UnitName)
{
    return (UnitName == "Overlord" || UnitName == "Pylon" || UnitName == "SupplyDepot");
}
__forceinline bool IsExtractorUnit(const String &UnitName)
{
    return (UnitName == "Extractor" || UnitName == "Assimilator" || UnitName == "Refinery");
}

//
// Time helpers
//
double GameTime();

//
// Key Helpers
//
bool Pressed(SHORT Key);

//
// Bit-level modifiers
//
__forceinline UINT CastBoolToBit(bool B)
{
    if(B)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

__forceinline UINT GetBit(DWORD D, UINT BitIndex)
{
    return CastBoolToBit((D & (1 << BitIndex)) != 0);
}

__forceinline void SetBit(DWORD &D, UINT BitIndex, UINT BitValue)
{
    if(BitValue == 0)
    {
        D &= (0xFFFFFFFF ^ (1 << BitIndex));
    }
    else
    {
        D |= (1 << BitIndex);
    }
}

__forceinline UINT GetInteger(DWORD D, UINT LowBit, UINT HighBit)
{
    DWORD Mask = 0;
    UINT Length = 1 + HighBit - LowBit;
    for(UINT i = 0; i < Length; i++)
    {
        Mask |= (1 << i);
    }
    return ((D >> LowBit) & Mask);
}

//
// Shader helpers
//
String GetSimulatorRegisterName(D3DSHADER_PARAM_REGISTER_TYPE Type);
UINT GetSimulatorRegisterCount(D3DSHADER_PARAM_REGISTER_TYPE Type);
UINT GetVSInstructionSize(DWORD dwOpcode);