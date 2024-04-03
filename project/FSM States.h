#ifndef FINITE_STATE_MACHINE_STATES
#define FINITE_STATE_MACHINE_STATES

namespace DecisionMaking
{
	class Blackboard;

	namespace FiniteStateMachine
	{
		class IState
		{
		public:
			IState() = default;
			virtual ~IState() = default;

			IState(const IState&) = delete;
			IState& operator=(const IState&) = delete;
			IState(IState&&) = delete;
			IState& operator=(IState&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const = 0;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const = 0;
			virtual void OnExit(Blackboard* pBlackboard) const = 0;
		};

		class Escape final : public IState
		{
		public:
			Escape() = default;
			virtual ~Escape() = default;

			Escape(const Escape&) = delete;
			Escape& operator=(const Escape&) = delete;
			Escape(Escape&&) = delete;
			Escape& operator=(Escape&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class Roam final : public IState
		{
		public:
			Roam() = default;
			virtual ~Roam() = default;

			Roam(const Roam&) = delete;
			Roam& operator=(const Roam&) = delete;
			Roam(Roam&&) = delete;
			Roam& operator=(Roam&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class LookBehind final : public IState
		{
		public:
			LookBehind() = default;
			virtual ~LookBehind() = default;

			LookBehind(const LookBehind&) = delete;
			LookBehind& operator=(const LookBehind&) = delete;
			LookBehind(LookBehind&&) = delete;
			LookBehind& operator=(LookBehind&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class SafeSeek final : public IState
		{
		public:
			SafeSeek() = default;
			virtual ~SafeSeek() = default;

			SafeSeek(const SafeSeek&) = delete;
			SafeSeek& operator=(const SafeSeek&) = delete;
			SafeSeek(SafeSeek&&) = delete;
			SafeSeek& operator=(SafeSeek&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class GetInsideUnexploredHouse final : public IState
		{
		public:
			GetInsideUnexploredHouse() = default;
			virtual ~GetInsideUnexploredHouse() = default;

			GetInsideUnexploredHouse(const GetInsideUnexploredHouse&) = delete;
			GetInsideUnexploredHouse& operator=(const GetInsideUnexploredHouse&) = delete;
			GetInsideUnexploredHouse(GetInsideUnexploredHouse&&) = delete;
			GetInsideUnexploredHouse& operator=(GetInsideUnexploredHouse&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class ExploreHouse final : public IState
		{
		public:
			ExploreHouse() = default;
			virtual ~ExploreHouse() = default;

			ExploreHouse(const ExploreHouse&) = delete;
			ExploreHouse& operator=(const ExploreHouse&) = delete;
			ExploreHouse(ExploreHouse&&) = delete;
			ExploreHouse& operator=(ExploreHouse&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class GetItem final : public IState
		{
		public:
			GetItem() = default;
			virtual ~GetItem() = default;

			GetItem(const GetItem&) = delete;
			GetItem& operator=(const GetItem&) = delete;
			GetItem(GetItem&&) = delete;
			GetItem& operator=(GetItem&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class LeaveHouse final : public IState
		{
		public:
			LeaveHouse() = default;
			virtual ~LeaveHouse() = default;

			LeaveHouse(const LeaveHouse&) = delete;
			LeaveHouse& operator=(const LeaveHouse&) = delete;
			LeaveHouse(LeaveHouse&&) = delete;
			LeaveHouse& operator=(LeaveHouse&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};

		class RunAwayFromZone final : public IState
		{
		public:
			RunAwayFromZone() = default;
			virtual ~RunAwayFromZone() = default;

			RunAwayFromZone(const RunAwayFromZone&) = delete;
			RunAwayFromZone& operator=(const RunAwayFromZone&) = delete;
			RunAwayFromZone(RunAwayFromZone&&) = delete;
			RunAwayFromZone& operator=(RunAwayFromZone&&) = delete;

			virtual void OnEnter(Blackboard* pBlackboard) const override;
			virtual void Update(Blackboard* pBlackboard, float deltaTime) const override;
			virtual void OnExit(Blackboard* pBlackboard) const override;
		};
	}
}

#endif