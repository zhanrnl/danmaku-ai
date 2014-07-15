//
// TextureEntryManager.h
//
// Handles all texture identification functions.  Texture matching
// is the primary method by which a rendered object is identified.
// Written by Matthew Fisher
//

//
// Stores information regarding the hash of a texture
//
struct TextureEntry
{
    TextureEntry(const String &_Filename, const String &_ID, RenderType _Type)
    {
        Filename = _Filename;
        ID = _ID;
        FirstCharacter = '&';
        if(ID.Length() > 0)
        {
            FirstCharacter = ID[0];
        }
        Type = _Type;
    }

    //
    // Original filename
    //
    String Filename;

    //
    // Texture ID
    //
    String ID;
    char FirstCharacter;

    //
    // Type of texture
    //
    RenderType Type;
};

//
// Map from a hash value to a TextureEntry structure
//
typedef hash_map<UINT, TextureEntry*> TextureEntryMap;

//
// Interface to all texture hash information
//
class TextureEntryManager : public Manager
{
public:
    void Init();

    const TextureEntry* FindTextureMatch(const Bitmap &Bmp);
    const TextureEntry* FindFontMatch(const Bitmap &Bmp);

private:
    static String FontUpdateStringName(const String &ID);

    const TextureEntry* FindMapMatch(const Bitmap &Bmp, const TextureEntryMap &Map);
    
    void AddDirectory(const String &Folder, RenderType Type, TextureEntryMap &Map, set<UINT> &Set);
    void AddBitmapFromFile(const String &Directory, const String &Filename, RenderType Type, TextureEntryMap &Map, set<UINT> &Set);
    void InsertBitmapIntoMap(const Bitmap &Bmp, const String &Filename, const String &ID, RenderType Type, TextureEntryMap &Map);
    
    void SaveMap(const TextureEntryMap &Map, const String &Filename);
    void LoadMap(TextureEntryMap &Map, set<UINT> &Set, const String &Filename);

    set<UINT> _TextureMapFilenameHashSet;
    TextureEntryMap _TextureMap;
    set<UINT> _FontMapFilenameHashSet;
    TextureEntryMap _FontMap;
};