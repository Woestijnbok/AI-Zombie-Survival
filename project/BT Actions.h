#ifndef BEHAVIOUR_TREE_ACTIONS
#define BEHAVIOUR_TREE_ACTIONS

#include "Behaviour Tree.h"

namespace DecisionMaking
{
	class Blackboard;

	namespace BehaviourTree
	{
		State CheckShooting(Blackboard* pBlackBoard);

		State CheckHealing(Blackboard* pBlackBoard);

		State CheckFood(Blackboard* pBlackBoard);
	}
}

#endif