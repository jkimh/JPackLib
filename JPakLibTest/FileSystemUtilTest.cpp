#include "pch.h"
#include "FileSystemUtil.h"
#include "JSharedConst.h"

TEST(JPAK_UTIL_TEST, ValidJPakFilePath_Test)
{
	const char testPath[FILE_PATH_LEN] = "test\test.jpk";
	EXPECT_TRUE(JPakLib::ValidJPakFilePath(testPath));
	const char testPath2[FILE_PATH_LEN] = "test\test.djpk";
	EXPECT_FALSE(JPakLib::ValidJPakFilePath(testPath2));
}

TEST(JPAK_UTIL_TEST, RemoveJPakExtention_Test)
{
	const char testPath[FILE_PATH_LEN] = "test\test.jpk";
	const char testPathRemovedExtention[FILE_PATH_LEN] = "test\test";
	std::string& result = JPakLib::RemoveJPakExtention(testPath);
	EXPECT_EQ(result.compare(testPathRemovedExtention), 0);
}