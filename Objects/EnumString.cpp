#include "Main.h"

String GetActionButtonStateTypeString(ActionButtonStateType Type)
{
    switch(Type)
    {
        case ActionButtonStateInvalid:
            return "Invalid";
        case ActionButtonStateNormal:
            return "Normal";
        case ActionButtonStateDisabled:
            return "Disabled";
        case ActionButtonStateSelected:
            return "Selected";
        case ActionButtonStateNotEnoughEnergy:
            return "NotEnoughEnergy";
    }
    return "Invalid ActionButtonStateType";
}

String GetKeyboardMouseEventTypeString(KeyboardMouseEventType Type)
{
    switch(Type)
    {
        case KeyboardMouseEventKeyDown:
            return "KeyDown";
        case KeyboardMouseEventKeyPress:
            return "KeyPress";
        case KeyboardMouseEventKeyUp:
            return "KeyUp";
        case KeyboardMouseEventFrameBarrier:
            return "FrameBarrier";
        case KeyboardMouseEventMouseMove:
            return "MouseMove";
        case KeyboardMouseEventMouseDown:
            return "MouseDown";
        case KeyboardMouseEventMouseUp:
            return "MouseUp";
        case KeyboardMouseEventMouseDownUp:
            return "MouseDownUp";
    }
    return "Invalid KeyboardMouseEventType";
}

String GetStratTypeString(StratType Type)
{
    switch(Type)
    {
        case StratZergDebug:
            return "ZergDebug";
        case StratZergRoaches:
            return "ZergRoaches";
        case StratZergHydralisks:
            return "ZergHydralisks";
        case StratZergMutalisks:
            return "ZergMutalisks";
        case StratZergZerglings:
            return "ZergZerglings";
        case StratZergRoachesAndHydralisks:
            return "ZergRoachesAndHydralisks";
        case StratProtossZealots:
            return "ProtossZealots";
        case StratProtossStalkers:
            return "ProtossStalkers";
    }
    return "Invalid StratType";
}

String GetProgramResultTypeString(ProgramResultType Type)
{
    switch(Type)
    {
        case ProgramResultSuccess:
            return "Success";
        case ProgramResultFail:
            return "Fail";
        case ProgramResultStillExecuting:
            return "StillExecuting";
    }
    return "Invalid ProgramResultType";
}

String GetLogTypeString(LogType Type)
{
    switch(Type)
    {
        case LogExecuteStep:
            return "ExecuteStep";
        case LogStep:
            return "Step";
        case LogError:
            return "Error";
        case LogDone:
            return "Done";
    }
    return "Invalid LogType";
}

String GetRenderTypeString(RenderType Type)
{
    switch(Type)
    {
        case RenderDecoration:
            return "Decoration";
        case RenderMinimap:
            return "Minimap";
        case RenderIcon:
            return "Icon";
        case RenderPortraitLarge:
            return "PortraitLarge";
        case RenderPortrait:
            return "Portrait";
        case RenderUnit:
            return "Unit";
        case RenderFont:
            return "Font";
        case RenderUnclassified:
            return "Unclassified";
        case RenderSpecial:
            return "Special";
        case RenderNoMatchFound:
            return "NoMatchFound";
        case RenderUnknown:
            return "Unknown";
        case RenderUninitialized:
            return "Uninitialized";
    }
    return "Invalid RenderType";
}

String GetPlayerTypeString(PlayerType Type)
{
    switch(Type)
    {
        case PlayerSelf:
            return "Self";
        case PlayerAlly:
            return "Ally";
        case PlayerEnemy:
            return "Enemy";
        case PlayerInvalid:
            return "Invalid";
    }
    return "Invalid PlayerType";
}

String GetRaceTypeString(RaceType Type)
{
    switch(Type)
    {
        case RaceZerg:
            return "Zerg";
        case RaceProtoss:
            return "Protoss";
        case RaceTerran:
            return "Terran";
        case RaceNone:
            return "None";
    }
    return "Invalid RaceType";
}

String GetUnitPrimaryTypeString(UnitPrimaryType Type)
{
    switch(Type)
    {
        case UnitPrimaryMobile:
            return "Mobile";
        case UnitPrimaryBuilding:
            return "Building";
        case UnitPrimaryResearch:
            return "Research";
        case UnitPrimaryResource:
            return "Resource";
    }
    return "Invalid UnitPrimaryType";
}

