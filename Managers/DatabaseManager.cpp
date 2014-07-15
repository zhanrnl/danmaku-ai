//
// DatabaseManager.cpp
//
// Stores all databases used by the AI
// Written by Matthew Fisher
//

#include "Main.h"

DatabaseHeader::DatabaseHeader(const String &Str)
{
    Str.Partition('\t', Words);
}

UINT DatabaseHeader::GetFieldIndex(const String &FieldName) const
{
    for(UINT i = 0; i < Words.Length(); i++)
    {
        if(Words[i] == FieldName)
        {
            return i;
        }
    }
    g_Context->Files.Assert << "Field not found: " << FieldName << endl;
    return 0;
}

UnitEntry::UnitEntry(const String &Str, const DatabaseHeader &Header)
{
    Vector<String> Words;
    Str.Partition('\t', Words);
    const float FastestGameSpeedFactor = 0.73f;

    Name = Words[Header.GetFieldIndex("Name")];
    Prims = Words[Header.GetFieldIndex("Prims")].ConvertToInteger();
    Race = GetRaceTypeFromString(String("Race") + Words[Header.GetFieldIndex("Race")]);
    PrimaryType = GetUnitPrimaryTypeFromString(String("UnitPrimary") + Words[Header.GetFieldIndex("Primary Type")]);
    SecondaryType = GetUnitSecondaryTypeFromString(String("UnitSecondary") + Words[Header.GetFieldIndex("Secondary Type")]);
    BuildClass = GetBuildClassTypeFromString(String("BuildClass") + Words[Header.GetFieldIndex("Build Class")]);
    BuiltAt = Words[Header.GetFieldIndex("Built At")];
    Requirement = Words[Header.GetFieldIndex("Requirement")];
    Complexity = GetComplexityTypeFromString(String("Complexity") + Words[Header.GetFieldIndex("Complexity")]);
    
	Hostile = Words[Header.GetFieldIndex("Hostile")].MakeLowercase().ConvertToBoolean();
	AttackAir = Words[Header.GetFieldIndex("AttackAir")].MakeLowercase().ConvertToBoolean();
	AttackGround = Words[Header.GetFieldIndex("AttackGround")].MakeLowercase().ConvertToBoolean();
	Flying = Words[Header.GetFieldIndex("Flying")].MakeLowercase().ConvertToBoolean();
	Range = Words[Header.GetFieldIndex("Range")].MakeLowercase().ConvertToInteger();
    
	MineralCost = Words[Header.GetFieldIndex("Mineral Cost")].ConvertToUnsignedInteger();
    GasCost = Words[Header.GetFieldIndex("Gas Cost")].ConvertToUnsignedInteger();
    SupplyCost = Words[Header.GetFieldIndex("Supply Cost")].ConvertToUnsignedInteger();
    BuildTime = Words[Header.GetFieldIndex("Build Time")].ConvertToUnsignedInteger() * FastestGameSpeedFactor;
    Hotkey = Words[Header.GetFieldIndex("Hotkey")][0];

	Detector = (Name == "Observer" || Name == "PhotonCannon" || Name == "Raven" || Name == "MissleTurret" || Name == "Overseer" || Name == "SporeCrawler");
}

void MinimapEntry::LoadBasesFromFile(const String &Filename)
{
    Vector<String> Lines, Words;
    Utility::GetFileLines(Filename, Lines);
    Lines[0].Partition(' ', Words);
    Bases.Allocate(Words[1].ConvertToUnsignedInteger());
    for(UINT BaseIndex = 0; BaseIndex < Bases.Length(); BaseIndex++)
    {
        BaseInfo &CurBase = Bases[BaseIndex];
        CurBase.ExtractorCount = 2;
        CurBase.Player = PlayerInvalid;
        CurBase.MyStartLocation = false;
        CurBase.EnemyStartLocation = false;
    }
    for(UINT LineIndex = 0; LineIndex < Lines.Length(); LineIndex++)
    {
        const String &CurLine = Lines[LineIndex];
        CurLine.Partition(' ', Words);
        if(Words.Length() == 4 || Words.Length() == 5)
        {
            UINT BaseIndex = Words[1].ConvertToUnsignedInteger();
            BaseInfo &CurBase = Bases[BaseIndex];
            const String &Property = Words[2];
            if(Words.Length() == 4)
            {
                bool Value = Words[3].MakeLowercase().ConvertToBoolean();
                if(Property == "Debris") CurBase.Debris = Value;
                else if(Property == "StartLocation") CurBase.StartLocation = Value;
                else if(Property == "Island") CurBase.Island = Value;
                else g_Context->Files.Assert << "Unexpected field property\n";
            }
            if (Words.Length() == 5)
            {
                Vec2f Value;
                Value.x = Words[3].ConvertToFloat();
                Value.y = Words[4].ConvertToFloat();
                if (Property == "AssaultLocation") CurBase.AssaultLocation = Value;
                else if (Property == "BaseLocation") CurBase.BaseLocation = Value;
                else if (Property == "BuildLocation") CurBase.BuildLocation = Value;
                else if (Property == "ExtractorLocation0") CurBase.ExtractorLocations[0] = Value;
                else if (Property == "ExtractorLocation1") CurBase.ExtractorLocations[1] = Value;
                else if (Property == "RampLocation") CurBase.RampLocation = Value;
                else g_Context->Files.Assert << "Unexpected field property\n";
            }
        }
    }
}

