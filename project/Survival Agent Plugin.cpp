#include "stdafx.h"
#include "Survival Agent Plugin.h"
#include "IExamInterface.h"
#include "Movement Behaviours.h"
#include "Finite State Machine.h"
#include "Blackboard.h"
#include "FSM States.h"
#include "FSM Conditions.h"
#include "Behaviour Tree.h"
#include "BT Actions.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <array>
#include <tuple>

using namespace Elite;

void SurvivalAgentPlugin::Initialize(IBaseInterface* pInterface, PluginInfo& pluginInfo)
{
	// Store our interface and set our pathfinding function for our behaviours
	m_Interface = dynamic_cast<IExamInterface*>(pInterface);
	MovementBehavior::ISteeringBehavior::SetPathfindingFunction(m_Interface, &IExamInterface::NavMesh_GetClosestPathPoint);

	// Information for the leaderboard
	pluginInfo.BotName = "John";
	pluginInfo.Student_Name = "Artuur Demeyer";
	pluginInfo.Student_Class = "2DAE09";
	pluginInfo.LB_Password = "LunaRimka";

	// Create the blackboard and store the starting difficulty
	CreateBlackboard();
	StatisticsInfo stats{};
	m_Blackboard->GetData("StatisticsInfo", stats);
	m_CurrentDifficultyLevel = stats.Difficulty;

	// Exploration States
	m_Roam = new DecisionMaking::FiniteStateMachine::Roam{};
	m_Escape = new DecisionMaking::FiniteStateMachine::Escape{};
	m_SafeSeek = new DecisionMaking::FiniteStateMachine::SafeSeek{};
	m_LookBehind = new DecisionMaking::FiniteStateMachine::LookBehind{};
	m_ExploreHouse = new DecisionMaking::FiniteStateMachine::ExploreHouse{};
	m_LeaveHouse = new DecisionMaking::FiniteStateMachine::LeaveHouse{};
	m_GetItem = new DecisionMaking::FiniteStateMachine::GetItem{};
	m_LeaveZone = new DecisionMaking::FiniteStateMachine::RunAwayFromZone{};
	m_GetInsideUnexploredHouse = new DecisionMaking::FiniteStateMachine::GetInsideUnexploredHouse{};

	// Exploration Conditions
	DecisionMaking::FiniteStateMachine::ICondition* inDanger{ new DecisionMaking::FiniteStateMachine::InDanger{} };
	DecisionMaking::FiniteStateMachine::ICondition* isSafe{ new DecisionMaking::FiniteStateMachine::IsSafe{} };
	DecisionMaking::FiniteStateMachine::ICondition* safePointReached{ new DecisionMaking::FiniteStateMachine::SafePointReached{} };
	DecisionMaking::FiniteStateMachine::ICondition* needToCheckBehind{ new DecisionMaking::FiniteStateMachine::NeedToCheckBehind{} };
	DecisionMaking::FiniteStateMachine::ICondition* checkedBehind{ new DecisionMaking::FiniteStateMachine::CheckedBehind{} };
	DecisionMaking::FiniteStateMachine::ICondition* exploredHouse{ new DecisionMaking::FiniteStateMachine::ExploredHouse };
	DecisionMaking::FiniteStateMachine::ICondition* foundUnexploredHouse{ new DecisionMaking::FiniteStateMachine::FoundUnexploredHouse };
	DecisionMaking::FiniteStateMachine::ICondition* outsideHouse{ new DecisionMaking::FiniteStateMachine::OutsideHouse{} };
	DecisionMaking::FiniteStateMachine::ICondition* pickableItemInSight{ new DecisionMaking::FiniteStateMachine::PickableItemInSight{} };
	DecisionMaking::FiniteStateMachine::ICondition* gotTargetItem{ new DecisionMaking::FiniteStateMachine::GotTargetItem{} };
	DecisionMaking::FiniteStateMachine::ICondition* zoneInSight{ new DecisionMaking::FiniteStateMachine::ZoneInSight{} };
	DecisionMaking::FiniteStateMachine::ICondition* insideHouse{ new DecisionMaking::FiniteStateMachine::InsideHouse{} };
	

	// Setting up the exploration finite state machine
	m_ExplorationFiniteStateMachine = new DecisionMaking::FiniteStateMachine::StateMachine{ m_Roam, m_Blackboard };

	m_ExplorationFiniteStateMachine->AddTransition(m_Roam, m_Escape, inDanger);
	m_ExplorationFiniteStateMachine->AddTransition(m_Roam, m_LookBehind, needToCheckBehind);
	m_ExplorationFiniteStateMachine->AddTransition(m_Roam, m_GetInsideUnexploredHouse, foundUnexploredHouse);
	m_ExplorationFiniteStateMachine->AddTransition(m_Roam, m_LeaveZone, zoneInSight);

	m_ExplorationFiniteStateMachine->AddTransition(m_Escape, m_SafeSeek, isSafe);
	m_ExplorationFiniteStateMachine->AddTransition(m_Escape, m_LeaveZone, zoneInSight);
	m_ExplorationFiniteStateMachine->AddTransition(m_Escape, m_LeaveHouse, insideHouse);

	m_ExplorationFiniteStateMachine->AddTransition(m_SafeSeek, m_Escape, inDanger);
	m_ExplorationFiniteStateMachine->AddTransition(m_SafeSeek, m_Roam, safePointReached);
	m_ExplorationFiniteStateMachine->AddTransition(m_SafeSeek, m_LeaveZone, zoneInSight);

	m_ExplorationFiniteStateMachine->AddTransition(m_LookBehind, m_Escape, inDanger);
	m_ExplorationFiniteStateMachine->AddTransition(m_LookBehind, m_Roam, checkedBehind);
	m_ExplorationFiniteStateMachine->AddTransition(m_LookBehind, m_LeaveZone, zoneInSight);

	m_ExplorationFiniteStateMachine->AddTransition(m_GetInsideUnexploredHouse, m_ExploreHouse, insideHouse);
	m_ExplorationFiniteStateMachine->AddTransition(m_GetInsideUnexploredHouse, m_LookBehind, needToCheckBehind);
	m_ExplorationFiniteStateMachine->AddTransition(m_GetInsideUnexploredHouse, m_Escape, inDanger);

	m_ExplorationFiniteStateMachine->AddTransition(m_ExploreHouse, m_GetItem, pickableItemInSight);
	m_ExplorationFiniteStateMachine->AddTransition(m_ExploreHouse, m_LeaveHouse, exploredHouse);
	m_ExplorationFiniteStateMachine->AddTransition(m_ExploreHouse, m_LookBehind, needToCheckBehind);
	m_ExplorationFiniteStateMachine->AddTransition(m_ExploreHouse, m_LeaveHouse, inDanger);
	m_ExplorationFiniteStateMachine->AddTransition(m_ExploreHouse, m_LeaveZone, zoneInSight);

	m_ExplorationFiniteStateMachine->AddTransition(m_GetItem, m_ExploreHouse, gotTargetItem);

	m_ExplorationFiniteStateMachine->AddTransition(m_LeaveHouse, m_Roam, outsideHouse);
	m_ExplorationFiniteStateMachine->AddTransition(m_LeaveHouse, m_LeaveZone, zoneInSight);

	m_ExplorationFiniteStateMachine->AddTransition(m_LeaveZone, m_Roam, safePointReached);

	// Setting up the inventory behaviour tree
	DecisionMaking::BehaviourTree::IBehaviour* shooting{ new DecisionMaking::BehaviourTree::Action{&DecisionMaking::BehaviourTree::CheckShooting} };
	DecisionMaking::BehaviourTree::IBehaviour* healing{ new DecisionMaking::BehaviourTree::Action{&DecisionMaking::BehaviourTree::CheckHealing} };
	DecisionMaking::BehaviourTree::IBehaviour* food{ new DecisionMaking::BehaviourTree::Action{&DecisionMaking::BehaviourTree::CheckFood} };
	DecisionMaking::BehaviourTree::IBehaviour* root{ new DecisionMaking::BehaviourTree::Sequence{ std::vector<DecisionMaking::BehaviourTree::IBehaviour*>{ shooting, healing, food } } };
	m_InventoryBehaviourTree = new DecisionMaking::BehaviourTree::Tree{ m_Blackboard, root };
}