String GetUnitSecondaryTypeString(UnitSecondaryType Type)
{
    switch(Type)
    {
        case UnitSecondaryNone:
            return "None";
        case UnitSecondaryCombat:
            return "Combat";
        case UnitSecondaryCombatMicro:
            return "CombatMicro";
        case UnitSecondaryBase:
            return "Base";
        case UnitSecondarySupply:
            return "Supply";
        case UnitSecondaryWorker:
            return "Worker";
        case UnitSecondaryExtractor:
            return "Extractor";
    }
    return "Invalid UnitSecondaryType";
}

String GetBuildClassTypeString(BuildClassType Type)
{
    switch(Type)
    {
        case BuildClassMorphUnit:
            return "MorphUnit";
        case BuildClassWorker:
            return "Worker";
        case BuildClassSpell:
            return "Spell";
        case BuildClassLarvae:
            return "Larvae";
        case BuildClassMorphBuilding:
            return "MorphBuilding";
        case BuildClassQueue:
            return "Queue";
        case BuildClassResearch:
            return "Research";
        case BuildClassNone:
            return "None";
    }
    return "Invalid BuildClassType";
}

String GetComplexityTypeString(ComplexityType Type)
{
    switch(Type)
    {
        case ComplexityNone:
            return "None";
        case ComplexityBasic:
            return "Basic";
        case ComplexityAdvanced:
            return "Advanced";
    }
    return "Invalid ComplexityType";
}

String GetScreenConstantVec2fTypeString(ScreenConstantVec2fType Type)
{
    switch(Type)
    {
        case ScreenConstantVec2fBubblePortraitTopLeft:
            return "BubblePortraitTopLeft";
        case ScreenConstantVec2fBubblePortraitDimensions:
            return "BubblePortraitDimensions";
        case ScreenConstantVec2fActionButtonTopLeft:
            return "ActionButtonTopLeft";
        case ScreenConstantVec2fActionButtonDimensions:
            return "ActionButtonDimensions";
        case ScreenConstantVec2fMinimapTopLeft:
            return "MinimapTopLeft";
        case ScreenConstantVec2fMinimapDimensions:
            return "MinimapDimensions";
        case ScreenConstantVec2fCount:
            return "Count";
    }
    return "Invalid ScreenConstantVec2fType";
}

String GetScreenConstantRectangle2iTypeString(ScreenConstantRectangle2iType Type)
{
    switch(Type)
    {
        case ScreenConstantRectangle2iClickableRegion:
            return "ClickableRegion";
        case ScreenConstantRectangle2iIdleWorkerRegion:
            return "IdleWorkerRegion";
        case ScreenConstantRectangle2iMenuRegion:
            return "MenuRegion";
        case ScreenConstantRectangle2iMinimapRegion:
            return "MinimapRegion";
        case ScreenConstantRectangle2iUnitInfoRegion:
            return "UnitInfoRegion";
        case ScreenConstantRectangle2iBaseSearchRegion:
            return "BaseSearchRegion";
        case ScreenConstantRectangle2iCount:
            return "Count";
    }
    return "Invalid ScreenConstantRectangle2iType";
}

String GetControlGroupTypeString(ControlGroupType Type)
{
    switch(Type)
    {
        case ControlGroupCombatAll:
            return "CombatAll";
        case ControlGroupCombatMicro:
            return "CombatMicro";
        case ControlGroupWorkerMineral:
            return "WorkerMineral";
        case ControlGroupWorkerGas0:
            return "WorkerGas0";
        case ControlGroupWorkerGas1:
            return "WorkerGas1";
        case ControlGroupWorkerGas2:
            return "WorkerGas2";
        case ControlGroupWorkerGas3:
            return "WorkerGas3";
        case ControlGroupBuildingMain:
            return "BuildingMain";
        case ControlGroupBuildingSupport:
            return "BuildingSupportAndQueens";
        case ControlGroupSupply:
            return "Supply";
        case ControlGroupUnused:
            return "Unused";
        case ControlGroupUngrouped:
            return "Ungrouped";
    }
    return "Invalid ControlGroupType";
}

String GetModifierTypeString(ModifierType Type)
{
    switch(Type)
    {
        case ModifierNone:
            return "None";
        case ModifierShift:
            return "Shift";
        case ModifierCtrl:
            return "Ctrl";
    }
    return "Invalid ModifierType";
}


