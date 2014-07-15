//
// EnumString.h
//
// Enumerations used throughout the AI and functions
// for converting them into strings
//

enum ArmyStateType
{
    ArmyStateIdleInBase,
    ArmyStateMovingToDefend,
    ArmyStateMovingToAttack,
    ArmyStateRetreating,
    ArmyStateGathering,
    ArmyStateInBattle,
};

enum ActionButtonStateType
{
    ActionButtonStateInvalid,
    ActionButtonStateNormal,
    ActionButtonStateDisabled,
    ActionButtonStateSelected,
    ActionButtonStateNotEnoughEnergy,
    ActionButtonCoolingDown,
};

enum KeyboardMouseEventType
{
    KeyboardMouseEventKeyDown,
    KeyboardMouseEventKeyPress,
    KeyboardMouseEventKeyUp,
    KeyboardMouseEventFrameBarrier,
    KeyboardMouseEventMouseMove,
    KeyboardMouseEventMouseDown,
    KeyboardMouseEventMouseUp,
    KeyboardMouseEventMouseDownUp,
};

enum StratType
{
    StratZergDebug,
    StratZergRoaches,
    StratZergHydralisks,
    StratZergMutalisks,
    StratZergZerglings,
    StratZergRoachesAndHydralisks,
    StratProtossZealots,
	StratProtossStalkers,
    StratProtossGroundHybrid,
};

enum ProgramResultType
{
    ProgramResultSuccess,
    ProgramResultFail,
    ProgramResultStillExecuting
};

enum LogType
{
    LogExecuteStep,
    LogStep,
    LogError,
    LogDone,
};

enum RenderType
{
    RenderDecoration,
    RenderMinimap,
    RenderIcon,
    RenderPortraitLarge,
    RenderPortrait,
    RenderUnit,
    RenderFont,
    RenderUnclassified,
    RenderSpecial,
    RenderNoMatchFound,
    RenderUnknown,
    RenderUninitialized,
};

enum PlayerType
{
    PlayerSelf,
    PlayerAlly,
    PlayerEnemy,
    PlayerInvalid,
};

enum RaceType
{
    RaceZerg,
    RaceProtoss,
    RaceTerran,
    RaceNone,
};

enum UnitPrimaryType
{
    UnitPrimaryMobile,
    UnitPrimaryBuilding,
    UnitPrimaryResearch,
    UnitPrimaryResource,
};

enum UnitSecondaryType
{
    UnitSecondaryNone,
    UnitSecondaryCombat,
    UnitSecondaryCombatMicro,
    UnitSecondaryBase,
    UnitSecondarySupply,
    UnitSecondaryWorker,
    UnitSecondaryExtractor,
};

enum BuildClassType
{
    BuildClassMorphUnit,
    BuildClassWorker,
    BuildClassSpell,
    BuildClassLarvae,
    BuildClassMorphBuilding,
    BuildClassQueue,
    BuildClassResearch,
    BuildClassNone,
};

enum ComplexityType
{
    ComplexityNone,
    ComplexityBasic,
    ComplexityAdvanced,
};

enum ScreenConstantVec2fType
{
    ScreenConstantVec2fBubblePortraitTopLeft,
    ScreenConstantVec2fBubblePortraitDimensions,
    ScreenConstantVec2fActionButtonTopLeft,
    ScreenConstantVec2fActionButtonDimensions,
    ScreenConstantVec2fMinimapTopLeft,
    ScreenConstantVec2fMinimapDimensions,
    ScreenConstantVec2fControlGroupLevelTopLeft,
    ScreenConstantVec2fControlGroupLevelDisplacement,
    ScreenConstantVec2fScreenCenter,
    ScreenConstantVec2fCount,
};

enum ScreenConstantRectangle2iType
{
    ScreenConstantRectangle2iClickableRegion,
    ScreenConstantRectangle2iIdleWorkerRegion,
    ScreenConstantRectangle2iMenuRegion,
    ScreenConstantRectangle2iMinimapRegion,
	ScreenConstantRectangle2iResourceRegion,
    ScreenConstantRectangle2iUnitInfoRegion,
    ScreenConstantRectangle2iBaseSearchRegion,
    ScreenConstantRectangle2iWarpGateRegion,
    ScreenConstantRectangle2iCount,
};

enum ControlGroupType
{
    ControlGroupCombatAll,
    ControlGroupCombatMicro,
    ControlGroupWorkerMineral,
    ControlGroupWorkerGas0,
    ControlGroupWorkerGas1,
    ControlGroupWorkerGas2,
    ControlGroupWorkerGas3,
    ControlGroupBuildingMain,
    ControlGroupBuildingSupport,
    ControlGroupSupply,
    ControlGroupUnused,
    ControlGroupUngrouped,
};

enum ModifierType
{
    ModifierNone,
    ModifierShift,
    ModifierCtrl,
};

String GetActionButtonStateTypeString(ActionButtonStateType Type);
ActionButtonStateType GetActionButtonStateTypeFromString(const String &Str);
String GetKeyboardMouseEventTypeString(KeyboardMouseEventType Type);
KeyboardMouseEventType GetKeyboardMouseEventTypeFromString(const String &Str);
String GetStratTypeString(StratType Type);
StratType GetStratTypeFromString(const String &Str);
String GetProgramResultTypeString(ProgramResultType Type);
ProgramResultType GetProgramResultTypeFromString(const String &Str);
String GetLogTypeString(LogType Type);
LogType GetLogTypeFromString(const String &Str);
String GetRenderTypeString(RenderType Type);
RenderType GetRenderTypeFromString(const String &Str);
String GetPlayerTypeString(PlayerType Type);
PlayerType GetPlayerTypeFromString(const String &Str);
String GetRaceTypeString(RaceType Type);
RaceType GetRaceTypeFromString(const String &Str);
String GetUnitPrimaryTypeString(UnitPrimaryType Type);
UnitPrimaryType GetUnitPrimaryTypeFromString(const String &Str);
String GetUnitSecondaryTypeString(UnitSecondaryType Type);
UnitSecondaryType GetUnitSecondaryTypeFromString(const String &Str);
String GetBuildClassTypeString(BuildClassType Type);
BuildClassType GetBuildClassTypeFromString(const String &Str);
String GetComplexityTypeString(ComplexityType Type);
ComplexityType GetComplexityTypeFromString(const String &Str);
String GetScreenConstantVec2fTypeString(ScreenConstantVec2fType Type);
ScreenConstantVec2fType GetScreenConstantVec2fTypeFromString(const String &Str);
String GetScreenConstantRectangle2iTypeString(ScreenConstantRectangle2iType Type);
ScreenConstantRectangle2iType GetScreenConstantRectangle2iTypeFromString(const String &Str);
String GetControlGroupTypeString(ControlGroupType Type);
ControlGroupType GetControlGroupTypeFromString(const String &Str);
String GetModifierTypeString(ModifierType Type);
ModifierType GetModifierTypeFromString(const String &Str);
