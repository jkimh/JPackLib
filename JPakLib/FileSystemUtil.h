#pragma once
#include <vector>
#include <Windows.h>

namespace JPakLib
{
	struct FILE_INFO
	{
		std::string filePath;
		SYSTEMTIME time;
	};
	typedef std::vector<FILE_INFO> JPAK_FILE_LIST;

	void FolderToFileList(std::string folderName, JPAK_FILE_LIST& fileList);
	void CreateFolder(std::string filePath, std::string  fileName);
	bool ValidJPakFilePath(const std::string& filePath);
	std::string RemoveJPakExtention(const std::string& filePath);
	std::string PathToFileName(const std::string& filePath);
}