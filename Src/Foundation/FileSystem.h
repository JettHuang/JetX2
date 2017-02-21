// \brief
//		FileSystem operations
//
#ifndef __JETX_FILESYSTEM_H__
#define __JETX_FILESYSTEM_H__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "JetX.h"
#include "OutputDevice.h"


// FileSystem
class FFileSystem
{
public:
	FFileSystem* SharedInstance();

	// set working root
	virtual void SetRootDir(const char *InRootDir);
	const std::string& RootDir() const { return mRootDir; }

	// read a text file
	virtual bool ReadTextFile(const char *InFileName, std::string &OutText, FOutputDevice *OutputDev);
	
private:
	std::string		mRootDir;
};


#endif // __JETX_FILESYSTEM_H__
