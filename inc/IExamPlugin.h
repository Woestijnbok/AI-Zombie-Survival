#pragma once
#include "IPluginBase.h"

struct SteeringPlugin_Output;
struct GameDebugParams;

class IExamPlugin :public IPluginBase
{
public:
	IExamPlugin() = default;
	virtual ~IExamPlugin() = default;

	IExamPlugin(const IExamPlugin&) = delete;
	IExamPlugin& operator=(const IExamPlugin&) = delete;
	IExamPlugin(IExamPlugin&&) = delete;
	IExamPlugin& operator=(IExamPlugin&&) = delete;

	virtual void InitGameDebugParams(GameDebugParams& params) = 0;
	virtual void Update_Debug(float dt) = 0;
	virtual SteeringPlugin_Output UpdateSteering(float dt) = 0;
	virtual void Render(float dt) const = 0;
};