//
// Loads all database entries from the database file
//
void DatabaseManager::Init()
{
    LoadUnitEntries(g_Context->Parameters.DatabaseLoadDirectory + String("Units.txt"));
    LoadMinimapEntries();
}

void DatabaseManager::LoadUnitEntries(const String &DatabaseFilename)
{
    if(Utility::FileExists(DatabaseFilename))
    {
        Vector<String> Lines;
        Utility::GetFileLines(DatabaseFilename, Lines);
        DatabaseHeader Header(Lines[0]);
        for(UINT LineIndex = 1; LineIndex < Lines.Length(); LineIndex++)
        {
            String &CurLine = Lines[LineIndex];
            if(CurLine.Length() > 3)
            {
                _UnitEntries.PushEnd(new UnitEntry(CurLine, Header));
            }
        }
    }
    else
    {
        g_Context->Files.Assert << "Unit database " << DatabaseFilename << " not found\n";
    }
}

void DatabaseManager::LoadMinimapEntries()
{
    String BaseDirectory = g_Context->Parameters.BaseDataDirectory + String("Minimaps\\");
    Directory Dir(BaseDirectory);
    for(UINT FileIndex = 0; FileIndex < Dir.Files().Length(); FileIndex++)
    {
        const String &CurFilename = Dir.Files()[FileIndex];
        if(CurFilename.EndsWith(".png"))
        {
            String Name = Dir.Files()[FileIndex].RemoveSuffix(".png");
            String CurDataFilename = Name + String(".txt");
            if(Utility::FileExists(Dir.DirectoryPath() + CurDataFilename))
            {
                MinimapEntry *NewEntry = new MinimapEntry;
                NewEntry->Bmp.LoadPNG(Dir.DirectoryPath() + CurFilename);
                NewEntry->Name = Name;
                NewEntry->LoadBasesFromFile(Dir.DirectoryPath() + CurDataFilename);
                _MinimapEntries.PushEnd(NewEntry);
            }
            else
            {
                g_Context->Files.Assert << "Minimap annotation not found: " << Name << endl;
            }
        }
    }
}

UnitEntry* DatabaseManager::GetUnitEntry(const String &Name) const
{
    for(UINT UnitIndex = 0; UnitIndex < _UnitEntries.Length(); UnitIndex++)
    {
        if(_UnitEntries[UnitIndex]->Name == Name)
        {
            return _UnitEntries[UnitIndex];
        }
    }
    g_Context->Files.Assert << "Unit not found: " << Name << endl;
    return NULL;
}

//UINT ComputeMinimapMatchError(const Bitmap &A, const Bitmap &B)
//{
//    if(A.Dimensions() != B.Dimensions())
//    {
//        g_Context->Files.Assert << "Minimap dimensions do not match\n";
//        return 200 * 200;
//    }
//    //A.SavePNG("C:\\SC2AICapture\\A.png");
//    //B.SavePNG("C:\\SC2AICapture\\B.png");
//    const UINT Width = A.Width();
//    const UINT Height = A.Height();
//    UINT Error = 0;
//    for(UINT Y = 0; Y < Height; Y++)
//    {
//        for(UINT X = 0; X < Width; X++)
//        {
//            bool ATest = (A[Y][X] == MinimapResourceColor);
//            bool BTest = (B[Y][X] == MinimapResourceColor);
//            if(ATest && !BTest)
//            {
//                Error++;
//            }
//            if(!ATest && BTest)
//            {
//                Error++;
//            }
//        }
//    }
//    return Error;
//}

//MinimapEntry* DatabaseManager::FindClosestMinimap(const Bitmap &Bmp) const
//{
//    MinimapEntry *BestMinimap = NULL;
//    UINT BestError = 200 * 200;
//    for(UINT MinimapIndex = 0; MinimapIndex < _MinimapEntries.Length(); MinimapIndex++)
//    {
//        MinimapEntry *CurMinimap = _MinimapEntries[MinimapIndex];
//        UINT CurError = ComputeMinimapMatchError(Bmp, CurMinimap->Bmp);
//        if(CurError < BestError)
//        {
//            BestError = CurError;
//            BestMinimap = CurMinimap;
//        }
//    }
//    if(BestMinimap != NULL)
//    {
//        g_Context->WriteConsole(String("Minimap matched:") + BestMinimap->Name + String(",") + String(BestError), RGBColor::Yellow, OverlayPanelSystem);
//    }
//    return BestMinimap;
//}
