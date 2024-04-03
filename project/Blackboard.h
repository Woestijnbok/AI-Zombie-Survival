#ifndef BLACKBOARD
#define BLACKBOARD

#include <unordered_map>
#include <string>
#include <type_traits>

namespace DecisionMaking
{
	class IBlackBoardField
	{
	public:
		IBlackBoardField() = default;
		virtual ~IBlackBoardField() = default;

		IBlackBoardField(const IBlackBoardField&) = delete;
		IBlackBoardField& operator=(const IBlackBoardField&) = delete;
		IBlackBoardField(IBlackBoardField&&) = delete;
		IBlackBoardField& operator=(IBlackBoardField&&) = delete;
	};

	template<typename Type>
	class BlackboardField final : public IBlackBoardField
	{
	public:
		explicit BlackboardField(Type data) :
			m_Data{ std::make_unique<Type>(data) }
		{

		}

		Type GetData() const
		{
			return *m_Data.get();
		}

		void SetData(Type data)
		{ 
			m_Data.reset();
			m_Data = std::make_unique<Type>(data);
		}

	private:
		std::unique_ptr<Type> m_Data;
	};

	class Blackboard final
	{
	public:
		Blackboard() = default;
		~Blackboard()
		{
			for (auto& pair : m_Data)
			{
				SAFE_DELETE(pair.second);
			}
		}

		Blackboard(const Blackboard&) = delete;
		Blackboard& operator=(const Blackboard&) = delete;
		Blackboard(Blackboard&&) = delete;
		Blackboard& operator=(Blackboard&&) = delete;

		template<typename Type>
		void AddData(const std::string& key, Type data)
		{
			if (!m_Data.contains(key))m_Data.emplace(std::make_pair(key, new BlackboardField<Type>(data)));
			else std::cout << "Blackboard already has an element with " << key << "as the key" << std::endl;
		}

		template<typename Type>
		void ChangeData(const std::string& key, Type data)
		{
			if (m_Data.contains(key))
			{
				BlackboardField<Type>* pBlackBoardField{ dynamic_cast<BlackboardField<Type>*>(m_Data.at(key)) };

				if (pBlackBoardField) pBlackBoardField->SetData(data);
				else std::cout << "Blackboard couldn't change data because of type mismatch (key: " << key << ")" << std::endl;
			}
			else std::cout << "Blackboard has no element with " << key << "as the key" << std::endl;
		}

		template<typename Type>
		void GetData(const std::string& key, Type& data) const
		{
			if (m_Data.contains(key))
			{
				BlackboardField<Type>* pBlackBoardField{ dynamic_cast<BlackboardField<Type>*>(m_Data.at(key)) };

				if (pBlackBoardField) data = pBlackBoardField->GetData();
				else std::cout << "Blackboard couldn't get data because of type mismatch (key: " << key << ")" << std::endl;
			}
			else std::cout << "Blackboard has no element with " << key << "as the key" << std::endl;
		}

	private:
		std::unordered_map<std::string, IBlackBoardField*> m_Data;
	};
}

#endif 