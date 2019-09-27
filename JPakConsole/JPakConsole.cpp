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
		JLogger.Error("대상 파일을 입력해주세요. (명령어 보기 : -help)");
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
			JLogger.Error("존재하지 않는 파일입니다.(%s)\n(명령어 보기 : -help)", argv[2]);
			return 0;
		}

		std::string patchFilePath = std::string(curDir) + "\\" + argv[3];
		if (_access(argv[3], 0) != -1)
		{
			patchFilePath = argv[3];
		}
		if (_access(patchFilePath.c_str(), 0) == -1)
		{
			JLogger.Error("존재하지 않는 파일입니다.(%s)\n(명령어 보기 : -help)", argv[3]);
			return 0;
		}

		JPakLib::MergeFileList(sourceFilePath.c_str(), patchFilePath.c_str());
		return 0;
	}

	std::string filePath = std::string(curDir) + "\\" + argv[1];
	std::string fileName = argv[1];
	//argv[1] 에 입력된 자체 경로로 접근이 가능한 경우 (경로 자체를 입력받음)
	if (_access(argv[1], 0) != -1)
	{
		filePath = argv[1];
		fileName = JPakLib::PathToFileName(argv[1]);
	}

	if (_access(filePath.c_str(), 0) == -1)
	{
		JLogger.Error("존재하지 않는 파일입니다.(%s)\n(명령어 보기 : -help)", argv[1]);
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
	std::cout << "pak과 unpak 은 JPakConsole에 jpk파일이나 폴더를 드래그 앤 드롭 해도 됩니다.\n";
	std::cout << "1. pak \n";
	std::cout << "JPakConsole.exe[FolderName] \n";
	std::cout << "[FolderName] 폴더를.jpk 파일로 압축합니다. \n";

	std::cout << "2. unpak\n";
	std::cout << "JPakConsole.exe[FIleName(.jpk)]\n";
	std::cout << "[FIleName(.jpk)] 파일을 FileName 폴더로 압축 해제 합니다.\n";

	std::cout << "3. patch\n";
	std::cout << "JPakConsole.exe - patch[Source FileName(.jpk)][Patch FileName(.jpk)]\n";
	std::cout << "[Source FileName(.jpk)] 파일에[Patch FileName(.jpk)] 내용과 비교하여 없거나 변경된 파일을 덮어 씌웁니다.\n";
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
