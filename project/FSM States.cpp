#include "stdafx.h"
#include "FSM States.h"
#include "Blackboard.h"
#include "Movement Behaviours.h"
#include "IExamInterface.h"
#include "Exam_HelperStructs.h"
#include <utility>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <tuple>

namespace DecisionMaking
{
	namespace FiniteStateMachine
	{
#pragma region Escape
		void Escape::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Escape" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = true;
		}

		void Escape::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			FOVStats fovStats{};
			pBlackboard->GetData("FOVStats", fovStats);

			std::pair<float, float>* escapeTimer{};
			pBlackboard->GetData("EscapeTimer", escapeTimer);

			// If we don't see enemies run in the oppiste direction of the last one we saw for a set time
			if (fovStats.NumEnemies == 0)
			{
				if (escapeTimer->first == 0.0f)
				{
					SteeringPlugin_Output* pSteering{};
					pBlackboard->GetData("SteeringOutput", pSteering);

					pSteering->AngularVelocity = 0.0f;
					pSteering->AutoOrient = false;
				}

				escapeTimer->first += deltaTime;
			}
			// Run awway from the closest enemy
			else
			{
				SteeringPlugin_Output* pSteering{};
				pBlackboard->GetData("SteeringOutput", pSteering);

				AgentInfo agentInfo{};
				pBlackboard->GetData("AgentInfo", agentInfo);

				std::vector<EnemyInfo> enemies{};
				pBlackboard->GetData("Enemies", enemies);

				MovementBehavior::ISteeringBehavior* pFlee{};
				pBlackboard->GetData("Flee", pFlee);

				auto itClosestEnemy{ std::ranges::min_element(enemies, [&agentInfo](const EnemyInfo& enemy1, const EnemyInfo& enemy2) -> bool
						{
							return agentInfo.Position.DistanceSquared(enemy1.Location) < agentInfo.Position.DistanceSquared(enemy2.Location);
						}
				) };

				MovementBehavior::TargetData targetData{ itClosestEnemy->Location, itClosestEnemy->LinearVelocity };
				SteeringPlugin_Output steeringOutput{ pFlee->CalculateSteering(deltaTime, agentInfo, targetData) };

				pSteering->LinearVelocity = steeringOutput.LinearVelocity;
				pSteering->AngularVelocity = steeringOutput.AngularVelocity;
				pSteering->AutoOrient = steeringOutput.AutoOrient;

				if (escapeTimer->first != 0.0f) escapeTimer->first = 0.0f;
			}
		}

		void Escape::OnExit(Blackboard* pBlackboard) const
		{
			std::pair<float, float>* escapeTimer{};
			pBlackboard->GetData("EscapeTimer", escapeTimer);

			escapeTimer->first = 0.0f;
		}
#pragma endregion

#pragma region Roam
		void Roam::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Roam" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = false;
		}

		void Roam::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			MovementBehavior::ISteeringBehavior* pWander{};
			pBlackboard->GetData("Wander", pWander);

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			// Calculate the steering
			MovementBehavior::TargetData targetData{};
			SteeringPlugin_Output steeringOutput{ pWander->CalculateSteering(deltaTime, agentInfo, targetData) };

			// Adapt the steering variable in our blackboard
			pSteering->AngularVelocity = steeringOutput.AngularVelocity;
			pSteering->LinearVelocity = steeringOutput.LinearVelocity;
			pSteering->AutoOrient = steeringOutput.AutoOrient;
		}

		void Roam::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion

#pragma region LookBehind
		void LookBehind::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State LookBehind" << std::endl;

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = false;
			pSteering->AutoOrient = false;

			// Set our desired orientation to oppisite one we have now
			const float desiredOrientation{ std::clamp((agentInfo.Orientation + float(E_PI)), 0.0f, (2.0f * float(E_PI))) };
			pBlackboard->ChangeData("CheckBehindOrientation", desiredOrientation);
		}

		void LookBehind::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->AngularVelocity = -agentInfo.MaxAngularSpeed;
		}

		void LookBehind::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion

#pragma region SafeSeek
		void SafeSeek::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State SafeSeek" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			IExamInterface* pInterface{};
			pBlackboard->GetData("Interface", pInterface);

			pSteering->RunMode = false;

			Elite::Vector2* safePoint{};
			pBlackboard->GetData("SafePoint", safePoint);

			// Set our safe point to a a point a set distance from where we are now going (direction)
			const Elite::Vector2 direction{ agentInfo.LinearVelocity.GetNormalized() };
			*safePoint = pInterface->NavMesh_GetClosestPathPoint(agentInfo.Position + (direction * 25.0f));
		}

		void SafeSeek::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			MovementBehavior::ISteeringBehavior* pSeek;
			pBlackboard->GetData("Seek", pSeek);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			Elite::Vector2* safePoint{};
			pBlackboard->GetData("SafePoint", safePoint);

			// Calculate the steering
			SteeringPlugin_Output steeringOutput{ pSeek->CalculateSteering(deltaTime, agentInfo, MovementBehavior::TargetData{ *safePoint, Elite::Vector2{} }) };

			// Adapt the steering variable in our blackboard
			pSteering->AngularVelocity = steeringOutput.AngularVelocity;
			pSteering->LinearVelocity = steeringOutput.LinearVelocity;
			pSteering->AutoOrient = steeringOutput.AutoOrient;
		}

		void SafeSeek::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion

#pragma region GetInsideUnexploredHouse
		void GetInsideUnexploredHouse::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Get Inside House" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);
			pSteering->RunMode = false;

			std::vector<HouseInfo> houses{};
			pBlackboard->GetData("Houses", houses);

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			// Store the new houses and store / update the entrances, also update our current house
			std::ranges::for_each(houses, [foundHouses, &agentInfo, &currentHouse](const HouseInfo& house) -> void
				{
					if (foundHouses->contains(house))
					{
						// A already found house but yet to be explored
						if (!std::get<0>(foundHouses->at(house))) 
						{
							std::get<1>(foundHouses->at(house)) = agentInfo.Position;
							delete currentHouse;
							currentHouse = new HouseInfo{ house };
						}
					}
					// Found a new house
					else 
					{
						foundHouses->emplace(std::make_pair(house, std::make_tuple(false, agentInfo.Position, std::unordered_set<ItemInfo>{})));
						delete currentHouse;
						currentHouse = new HouseInfo{ house };
					}
				});

			pBlackboard->ChangeData("CurrentHouse", currentHouse);
		}

		void GetInsideUnexploredHouse::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			MovementBehavior::ISteeringBehavior* pSeek;
			pBlackboard->GetData("Seek", pSeek);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			// Calculate the steering
			SteeringPlugin_Output steeringOutput{ pSeek->CalculateSteering(deltaTime, agentInfo, MovementBehavior::TargetData{ currentHouse->Center, Elite::Vector2{} }) };

			// Adapt the steering variable in our blackboard
			pSteering->AngularVelocity = steeringOutput.AngularVelocity;
			pSteering->LinearVelocity = steeringOutput.LinearVelocity;
			pSteering->AutoOrient = steeringOutput.AutoOrient;

			std::vector<ItemInfo> items{};
			pBlackboard->GetData("Items", items);

			// Store new items that we see
			if (items.size() > 0)
			{
				std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
				pBlackboard->GetData("FoundHouses", foundHouses);

				std::ranges::for_each(items, [foundHouses, currentHouse](const ItemInfo& item) -> void { std::get<2>(foundHouses->at(*currentHouse)).emplace(item); });
			}
		}

		void GetInsideUnexploredHouse::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion

#pragma region ExploreHouse
		void ExploreHouse::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Explore House" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = false;

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::array<std::pair<bool, Elite::Vector2>, 4>* path;
			pBlackboard->GetData("InHousePath", path);

			// Create a path that takes us around the inside of the house

			// Left bottom
			path->at(0).second.x = currentHouse->Center.x - (currentHouse->Size.x / 2.0f) + 4.5f;
			path->at(0).second.y = currentHouse->Center.y - (currentHouse->Size.y / 2.0f) + 4.5f;
			path->at(0).first = false;

			// left Top
			path->at(3).second.x = currentHouse->Center.x - (currentHouse->Size.x / 2.0f) + 4.5f;
			path->at(3).second.y = currentHouse->Center.y + (currentHouse->Size.y / 2.0f) - 4.5f;
			path->at(3).first = false;

			// Right Top
			path->at(2).second.x = currentHouse->Center.x + (currentHouse->Size.x / 2.0f) - 4.5f;
			path->at(2).second.y = currentHouse->Center.y + (currentHouse->Size.y / 2.0f) - 4.5f;
			path->at(2).first = false;

			// Right bottom
			path->at(1).second.x = currentHouse->Center.x + (currentHouse->Size.x / 2.0f) - 4.5f;
			path->at(1).second.y = currentHouse->Center.y - (currentHouse->Size.y / 2.0f) + 4.5f;
			path->at(1).first = false;
		}

		void ExploreHouse::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			MovementBehavior::ISteeringBehavior* seek;
			pBlackboard->GetData("Seek", seek);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			std::array<std::pair<bool, Elite::Vector2>, 4>* path;
			pBlackboard->GetData("InHousePath", path);

			// Go to the next point of our path
			auto itNextPoint{ std::ranges::find_if(*path, [](const std::pair<bool, Elite::Vector2>& point) -> bool { return !point.first; }) };
			if (itNextPoint != std::end(*path))
			{
				// Calculate the steering
				SteeringPlugin_Output steeringOutput{ seek->CalculateSteering(deltaTime, agentInfo, MovementBehavior::TargetData{ itNextPoint->second, Elite::Vector2{} }) };

				// Adapt the steering variable in our blackboard
				pSteering->AngularVelocity = steeringOutput.AngularVelocity;
				pSteering->LinearVelocity = steeringOutput.LinearVelocity;
				pSteering->AutoOrient = steeringOutput.AutoOrient;

				if (agentInfo.Position.Distance(itNextPoint->second) < 0.5f)
				{
					itNextPoint->first = true;
				}
			}
		}

		void ExploreHouse::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion

#pragma region GetItem
		void GetItem::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Get Item" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = false;

			std::vector<ItemInfo> items{};
			pBlackboard->GetData("Items", items);

			pBlackboard->ChangeData("TargetItem", new ItemInfo{ items.at(0) });

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			// Store new items
			std::ranges::for_each(items, [foundHouses, currentHouse](const ItemInfo& item) -> void { std::get<2>(foundHouses->at(*currentHouse)).emplace(item); });
		}

		void GetItem::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			ItemInfo* targetItem{};
			pBlackboard->GetData("TargetItem", targetItem);

			SteeringPlugin_Output* steering{};
			pBlackboard->GetData("SteeringOutput", steering);

			MovementBehavior::ISteeringBehavior* pSeek{};
			pBlackboard->GetData("Seek", pSeek);

			SteeringPlugin_Output output{ pSeek->CalculateSteering(deltaTime, agentInfo, MovementBehavior::TargetData{ targetItem->Location, Elite::Vector2{} }) };

			steering->LinearVelocity = output.LinearVelocity;
			steering->AngularVelocity = output.AngularVelocity;
			steering->AutoOrient = output.AutoOrient;

			// Pickup non garbage items if we are close enough, if it is garbage destroy it
			if (agentInfo.Position.Distance(targetItem->Location) <= agentInfo.GrabRange)
			{
				IExamInterface* pInterface{};
				pBlackboard->GetData("Interface", pInterface);

				std::vector<ItemInfo>* inventory{};
				pBlackboard->GetData("Inventory", inventory);

				auto itEmptySlot{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::GARBAGE; }) };
				UINT inventorySlot{ UINT(std::distance(std::begin(*inventory), itEmptySlot)) };

				switch (targetItem->Type)
				{
				case eItemType::GARBAGE:
				{
					pInterface->DestroyItem(*targetItem);

					// Update inventory slot needed for condition GotTargetItem
					inventory->at(inventorySlot) = ItemInfo{ *targetItem };
					break;
				}
				default:
					if (pInterface->GrabItem(*targetItem))
					{
						// Add item
						pInterface->Inventory_AddItem(inventorySlot, *targetItem);
						inventory->at(inventorySlot) = ItemInfo{ *targetItem };
					}
					break;
				}
			}

			// Store new items we see
			std::vector<ItemInfo> items{};
			pBlackboard->GetData("Items", items);

			if (items.size() > 0)
			{
				HouseInfo* currentHouse{};
				pBlackboard->GetData("CurrentHouse", currentHouse);

				std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
				pBlackboard->GetData("FoundHouses", foundHouses);

				std::ranges::for_each(items, [foundHouses, currentHouse](const ItemInfo& item) -> void { std::get<2>(foundHouses->at(*currentHouse)).emplace(item); });
			}
		}

		void GetItem::OnExit(Blackboard* pBlackboard) const
		{
			ItemInfo* targetItem{};
			pBlackboard->GetData("TargetItem", targetItem);

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			// Remove the item from our found houses information, at the current house
			std::get<2>(foundHouses->at(*currentHouse)).erase(*targetItem);

			delete targetItem;
			targetItem = nullptr;
			pBlackboard->ChangeData("TargetItem", targetItem);

		}
