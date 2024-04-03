#ifndef STEERING_BEHAVIOURS
#define STEERING_BEHAVIOURS

#include "Exam_HelperStructs.h"

class IExamInterface;

namespace MovementBehavior
{
	struct TargetData final
	{
		Elite::Vector2 position;
		Elite::Vector2 velocity;
	};

	class ISteeringBehavior
	{
	public:
		ISteeringBehavior() = default;
		virtual ~ISteeringBehavior() = default;

		ISteeringBehavior(const ISteeringBehavior& other) = delete;
		ISteeringBehavior& operator=(const ISteeringBehavior& other) = delete;
		ISteeringBehavior(ISteeringBehavior&& other) = delete;
		ISteeringBehavior& operator=(ISteeringBehavior&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) = 0;
		static void SetPathfindingFunction(IExamInterface* pInterface, std::function<Elite::Vector2(IExamInterface*, Elite::Vector2)> function);

	protected:
		static std::function<Elite::Vector2(Elite::Vector2)> m_PathfindingFunction;
		static void OrientTo(float& angularVelocity, const AgentInfo& agentInfo, const Elite::Vector2& point);
	};

	class Seek final : public ISteeringBehavior
	{
	public:
		Seek() = default;
		virtual ~Seek() = default;

		Seek(const Seek& other) = delete;
		Seek& operator=(const Seek& other) = delete;
		Seek(Seek&& other) = delete;
		Seek& operator=(Seek&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
	};

	class Flee final : public ISteeringBehavior
	{
	public:
		Flee() = default;
		virtual ~Flee() = default;

		Flee(const Flee& other) = delete;
		Flee& operator=(const Flee& other) = delete;
		Flee(Flee&& other) = delete;
		Flee& operator=(Flee&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
	};

	class Arrive final : public ISteeringBehavior
	{
	public:
		Arrive(float slowRadius = 15.0f, float targetRadius = 3.0f);
		virtual ~Arrive() = default;

		Arrive(const Arrive& other) = delete;
		Arrive& operator=(const Arrive& other) = delete;
		Arrive(Arrive&& other) = delete;
		Arrive& operator=(Arrive&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
		void SetTargetRadius(float radius);
		void SetSlowRadius(float radius);

	private:
		float m_SlowRadius;
		float m_TargetRadius;
	};

	class Pursuit final : public ISteeringBehavior
	{
	public:
		Pursuit() = default;
		virtual ~Pursuit() = default;

		Pursuit(const Pursuit& other) = delete;
		Pursuit& operator=(const Pursuit& other) = delete;
		Pursuit(Pursuit&& other) = delete;
		Pursuit& operator=(Pursuit&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
	};

	class Evade final : public ISteeringBehavior
	{
	public:
		Evade() = default;
		virtual ~Evade() = default;

		Evade(const Evade& other) = delete;
		Evade& operator=(const Evade& other) = delete;
		Evade(Evade&& other) = delete;
		Evade& operator=(Evade&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
	};

	class Wander final : public ISteeringBehavior
	{
	public:
		Wander(float offset = 6.0f, float radius = 4.0f, float maxAngleChangeDegrees = Elite::ToRadians(70.0f));
		virtual ~Wander() = default;

		Wander(const Wander& other) = delete;
		Wander& operator=(const Wander& other) = delete;
		Wander(Wander&& other) = delete;
		Wander& operator=(Wander&& other) = delete;

		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
		void SetWanderOffset(float offset);
		void SetWanderRadius(float radius);
		void SetMaxAngleChange(float radians);
		void SetWanderAngle(float angle);

	private:
		float m_WanderOffset;
		float m_WanderRadius;
		float m_MaxAngleChange;
		float m_WanderAngle;
	};

	class BlendedSteering final : public ISteeringBehavior
	{
	public:
		BlendedSteering(std::vector<std::pair<ISteeringBehavior*, float>> weightedBehaviors);
		virtual ~BlendedSteering();

		BlendedSteering(const BlendedSteering& other) = delete;
		BlendedSteering& operator=(const BlendedSteering& other) = delete;
		BlendedSteering(BlendedSteering&& other) = delete;
		BlendedSteering& operator=(BlendedSteering&& other) = delete;

		void AddBehaviour(std::pair<ISteeringBehavior*, float> weightedBehavior);
		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;
		std::vector<std::pair<ISteeringBehavior*, float>>& GetWeightedBehaviorsReference();

	private:
		std::vector<std::pair<ISteeringBehavior*, float>> m_WeightedBehaviors;
	};

	class PrioritySteering final : public ISteeringBehavior
	{
	public:
		PrioritySteering(std::vector<ISteeringBehavior*> steeringBehaviors);
		virtual ~PrioritySteering();

		PrioritySteering(const PrioritySteering& other) = delete;
		PrioritySteering& operator=(const PrioritySteering& other) = delete;
		PrioritySteering(PrioritySteering&& other) = delete;
		PrioritySteering& operator=(PrioritySteering&& other) = delete;

		void AddBehaviour(ISteeringBehavior* steeringBehavior);
		virtual SteeringPlugin_Output CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData) override;

	private:
		std::vector<ISteeringBehavior*> m_SteeringBehaviors;
	};
}

#endif