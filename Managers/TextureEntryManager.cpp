//
// TextureEntryManager.cpp
//
// Handles all texture identification functions.  Texture matching
// is the primary method by which a rendered object is identified.
// Written by Matthew Fisher
//

#include "Main.h"

//
// Textures use long string literals to represent characters that cannot be used in Windows file system.
// This converts a single string literal into the appropriate character.
//
String TextureEntryManager::FontUpdateStringName(const String &ID)
{
    if(ID == "Space")
    {
        return " ";
    }
    else if(ID == "Apostrophe")
    {
        return "'";
    }
    else if(ID == "OpenBrace")
    {
        return "[";
    }
    else if(ID == "CloseBrace")
    {
        return "]";
    }
    else if(ID == "Colon")
    {
        return ":";
    }
    else if(ID == "Slash")
    {
        return "/";
    }
    else if(ID == "OpenParens")
    {
        return "(";
    }
    else if(ID == "CloseParens")
    {
        return ")";
    }
    else if(ID == "Colon")
    {
        return ":";
    }
    else if(ID == "Comma")
    {
        return ",";
    }
    else if(ID == "Dash")
    {
        return "-";
    }
    else if(ID == "Plus")
    {
        return "+";
    }
    else if(ID == "Period")
    {
        return ".";
    }
    else if(ID == "Exclimation")
    {
        return "!";
    }
    else if(ID == "Question")
    {
        return "?";
    }
    else if(ID == "Quotation")
    {
        return "\"";
    }
    else if(ID == "LessThan")
    {
        return "<";
    }
    else if(ID == "GreaterThan")
    {
        return ">";
    }
    else if(ID == "Star")
    {
        return "*";
    }
    return ID;
}

//
// Saves the currently loaded texture map to MapSaveString
//
void TextureEntryManager::SaveMap(const TextureEntryMap &Map, const String &Filename)
{
    ofstream File(Filename.CString());
    for(TextureEntryMap::const_iterator Iterator = Map.begin(); Iterator != Map.end(); Iterator++)
    {
        File << UINT(Iterator->second->Type) << '\t' << Iterator->second->ID << '\t' << Iterator->second->Filename << '\t' << Iterator->first << endl;
    }
}

//
// Loads the texture map from MapSaveString
//
void TextureEntryManager::LoadMap(TextureEntryMap &Map, set<UINT> &Set, const String &Filename)
{
    ifstream File(Filename.CString());
    Vector<String> Lines;
    Utility::GetFileLines(File, Lines);
    for(UINT i = 0; i < Lines.Length(); i++)
    {
        if(Lines[i].Length() > 3)
        {
            Vector<String> Parameters;
            Lines[i].Partition('\t', Parameters);
            Assert(Parameters.Length() == 4, "Output.Length() != 4");

            RenderType Type = RenderType(Parameters[0].ConvertToInteger());
            String ID = Parameters[1];
            String Filename = Parameters[2];
            UINT HashValue = Parameters[3].ConvertToUnsignedInteger();

            if(Utility::FileExists(g_Context->Parameters.TextureLoadDirectory + Filename + String(".png")))
            {
                Assert(Map.find(HashValue) == Map.end(), "Hash collision in TextureEntryManager::LoadMap");
                Map[HashValue] = new TextureEntry(Filename, ID, Type);
                Set.insert(Filename.Hash32());
            }
            else
            {
                g_Context->Files.Events << "Texture removed: " << Filename << endl;
            }
        }
    }
}

//
// Adds the bitmap at Directory + Filename to the texture hash
//
void TextureEntryManager::AddBitmapFromFile(const String &Directory, const String &Filename, RenderType Type, TextureEntryMap &Map, set<UINT> &Set)
{
    String TruncatedFilename = Filename;
    TruncatedFilename.ReSize(TruncatedFilename.Length() - 4);
    const String LocalDirectory = Directory.RemovePrefix(g_Context->Parameters.TextureLoadDirectory);
    
    const String FinalFilename = LocalDirectory + TruncatedFilename;
    UINT FilenameHash = FinalFilename.Hash32();
    if(Set.count(FilenameHash) > 0)
    {
        return;
    }
    Set.insert(FilenameHash);
    g_Context->Files.Events << "Adding texture: " << FinalFilename << endl;

    String ID = TruncatedFilename;
    String FullFilename = Directory + Filename;
    
    Bitmap Bmp;
    Bmp.LoadPNG(FullFilename);
    Bmp.FlipBlueAndRed();

    bool Exit = false;
    for(int i = ID.Length() - 1; !Exit && i >= 0; i--)
    {
        if(ID[i] >= '0' && ID[i] <= '9' && ID.Length() > 1)
        {
                ID.PopEnd();
        }
        else
        {
            Exit = true;
        }
    }
    ID = FontUpdateStringName(ID);
    InsertBitmapIntoMap(Bmp, FinalFilename, ID, Type, Map);
}

