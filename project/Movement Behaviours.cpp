#include "stdafx.h"
#include "Movement Behaviours.h"
#include "IExamInterface.h"
#include <numeric>

namespace MovementBehavior
{
#pragma region ISteeringBehavior
	std::function<Elite::Vector2(Elite::Vector2)> ISteeringBehavior::m_PathfindingFunction{};

	void ISteeringBehavior::SetPathfindingFunction(IExamInterface* pInterface, std::function<Elite::Vector2(IExamInterface*, Elite::Vector2)> function)
	{
		m_PathfindingFunction = std::bind(function, pInterface, std::placeholders::_1);
	}

	void ISteeringBehavior::OrientTo(float& angularVelocity, const AgentInfo& agentInfo, const Elite::Vector2& point)
	{
		const Elite::Vector2 direction{ (point - agentInfo.Position) };
		const float orientationDifference{ Elite::AngleBetween(direction, Elite::OrientationToVector(agentInfo.Orientation)) };

		if (!Elite::AreEqual(orientationDifference, 0.0f))
		{
			if (orientationDifference > 0.0f) angularVelocity = -agentInfo.MaxAngularSpeed;
			else angularVelocity = agentInfo.MaxAngularSpeed;
		}
		else angularVelocity = 0.0f;
	}
#pragma endregion

#pragma region Seek
	SteeringPlugin_Output Seek::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move towards the target
		const Elite::Vector2 targetPosition{ m_PathfindingFunction(targetData.position) };						// Make this a reachable point
		const Elite::Vector2 direction{ (targetPosition - agentInfo.Position).GetNormalized() };				// Towards the target position
		steering.LinearVelocity = direction * agentInfo.MaxLinearSpeed;

		// Look at the direction you are going
		steering.AngularVelocity = 0.0f;
		steering.AutoOrient = true;

		return steering;
	}
#pragma endregion

#pragma region Flee
	SteeringPlugin_Output Flee::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move away from the target
		Elite::Vector2 direction{ (agentInfo.Position - targetData.position).GetNormalized() };
		Elite::Vector2 targetPosition{ agentInfo.Position + (direction * 6.0f) };
		targetPosition = m_PathfindingFunction(targetPosition);
		direction = (targetPosition - agentInfo.Position).GetNormalized();
		steering.LinearVelocity = direction * agentInfo.MaxLinearSpeed;

		// Look towards the target
		OrientTo(steering.AngularVelocity, agentInfo, targetData.position);
		steering.AutoOrient = false;

		return steering;
	}
#pragma endregion

#pragma region Arrive
	Arrive::Arrive(float slowRadius, float targetRadius) :
		ISteeringBehavior(),
		m_SlowRadius{ slowRadius },
		m_TargetRadius{ targetRadius }
	{

	}

	SteeringPlugin_Output Arrive::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move towards the target
		Elite::Vector2 direction{ (targetData.position - agentInfo.Position).GetNormalized() };
		Elite::Vector2 targetPosition{ agentInfo.Position + (direction * 1.0f) };
		targetPosition = m_PathfindingFunction(targetPosition);
		direction = (targetPosition - agentInfo.Position).GetNormalized();

		// Move faster when further away
		const float distance{ targetData.position.Distance(agentInfo.Position) };
		float speed{ 0.0f };
		if (distance < m_SlowRadius)
		{
			speed = agentInfo.MaxLinearSpeed * ((distance - m_TargetRadius) / (m_SlowRadius - m_TargetRadius));
		}
		else
		{
			speed = agentInfo.MaxLinearSpeed;
		}
		steering.LinearVelocity = direction.GetNormalized() * speed;

		// Look towards where you are going
		steering.AngularVelocity = 0.0f;
		steering.AutoOrient = true;

		return steering;
	}

	void Arrive::SetTargetRadius(float radius)
	{
		m_TargetRadius = radius;
	}

	void Arrive::SetSlowRadius(float radius)
	{
		m_SlowRadius = radius;
	}
#pragma endregion

#pragma region Pursuit
	SteeringPlugin_Output Pursuit::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move towards where the target is going to be
		const float distance{ targetData.position.Distance(agentInfo.Position) };						// Between target position and current position
		const float time{ distance / agentInfo.MaxLinearSpeed };										// Time it takes us to travel that distance
		Elite::Vector2 predictedPosition{ targetData.position + (time * targetData.velocity) };			// Where the target will be after that delta time
		predictedPosition = m_PathfindingFunction(predictedPosition);									// Make sure this is a point on the nav mesh
		const Elite::Vector2 direction{ (predictedPosition - agentInfo.Position).GetNormalized() };		// Towards this predicted position
		steering.LinearVelocity = direction * agentInfo.MaxLinearSpeed;

		// Look at where you are going
		steering.AngularVelocity = 0.0f;
		steering.AutoOrient = true;

		return steering;
	}
#pragma endregion

#pragma region Evade
	SteeringPlugin_Output Evade::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move away from where the target is going
		const float distance{ targetData.position.Distance(agentInfo.Position) };						    // Between target position and current position
		const float time{ distance / agentInfo.MaxLinearSpeed };											// Time it takes us to travel that distance
		Elite::Vector2 predictedPosition{ targetData.position + (targetData.velocity * time) };				// Where the target will be after that delta time
		predictedPosition = m_PathfindingFunction(predictedPosition);										// Make sure this is a point on the nav mesh
		const Elite::Vector2 direction{ (agentInfo.Position - predictedPosition).GetNormalized() };			// Away from this predicted position
		steering.LinearVelocity = direction * agentInfo.MaxLinearSpeed;

		// Look at where you are going
		OrientTo(steering.AngularVelocity, agentInfo, targetData.position);
		steering.AutoOrient = false;

		return steering;
	}
