#pragma once

struct PluginInfo;
class IBaseInterface;

class IPluginBase
{
public:
	IPluginBase() = default;
	virtual ~IPluginBase() = default;

	IPluginBase(const IPluginBase&) = delete;
	IPluginBase& operator=(const IPluginBase&) = delete;
	IPluginBase(IPluginBase&&) = delete;
	IPluginBase& operator=(IPluginBase&&) = delete;

	virtual void Initialize(IBaseInterface *pInterface, PluginInfo& info) = 0;
	virtual void DllInit() = 0;
	virtual void DllShutdown() = 0;
};

