//
// DatabaseManager.h
//
// Stores all databases used by the AI
// Written by Matthew Fisher
//

class DatabaseHeader
{
public:
    DatabaseHeader(const String &Str);
    UINT GetFieldIndex(const String &FieldName) const;
    __forceinline UINT GetEntryCount() const
    {
        return Words.Length();
    }

private:
    Vector<String> Words;
};

struct UnitEntry
{
    UnitEntry(const String &Str, const DatabaseHeader &Header);

    String Name;
    int Prims;
    RaceType Race;
    UnitPrimaryType PrimaryType;
    UnitSecondaryType SecondaryType;
    BuildClassType BuildClass;
    String BuiltAt;
    String Requirement;
    ComplexityType Complexity;
    bool Hostile, AttackAir, AttackGround, Flying, Detector;
	int Range;

    UINT MineralCost, GasCost, SupplyCost;
    float BuildTime;
    char Hotkey;
};

struct MinimapEntry
{
    void LoadBasesFromFile(const String &Filename);

    Bitmap Bmp;
    String Name;
    Vector<BaseInfo> Bases;
};

//
// Stores all databases used by the AI
//
class DatabaseManager : public Manager
{
public:
    void Init();

    UnitEntry* GetUnitEntry(const String &Name) const;
    MinimapEntry* FindClosestMinimap(const Bitmap &Bmp) const;
    
private:
    void LoadUnitEntries(const String &DatabaseFilename);
    void LoadMinimapEntries();
    
    Vector<UnitEntry*> _UnitEntries;
    Vector<MinimapEntry*> _MinimapEntries;
};
