#include "pch.h"
#include "FileSystem.h"
#include "JLogger.h"
#include "JSharedFunc.h"
#include "JSharedConst.h"

namespace JPakLibTest_Util
{
	bool MakeTextFile(const char* path, const char* text)
	{
		std::ofstream outfp;
		outfp.open(path);
		if (!outfp.is_open())
			return false;
		outfp << text;
		outfp.close();
		return true;
	}

	bool CompareTextFile(const char* txtPath, const char* txtPath2)
	{
		std::ifstream infp1(txtPath);
		std::ifstream infp2(txtPath2);
		if (!infp1.is_open())
			return false;
		if (!infp2.is_open())
		{
			infp1.close();
			return false;
		}

		char textBuf1[FILE_READ_BUF_SIZE], textBuf2[FILE_READ_BUF_SIZE];
		while (!infp1.eof())
		{
			memset(textBuf1, NULL, FILE_READ_BUF_SIZE);
			memset(textBuf2, NULL, FILE_READ_BUF_SIZE);

			infp1.read(textBuf1, FILE_READ_BUF_SIZE);
			infp2.read(textBuf2, FILE_READ_BUF_SIZE);

			if (0 != strcmp(textBuf1, textBuf2))
			{
				infp1.close();
				infp2.close();
				return false;
			}
		}
		infp1.close();
		infp2.close();
		return true;
	}

	//filepath가 존재하는 상위 폴더 리스트를 얻고, 각각에 rootFolder를 적용하여 최종 path목록을 저장한다.
	void GetPrevFolderList(const std::string& rootFolder, const std::string& filePath, JPakLib::JPAK_FILE_LIST& folderList)
	{
		std::string folderName;
		for (char c : filePath)
		{
			if (c == '\\')
			{
				JPakLib::FILE_INFO fileInfo;
				fileInfo.filePath = rootFolder + "\\" + folderName;
				folderList.push_back(fileInfo);
			}
			folderName.push_back(c);
		}
	}
	
	bool RemoveTestFolder(const std::string& path)
	{
		std::string sfolder = path + "\\*.*";
		WIN32_FIND_DATAA ffd;
		HANDLE hFind = FindFirstFileA(sfolder.c_str(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
			return false;

		do
		{
			if (!_strcmpi(ffd.cFileName, ".") || !_strcmpi(ffd.cFileName, ".."))
				continue;

			std::string filePath = path + "\\" + ffd.cFileName;
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (RemoveTestFolder(filePath) == false)
					return false;
				continue;
			}
			if (remove(filePath.c_str()) != 0)
				return false;
		} while (FindNextFileA(hFind, &ffd));
		_rmdir(path.c_str());
		return true;
	}
}