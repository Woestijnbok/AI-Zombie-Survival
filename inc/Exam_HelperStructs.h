#pragma once
#pragma region MISC
#include <string>

struct SteeringPlugin_Output
{
	Elite::Vector2 LinearVelocity{ 0.0f, 0.0f };
	float AngularVelocity{ 0.0f };
	bool AutoOrient = true;
	bool RunMode = false;
};

struct PluginInfo
{
	std::string BotName = "NoName";
	std::string Student_Name = "John Cena";
	std::string Student_Class = "2DAE00";
	std::string LB_Password = "ABC123";
};

struct GameDebugParams //Debuggin Purposes only (Ignored during release build)
{
	GameDebugParams(bool spawnEnemies = true, int enemyCount = 20, bool godMode = false, bool autoFollowCam = false)
	{
		SpawnEnemies = spawnEnemies;
		GodMode = godMode;
		AutoFollowCam = autoFollowCam;
		EnemyCount = enemyCount;
	}

	bool SpawnEnemies = true;
	int EnemyCount = 20;
	int ItemCount = 40;
	bool GodMode = false;
	bool IgnoreEnergy = false;									//Ignore energy depletion
	bool AutoFollowCam = false;									
	bool RenderUI = false;
	bool AutoGrabClosestItem = false;
	std::string LevelFile = "GameLevel.gppl";					
	int Seed = -1;												
	int StartingDifficultyStage = 0;
	bool InfiniteStamina = false;
	bool SpawnDebugPistol = false;								// Spawns pistol with 1000 ammo at start
	bool SpawnDebugShotgun = false;								// Spawns shotgun with 1000 ammo at start
	bool SpawnPurgeZonesOnMiddleClick = false;
	bool SpawnZombieOnRightClick = false;
	bool PrintDebugMessages = true;
	bool ShowDebugItemNames = true;
};
#pragma endregion

#pragma region ENTITIES
//Enumerations
//************
enum class eEntityType
{
	ITEM,
	ENEMY,
	PURGEZONE,

	//@END
	_LAST = PURGEZONE
};

enum class eItemType
{
	PISTOL,
	SHOTGUN,
	MEDKIT,
	FOOD,
	GARBAGE,
	RANDOM_DROP, //Internal Only
	RANDOM_DROP_WITH_CHANCE, //Internal Only

	//@END
	_LAST = GARBAGE
};

enum class eEnemyType
{
	DEFAULT,
	ZOMBIE_NORMAL,
	ZOMBIE_RUNNER,
	ZOMBIE_HEAVY,
	RANDOM_ENEMY, //Internal Only
	//...

	//@END
	_LAST = ZOMBIE_HEAVY
};


//Structures
//**********
struct StatisticsInfo
{
	int Score; //Current Score
	float Difficulty; //Current difficulty (0 > 1 > ... / Easy > Normal > Hard)
	float TimeSurvived; //Total time survived
	float KillCountdown; //Make sure to make a kill before this timer runs out

	int NumEnemiesKilled; //Amount of enemies killed
	int NumEnemiesHit; //Amount of enemy hits
	int NumItemsPickUp; //Amount of items picked up
	int NumMissedShots; //Shots missed after firing
	int NumChkpntsReached; //Amount of checkpoints reached

};

struct FOVStats
{
	int NumHouses;
	int NumEnemies;
	int NumItems;
	int NumPurgeZones;
};

struct HouseInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Size;

	bool operator==(const HouseInfo& other) const
	{
		return Center == other.Center;
	}
};

struct EnemyInfo
{
	eEnemyType Type;
	Elite::Vector2 Location;
	Elite::Vector2 LinearVelocity;

	int EnemyHash = 0;
	float Size;
	float Health;
};

struct ItemInfo
{
	eItemType Type;
	Elite::Vector2 Location;

	int ItemHash = 0;
	int Value = 0;

	bool operator==(const ItemInfo& other) const
	{
		return ItemHash == other.ItemHash;
	}
};

struct PurgeZoneInfo
{
	Elite::Vector2 Center;
	float Radius = 0.0f;

	int ZoneHash = 0;
};

struct EntityInfo
{
	eEntityType Type;
	Elite::Vector2 Location;

	int EntityHash = 0;
};

struct WorldInfo
{
	Elite::Vector2 Center;
	Elite::Vector2 Dimensions;
};

struct AgentInfo
{
	float Stamina;
	float Health;
	float Energy;
	bool RunMode;
	bool IsInHouse;
	bool Bitten;		// agent was bitten by a zombie this frame (for internal use)
	bool WasBitten;		// agent was bitten by a zombie recently (0.5 seconds)
	bool Death;

	float FOV_Angle;
	float FOV_Range;

	Elite::Vector2 LinearVelocity;
	float AngularVelocity;
	float CurrentLinearSpeed;
	Elite::Vector2 Position;
	float Orientation;
	float MaxLinearSpeed;
	float MaxAngularSpeed;
	float GrabRange;
	float AgentSize;
};
#pragma endregion

namespace std
{
	template <>
	struct hash<ItemInfo>
	{
		std::size_t operator()(const ItemInfo& item) const
		{
			return std::hash<int>()(item.Value);
		}
	};

	template <>
	struct hash<HouseInfo>
	{
		std::size_t operator()(const HouseInfo& house) const
		{
			return std::hash<float>()(house.Center.x) + std::hash<float>()(house.Center.y);
		}
	};
}