#include "stdafx.h"
#include "FileSystemUtil.h"
#include "JSharedConst.h"

namespace JPakLib
{
	void FolderToFileList(std::string folderName, JPAK_FILE_LIST& fileList)
	{
		std::string sfolder = folderName + "\\*.*";
		WIN32_FIND_DATAA ffd;
		HANDLE hFind = FindFirstFileA(sfolder.c_str(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
			return;

		FILE_INFO fileInfo;
		FILETIME fileTime;

		do
		{
			if (!_strcmpi(ffd.cFileName, ".") || !_strcmpi(ffd.cFileName, ".."))
				continue;

			fileInfo.filePath = folderName + "\\" + ffd.cFileName;
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				FolderToFileList(fileInfo.filePath, fileList);
				continue;
			}
			FileTimeToLocalFileTime(&ffd.ftCreationTime, &fileTime);
			FileTimeToSystemTime(&fileTime, &fileInfo.time);
			fileList.push_back(fileInfo);
		} while (FindNextFileA(hFind, &ffd));
	}

	void CreateFolder(std::string filePath, std::string  fileName)
	{
		std::string sfolder;
		for (auto& item : fileName)
		{
			if ('\\' == item)
			{
				_mkdir((filePath + sfolder).c_str());
			}
			sfolder.push_back(item);
		}
	}

	bool ValidJPakFilePath(const std::string& filePath)
	{
		auto extension = filePath.substr(filePath.size() - JPAK_FILE_EXTENSION_SIZE, JPAK_FILE_EXTENSION_SIZE);
		return extension.compare(JPAK_FILE_EXTENSION) == 0;
	}

	std::string RemoveJPakExtention(const std::string& filePath)
	{
		return filePath.substr(0, filePath.size() - JPAK_FILE_EXTENSION_SIZE);
	}

	std::string PathToFileName(const std::string& filePath)
	{
		std::string fileName;
		for (auto& item : filePath)
		{
			if ('\\' == item)
			{
				fileName.clear();
			}
			fileName.push_back(item);
		}
		return fileName;
	}
}