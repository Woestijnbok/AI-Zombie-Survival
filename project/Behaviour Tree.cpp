#include "stdafx.h"
#include "Behaviour Tree.h"
#include "Blackboard.h"

namespace DecisionMaking
{
	namespace BehaviourTree
	{

		IBehaviour::IBehaviour() :
			m_CurrentState{ State::Failure }
		{

		}

		IComposite::IComposite(std::vector<IBehaviour*> childBehaviours) :
			IBehaviour(),
			m_ChildBehaviours{ childBehaviours }
		{

		}

		IComposite::~IComposite()
		{
			for (auto behaviour : m_ChildBehaviours) SAFE_DELETE(behaviour);
		}

		Selector::Selector(std::vector<IBehaviour*> childBehaviours) :
			IComposite{ childBehaviours }
		{

		}

		State Selector::Execute(Blackboard* pBlackBoard)
		{
			// Goes through all child behaviours and stops when a child returns something different then failure
			auto itNonFailedState{ std::ranges::find_if(m_ChildBehaviours, [this, pBlackBoard](IBehaviour* behaviour) -> bool
				{
					m_CurrentState = behaviour->Execute(pBlackBoard);
					return m_CurrentState != State::Failure;
				}
			) };

			return m_CurrentState;
		}

		Sequence::Sequence(std::vector<IBehaviour*> childBehaviours) :
			IComposite{ childBehaviours }
		{

		}

		State Sequence::Execute(Blackboard* pBlackBoard)
		{
			// Goes through all child behaviours and stops when a child returns something different then success
			auto itUnsuccessfulState{ std::ranges::find_if(m_ChildBehaviours, [this, pBlackBoard](IBehaviour* behaviour) -> bool
				{
					m_CurrentState = behaviour->Execute(pBlackBoard);
					return m_CurrentState != State::Success;
				}
			) };

			return m_CurrentState;
		}

		PartialSequence::PartialSequence(std::vector<IBehaviour*> childBehaviours) :
			Sequence{ childBehaviours },
			m_CurrentBehaviourIndex{ 0 }
		{

		}

		State PartialSequence::Execute(Blackboard* pBlackBoard)
		{
			// Goes through all child behaviours and stops when a child returns something different then success
			auto itUnsuccessfulState{ std::find_if(std::begin(m_ChildBehaviours) + m_CurrentBehaviourIndex, std::end(m_ChildBehaviours), [this, pBlackBoard](IBehaviour* behaviour) -> bool
				{
					m_CurrentState = behaviour->Execute(pBlackBoard);
					return m_CurrentState != State::Success;
				}
			) };

			// Stores the position of the child that returned running for the next execute, if non did it will start from the first child again
			if (m_CurrentState == State::Running) m_CurrentBehaviourIndex = UINT(std::distance(std::begin(m_ChildBehaviours), itUnsuccessfulState));
			else m_CurrentBehaviourIndex = 0;

			return m_CurrentState;
		}

		Condition::Condition(std::function<bool(Blackboard*)> function) :
			IBehaviour{},
			m_ConditionFunction{ function }
		{

		}

		State Condition::Execute(Blackboard* pBlackBoard)
		{
			// A condition is a state that either returns success or failure
			m_CurrentState = (m_ConditionFunction(pBlackBoard)) ? State::Success : State::Failure;

			return m_CurrentState;
		}

		Action::Action(std::function<State(Blackboard*)> function) :
			IBehaviour{},
			m_ActionFunction{ function }
		{

		}

		State Action::Execute(Blackboard* pBlackBoard)
		{
			m_CurrentState = m_ActionFunction(pBlackBoard);

			return m_CurrentState;
		}

		Tree::Tree(Blackboard* blackBoard, IBehaviour* rootBehaviour) :
			m_CurrentState{ State::Failure },
			m_BlackBoard{ blackBoard },
			m_RootBehaviour{ rootBehaviour }
		{

		}

		Tree::~Tree()
		{
			SAFE_DELETE(m_RootBehaviour);
		}

		void Tree::Update(float deltaTime)
		{
			// Will execute the rood behaviour which in turn will executre his children and so on
			m_CurrentState = m_RootBehaviour->Execute(m_BlackBoard);
		}

		Blackboard* Tree::GetBlackboard() const
		{
			return m_BlackBoard;
		}
	}
}