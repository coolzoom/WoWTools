#pragma once
#include "base.h"
#include "COSInfo.h"
#include "CTimer.h"
#include "CInputReader.h"

//
bool createEngine(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode);

void destroyEngine();

class Engine
{
public:
	explicit Engine(const SWindowInfo& wndInfo);
	~Engine();

public:
	void setMessageHandler(IMessageHandler* messageHandler) { MessageHandler = messageHandler; }
	IMessageHandler* getMessageHandler() { return MessageHandler; }

public:
	const SWindowInfo& getWindowInfo() const { return WindowInfo; }
	const COSInfo&	getOSInfo() const { return OSInfo; }
	CInputReader* getInputReader() { return &InputReader; }

private:
	SWindowInfo		WindowInfo;
	COSInfo		OSInfo;
	CTimer		Timer;
	CInputReader		InputReader;
	IMessageHandler*		MessageHandler;
};

extern Engine* g_Engine;