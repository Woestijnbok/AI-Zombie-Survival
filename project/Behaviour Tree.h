#ifndef BEHAVIOUR_TREE
#define BEHAVIOUR_TREE

namespace DecisionMaking
{
	class Blackboard;

	namespace BehaviourTree
	{
		enum class State
		{
			Failure,
			Success,
			Running
		};

		class IBehaviour
		{
		public:
			IBehaviour();
			virtual ~IBehaviour() = default;

			IBehaviour(const IBehaviour&) = delete;
			IBehaviour& operator=(const IBehaviour&) = delete;
			IBehaviour(IBehaviour&&) = delete;
			IBehaviour& operator=(IBehaviour&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) = 0;

		protected:
			State m_CurrentState;
		};

		class IComposite : public IBehaviour
		{
		public:
			explicit IComposite(std::vector<IBehaviour*> childBehaviours);
			virtual ~IComposite();

			IComposite(const IComposite&) = delete;
			IComposite& operator=(const IComposite&) = delete;
			IComposite(IComposite&&) = delete;
			IComposite& operator=(IComposite&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override = 0;

		protected:
			std::vector<IBehaviour*> m_ChildBehaviours;
		};

		class Selector final : public IComposite
		{
		public:
			explicit Selector(std::vector<IBehaviour*> childBehaviours);
			virtual ~Selector() = default;

			Selector(const Selector&) = delete;
			Selector& operator=(const Selector&) = delete;
			Selector(Selector&&) = delete;
			Selector& operator=(Selector&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override;
		};

		class Sequence : public IComposite
		{
		public:
			explicit Sequence(std::vector<IBehaviour*> childBehaviours);
			virtual ~Sequence() = default;

			Sequence(const Sequence&) = delete;
			Sequence& operator=(const Sequence&) = delete;
			Sequence(Sequence&&) = delete;
			Sequence& operator=(Sequence&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override;
		};

		class PartialSequence final : public Sequence
		{
		public:
			explicit PartialSequence(std::vector<IBehaviour*> childBehaviours);
			virtual ~PartialSequence() = default;

			PartialSequence(const PartialSequence&) = delete;
			PartialSequence& operator=(const PartialSequence&) = delete;
			PartialSequence(PartialSequence&&) = delete;
			PartialSequence& operator=(PartialSequence&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override;

		private:
			unsigned int m_CurrentBehaviourIndex;
		};

		class Condition final : public IBehaviour
		{
		public:
			explicit Condition(std::function<bool(Blackboard*)> function);
			virtual ~Condition() = default;

			Condition(const Condition&) = delete;
			Condition& operator=(const Condition&) = delete;
			Condition(Condition&&) = delete;
			Condition& operator=(Condition&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override;

		private:
			std::function<bool(Blackboard*)> m_ConditionFunction;
		};

		class Action final : public IBehaviour
		{
		public:
			explicit Action(std::function<State(Blackboard*)> function);
			virtual ~Action() = default;

			Action(const Action&) = delete;
			Action& operator=(const Action&) = delete;
			Action(Action&&) = delete;
			Action& operator=(Action&&) = delete;

			virtual State Execute(Blackboard* pBlackBoard) override;

		private:
			std::function<State(Blackboard*)> m_ActionFunction;
		};

		class Tree final
		{
		public:
			explicit Tree(Blackboard* blackBoard, IBehaviour* rootBehaviour);
			virtual ~Tree();

			Tree(const Tree&) = delete;
			Tree& operator=(const Tree&) = delete;
			Tree(Tree&&) = delete;
			Tree& operator=(Tree&&) = delete;

			void Update(float deltaTime);
			Blackboard* GetBlackboard() const;

		private:
			State m_CurrentState;
			Blackboard* m_BlackBoard;
			IBehaviour* m_RootBehaviour;
		};
	}
}

#endif 