#pragma endregion

#pragma region LeaveHouse
		void LeaveHouse::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Leave House" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = false;
		}

		void LeaveHouse::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			MovementBehavior::ISteeringBehavior* pSeek{};
			pBlackboard->GetData("Seek", pSeek);

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			// Calculate the steering
			SteeringPlugin_Output steeringOutput{ pSeek->CalculateSteering(deltaTime, agentInfo, 
				MovementBehavior::TargetData
				{ 
					std::get<1>(foundHouses->at(*currentHouse)),
					Elite::Vector2{}
				}
			) };

			// Adapt the steering variable in our blackboard
			pSteering->AngularVelocity = steeringOutput.AngularVelocity;
			pSteering->LinearVelocity = steeringOutput.LinearVelocity;
			pSteering->AutoOrient = steeringOutput.AutoOrient;
		}

		void LeaveHouse::OnExit(Blackboard* pBlackboard) const
		{
			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			std::array<std::pair<bool, Elite::Vector2>, 4>* path{};
			pBlackboard->GetData("InHousePath", path);

			bool pathCompleted{ std::ranges::all_of(*path, [](const std::pair<bool, Elite::Vector2>& point) -> bool { return point.first; }) };

			// If we left the house without any items left behind, and we did a full tour inside the house mark it as explored
			if ((std::get<2>(foundHouses->at(*currentHouse)).size() == 0) && pathCompleted)
			{
				std::get<0>(foundHouses->at(*currentHouse)) = true;
			}
		}
#pragma endregion

#pragma region LeavePurgeZone
		void RunAwayFromZone::OnEnter(Blackboard* pBlackboard) const
		{
			std::cout << "State Leave Purge Zone" << std::endl;

			SteeringPlugin_Output* pSteering{};
			pBlackboard->GetData("SteeringOutput", pSteering);

			pSteering->RunMode = true;

			Elite::Vector2* safePoint{};
			pBlackboard->GetData("SafePoint", safePoint);

			std::vector<PurgeZoneInfo> zones{};
			pBlackboard->GetData("PurgeZones", zones);

			// Set our safe point to a set distance outside the zone as close as possible to the center of the world
			if (zones.at(0).Center.x > 0.0f) safePoint->x = zones.at(0).Center.x - (zones.at(0).Radius + 5.0f);
			else safePoint->x = zones.at(0).Center.x + (zones.at(0).Radius + 5.0f);
			if (zones.at(0).Center.y > 0.0f) safePoint->y = zones.at(0).Center.y - (zones.at(0).Radius + 5.0f);
			else safePoint->y = zones.at(0).Center.y + (zones.at(0).Radius + 5.0f);

			IExamInterface* pInterface{};
			pBlackboard->GetData("Interface", pInterface);

			*safePoint = pInterface->NavMesh_GetClosestPathPoint(*safePoint);
		}

		void RunAwayFromZone::Update(Blackboard* pBlackboard, float deltaTime) const
		{
			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			Elite::Vector2* safePoint{};
			pBlackboard->GetData("SafePoint", safePoint);

			SteeringPlugin_Output* steering{};
			pBlackboard->GetData("SteeringOutput", steering);

			MovementBehavior::ISteeringBehavior* pSeek{};
			pBlackboard->GetData("Seek", pSeek);

			SteeringPlugin_Output output{ pSeek->CalculateSteering(deltaTime, agentInfo, MovementBehavior::TargetData{ *safePoint, Elite::Vector2{} }) };

			steering->LinearVelocity = output.LinearVelocity;
			steering->AngularVelocity = output.AngularVelocity;
			steering->AutoOrient = output.AutoOrient;
		}

		void RunAwayFromZone::OnExit(Blackboard* pBlackboard) const
		{

		}
#pragma endregion
	}
}