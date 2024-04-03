#ifndef SURVIVAL_AGENT_PLUGIN
#define SURVIVAL_AGENT_PLUGIN

#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

class IBaseInterface;
class IExamInterface;

namespace DecisionMaking
{
	class Blackboard;

	namespace FiniteStateMachine
	{
		class IState;
		class StateMachine;
	}

	namespace BehaviourTree 
	{ 
		class Tree; 
	}
}

namespace MovementBehavior
{
	class ISteeringBehavior;
}

class SurvivalAgentPlugin final : public IExamPlugin
{
	public:
		SurvivalAgentPlugin() = default;
		virtual ~SurvivalAgentPlugin() = default;

		SurvivalAgentPlugin(const SurvivalAgentPlugin&) = delete;
		SurvivalAgentPlugin& operator=(SurvivalAgentPlugin&) = delete;
		SurvivalAgentPlugin(const SurvivalAgentPlugin&&) = delete;
		SurvivalAgentPlugin& operator=(SurvivalAgentPlugin&&) = delete;

		virtual void Initialize(IBaseInterface* pInterface, PluginInfo& pluginInfo) override;
		virtual void DllInit() override;
		virtual void DllShutdown() override;
		virtual void InitGameDebugParams(GameDebugParams& debugParameters) override;
		virtual void Update_Debug(float deltaTime) override;
		virtual SteeringPlugin_Output UpdateSteering(float deltaTime) override;
		virtual void Render(float deltaTime) const override;

	private:
		IExamInterface* m_Interface;
		DecisionMaking::Blackboard* m_Blackboard;
		DecisionMaking::FiniteStateMachine::StateMachine* m_ExplorationFiniteStateMachine;
		DecisionMaking::BehaviourTree::Tree* m_InventoryBehaviourTree;	
		float m_CurrentDifficultyLevel;

		// Exploration States, also stored here for rendering purposes (also stored in the blackboard)
		DecisionMaking::FiniteStateMachine::IState* m_Roam;
		DecisionMaking::FiniteStateMachine::IState* m_Escape;
		DecisionMaking::FiniteStateMachine::IState* m_SafeSeek;
		DecisionMaking::FiniteStateMachine::IState* m_LookBehind;
		DecisionMaking::FiniteStateMachine::IState* m_ExploreHouse;
		DecisionMaking::FiniteStateMachine::IState* m_LeaveHouse;
		DecisionMaking::FiniteStateMachine::IState* m_GetItem;
		DecisionMaking::FiniteStateMachine::IState* m_LeaveZone;
		DecisionMaking::FiniteStateMachine::IState* m_GetInsideUnexploredHouse;

		// Update our pch
		// Use blended steering when working with wandering and seek to the middle

		void CreateBlackboard();
		void UpdateBlackboard(float deltaTime);
};

extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new SurvivalAgentPlugin();
	}
}

#endif