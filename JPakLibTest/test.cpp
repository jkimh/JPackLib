#include "pch.h"
#include "FileSystem.h"
#include "JSharedFunc.h"
#include "JSharedConst.h"
#include "JPakLibTestUtil.h"
//legacy_stdio_definitions.lib ��ũ (������ zlib.lib�� ����ϱ� ���ؼ� �߰��� ��ũ���־���.)

using namespace JPakLibTest_Util;

const char* testText = "testText @#$@#$@#%@%%^$&*$^&* TEST";
/*
1. ������ �ؽ�Ʈ ������ �����
2. �ش� �ؽ�Ʈ ������ ����
3. ������ ������ �ٽ� ����
4. �����ؼ� ���� �ؽ�Ʈ ���� ������ ��
5. �׽�Ʈ ��� ���� ����
*/
TEST(JPAK_TEST, TEXT_FILE_TEST) 
{
	const char* testTextFileName = "testTextFile.txt";
	const char* testTextOutFileName = "testTextFile2.txt";
	const char* testJPakFileName = "testTextFile"; //JPAK_FILE_EXTENSION Ȯ���� Ȯ��
	
	char curDir[FILE_PATH_LEN];
	_getcwd(curDir, FILE_PATH_LEN);
	std::string textFilePath = std::string(curDir) + "\\" + testTextFileName;
	EXPECT_TRUE(MakeTextFile(textFilePath.c_str(), testText));
	EXPECT_NE(_access(textFilePath.c_str(), 0), -1);

	//���� ���� �� ���� ���� Ȯ��
	std::string jpakFilePath = std::string(curDir) + "\\" + testJPakFileName + JPAK_FILE_EXTENSION;
	EXPECT_EQ(JPakLib::DeflateFile(textFilePath.c_str(), jpakFilePath.c_str(), 1), 0);
	EXPECT_NE(_access(jpakFilePath.c_str(), 0), -1);

	//���� ���� �� ���� ���� Ȯ��
	std::string outTextFilePath = std::string(curDir) + "\\" + testTextOutFileName;
	EXPECT_EQ(JPakLib::InflateFile(jpakFilePath.c_str(), outTextFilePath.c_str()), 0);
	EXPECT_NE(_access(outTextFilePath.c_str(), 0), -1);

	//���� ��
	EXPECT_TRUE(CompareTextFile(textFilePath.c_str(), outTextFilePath.c_str()));

	//������ ���� ����
	EXPECT_EQ(remove(textFilePath.c_str()), 0);
	EXPECT_EQ(remove(jpakFilePath.c_str()), 0);
	EXPECT_EQ(remove(outTextFilePath.c_str()), 0);
}