ActionButtonStateType GetActionButtonStateTypeFromString(const String &Str)
{
    if(Str == "ActionButtonStateInvalid")
    {
        return ActionButtonStateInvalid;
    }
    else if(Str == "ActionButtonStateNormal")
    {
        return ActionButtonStateNormal;
    }
    else if(Str == "ActionButtonStateDisabled")
    {
        return ActionButtonStateDisabled;
    }
    else if(Str == "ActionButtonStateSelected")
    {
        return ActionButtonStateSelected;
    }
    else if(Str == "ActionButtonStateNotEnoughEnergy")
    {
        return ActionButtonStateNotEnoughEnergy;
    }
    else
    {
        SignalError("Invalid ActionButtonStateType");
        return ActionButtonStateType(0);
    }
}

KeyboardMouseEventType GetKeyboardMouseEventTypeFromString(const String &Str)
{
    if(Str == "KeyboardMouseEventKeyDown")
    {
        return KeyboardMouseEventKeyDown;
    }
    else if(Str == "KeyboardMouseEventKeyPress")
    {
        return KeyboardMouseEventKeyPress;
    }
    else if(Str == "KeyboardMouseEventKeyUp")
    {
        return KeyboardMouseEventKeyUp;
    }
    else if(Str == "KeyboardMouseEventFrameBarrier")
    {
        return KeyboardMouseEventFrameBarrier;
    }
    else if(Str == "KeyboardMouseEventMouseMove")
    {
        return KeyboardMouseEventMouseMove;
    }
    else if(Str == "KeyboardMouseEventMouseDown")
    {
        return KeyboardMouseEventMouseDown;
    }
    else if(Str == "KeyboardMouseEventMouseUp")
    {
        return KeyboardMouseEventMouseUp;
    }
    else if(Str == "KeyboardMouseEventMouseDownUp")
    {
        return KeyboardMouseEventMouseDownUp;
    }
    else
    {
        SignalError("Invalid KeyboardMouseEventType");
        return KeyboardMouseEventType(0);
    }
}

StratType GetStratTypeFromString(const String &Str)
{
    if(Str == "StratZergDebug")
    {
        return StratZergDebug;
    }
    else if(Str == "StratZergRoaches")
    {
        return StratZergRoaches;
    }
    else if(Str == "StratZergHydralisks")
    {
        return StratZergHydralisks;
    }
    else if(Str == "StratZergMutalisks")
    {
        return StratZergMutalisks;
    }
    else if(Str == "StratZergZerglings")
    {
        return StratZergZerglings;
    }
    else if(Str == "StratZergRoachesAndHydralisks")
    {
        return StratZergRoachesAndHydralisks;
    }
    else if(Str == "StratProtossZealots")
    {
        return StratProtossZealots;
    }
    else if(Str == "StratProtossStalkers")
    {
        return StratProtossStalkers;
    }
    else
    {
        SignalError("Invalid StratType");
        return StratType(0);
    }
}

ProgramResultType GetProgramResultTypeFromString(const String &Str)
{
    if(Str == "ProgramResultSuccess")
    {
        return ProgramResultSuccess;
    }
    else if(Str == "ProgramResultFail")
    {
        return ProgramResultFail;
    }
    else if(Str == "ProgramResultStillExecuting")
    {
        return ProgramResultStillExecuting;
    }
    else
    {
        SignalError("Invalid ProgramResultType");
        return ProgramResultType(0);
    }
}

LogType GetLogTypeFromString(const String &Str)
{
    if(Str == "LogExecuteStep")
    {
        return LogExecuteStep;
    }
    else if(Str == "LogStep")
    {
        return LogStep;
    }
    else if(Str == "LogError")
    {
        return LogError;
    }
    else if(Str == "LogDone")
    {
        return LogDone;
    }
    else
    {
        SignalError("Invalid LogType");
        return LogType(0);
    }
}

RenderType GetRenderTypeFromString(const String &Str)
{
    if(Str == "RenderDecoration")
    {
        return RenderDecoration;
    }
    else if(Str == "RenderMinimap")
    {
        return RenderMinimap;
    }
    else if(Str == "RenderIcon")
    {
        return RenderIcon;
    }
    else if(Str == "RenderPortraitLarge")
    {
        return RenderPortraitLarge;
    }
    else if(Str == "RenderPortrait")
    {
        return RenderPortrait;
    }
    else if(Str == "RenderUnit")
    {
        return RenderUnit;
    }
    else if(Str == "RenderFont")
    {
        return RenderFont;
    }
    else if(Str == "RenderUnclassified")
    {
        return RenderUnclassified;
    }
    else if(Str == "RenderSpecial")
    {
        return RenderSpecial;
    }
    else if(Str == "RenderNoMatchFound")
    {
        return RenderNoMatchFound;
    }
    else if(Str == "RenderUnknown")
    {
        return RenderUnknown;
    }
    else if(Str == "RenderUninitialized")
    {
        return RenderUninitialized;
    }
    else
    {
        SignalError("Invalid RenderType");
        return RenderType(0);
    }
}

