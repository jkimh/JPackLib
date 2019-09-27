#include "pch.h"
#include "FileSystem.h"
#include "JSharedFunc.h"
#include "JSharedConst.h"
#include "JPakLibTestUtil.h"
//legacy_stdio_definitions.lib 링크 (구버전 zlib.lib를 사용하기 위해서 추가로 링크해주었음.)

using namespace JPakLibTest_Util;

const char* testText = "testText @#$@#$@#%@%%^$&*$^&* TEST";
/*
1. 임의의 텍스트 파일을 만든다
2. 해당 텍스트 파일을 압축
3. 압축한 파일을 다시 해제
4. 해제해서 나온 텍스트 파일 내용을 비교
5. 테스트 대상 파일 삭제
*/
TEST(JPAK_TEST, TEXT_FILE_TEST) 
{
	const char* testTextFileName = "testTextFile.txt";
	const char* testTextOutFileName = "testTextFile2.txt";
	const char* testJPakFileName = "testTextFile"; //JPAK_FILE_EXTENSION 확장자 확인
	
	char curDir[FILE_PATH_LEN];
	_getcwd(curDir, FILE_PATH_LEN);
	std::string textFilePath = std::string(curDir) + "\\" + testTextFileName;
	EXPECT_TRUE(MakeTextFile(textFilePath.c_str(), testText));
	EXPECT_NE(_access(textFilePath.c_str(), 0), -1);

	//압축 진행 및 파일 생성 확인
	std::string jpakFilePath = std::string(curDir) + "\\" + testJPakFileName + JPAK_FILE_EXTENSION;
	EXPECT_EQ(JPakLib::DeflateFile(textFilePath.c_str(), jpakFilePath.c_str(), 1), 0);
	EXPECT_NE(_access(jpakFilePath.c_str(), 0), -1);

	//압축 해제 및 파일 생성 확인
	std::string outTextFilePath = std::string(curDir) + "\\" + testTextOutFileName;
	EXPECT_EQ(JPakLib::InflateFile(jpakFilePath.c_str(), outTextFilePath.c_str()), 0);
	EXPECT_NE(_access(outTextFilePath.c_str(), 0), -1);

	//파일 비교
	EXPECT_TRUE(CompareTextFile(textFilePath.c_str(), outTextFilePath.c_str()));

	//생성된 파일 삭제
	EXPECT_EQ(remove(textFilePath.c_str()), 0);
	EXPECT_EQ(remove(jpakFilePath.c_str()), 0);
	EXPECT_EQ(remove(outTextFilePath.c_str()), 0);
}

/*
1. 임의의 폴더 목록 생성
2. 폴더 압축 
3. 압축 해제
4. 해제된 폴더 목록 확인
*/
#define JPAK_TEST_FOLDER_COUNT 4
TEST(JPAK_TEST, FOLDER_TEST)
{	
	/*
	테스트용으로 생성할 폴더 구조
	빈폴더는 압축의 의미가 없으므로 파일 하나를 꼭 포함해야한다.

	testFolderRoot
	 ㄴ TestFolder1
		ㄴ TestText1.txt
		ㄴ TestText2.txt
	 ㄴ TestFolder2
		ㄴTestText3.txt
	 ㄴ TestFolder3	
		ㄴTestText4.txt
	*/
	const char testRootFolderName[FILE_PATH_LEN] = "testFolderRoot";
	const char testFolderList[JPAK_TEST_FOLDER_COUNT][FILE_PATH_LEN] = {
		"TestFolder1\\TestText1.txt",
		"TestFolder1\\TestText2.txt",
		"TestFolder2\\TestText3.txt",
		"TestFolder3\\TestText4.txt"
	};

	//테스트용 폴더 및 파일 생성
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
	const char* testJPakFileName = "testFolderJPak"; //JPAK_FILE_EXTENSION 확장자 확인

	std::string totalFolderOutPath = std::string(curDir) + "\\" + testOutRootFolderName;
	std::string totalJPakFilePath = std::string(curDir) + "\\" + testJPakFileName + JPAK_FILE_EXTENSION;

	JPakLib::JPAK_FILE_LIST flist;
	JPakLib::FolderToFileList(totalRootFolderPath.c_str(), flist);
	EXPECT_EQ(flist.size(), JPAK_TEST_FOLDER_COUNT);

	//압축 및 해제
	JPakLib::DeflateFileList(totalJPakFilePath.c_str(), totalRootFolderPath.c_str(), flist);
	EXPECT_NE(_access(totalJPakFilePath.c_str(), 0), -1);

	JPakLib::InflateFileList(totalJPakFilePath.c_str(), curDir, testOutRootFolderName);
	EXPECT_NE(_access(totalFolderOutPath.c_str(), 0), -1);

	//해제된 파일 확인
	for (int i = 0; i < JPAK_TEST_FOLDER_COUNT; i++)
	{
		std::string &path1 = totalRootFolderPath + "\\" + testFolderList[i];
		std::string &path2 = totalFolderOutPath + "\\" + testFolderList[i];
		EXPECT_NE(_access(path2.c_str(), 0), -1);
		EXPECT_TRUE(CompareTextFile(path1.c_str(), path2.c_str()));
	}
	//생성된 파일 삭제
	EXPECT_EQ(remove(totalJPakFilePath.c_str()), 0);
	EXPECT_TRUE(RemoveTestFolder(totalRootFolderPath.c_str()));
	EXPECT_TRUE(RemoveTestFolder(totalFolderOutPath.c_str()));
}
/*
1. 테스트용 pak 파일 두 개 생성
  testJPak.jpk 
   ㄴ TestText1.txt
   ㄴ TestText2.txt (testText2Before)
  testJPak2.jpk
   ㄴ testText2.txt (testText2After)
2. 머지 진행
3. 압축 해제
4. 예상 결과와 동일한지 확인
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

	//테스트용 폴더 및 파일 생성
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