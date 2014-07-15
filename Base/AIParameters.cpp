//
// AIParameters.cpp
//
// All parameters used in the AI, loaded from a parameter file
//

#include "Main.h"

String FixDirectoryString(const String &S)
{
    if(S.Last() != '\\' && S.Last() != '/')
    {
        return S + String('\\');
    }
    else
    {
        return S;
    }
}

void AIParameters::LoadFromFile(const String &Filename)
{
    ParameterFile Parameters(Filename.CString());
    
    BaseCaptureDirectory = FixDirectoryString(Parameters.GetRequiredString("BaseCaptureDirectory"));
    BaseDataDirectory = FixDirectoryString(Parameters.GetRequiredString("BaseDataDirectory"));

    ShaderLoadDirectory = BaseDataDirectory + Parameters.GetRequiredString("ShaderLoadDirectory") + String("\\");
    TextureLoadDirectory = BaseDataDirectory + Parameters.GetRequiredString("TextureLoadDirectory") + String("\\");
    DatabaseLoadDirectory = BaseDataDirectory + Parameters.GetRequiredString("DatabaseLoadDirectory") + String("\\");
    
    OutputFileDirectory = BaseCaptureDirectory + Parameters.GetRequiredString("OutputFileDirectory") + String("\\");
    ScreenCaptureDirectory = BaseCaptureDirectory + Parameters.GetRequiredString("ScreenCaptureDirectory") + String("\\");    
    TextureCaptureDirectory = BaseCaptureDirectory + Parameters.GetRequiredString("TextureCaptureDirectory") + String("\\");    
    LetterCaptureDirectory = BaseCaptureDirectory + Parameters.GetRequiredString("LetterCaptureDirectory") + String("\\");    
    ShaderCaptureDirectory = BaseCaptureDirectory + Parameters.GetRequiredString("ShaderCaptureDirectory") + String("\\");
    
    FinalShaderCodeDirectory = Parameters.GetRequiredString("FinalShaderCodeDirectory") + String("\\");

    LoadDataFromDirectories = Parameters.GetBoolean("LoadDataFromDirectories");
    DebugTextureCreation = Parameters.GetBoolean("DebugTextureCreation");
    SaveAllTextures = Parameters.GetBoolean("SaveAllTextures");
    DiscardAllBitmaps = Parameters.GetBoolean("DiscardAllBitmaps");
    OutputCSource = Parameters.GetBoolean("OutputCSource");
    VideoCaptureMode = Parameters.GetBoolean("VideoCaptureMode");
    AllowWindowResizing = Parameters.GetBoolean("AllowWindowResizing");
    IgnoreAllFonts = Parameters.GetBoolean("IgnoreAllFonts");
}
