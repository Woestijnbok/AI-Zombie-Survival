#include "stdafx.h"
#include "BT Actions.h"
#include "IExamInterface.h"
#include "Blackboard.h"
#include "Exam_HelperStructs.h"
#include <algorithm>

namespace DecisionMaking
{
	namespace BehaviourTree
	{
		State CheckShooting(Blackboard* pBlackBoard)
		{
			// Will always return success since we are working with only one sequence in our tree
			State output{ State::Success };

			FOVStats fovStats{};
			pBlackBoard->GetData("FOVStats", fovStats);

			// Check if there are any enemies in sight
			if (fovStats.NumEnemies > 0)
			{
				std::vector<ItemInfo>* inventory{};
				pBlackBoard->GetData("Inventory", inventory);

				auto itShotgun{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::SHOTGUN; }) };
				auto itPistol{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::PISTOL; }) };

				// Check if we have either a shotgun or a pistol in our inventory
				if ((itShotgun != std::end(*inventory)) || (itPistol != std::end(*inventory)))
				{
					AgentInfo agentInfo{};
					pBlackBoard->GetData("AgentInfo", agentInfo);

					std::vector<EnemyInfo> enemies{};
					pBlackBoard->GetData("Enemies", enemies);

					// Get the closest enemy
					auto itClosestEnemy{ std::ranges::min_element(enemies, [agentInfo](const EnemyInfo& enemy1, const EnemyInfo& enemy2) -> bool
						{
							return agentInfo.Position.DistanceSquared(enemy1.Location) < agentInfo.Position.DistanceSquared(enemy2.Location);
						}
					) };

					// Check if the closest enemy is not dead already
					if (itClosestEnemy->Health > 0.0f)
					{
						IExamInterface* pInterface{};
						pBlackBoard->GetData("Interface", pInterface);

						float maximumShotgunDistance{}, maximumShotgunAngle{};
						pBlackBoard->GetData("MaximumShotgunDistance", maximumShotgunDistance);
						pBlackBoard->GetData("MaximumShotgunAngle", maximumShotgunAngle);

						float maximumPistolDistance{}, maximumPistolAngle{};
						pBlackBoard->GetData("MaximumPistolDistance", maximumPistolDistance);
						pBlackBoard->GetData("MaximumPistolAngle", maximumPistolAngle);

						// Calculate the angle between the player and the closest enemy (in degrees)
						const Elite::Vector2 agentToEnemy{ (itClosestEnemy->Location - agentInfo.Position) };
						const float angle{ std::abs(Elite::ToDegrees(Elite::AngleBetween(agentToEnemy, Elite::OrientationToVector(agentInfo.Orientation)))) };

						// Check if we have a shotgun, the enemy is in shotgun range and between the maximum shotgun angle
						if ((itShotgun != std::end(*inventory)) && (agentInfo.Position.Distance(itClosestEnemy->Location) < maximumShotgunDistance) && (angle < maximumShotgunAngle))
						{
							// Shoot the shotgun
							UINT indexShotgun{ UINT(std::distance(std::begin(*inventory), itShotgun)) };
							pInterface->Inventory_UseItem(indexShotgun);
							--inventory->at(indexShotgun).Value;

							// Remove the shotgun if the shotgun is empty (no ammo left)
							if (inventory->at(indexShotgun).Value <= 0)
							{
								pInterface->Inventory_RemoveItem(indexShotgun);
								inventory->at(indexShotgun) = ItemInfo{ eItemType::GARBAGE };
							}
						}
						// Check if we have a pistol, the enemy is in pistol range and the maximum pistol angle
						else if ((itPistol != std::end(*inventory)) && (agentInfo.Position.Distance(itClosestEnemy->Location) < maximumPistolDistance) && (angle < maximumPistolAngle))
						{
							// Shoot the pistol
							UINT indexPistol{ UINT(std::distance(std::begin(*inventory), itPistol)) };
							pInterface->Inventory_UseItem(indexPistol);
							--inventory->at(indexPistol).Value;

							// Remove the pistol if the pistol is empty (no ammo left)
							if (inventory->at(indexPistol).Value <= 0)
							{
								pInterface->Inventory_RemoveItem(indexPistol);
								inventory->at(indexPistol) = ItemInfo{ eItemType::GARBAGE };
							}
						}
					}
				}
			}

			return output;
		}

		State CheckHealing(Blackboard* pBlackBoard)
		{
			// Will always return success since we are working with only one sequence in our tree
			State output{ State::Success };

			AgentInfo agentInfo{};
			pBlackBoard->GetData("AgentInfo", agentInfo);

			// Are we injured
			if (agentInfo.Health < 8.0f)
			{
				std::vector<ItemInfo>* inventory{};
				pBlackBoard->GetData("Inventory", inventory);

				// Do we have healing
				auto itMedkit{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::MEDKIT; }) };
				if (itMedkit != std::end(*inventory))
				{
					// Will we fully utilize our medkit
					UINT indexMedkit{ UINT(std::distance(std::begin(*inventory), itMedkit)) };
					if (10.0f - agentInfo.Energy > inventory->at(indexMedkit).Value)
					{
						IExamInterface* pInterface{};
						pBlackBoard->GetData("Interface", pInterface);

						// Use medkit
						pInterface->Inventory_UseItem(indexMedkit);
						pInterface->Inventory_RemoveItem(indexMedkit);
						inventory->at(indexMedkit) = ItemInfo{ eItemType::GARBAGE };
					}
				}
			}

			return output;
		}

		State CheckFood(Blackboard* pBlackBoard)
		{
			// Will always return success since we are working with only one sequence in our tree
			State output{ State::Success };

			AgentInfo agentInfo{};
			pBlackBoard->GetData("AgentInfo", agentInfo);

			// Are we tired
			if (agentInfo.Energy < 8.0f)
			{
				std::vector<ItemInfo>* inventory{};
				pBlackBoard->GetData("Inventory", inventory);

				// Do we have food
				auto itFood{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::FOOD; }) };
				if (itFood != std::end(*inventory))
				{
					// Will we fully utilize our food
					UINT indexFood{ UINT(std::distance(std::begin(*inventory), itFood)) };
					if (10.0f - agentInfo.Energy > inventory->at(indexFood).Value)
					{
						IExamInterface* pInterface{};
						pBlackBoard->GetData("Interface", pInterface);

						// Eat the food
						pInterface->Inventory_UseItem(indexFood);
						pInterface->Inventory_RemoveItem(indexFood);
						inventory->at(indexFood) = ItemInfo{ eItemType::GARBAGE };
					}
				}
			}

			return output;
		}
	}
}