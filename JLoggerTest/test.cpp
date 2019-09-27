#include "pch.h"
#include "../JLogger/JLogger.h"
#include "../JSharedLib/JSharedFunc.h"

//파일 존재 여부 확인
TEST(LogTest, CreateFile)
{
	JLogger.Init("TEST", JSharedFunc::GetLogFileName("JLoggerTest").c_str());

	auto logFileName = JLogger.GetLogFileName();
	std::ifstream in(logFileName);
	EXPECT_NE(_access(logFileName.c_str(), 0), -1);
}
/*
TEST(LogTest, LogLine)
{
	const int testCount = 4;
	const char testStr[testCount][16] = {
		"testline 1",
		"testline 2",
		"testline 3",
		"testline 4",
	};

	auto logFileName = JLogger.GetLogFileName();
	if (logFileName.compare("None") == 0)
	{
		JLogger.Init("TEST", GetLogFileName().c_str());
		logFileName = JLogger.GetLogFileName();
	}
	std::ifstream in(logFileName);
	EXPECT_EQ(in.is_open(), true); //파일 존재 여부 확인
	in.seekg(std::ios::beg);
	std::string lineBuf;
	for (int i = 0; i < testCount; i++)
	{
		//JLogger.Log(testStr[i]);
		getline(in, lineBuf);
		EXPECT_NE(lineBuf.find(testStr[i]), std::string::npos);
		
	}
}
*/