void TextureEntryManager::InsertBitmapIntoMap(const Bitmap &Bmp, const String &Filename, const String &ID, RenderType Type, TextureEntryMap &Map)
{
    UINT Hash = Bmp.Hash32();
    if(Map.find(Hash) == Map.end())
    {
        Map[Hash] = new TextureEntry(Filename, ID, Type);
    }
    else
    {
        if(Map[Hash]->ID == ID)
        {
            //g_Context->Files.Assert << "Harmless hash collision, " << ID << ' ' << FullFilename << endl;
        }
        else
        {
            g_Context->Files.Assert << "Add hash collision. Source: " << Map[Hash]->Filename << " New: " << Filename << endl;
        }
    }
}

//
// Adds all bitmaps in the given directory to the texture hash
//
void TextureEntryManager::AddDirectory(const String &Folder, RenderType Type, TextureEntryMap &Map, set<UINT> &Set)
{
    String DirectoryName = g_Context->Parameters.TextureLoadDirectory + Folder + String("\\");
    Directory Dir(DirectoryName);
    for(UINT FileIndex = 0; FileIndex < Dir.Files().Length(); FileIndex++)
    {
        const String &Filename = Dir.Files()[FileIndex];
        if(Filename.EndsWith(".png"))
        {
            AddBitmapFromFile(DirectoryName, Filename, Type, Map, Set);
        }
    }
    for(UINT DirectoryIndex = 0; DirectoryIndex < Dir.Directories().Length(); DirectoryIndex++)
    {
        AddDirectory(Folder + String("\\") + Dir.Directories()[DirectoryIndex], Type, Map, Set);
    }
}

//
// Loads all directories into the texture hash, or loads the texture hash from the texture map file
//
void TextureEntryManager::Init()
{
    if(g_Context->Parameters.VideoCaptureMode)
    {
        return;
    }

    String TextureMapFilename(g_Context->Parameters.DatabaseLoadDirectory + String("TextureMap.txt"));
    String FontMapFilename(g_Context->Parameters.DatabaseLoadDirectory + String("FontMap.txt"));

    LoadMap(_TextureMap, _TextureMapFilenameHashSet, TextureMapFilename);
    LoadMap(_FontMap, _FontMapFilenameHashSet, FontMapFilename);
    UINT StartSize = _TextureMap.size() + _FontMap.size();
    g_Context->Files.Events << String(StartSize) << " textures loaded from map file\n";
    if(g_Context->Parameters.LoadDataFromDirectories)
    {
        AddDirectory("Decoration", RenderDecoration, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Minimap", RenderMinimap, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Icon", RenderIcon, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("PortraitLarge", RenderPortraitLarge, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Portrait", RenderPortrait, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Unit", RenderUnit, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Special", RenderSpecial, _TextureMap, _TextureMapFilenameHashSet);
        AddDirectory("Unclassified", RenderUnclassified, _TextureMap, _TextureMapFilenameHashSet);
        
        AddDirectory("Font", RenderFont, _FontMap, _FontMapFilenameHashSet);
        
        g_Context->Files.Events << String(_TextureMap.size() + _FontMap.size() - StartSize) << " new textures loaded from disk\n";
        SaveMap(_TextureMap, TextureMapFilename);
        SaveMap(_FontMap, FontMapFilename);
    }
    else
    {
        
    }
}

const TextureEntry* TextureEntryManager::FindTextureMatch(const Bitmap &Bmp)
{
    return FindMapMatch(Bmp, _TextureMap);
}

const TextureEntry* TextureEntryManager::FindFontMatch(const Bitmap &Bmp)
{
    const TextureEntry* Result = FindMapMatch(Bmp, _FontMap);
    if(Result == NULL)
    {
        InsertBitmapIntoMap(Bmp, "*", "*", RenderNoMatchFound, _FontMap);
    }
    return Result;
}

//
// Searches the texture map for a texture with hash equal to the given bitmap, returning its ID and Type.
//
const TextureEntry* TextureEntryManager::FindMapMatch(const Bitmap &Bmp, const TextureEntryMap &Map)
{
    UINT Hash = Bmp.Hash32();
    TextureEntryMap::const_iterator Iterator = Map.find(Hash);
    if(Iterator == Map.end())
    {
        //g_Context->Files.Events << "Hash not found: " << Hash << endl;
        return NULL;
    }
    else
    {
        //g_Context->Files.Events << "Hash found: " << Hash << endl;
        return Iterator->second;
    }
}
