#include "stdafx.h"
#include "FSM Conditions.h"
#include "Exam_HelperStructs.h"
#include "Blackboard.h"
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <algorithm>
#include <array>
#include <tuple>

namespace DecisionMaking
{
	namespace FiniteStateMachine
	{
		bool InDanger::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			FOVStats fovStats{};
			pBlackboard->GetData("FOVStats", fovStats);

			output = fovStats.NumEnemies > 0;

			return output;
		}

		bool IsSafe::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			std::pair<float, float>* escapeTimer{};
			pBlackboard->GetData("EscapeTimer", escapeTimer);

			// Have we ran away from the last enemy we saw long enough
			output = escapeTimer->first > escapeTimer->second;

			return output;
		}

		bool SafePointReached::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			Elite::Vector2* safePoint{};
			pBlackboard->GetData("SafePoint", safePoint);

			output = agentInfo.Position.Distance(*safePoint) < 2.0f;

			return output;
		}

		bool FoundUnexploredHouse::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			FOVStats fovStats{};
			pBlackboard->GetData("FOVStats", fovStats);

			// Do we see houses
			if (fovStats.NumHouses > 0)
			{
				std::vector<HouseInfo> houses{};
				pBlackboard->GetData("Houses", houses);

				std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
				pBlackboard->GetData("FoundHouses", foundHouses);

				// Do we see an unexplored house
				output = std::ranges::any_of(houses, [foundHouses](const HouseInfo& house) -> bool 
					{ 
						// We already found the house but maybe we didn't explore it yet
						if (foundHouses->contains(house))
						{
							return !std::get<0>(foundHouses->at(house));
						}
						// This is a completly new house
						else return true;
					});
			}

			return output;
		}

		bool InsideHouse::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ true };

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			// Check if we are outside the house
			if (currentHouse != nullptr)
			{
				if (agentInfo.Position.x > (currentHouse->Center.x + (currentHouse->Size.x / 2.0f))) output = false;
				else if (agentInfo.Position.x < (currentHouse->Center.x - (currentHouse->Size.x / 2.0f))) output = false;
				else if (agentInfo.Position.y > (currentHouse->Center.y + (currentHouse->Size.y / 2.0f))) output = false;
				else if (agentInfo.Position.y < (currentHouse->Center.y - (currentHouse->Size.y / 2.0f))) output = false;
			}
			else output = false;

			return output;
		}

		bool PickableItemInSight::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			FOVStats fovStats{};
			pBlackboard->GetData("FOVStats", fovStats);

			std::vector<ItemInfo>* inventory{};
			pBlackboard->GetData("Inventory", inventory);

			auto itEmptySlot{ std::ranges::find_if(*inventory, [](const ItemInfo& item) -> bool { return item.Type == eItemType::GARBAGE; }) };

			// Do we see items and do we still have space left in our inventory
			output = (fovStats.NumItems) > 0 && (itEmptySlot != std::end(*inventory));

			return output;
		}

		bool GotTargetItem::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			std::vector<ItemInfo>* inventory{};
			pBlackboard->GetData("Inventory", inventory);

			ItemInfo* targetItem{};
			pBlackboard->GetData("TargetItem", targetItem);

			// Do we have our target item in our inventory
			output = std::ranges::any_of(*inventory, [&targetItem](const ItemInfo& item) -> bool { return item.ItemHash == targetItem->ItemHash; });

			return output;
		}

		bool ExploredHouse::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			std::array<std::pair<bool, Elite::Vector2>, 4>* path{};
			pBlackboard->GetData("InHousePath", path);

			bool pathCompleted{ std::ranges::all_of(*path, [](const std::pair<bool, Elite::Vector2>& point) -> bool { return point.first; })};

			// Do we have no items left in our current house (that we know of) and did we complete the our explore path of the house
			output = (std::get<2>(foundHouses->at(*currentHouse)).size() == 0) && (pathCompleted);

			return output;
		}

		bool OutsideHouse::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			HouseInfo* currentHouse{};
			pBlackboard->GetData("CurrentHouse", currentHouse);

			std::unordered_map<HouseInfo, std::tuple<bool, Elite::Vector2, std::unordered_set<ItemInfo>>>* foundHouses{};
			pBlackboard->GetData("FoundHouses", foundHouses);

			// Are we at the entrance of the current house
			output = agentInfo.Position.Distance(std::get<1>(foundHouses->at(*currentHouse))) < 2.0f;

			return output;
		}

		bool CheckedBehind::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			float desiredOrientation{};
			pBlackboard->GetData("CheckBehindOrientation", desiredOrientation);

			output = Elite::AreEqual(agentInfo.Orientation, desiredOrientation);

			return output;
		}

		bool NeedToCheckBehind::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			AgentInfo agentInfo{};
			pBlackboard->GetData("AgentInfo", agentInfo);

			output = agentInfo.Bitten;

			return output;
		}

		bool ZoneInSight::Evaluate(Blackboard* pBlackboard) const
		{
			bool output{ false };

			FOVStats fovStats{};
			pBlackboard->GetData("FOVStats", fovStats);

			output = fovStats.NumPurgeZones > 0;

			return output;
		}
	}
}