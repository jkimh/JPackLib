#pragma once
#include "FileSystemUtil.h"

/*
����� MergeFileList �Լ��� ���� ���ο� ���Ͽ� �������Ͽ��� �ߺ��� ������ ������ �������� �ڿ� �ٿ� �ִ� ������� ������ ��.

���� ������ pak ���ϵ��� �뷮�� �۰� �����Ѵٸ�,
��� �Ϻθ� �����ϴ� ���� �ƴ� ������ ������ ��ä�� ������ ��ĵ� ������ �� ����. 
(������ ��� �ش� ����� ������ ����ϰ� ��)

*/
namespace JPakLib
{
	int DeflateFile(const char* source, const char* dest, int level);
	int InflateFile(const char* source, const char* dest);

	bool DeflateFileList(const char* dest, const  char* root_folder, JPAK_FILE_LIST& mlist);
	bool InflateFileList(const char* source, const char* outFolderPath, const char* outFolderName);

	bool MergeFileList(const char* source, const char* mergeFile);
	/*
	//���� �ʿ���� ���� �׽�Ʈ �ڵ�� ���Ƿ� ����
	int deflateFile2();
	void ZipFileTest();
	void UnZipFileTest();
	*/
}