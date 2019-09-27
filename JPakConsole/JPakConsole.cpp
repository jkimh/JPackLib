#include "pch.h"
#include <iostream>
#include <io.h>
#include <direct.h>
#include "JLogger.h"
#include "FileSystem.h"
#include "FileSystemUtil.h"
#include "JSharedConst.h"
#include "JSharedFunc.h"

bool IsPakFile(const std::string& filePath);
void PrintMenual();
int main(int argc, char* argv[])
{
	JLogger.Init("JPAK_CONSOLE", JSharedFunc::GetLogFileName("JPak_Console").c_str());
	if (argc < 2)
	{
		JLogger.Error("��� ������ �Է����ּ���. (��ɾ� ���� : -help)");
		return 0;
	}

	const char* helpCommand = "-help";
	const char* mergeCommand = "-merge";
	if (strncmp(argv[1], helpCommand, strlen(helpCommand)) == 0)
	{
		PrintMenual();
		return 0;
	}

	char curDir[FILE_PATH_LEN];
	_getcwd(curDir, FILE_PATH_LEN);

	if (strncmp(argv[1], mergeCommand, strlen(mergeCommand)) == 0)
	{
		//merge
		std::string sourceFilePath = std::string(curDir) + "\\" + argv[2];
		if (_access(argv[2], 0) != -1)
		{
			sourceFilePath = argv[2];
		}
		if (_access(sourceFilePath.c_str(), 0) == -1)
		{
			JLogger.Error("�������� �ʴ� �����Դϴ�.(%s)\n(��ɾ� ���� : -help)", argv[2]);
			return 0;
		}

		std::string patchFilePath = std::string(curDir) + "\\" + argv[3];
		if (_access(argv[3], 0) != -1)
		{
			patchFilePath = argv[3];
		}
		if (_access(patchFilePath.c_str(), 0) == -1)
		{
			JLogger.Error("�������� �ʴ� �����Դϴ�.(%s)\n(��ɾ� ���� : -help)", argv[3]);
			return 0;
		}

		JPakLib::MergeFileList(sourceFilePath.c_str(), patchFilePath.c_str());
		return 0;
	}

	std::string filePath = std::string(curDir) + "\\" + argv[1];
	std::string fileName = argv[1];
	//argv[1] �� �Էµ� ��ü ��η� ������ ������ ��� (��� ��ü�� �Է¹���)
	if (_access(argv[1], 0) != -1)
	{
		filePath = argv[1];
		fileName = JPakLib::PathToFileName(argv[1]);
	}

	if (_access(filePath.c_str(), 0) == -1)
	{
		JLogger.Error("�������� �ʴ� �����Դϴ�.(%s)\n(��ɾ� ���� : -help)", argv[1]);
		return 0;
	}

	if (IsPakFile(filePath))
	{
		//unpak
		std::string destName = fileName.substr(0, fileName.size() - JPAK_FILE_EXTENSION_SIZE);
		JLogger.Log("unpak : %s >> %s", filePath.c_str(), destName.c_str());
		JPakLib::InflateFileList(filePath.c_str(), curDir, destName.c_str());
	}
	else
	{
		//pak
		JPakLib::JPAK_FILE_LIST flist;
		JPakLib::FolderToFileList(filePath.c_str(), flist);
		std::string destPath = filePath + JPAK_FILE_EXTENSION;
		JLogger.Log("pak : %s >> %s", filePath.c_str(), destPath.c_str());
		JPakLib::DeflateFileList(destPath.c_str(), filePath.c_str(), flist);
	}
}

bool IsPakFile(const std::string& filePath)
{
	return filePath.find(JPAK_FILE_EXTENSION) != std::string::npos;
}
void PrintMenual()
{
	std::cout << "\n";
	std::cout << "pak�� unpak �� JPakConsole�� jpk�����̳� ������ �巡�� �� ��� �ص� �˴ϴ�.\n";
	std::cout << "1. pak \n";
	std::cout << "JPakConsole.exe[FolderName] \n";
	std::cout << "[FolderName] ������.jpk ���Ϸ� �����մϴ�. \n";

	std::cout << "2. unpak\n";
	std::cout << "JPakConsole.exe[FIleName(.jpk)]\n";
	std::cout << "[FIleName(.jpk)] ������ FileName ������ ���� ���� �մϴ�.\n";

	std::cout << "3. patch\n";
	std::cout << "JPakConsole.exe - patch[Source FileName(.jpk)][Patch FileName(.jpk)]\n";
	std::cout << "[Source FileName(.jpk)] ���Ͽ�[Patch FileName(.jpk)] ����� ���Ͽ� ���ų� ����� ������ ���� ����ϴ�.\n";
	std::cout << "\n";
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