#pragma endregion

#pragma region Wander
	Wander::Wander(float offset, float radius, float maxAngleChangeDegrees) :
		ISteeringBehavior(),
		m_WanderOffset{ offset },
		m_WanderRadius{ radius },
		m_MaxAngleChange{ Elite::ToRadians(maxAngleChangeDegrees) },
		m_WanderAngle{ 0.0f }
	{

	}

	SteeringPlugin_Output Wander::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output steering{};

		// Move towards a random position, but withtin a certain range away from you
		const Elite::Vector2 directionAgent{ agentInfo.LinearVelocity.GetNormalized() };							// The current direction we are facing
		const Elite::Vector2 centerCircle{ agentInfo.Position + (directionAgent * m_WanderOffset) };				// The center of the "wander" circle in front of us
		m_WanderAngle += Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange);									// A random angle change between set values
		Elite::Vector2 targetPosition																				// A point on on the wander circle using the random angle
		{
			centerCircle.x + m_WanderRadius * cosf(m_WanderAngle),
			centerCircle.y + m_WanderRadius * sinf(m_WanderAngle)
		};
		targetPosition = m_PathfindingFunction(targetPosition);														// Make sure this point is on the nav mesh
		const Elite::Vector2 direction{ (targetPosition - agentInfo.Position).GetNormalized() };					// Towards this point on the circle	
		steering.LinearVelocity = direction * agentInfo.MaxLinearSpeed;

		// Look towards where you are going
		steering.AngularVelocity = 0.0f;
		steering.AutoOrient = true;

		return steering;
	}

	void Wander::SetWanderOffset(float offset)
	{
		m_WanderOffset = offset;
	}

	void Wander::SetWanderRadius(float radius)
	{
		m_WanderRadius = radius;
	}

	void Wander::SetMaxAngleChange(float radians)
	{
		m_MaxAngleChange = radians;
	}

	void Wander::SetWanderAngle(float angle)
	{
		m_WanderAngle = angle;
	}
#pragma endregion

#pragma region BlendedSteering
	BlendedSteering::BlendedSteering(std::vector<std::pair<ISteeringBehavior*, float>> weightedBehaviors) :
		ISteeringBehavior(),
		m_WeightedBehaviors{ weightedBehaviors }
	{

	}

	BlendedSteering::~BlendedSteering()
	{
		for (std::pair<ISteeringBehavior*, float>& weightedBehavior : m_WeightedBehaviors) delete weightedBehavior.first;
	}

	void BlendedSteering::AddBehaviour(std::pair<ISteeringBehavior*, float> weightedBehavior)
	{
		m_WeightedBehaviors.push_back(weightedBehavior);
	}

	SteeringPlugin_Output BlendedSteering::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		SteeringPlugin_Output blendedSteering{};
		float totalWeight{};

		for (std::pair<ISteeringBehavior*, float>& weightedBehavior : m_WeightedBehaviors)
		{
			const SteeringPlugin_Output steering{ weightedBehavior.first->CalculateSteering(deltaT, agentInfo, targetData) };

			blendedSteering.LinearVelocity += steering.LinearVelocity * weightedBehavior.second;
			blendedSteering.AngularVelocity += steering.AngularVelocity * weightedBehavior.second;
			totalWeight += weightedBehavior.second;
		}

		if (totalWeight > 0.0f)
		{
			blendedSteering.LinearVelocity /= totalWeight;
			blendedSteering.AngularVelocity /= totalWeight;
		}

		return blendedSteering;
	}

	std::vector<std::pair<ISteeringBehavior*, float>>& BlendedSteering::GetWeightedBehaviorsReference()
	{
		return m_WeightedBehaviors;
	}
#pragma endregion

#pragma region PrioritySteering // Disabled since we don't use it and are not planning on
	PrioritySteering::PrioritySteering(std::vector<ISteeringBehavior*> steeringBehaviors) :
		ISteeringBehavior(),
		m_SteeringBehaviors{ steeringBehaviors }
	{

	}

	PrioritySteering::~PrioritySteering()
	{
		for (ISteeringBehavior* pBehavior : m_SteeringBehaviors) delete pBehavior;
	}

	void PrioritySteering::AddBehaviour(ISteeringBehavior* steeringBehavior)
	{
		m_SteeringBehaviors.push_back(steeringBehavior);
	}

	SteeringPlugin_Output PrioritySteering::CalculateSteering(float deltaT, const AgentInfo& agentInfo, const TargetData& targetData)
	{
		/*SteeringPlugin_Output prioritySteering{};

		for (SteeringBehavior* pBehavior : m_SteeringBehaviors)
		{
			prioritySteering = pBehavior->CalculateSteering(deltaT, agentInfo, targetData);
			if (steering.IsValid) break;
		}

		return prioritySteering;*/

		return m_SteeringBehaviors.at(0)->CalculateSteering(deltaT, agentInfo, targetData);
	}
#pragma endregion
}