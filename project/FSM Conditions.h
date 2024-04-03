#ifndef FINITE_STATE_MACHINE_CONDITIONS
#define FINITE_STATE_MACHINE_CONDITIONS

namespace DecisionMaking
{
	class Blackboard;

	namespace FiniteStateMachine
	{
		class ICondition
		{
		public:
			ICondition() = default;
			virtual ~ICondition() = default;

			ICondition(const ICondition&) = delete;
			ICondition& operator=(const ICondition&) = delete;
			ICondition(ICondition&&) = delete;
			ICondition& operator=(ICondition&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const = 0;
		};

		class InDanger final : public ICondition
		{
		public:
			InDanger() = default;
			virtual ~InDanger() = default;

			InDanger(const InDanger&) = delete;
			InDanger& operator=(const InDanger&) = delete;
			InDanger(InDanger&&) = delete;
			InDanger& operator=(InDanger&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class CheckedBehind final : public ICondition
		{
		public:
			CheckedBehind() = default;
			virtual ~CheckedBehind() = default;

			CheckedBehind(const CheckedBehind&) = delete;
			CheckedBehind& operator=(const CheckedBehind&) = delete;
			CheckedBehind(CheckedBehind&&) = delete;
			CheckedBehind operator=(CheckedBehind&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class NeedToCheckBehind final : public ICondition
		{
		public:
			NeedToCheckBehind() = default;
			virtual ~NeedToCheckBehind() = default;

			NeedToCheckBehind(const NeedToCheckBehind&) = delete;
			NeedToCheckBehind& operator=(const NeedToCheckBehind&) = delete;
			NeedToCheckBehind(NeedToCheckBehind&&) = delete;
			NeedToCheckBehind operator=(NeedToCheckBehind&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class IsSafe final : public ICondition
		{
		public:
			IsSafe() = default;
			virtual ~IsSafe() = default;

			IsSafe(const IsSafe&) = delete;
			IsSafe& operator=(const IsSafe&) = delete;
			IsSafe(IsSafe&&) = delete;
			IsSafe& operator=(IsSafe&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class SafePointReached final : public ICondition
		{
		public:
			SafePointReached() = default;
			virtual ~SafePointReached() = default;

			SafePointReached(const SafePointReached&) = delete;
			SafePointReached& operator=(const SafePointReached&) = delete;
			SafePointReached(SafePointReached&&) = delete;
			SafePointReached& operator=(SafePointReached&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class FoundUnexploredHouse final : public ICondition
		{
		public:
			FoundUnexploredHouse() = default;
			virtual ~FoundUnexploredHouse() = default;

			FoundUnexploredHouse(const FoundUnexploredHouse&) = delete;
			FoundUnexploredHouse& operator=(const FoundUnexploredHouse&) = delete;
			FoundUnexploredHouse(FoundUnexploredHouse&&) = delete;
			FoundUnexploredHouse& operator=(FoundUnexploredHouse&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class InsideHouse final : public ICondition
		{
		public:
			InsideHouse() = default;
			virtual ~InsideHouse() = default;

			InsideHouse(const InsideHouse&) = delete;
			InsideHouse& operator=(const InsideHouse&) = delete;
			InsideHouse(InsideHouse&&) = delete;
			InsideHouse& operator=(InsideHouse&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class PickableItemInSight final : public ICondition
		{
		public:
			PickableItemInSight() = default;
			virtual ~PickableItemInSight() = default;

			PickableItemInSight(const PickableItemInSight&) = delete;
			PickableItemInSight& operator=(const PickableItemInSight&) = delete;
			PickableItemInSight(PickableItemInSight&&) = delete;
			PickableItemInSight& operator=(PickableItemInSight&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class GotTargetItem final : public ICondition
		{
		public:
			GotTargetItem() = default;
			virtual ~GotTargetItem() = default;

			GotTargetItem(const GotTargetItem&) = delete;
			GotTargetItem& operator=(const GotTargetItem&) = delete;
			GotTargetItem(GotTargetItem&&) = delete;
			GotTargetItem operator=(GotTargetItem&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class ExploredHouse final : public ICondition
		{
		public:
			ExploredHouse() = default;
			virtual ~ExploredHouse() = default;

			ExploredHouse(const ExploredHouse&) = delete;
			ExploredHouse& operator=(const ExploredHouse&) = delete;
			ExploredHouse(ExploredHouse&&) = delete;
			ExploredHouse& operator=(ExploredHouse&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class OutsideHouse final : public ICondition
		{
		public:
			OutsideHouse() = default;
			virtual ~OutsideHouse() = default;

			OutsideHouse(const OutsideHouse&) = delete;
			OutsideHouse& operator=(const OutsideHouse&) = delete;
			OutsideHouse(OutsideHouse&&) = delete;
			OutsideHouse& operator=(OutsideHouse&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};

		class ZoneInSight final : public ICondition
		{
		public:
			ZoneInSight() = default;
			virtual ~ZoneInSight() = default;

			ZoneInSight(const ZoneInSight&) = delete;
			ZoneInSight& operator=(const ZoneInSight&) = delete;
			ZoneInSight(ZoneInSight&&) = delete;
			ZoneInSight operator=(ZoneInSight&&) = delete;

			virtual bool Evaluate(Blackboard* pBlackboard) const override;
		};
	}
}

#endif 