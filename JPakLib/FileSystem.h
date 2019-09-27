#pragma once
#include "FileSystemUtil.h"

/*
현재는 MergeFileList 함수를 통해 새로운 파일에 기존파일에서 중복된 파일을 제외한 나머지를 뒤에 붙여 넣는 방식으로 구현을 함.

만약 각각의 pak 파일들의 용량을 작게 구성한다면,
열어서 일부만 수정하는 것이 아닌 각각의 파일을 통채로 덮어씌우는 방식도 괜찮을 것 같다. 
(스팀의 경우 해당 방식을 강제로 사용하게 됨)

*/
namespace JPakLib
{
	int DeflateFile(const char* source, const char* dest, int level);
	int InflateFile(const char* source, const char* dest);

	bool DeflateFileList(const char* dest, const  char* root_folder, JPAK_FILE_LIST& mlist);
	bool InflateFileList(const char* source, const char* outFolderPath, const char* outFolderName);

	bool MergeFileList(const char* source, const char* mergeFile);
	/*
	//딱히 필요없는 예제 테스트 코드들 임의로 남김
	int deflateFile2();
	void ZipFileTest();
	void UnZipFileTest();
	*/
}