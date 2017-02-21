// \brief
//		Output Device
//

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "OutputDevice.h"

#define MAX_BUFFER_SIZE		2048


void FOutputDevice::Log(ELogVerbosity InVerbosity, const char* szFormat, ...)
{
	char szBuf[MAX_BUFFER_SIZE];

	va_list ap;
	va_start(ap, szFormat);
	vsnprintf(szBuf, sizeof(szBuf), szFormat, ap);
	va_end(ap);

	Serialize(InVerbosity, szBuf);
}

void FOutputConsole::Serialize(ELogVerbosity InVerbosity, const char* szMsg)
{
	switch (InVerbosity)
	{
	case Log_Error:
		std::cout << "Error: " << szMsg << std::endl;
		break;
	case Log_Warning:
		std::cout << "Warning: " << szMsg << std::endl;
		break;
	case Log_Info:
		std::cout << "Info: " << szMsg << std::endl;
		break;
	default:
		break;
	}
}

