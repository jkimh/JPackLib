#pragma once
/*
�α׸� ����� ���� ���̺귯��
����, �ܼ�â ��� ����
���� :
 - �ش� ������Ʈ�� �����Ͽ� �α� ����� ����� �� �־���Ѵ�. 
	(���� �ڷ��� ���� ������Ʈ �� ������Ʈ ���� ����)
*/
#include <fstream>
class JLoggerClass
{
public:
	JLoggerClass();
	~JLoggerClass();

	bool Init(const char* logCategory, const char* fileName); //ī�װ������� �α����� �̸��� �����.
	void Log(const char *format, ...);
	void Error(const char *format, ...);
	const std::string& GetLogFileName();

	inline void SetFileName(const char* funcName)
	{
		m_funcName = funcName;
	}
	inline std::string GetFuncName()
	{
		return m_funcName;
	}
	inline void SetLine(int line)
	{
		m_line = line;
	}
	inline int GetLine()
	{
		return m_line;
	}

private:
	std::ofstream m_fileOut;
	std::string m_funcName;
	std::string m_logFilePath;
	int m_line;
};

JLoggerClass& GetJLogger(const char *funcName, int line);


#define JLogger GetJLogger(__FILE__, __LINE__)