void SurvivalAgentPlugin::DllInit()
{

}

void SurvivalAgentPlugin::DllShutdown()
{
	delete m_Blackboard;
	delete m_ExplorationFiniteStateMachine;
	delete m_InventoryBehaviourTree;
}

void SurvivalAgentPlugin::InitGameDebugParams(GameDebugParams& debugParameters)
{
	debugParameters.SpawnEnemies = true;
	debugParameters.EnemyCount = 20;
	debugParameters.ItemCount = 20;
	debugParameters.GodMode = false;
	debugParameters.IgnoreEnergy = false;
	debugParameters.AutoFollowCam = true;
	debugParameters.RenderUI = true;
	debugParameters.AutoGrabClosestItem = false;
	debugParameters.LevelFile = "GameLevel.gppl";
	debugParameters.Seed = 4;
	debugParameters.StartingDifficultyStage = 1;
	debugParameters.InfiniteStamina = false;
	debugParameters.SpawnDebugPistol = false;
	debugParameters.SpawnDebugShotgun = false;
	debugParameters.SpawnPurgeZonesOnMiddleClick = true;
	debugParameters.SpawnZombieOnRightClick = true;
	debugParameters.PrintDebugMessages = true;
	debugParameters.ShowDebugItemNames = true;
}

void SurvivalAgentPlugin::Update_Debug(float deltaTime)
{
	if (m_Interface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete)) m_Interface->RequestShutdown();
}