/*
1. ������ ���� ��� ����
2. ���� ���� 
3. ���� ����
4. ������ ���� ��� Ȯ��
*/
#define JPAK_TEST_FOLDER_COUNT 4
TEST(JPAK_TEST, FOLDER_TEST)
{	
	/*
	�׽�Ʈ������ ������ ���� ����
	�������� ������ �ǹ̰� �����Ƿ� ���� �ϳ��� �� �����ؾ��Ѵ�.

	testFolderRoot
	 �� TestFolder1
		�� TestText1.txt
		�� TestText2.txt
	 �� TestFolder2
		��TestText3.txt
	 �� TestFolder3	
		��TestText4.txt
	*/
	const char testRootFolderName[FILE_PATH_LEN] = "testFolderRoot";
	const char testFolderList[JPAK_TEST_FOLDER_COUNT][FILE_PATH_LEN] = {
		"TestFolder1\\TestText1.txt",
		"TestFolder1\\TestText2.txt",
		"TestFolder2\\TestText3.txt",
		"TestFolder3\\TestText4.txt"
	};

	//�׽�Ʈ�� ���� �� ���� ����
	char curDir[FILE_PATH_LEN];
	_getcwd(curDir, FILE_PATH_LEN);
	std::string totalRootFolderPath = std::string(curDir) + "\\" + testRootFolderName;
	_mkdir(totalRootFolderPath.c_str());
	EXPECT_NE(_access(totalRootFolderPath.c_str(), 0), -1);

	for (int i = 0; i < JPAK_TEST_FOLDER_COUNT; i++)
	{
		JPakLib::JPAK_FILE_LIST PrevFolderList;
		GetPrevFolderList(totalRootFolderPath, testFolderList[i], PrevFolderList);
		for (auto folderPath : PrevFolderList)
		{
			_mkdir(folderPath.filePath.c_str());
			EXPECT_NE(_access(folderPath.filePath.c_str(), 0), -1);
		}

		std::string &textFilePath = totalRootFolderPath + "\\" + testFolderList[i];
		EXPECT_TRUE(MakeTextFile(textFilePath.c_str(), testText));
		EXPECT_NE(_access(textFilePath.c_str(), 0), -1);
	}

	const char* testOutRootFolderName = "testFolderRoot2";
	const char* testJPakFileName = "testFolderJPak"; //JPAK_FILE_EXTENSION Ȯ���� Ȯ��

	std::string totalFolderOutPath = std::string(curDir) + "\\" + testOutRootFolderName;
	std::string totalJPakFilePath = std::string(curDir) + "\\" + testJPakFileName + JPAK_FILE_EXTENSION;

	JPakLib::JPAK_FILE_LIST flist;
	JPakLib::FolderToFileList(totalRootFolderPath.c_str(), flist);
	EXPECT_EQ(flist.size(), JPAK_TEST_FOLDER_COUNT);

	//���� �� ����
	JPakLib::DeflateFileList(totalJPakFilePath.c_str(), totalRootFolderPath.c_str(), flist);
	EXPECT_NE(_access(totalJPakFilePath.c_str(), 0), -1);

	JPakLib::InflateFileList(totalJPakFilePath.c_str(), curDir, testOutRootFolderName);
	EXPECT_NE(_access(totalFolderOutPath.c_str(), 0), -1);

	//������ ���� Ȯ��
	for (int i = 0; i < JPAK_TEST_FOLDER_COUNT; i++)
	{
		std::string &path1 = totalRootFolderPath + "\\" + testFolderList[i];
		std::string &path2 = totalFolderOutPath + "\\" + testFolderList[i];
		EXPECT_NE(_access(path2.c_str(), 0), -1);
		EXPECT_TRUE(CompareTextFile(path1.c_str(), path2.c_str()));
	}
	//������ ���� ����
	EXPECT_EQ(remove(totalJPakFilePath.c_str()), 0);
	EXPECT_TRUE(RemoveTestFolder(totalRootFolderPath.c_str()));
	EXPECT_TRUE(RemoveTestFolder(totalFolderOutPath.c_str()));
}
/*
1. �׽�Ʈ�� pak ���� �� �� ����
  testJPak.jpk 
   �� TestText1.txt
   �� TestText2.txt (testText2Before)
  testJPak2.jpk
   �� testText2.txt (testText2After)
2. ���� ����
3. ���� ����
4. ���� ����� �������� Ȯ��
*/
TEST(JPAK_TEST, MERGE_TEST)
{
	const char *testRootFolderName = "testPatchTestRoot";
	const char *testFile[2] = {
		"TestText1.txt",
		"TestText2.txt",
	};
	const char *beforeTestText = "testText2Before";

	const char *testRootPatchFolderName = "testPatch";
	const char *testPatchFile[1] = {
		"TestText2.txt",
	};
	const char *afterTestText = "testText2After";

	//�׽�Ʈ�� ���� �� ���� ����
	char curDir[FILE_PATH_LEN];
	_getcwd(curDir, FILE_PATH_LEN);
	std::string totalRootFolderPath = std::string(curDir) + "\\" + testRootFolderName;
	_mkdir(totalRootFolderPath.c_str());
	EXPECT_NE(_access(totalRootFolderPath.c_str(), 0), -1);

	for (int i = 0; i < 2; i++)
	{
		JPakLib::JPAK_FILE_LIST PrevFolderList;
		GetPrevFolderList(totalRootFolderPath, testFile[i], PrevFolderList);

		std::string &textFilePath = totalRootFolderPath + "\\" + testFile[i];
		EXPECT_TRUE(MakeTextFile(textFilePath.c_str(), beforeTestText));
		EXPECT_NE(_access(textFilePath.c_str(), 0), -1);
	}

	std::string totalRootPatchFolderPath = std::string(curDir) + "\\" + testRootPatchFolderName;
	_mkdir(totalRootPatchFolderPath.c_str());
	EXPECT_NE(_access(totalRootPatchFolderPath.c_str(), 0), -1);
	for (int i = 0; i < 1; i++)
	{
		JPakLib::JPAK_FILE_LIST PrevFolderList;
		GetPrevFolderList(totalRootPatchFolderPath, testPatchFile[i], PrevFolderList);

		std::string &textFilePath = totalRootPatchFolderPath + "\\" + testPatchFile[i];
		EXPECT_TRUE(MakeTextFile(textFilePath.c_str(), afterTestText));
		EXPECT_NE(_access(textFilePath.c_str(), 0), -1);
	}

	std::string totalJPakFilePath = std::string(curDir) + "\\" + testRootFolderName + JPAK_FILE_EXTENSION;
	std::string totalJPakPatchFilePath = std::string(curDir) + "\\" + testRootPatchFolderName + JPAK_FILE_EXTENSION;

	JPakLib::JPAK_FILE_LIST flist;
	JPakLib::FolderToFileList(totalRootFolderPath.c_str(), flist);
	EXPECT_EQ(flist.size(), 2);
	JPakLib::DeflateFileList(totalJPakFilePath.c_str(), totalRootFolderPath.c_str(), flist);
	EXPECT_NE(_access(totalJPakFilePath.c_str(), 0), -1);
	
	JPakLib::JPAK_FILE_LIST flist2;
	JPakLib::FolderToFileList(totalRootPatchFolderPath.c_str(), flist2);
	EXPECT_EQ(flist2.size(), 1);
	JPakLib::DeflateFileList(totalJPakPatchFilePath.c_str(), totalRootPatchFolderPath.c_str(), flist2);
	EXPECT_NE(_access(totalJPakPatchFilePath.c_str(), 0), -1);
	
	JPakLib::MergeFileList(totalJPakFilePath.c_str(), totalJPakPatchFilePath.c_str());
	const char* resultFolderName = "Result";
	JPakLib::InflateFileList(totalJPakFilePath.c_str(), curDir, resultFolderName);
	std::string afterFilePath = std::string(curDir) + "\\" + resultFolderName + "\\" + testPatchFile[0];
	std::string beforePatchFilePath = totalRootPatchFolderPath + "\\" + testPatchFile[0];
	EXPECT_NE(_access(afterFilePath.c_str(), 0), -1);
	EXPECT_TRUE(CompareTextFile(beforePatchFilePath.c_str(), afterFilePath.c_str()));

	EXPECT_EQ(remove(totalJPakPatchFilePath.c_str()), 0);
	EXPECT_EQ(remove(totalJPakFilePath.c_str()), 0);
	EXPECT_TRUE(RemoveTestFolder(totalRootPatchFolderPath.c_str()));
	EXPECT_TRUE(RemoveTestFolder((std::string(curDir) + "\\" + resultFolderName).c_str()));
	EXPECT_TRUE(RemoveTestFolder(totalRootFolderPath.c_str()));
	
}