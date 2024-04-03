#ifndef FINITE_STATE_MACHINE
#define FINITE_STATE_MACHINE

#include <vector>
#include <map>

namespace DecisionMaking
{
	class Blackboard;

	namespace FiniteStateMachine
	{
		class IState;
		class ICondition;

		class StateMachine final
		{
		public:
			StateMachine(IState* startState, Blackboard* pBlackboard);
			~StateMachine();

			StateMachine(const StateMachine&) = delete;
			StateMachine& operator=(const StateMachine&) = delete;
			StateMachine(StateMachine&&) = delete;
			StateMachine& operator=(StateMachine&&) = delete;

			void AddTransition(IState* start, IState* end, ICondition* connection);
			void Update(float deltaTime);
			Blackboard* GetBlackboard() const;
			bool AtState(IState* state) const;

		private:
			typedef std::pair<ICondition*, IState*> Transition;
			typedef std::vector<Transition> Transitions;

			std::map<IState*, Transitions> m_Transitions;
			IState* m_pCurrentState;
			Blackboard* m_pBlackboard;

			void ChangeState(IState* newState);
		};
	}
}

#endif 