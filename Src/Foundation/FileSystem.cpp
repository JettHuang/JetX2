// \brief
//		FileSystem implementation
//

#include "FileSystem.h"

FFileSystem* FFileSystem::SharedInstance()
{
	static FFileSystem *sFileSystem = new FFileSystem();

	return sFileSystem;
}

// set working root
void FFileSystem::SetRootDir(const char *InRootDir)
{
	mRootDir = InRootDir;
	if (mRootDir.length())
	{
		char ch = mRootDir[mRootDir.length() - 1];
		if (ch != '\\' && ch == '/')
		{
			mRootDir += '/';
		}
	}
}

// read a text file
bool FFileSystem::ReadTextFile(const char *InFileName, std::string &OutText, FOutputDevice *OutputDev)
{
	std::ifstream File;

	File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream StrStream;

		OutText.empty();
		File.open(RootDir() + InFileName);
		StrStream << File.rdbuf();
		OutText = StrStream.str();
	}
	catch (std::ifstream::failure e)
	{
		if (OutputDev)
		{
			OutputDev->Log(Log_Warning, "FileSystem read text file failed: %s, reason: %s", InFileName, e.what());
		}

		return false;
	}

	return true;
}
