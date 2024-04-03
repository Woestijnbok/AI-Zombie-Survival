#include "stdafx.h"
#include "Finite State Machine.h"
#include "FSM Conditions.h"
#include "FSM States.h"
#include "Blackboard.h"

namespace DecisionMaking
{
	namespace FiniteStateMachine
	{
		StateMachine::StateMachine(IState* startState, Blackboard* pBlackboard) :
			m_pCurrentState{ startState },
			m_pBlackboard(pBlackboard)
		{
			m_pCurrentState->OnEnter(pBlackboard);
		}

		StateMachine::~StateMachine()
		{
			for (auto& pair : m_Transitions)
			{
				delete pair.first;
			}
		}

		bool StateMachine::AtState(IState* state) const
		{
			return m_pCurrentState == state;
		}

		void StateMachine::AddTransition(IState* start, IState* end, ICondition* connection)
		{
			// Is this a new state
			if (!m_Transitions.contains(start))
			{
				m_Transitions.emplace(std::make_pair(start, Transitions{}));
			}

			m_Transitions.at(start).emplace_back(std::make_pair(connection, end));
		}

		void StateMachine::Update(float deltaTime)
		{
			for (Transition& transition : m_Transitions.at(m_pCurrentState))
			{
				if (transition.first->Evaluate(m_pBlackboard))
				{
					ChangeState(transition.second);
					break;
				}
			}

			m_pCurrentState->Update(m_pBlackboard, deltaTime);
		}

		Blackboard* StateMachine::GetBlackboard() const
		{
			return m_pBlackboard;
		}

		void StateMachine::ChangeState(IState* newState)
		{
			m_pCurrentState->OnExit(m_pBlackboard);
			m_pCurrentState = newState;
			m_pCurrentState->OnEnter(m_pBlackboard);
		}
	}
}