SteeringPlugin_Output SurvivalAgentPlugin::UpdateSteering(float deltaTime)
{
	UpdateBlackboard(deltaTime);
	m_ExplorationFiniteStateMachine->Update(deltaTime);
	m_InventoryBehaviourTree->Update(deltaTime);

	SteeringPlugin_Output* output{};
	m_Blackboard->GetData("SteeringOutput", output);
	return *output;
}

void SurvivalAgentPlugin::Render(float deltaTime) const
{
	AgentInfo agentInfo{};
	m_Blackboard->GetData("AgentInfo", agentInfo);

	// Debug orientation in green and direction in red
	m_Interface->Draw_Direction(agentInfo.Position, agentInfo.LinearVelocity.GetNormalized(), 15.0f, Vector3{ 1.0f, 0.0f, 0.0f });
	m_Interface->Draw_Direction(agentInfo.Position, Elite::OrientationToVector(agentInfo.Orientation), 15.0f, Vector3{ 0.0f, 1.0f, 0.0f });

	FOVStats fovStats{};
	m_Blackboard->GetData("FOVStats", fovStats);

	// Draw Shooting ranges in blue
	if (fovStats.NumEnemies > 0)
	{
		float maximumShotgunDistance{}, maximumShotgunAngle{};
		m_Blackboard->GetData("MaximumShotgunDistance", maximumShotgunDistance);
		m_Blackboard->GetData("MaximumShotgunAngle", maximumShotgunAngle);

		float maximumPistolDistance{}, maximumPistolAngle{};
		m_Blackboard->GetData("MaximumPistolDistance", maximumPistolDistance);
		m_Blackboard->GetData("MaximumPistolAngle", maximumPistolAngle);

		// Draw shotgun range
		m_Interface->Draw_Direction(agentInfo.Position, Elite::OrientationToVector(agentInfo.Orientation - Elite::ToRadians(maximumShotgunAngle)), maximumShotgunDistance, Vector3{ 0.0f, 0.0f, 1.0f });
		m_Interface->Draw_Direction(agentInfo.Position, Elite::OrientationToVector(agentInfo.Orientation + Elite::ToRadians(maximumShotgunAngle)), maximumShotgunDistance, Vector3{ 0.0f, 0.0f, 1.0f });

		// Draw pistol rang
		m_Interface->Draw_Direction(agentInfo.Position, Elite::OrientationToVector(agentInfo.Orientation - Elite::ToRadians(maximumPistolAngle)), maximumPistolDistance, Vector3{ 0.0f, 0.0f, 1.0f });
		m_Interface->Draw_Direction(agentInfo.Position, Elite::OrientationToVector(agentInfo.Orientation + Elite::ToRadians(maximumPistolAngle)), maximumPistolDistance, Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Debug the entrance / exit to our current in blue
	if (m_ExplorationFiniteStateMachine->AtState(m_LeaveHouse))
	{
		HouseInfo* currentHouse{};
		m_Blackboard->GetData("CurrentHouse", currentHouse);

		std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
		m_Blackboard->GetData("FoundHouses", foundHouses);

		m_Interface->Draw_Point(std::get<1>(foundHouses->at(*currentHouse)), 3.0f, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Debug our safe seek point in blue
	else if (m_ExplorationFiniteStateMachine->AtState(m_SafeSeek))
	{
		Elite::Vector2* safePoint{};
		m_Blackboard->GetData("SafePoint", safePoint);

		m_Interface->Draw_Point(*safePoint, 3.0f, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Debug the center of the unexplored house we are currently going to in blue
	else if (m_ExplorationFiniteStateMachine->AtState(m_GetInsideUnexploredHouse))
	{
		HouseInfo* currentHouse{};
		m_Blackboard->GetData("CurrentHouse", currentHouse);
		m_Interface->Draw_Point(currentHouse->Center, 3.0f, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Draw the inside path / tour route of our current house in blue
	else if (m_ExplorationFiniteStateMachine->AtState(m_ExploreHouse))
	{
		std::array<std::pair<bool, Elite::Vector2>, 4>* path;
		m_Blackboard->GetData("InHousePath", path);

		std::array<Elite::Vector2, 4> points { path->at(0).second, path->at(1).second, path->at(2).second, path->at(3).second };

		m_Interface->Draw_Polygon(points.data(), 4, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Draw a blue dot at the location of our target item
	else if (m_ExplorationFiniteStateMachine->AtState(m_GetItem))
	{
		ItemInfo* item{};
		m_Blackboard->GetData("TargetItem", item);
		m_Interface->Draw_Point(item->Location, 3.0f, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
	// Draw the spot we have to go to get be at a safe distance from the zone in blue
	else if (m_ExplorationFiniteStateMachine->AtState(m_LeaveZone))
	{
		Elite::Vector2* safePoint{};
		m_Blackboard->GetData("SafePoint", safePoint);
		m_Interface->Draw_Point(*safePoint, 3.0f, Elite::Vector3{ 0.0f, 0.0f, 1.0f });
	}
}

void SurvivalAgentPlugin::CreateBlackboard()
{
	m_Blackboard = new DecisionMaking::Blackboard{};

	m_Blackboard->AddData("Interface", m_Interface);

	// Movement behaviors
	MovementBehavior::ISteeringBehavior* steering{ new MovementBehavior::Seek{} };
	m_Blackboard->AddData("Seek", steering);
	steering = new MovementBehavior::Flee{};
	m_Blackboard->AddData("Flee", steering);
	steering = new MovementBehavior::Arrive{};
	m_Blackboard->AddData("Arrive", steering);
	steering = new MovementBehavior::Pursuit{};
	m_Blackboard->AddData("Pursuit", steering);
	steering = new MovementBehavior::Evade{};
	m_Blackboard->AddData("Evade", steering);
	steering = new MovementBehavior::Wander{};
	m_Blackboard->AddData("Wander", steering);
	m_Blackboard->AddData("SteeringOutput", new SteeringPlugin_Output{});

	// Exam Help structs
	m_Blackboard->AddData("WorldInfo", m_Interface->World_GetInfo());
	m_Blackboard->AddData("StatisticsInfo", StatisticsInfo{});
	m_Blackboard->AddData("Houses", std::vector<HouseInfo>{});
	m_Blackboard->AddData("Enemies", std::vector<EnemyInfo>{});
	m_Blackboard->AddData("PurgeZones", std::vector<PurgeZoneInfo>{});
	m_Blackboard->AddData("Items", std::vector<ItemInfo>{});
	m_Blackboard->AddData("FOVStats", FOVStats{});
	m_Blackboard->AddData("AgentInfo", AgentInfo{});

	// Exploration
		// First element is the counter and the second is the count the counter has to reach
	m_Blackboard->AddData("EscapeTimer", new std::pair<float, float>{ 0.0f, 3.0f });	
		// Point used when running away from a certain things
	m_Blackboard->AddData("SafePoint", new Elite::Vector2{});
		// Orientation that corrosponds to the player's back
	m_Blackboard->AddData("CheckBehindOrientation", 0.0f);
		// A map / dictionary with the house info as key
		// The value will be a tuple of: an is explored bool, the entrance position Elite::Vector2, and a set of known items in the house
	m_Blackboard->AddData("FoundHouses", new std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>{});
		// A pointer to the house info about our current house
	HouseInfo* currentHouse{ nullptr };
	m_Blackboard->AddData("CurrentHouse", currentHouse);
		// The path / tour used to explore the inside of a house
	m_Blackboard->AddData("InHousePath", new std::array<std::pair<bool, Elite::Vector2>, 4>);
		// Target item, will be used to go for items
	m_Blackboard->AddData("TargetItem", new ItemInfo{});

	// Inventory Management
		// Our inventory itself
	m_Blackboard->AddData("Inventory", new std::vector<ItemInfo>
		{
			ItemInfo{ eItemType::GARBAGE },
			ItemInfo{ eItemType::GARBAGE },
			ItemInfo{ eItemType::GARBAGE },
			ItemInfo{ eItemType::GARBAGE },
			ItemInfo{ eItemType::GARBAGE }
		}
	);
		// The distance where we won't shoot anymore with a shotgun
	m_Blackboard->AddData("MaximumShotgunDistance", 5.0f);
		// The angle that is to wide for a shotgun
	m_Blackboard->AddData("MaximumShotgunAngle", 20.0f);
		// The distance where we won't shoot anymore with a pistol
	m_Blackboard->AddData("MaximumPistolDistance", 7.0f);
		// The angle that is to wide for a pistol
	m_Blackboard->AddData("MaximumPistolAngle", 10.0f);
}

void SurvivalAgentPlugin::UpdateBlackboard(float deltaTime)
{
	// Mark all explored houses as unexplored again when there is a new wave
	StatisticsInfo stats{ m_Interface->World_GetStats() };
	if (stats.Difficulty > m_CurrentDifficultyLevel)
	{
		m_CurrentDifficultyLevel = stats.Difficulty;

		std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
		m_Blackboard->GetData("FoundHouses", foundHouses);
		std::ranges::for_each(*foundHouses, [](auto& element) -> void 
			{
				std::get<0>(element.second) = false;
			});
	}

	m_Blackboard->ChangeData("StatisticsInfo", stats);
	m_Blackboard->ChangeData("Houses", m_Interface->GetHousesInFOV());
	m_Blackboard->ChangeData("Enemies", m_Interface->GetEnemiesInFOV());
	m_Blackboard->ChangeData("PurgeZones", m_Interface->GetPurgeZonesInFOV());
	m_Blackboard->ChangeData("Items", m_Interface->GetItemsInFOV());
	m_Blackboard->ChangeData("FOVStats", m_Interface->FOV_GetStats());
	m_Blackboard->ChangeData("AgentInfo", m_Interface->Agent_GetInfo());
}