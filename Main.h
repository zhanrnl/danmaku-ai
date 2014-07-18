//
// Main.h
//
// Precompiled header for the SCII AI
// Written by Matthew Fisher
//

#pragma once

#define NOMINMAX

#include "Base/Config.h"
#include "Base/CompileOptions.h"

//
// Windows Header Files
//
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <hash_map>
#include <hash_set>

namespace D3D9Base
{
#include <d3d9.h>
#include <d3dx9.h>
}
using namespace D3D9Base;

#define ULONG ::ULONG
#define SHORT ::SHORT
#define UCHAR ::UCHAR

//
// Forward definitions
//
class AIThread;
class ManagerController;
class VShaderSimulator;

//
// basic objects (vector, matrix, bitmap, color, etc.)
//
#include "Base/Engine.h"

#include "Base/DisplayOptions.h"
#include "Base/AIParameters.h"
#include "Base/d3d9CallbackStructures.h"

//
// Objects
//
#include "Objects/HelperStructures.h"
#include "Objects/EnumString.h"
#include "Objects/EnumStringDirectX.h"
#include "Objects/Constants.h"
#include "Objects/HelperFunctions.h"
#include "Objects/Buffers.h"
#include "Objects/ShaderObjects.h"
#include "Objects/VertexShader.h"
#include "Objects/VertexShaderSimulator.h"

//
// Programs
//
//#include "Programs/Program.h"
//#include "Programs/ProgramCompound.h"
//#include "Programs/ProgramClick.h"
//#include "Programs/ProgramPressKey.h"
//#include "Programs/ProgramIdleWorker.h"
//#include "Programs/ProgramBaseSearch.h"
//#include "Programs/ProgramControlGroupCheck.h"
//#include "Programs/ProgramProduceUnitLarvae.h"
//#include "Programs/ProgramProduceUnitQueen.h"
//#include "Programs/ProgramProduceUnitQueue.h"
//#include "Programs/ProgramProduceUnitBase.h"
//#include "Programs/ProgramProduceUnitBuilding.h"
//#include "Programs/ProgramProduceUnitExtractor.h"
//#include "Programs/ProgramProduceUnitUpgrade.h"
//#include "Programs/ProgramZergBaseUpgrade.h"
//#include "Programs/ProgramOverlord.h"
//#include "Programs/ProgramGasWorker.h"
//#include "Programs/ProgramArmy.h"
//#include "Programs/ProgramQueen.h"
//#include "Programs/ProgramChronoBoost.h"
//#include "Programs/ProgramLargeControlGroupCheck.h"
//#include "Programs/ProgramMicroAll.h"

//
// Threads
//
//#include "Threads/AIThread.h"
//#include "Threads/SearchThread.h"
//#include "Threads/WorkerThread.h"
//#include "Threads/ProductionThread.h"
//#include "Threads/ControlGroupCheckThread.h"
//#include "Threads/QueenThread.h"
//#include "Threads/GasWorkerThread.h"
//#include "Threads/ArmyThread.h"
//#include "Threads/OverlordThread.h"
//#include "Threads/ChronoBoostThread.h"
//#include "Threads/ArmyCheckThread.h"
//#include "Threads/MicroThread.h"

//
// Managers
//
#include "Managers/Manager.h"
#include "Managers/StateManager.h"
#include "Managers/RenderManager.h"
//#include "Managers/KnowledgeManager.h"
//#include "Managers/DatabaseManager.h"
#include "Managers/TextureEntryManager.h"
//#include "Managers/FrameUnitManager.h"
//#include "Managers/FrameHUDManager.h"
//#include "Managers/MinimapManager.h"
#include "Managers/KeyboardMouseManager.h"
//#include "Managers/ThoughtManager.h"
//#include "Managers/ControlGroupManager.h"
#include "Managers/BulletManager.h"
#include "Managers/PowerupManager.h"
#include "Managers/CharacterManager.h"
#include "Managers/PlayerManager.h"

#include "Base/ManagerController.h"
#include "Base/GlobalContext.h"
#include "Objects/CapturedGeometry.h"
