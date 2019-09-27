#include "stdafx.h"
#include "JLogger.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <io.h>
#include <direct.h>

#define MAX_BUF_SIZE  4096
#define COLOR_ERROR "\x1b[31m"

JLoggerClass& GetJLogger(const char *funcName, int line)
{
	static JLoggerClass logger;
	logger.SetFileName(funcName);
	logger.SetLine(line);
	return logger;
}

JLoggerClass::JLoggerClass()
{
	m_logFilePath = "None";
}


JLoggerClass::~JLoggerClass()
{
	if (m_fileOut.is_open())
	{
		m_fileOut.close();
	}
}

bool JLoggerClass::Init(const char* logCategory, const char* fileName)
{
	std::string logFolderName = "LOG_" + std::string(logCategory);
	m_logFilePath = logFolderName + "\\" + fileName;
	if (_access(logFolderName.c_str(), 0) == -1)
	{
		if (_mkdir(logFolderName.c_str()) == -1)
		{
			printf("log folder create fail");
			return false;
		}
	}
	m_fileOut.open(m_logFilePath.c_str());
	JLogger.Log("Log write start");
	return true;
}


void JLoggerClass::Log(const char *format, ...)
{
	va_list ap;
	char buf[MAX_BUF_SIZE];
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	char logbuf[MAX_BUF_SIZE];
	sprintf_s(logbuf, "[LOG][File:%s][Line:%d] %s\n", GetFuncName().c_str(), GetLine(), buf);
	printf(logbuf);
	m_fileOut << logbuf;

}
void JLoggerClass::Error(const char *format, ...)
{
	va_list ap;
	char buf[MAX_BUF_SIZE];
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000C | FOREGROUND_INTENSITY);
	char logbuf[MAX_BUF_SIZE];
	sprintf_s(logbuf, "[ERROR][File:%s][Line:%d] %s\n", GetFuncName().c_str(), GetLine(), buf);
	printf(logbuf);
	m_fileOut << logbuf;
}

const std::string& JLoggerClass::GetLogFileName()
{
	return m_logFilePath;
}