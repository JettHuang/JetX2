//\brief
//		Output Device
//

#ifndef __JETX_OUTPUT_DEVICE_H__
#define __JETX_OUTPUT_DEVICE_H__


enum ELogVerbosity
{
	Log_Error,
	Log_Warning,
	Log_Info
};

class FOutputDevice
{
public:
	FOutputDevice()
	{}

	virtual ~FOutputDevice()
	{}

	virtual void Log(ELogVerbosity InVerbosity, const char* szFormat, ...);

	virtual void Serialize(ELogVerbosity InVerbosity, const char* szMsg) = 0;
};

class FOutputConsole : public FOutputDevice
{
public:
	FOutputConsole()
	{}

	void Serialize(ELogVerbosity InVerbosity, const char* szMsg) override;
};

#endif // __JETX_OUTPUT_DEVICE_H__