PlayerType GetPlayerTypeFromString(const String &Str)
{
    if(Str == "PlayerSelf")
    {
        return PlayerSelf;
    }
    else if(Str == "PlayerAlly")
    {
        return PlayerAlly;
    }
    else if(Str == "PlayerEnemy")
    {
        return PlayerEnemy;
    }
    else if(Str == "PlayerInvalid")
    {
        return PlayerInvalid;
    }
    else
    {
        SignalError("Invalid PlayerType");
        return PlayerType(0);
    }
}

RaceType GetRaceTypeFromString(const String &Str)
{
    if(Str == "RaceZerg")
    {
        return RaceZerg;
    }
    else if(Str == "RaceProtoss")
    {
        return RaceProtoss;
    }
    else if(Str == "RaceTerran")
    {
        return RaceTerran;
    }
    else if(Str == "RaceNone")
    {
        return RaceNone;
    }
    else
    {
        SignalError("Invalid RaceType");
        return RaceType(0);
    }
}

UnitPrimaryType GetUnitPrimaryTypeFromString(const String &Str)
{
    if(Str == "UnitPrimaryMobile")
    {
        return UnitPrimaryMobile;
    }
    else if(Str == "UnitPrimaryBuilding")
    {
        return UnitPrimaryBuilding;
    }
    else if(Str == "UnitPrimaryResearch")
    {
        return UnitPrimaryResearch;
    }
    else if(Str == "UnitPrimaryResource")
    {
        return UnitPrimaryResource;
    }
    else
    {
        SignalError("Invalid UnitPrimaryType");
        return UnitPrimaryType(0);
    }
}

UnitSecondaryType GetUnitSecondaryTypeFromString(const String &Str)
{
    if(Str == "UnitSecondaryNone")
    {
        return UnitSecondaryNone;
    }
    else if(Str == "UnitSecondaryCombat")
    {
        return UnitSecondaryCombat;
    }
    else if(Str == "UnitSecondaryCombatMicro")
    {
        return UnitSecondaryCombatMicro;
    }
    else if(Str == "UnitSecondaryBase")
    {
        return UnitSecondaryBase;
    }
    else if(Str == "UnitSecondarySupply")
    {
        return UnitSecondarySupply;
    }
    else if(Str == "UnitSecondaryWorker")
    {
        return UnitSecondaryWorker;
    }
    else if(Str == "UnitSecondaryExtractor")
    {
        return UnitSecondaryExtractor;
    }
    else
    {
        SignalError("Invalid UnitSecondaryType");
        return UnitSecondaryType(0);
    }
}

BuildClassType GetBuildClassTypeFromString(const String &Str)
{
    if(Str == "BuildClassMorphUnit")
    {
        return BuildClassMorphUnit;
    }
    else if(Str == "BuildClassWorker")
    {
        return BuildClassWorker;
    }
    else if(Str == "BuildClassSpell")
    {
        return BuildClassSpell;
    }
    else if(Str == "BuildClassLarvae")
    {
        return BuildClassLarvae;
    }
    else if(Str == "BuildClassMorphBuilding")
    {
        return BuildClassMorphBuilding;
    }
    else if(Str == "BuildClassQueue")
    {
        return BuildClassQueue;
    }
    else if(Str == "BuildClassResearch")
    {
        return BuildClassResearch;
    }
    else if(Str == "BuildClassNone")
    {
        return BuildClassNone;
    }
    else
    {
        SignalError("Invalid BuildClassType");
        return BuildClassType(0);
    }
}

ComplexityType GetComplexityTypeFromString(const String &Str)
{
    if(Str == "ComplexityNone")
    {
        return ComplexityNone;
    }
    else if(Str == "ComplexityBasic")
    {
        return ComplexityBasic;
    }
    else if(Str == "ComplexityAdvanced")
    {
        return ComplexityAdvanced;
    }
    else
    {
        SignalError("Invalid ComplexityType");
        return ComplexityType(0);
    }
}

