#pragma once
#include <string>
#include "FileSystem.h"
namespace JPakLibTest_Util
{
	bool MakeTextFile(const char* path, const char* text);
	bool CompareTextFile(const char* txtPath, const char* txtPath2);
	void GetPrevFolderList(const std::string& rootFolder, const std::string& filePath, JPakLib::JPAK_FILE_LIST& folderList);
	bool RemoveTestFolder(const std::string& path);
}