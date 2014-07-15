//
// AIParameters.h
//
// All parameters used in the AI, loaded from a parameter file
//

struct AIParameters
{
    void LoadFromFile(const String &Filename);

    //
    // Strings
    //
    String BaseCaptureDirectory;
    String BaseDataDirectory;

    String OutputFileDirectory;
    
    String ShaderLoadDirectory;
    String TextureLoadDirectory;
    String DatabaseLoadDirectory;
    
    String ShaderCaptureDirectory;
    String ScreenCaptureDirectory;
    String TextureCaptureDirectory;
    String LetterCaptureDirectory;
    
    String FinalShaderCodeDirectory;

    //
    // Booleans
    //
    bool DebugTextureCreation;

    bool LoadDataFromDirectories;
    bool SaveAllTextures;
    bool DiscardAllBitmaps;

    bool OutputCSource;
    bool VideoCaptureMode;

    bool AllowWindowResizing;
    bool IgnoreAllFonts;
};