ScreenConstantVec2fType GetScreenConstantVec2fTypeFromString(const String &Str)
{
    if(Str == "ScreenConstantVec2fBubblePortraitTopLeft")
    {
        return ScreenConstantVec2fBubblePortraitTopLeft;
    }
    else if(Str == "ScreenConstantVec2fBubblePortraitDimensions")
    {
        return ScreenConstantVec2fBubblePortraitDimensions;
    }
    else if(Str == "ScreenConstantVec2fActionButtonTopLeft")
    {
        return ScreenConstantVec2fActionButtonTopLeft;
    }
    else if(Str == "ScreenConstantVec2fActionButtonDimensions")
    {
        return ScreenConstantVec2fActionButtonDimensions;
    }
    else if(Str == "ScreenConstantVec2fMinimapTopLeft")
    {
        return ScreenConstantVec2fMinimapTopLeft;
    }
    else if(Str == "ScreenConstantVec2fMinimapDimensions")
    {
        return ScreenConstantVec2fMinimapDimensions;
    }
    else if(Str == "ScreenConstantVec2fCount")
    {
        return ScreenConstantVec2fCount;
    }
    else
    {
        SignalError("Invalid ScreenConstantVec2fType");
        return ScreenConstantVec2fType(0);
    }
}

ScreenConstantRectangle2iType GetScreenConstantRectangle2iTypeFromString(const String &Str)
{
    if(Str == "ScreenConstantRectangle2iClickableRegion")
    {
        return ScreenConstantRectangle2iClickableRegion;
    }
    else if(Str == "ScreenConstantRectangle2iIdleWorkerRegion")
    {
        return ScreenConstantRectangle2iIdleWorkerRegion;
    }
    else if(Str == "ScreenConstantRectangle2iMenuRegion")
    {
        return ScreenConstantRectangle2iMenuRegion;
    }
    else if(Str == "ScreenConstantRectangle2iMinimapRegion")
    {
        return ScreenConstantRectangle2iMinimapRegion;
    }
    else if(Str == "ScreenConstantRectangle2iUnitInfoRegion")
    {
        return ScreenConstantRectangle2iUnitInfoRegion;
    }
    else if(Str == "ScreenConstantRectangle2iBaseSearchRegion")
    {
        return ScreenConstantRectangle2iBaseSearchRegion;
    }
    else if(Str == "ScreenConstantRectangle2iCount")
    {
        return ScreenConstantRectangle2iCount;
    }
    else
    {
        SignalError("Invalid ScreenConstantRectangle2iType");
        return ScreenConstantRectangle2iType(0);
    }
}

ControlGroupType GetControlGroupTypeFromString(const String &Str)
{
    if(Str == "ControlGroupCombatAll")
    {
        return ControlGroupCombatAll;
    }
    else if(Str == "ControlGroupCombatMicro")
    {
        return ControlGroupCombatMicro;
    }
    else if(Str == "ControlGroupWorkerMineral")
    {
        return ControlGroupWorkerMineral;
    }
    else if(Str == "ControlGroupWorkerGas0")
    {
        return ControlGroupWorkerGas0;
    }
    else if(Str == "ControlGroupWorkerGas1")
    {
        return ControlGroupWorkerGas1;
    }
    else if(Str == "ControlGroupWorkerGas2")
    {
        return ControlGroupWorkerGas2;
    }
    else if(Str == "ControlGroupWorkerGas3")
    {
        return ControlGroupWorkerGas3;
    }
    else if(Str == "ControlGroupBuildingMain")
    {
        return ControlGroupBuildingMain;
    }
    else if(Str == "ControlGroupBuildingSupport")
    {
        return ControlGroupBuildingSupport;
    }
    else if(Str == "ControlGroupSupply")
    {
        return ControlGroupSupply;
    }
    else if(Str == "ControlGroupUnused")
    {
        return ControlGroupUnused;
    }
    else if(Str == "ControlGroupUngrouped")
    {
        return ControlGroupUngrouped;
    }
    else
    {
        SignalError("Invalid ControlGroupType");
        return ControlGroupType(0);
    }
}

ModifierType GetModifierTypeFromString(const String &Str)
{
    if(Str == "ModifierNone")
    {
        return ModifierNone;
    }
    else if(Str == "ModifierShift")
    {
        return ModifierShift;
    }
    else if(Str == "ModifierCtrl")
    {
        return ModifierCtrl;
    }
    else
    {
        SignalError("Invalid ModifierType");
        return ModifierType(0);
    }
}


