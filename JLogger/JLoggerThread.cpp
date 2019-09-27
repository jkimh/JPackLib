#include "stdafx.h"
#include "JLoggerThread.h"


JLoggerThread::JLoggerThread()
{
}


JLoggerThread::~JLoggerThread()
{
}

void JLoggerThread::Run()
{
	
}

void JLoggerThread::LoopLogging()
{
	MoveQueue();
	ProcessLogging();
}

void JLoggerThread::MoveQueue()
{
	if (!m_queueMutex.try_lock())
		return;
	//ť�� �ű��.
	int prevQueueSize = m_loginfoPrevQueue.size();
	for (int i = 0; i < prevQueueSize; i++)
	{
		m_loginfoMoveQueue.push(m_loginfoPrevQueue.front());
		m_loginfoPrevQueue.pop();
	}
	m_queueMutex.unlock();
}

void JLoggerThread::ProcessLogging()
{
	int queueSize = m_loginfoMoveQueue.size();
	for (int i = 0; i < queueSize; i++)
	{
		auto &LogInfo = m_loginfoMoveQueue.front();
		Log(LogInfo);
		m_loginfoPrevQueue.pop();
	}
}

void JLoggerThread::Log(const JLoggerLogInfo& logInfo)
{
	//PrintConsole(logInfo);
}

//�ܼ��� ���� ť�� �̿����� �ʰ� �ٷ� �ֿܼ� ���� (����� �뵵�� ũ�⶧��)
void JLoggerThread::PrintConsole(const JLoggerLogInfo& logInfo)
{	
	switch (logInfo.logLv)
	{
		case LOG_ERROR :
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LOG_INFO :
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case LOG_CRITICAL :
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		default:
			break;
	}
	printf("[LOG][func:%s, line:%d] %s", logInfo.funcName.c_str(), logInfo.funcLine, logInfo.logString.c_str());
}