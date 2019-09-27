#include "stdafx.h"
#include "FileSystem.h"
#include "zlib.h"
#include "unzip.h"
#include "zip.h"
#include "JSharedConst.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

namespace JPakLib
{
	
	/*
	레벨 참고
	#define Z_NO_COMPRESSION         0
	#define Z_BEST_SPEED             1
	#define Z_BEST_COMPRESSION       9
	#define Z_DEFAULT_COMPRESSION  (-1)
	*/
	int DeflateFile(const char* source, const char* dest, int level)
	{
		int ret, flush;
		unsigned have;
		z_stream stream;
		unsigned char in[FILE_READ_BUF_SIZE];
		unsigned char out[FILE_READ_BUF_SIZE];

		std::ifstream infp;
		infp.open(source, std::ios_base::binary);
		if (!infp.is_open())
		{
			JLogger.Error("source file open fail (%s)", source);
			return Z_ERRNO;
		}
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		ret = deflateInit(&stream, level);
		if (ret != Z_OK)
		{
			JLogger.Error("deflateInit fail");
			deflateEnd(&stream);
			infp.close();
			return ret;
		}

		std::ofstream outfp;
		outfp.open(dest, std::ios_base::binary);
		if (!outfp.is_open())
		{
			JLogger.Error("dest file open fail (%s)", dest);
			deflateEnd(&stream);
			infp.close();
			return Z_ERRNO;
		}

		do
		{
			infp.read((char*)in, FILE_READ_BUF_SIZE);
			int readsize = (int)infp.gcount();

			stream.avail_in = readsize;
			stream.next_in = in;

			if (infp.eof())
			{
				flush = Z_FINISH;
			}
			else
			{
				flush = Z_NO_FLUSH;
			}

			do
			{
				stream.avail_out = FILE_READ_BUF_SIZE;
				stream.next_out = out;
				ret = deflate(&stream, flush);

				have = FILE_READ_BUF_SIZE - stream.avail_out;
				outfp.write((char*)out, have);
			} while (stream.avail_out == 0);

		} while (ret != Z_STREAM_END);

		deflateEnd(&stream);

		outfp.close();
		infp.close();
		return Z_OK;
	}
	
	int InflateFile(const char* source, const char* dest)
	{
		int ret;
		unsigned have;
		z_stream stream;
		unsigned char in[FILE_READ_BUF_SIZE];
		unsigned char out[FILE_READ_BUF_SIZE];

		std::ifstream infp;
		infp.open(source, std::ios::binary);
		if (!infp.is_open())
		{
			JLogger.Error("source file open fail (%s)", source);
			return Z_ERRNO;
		}

		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		stream.avail_in = 0;
		stream.next_in = Z_NULL;
		ret = inflateInit(&stream);
		if (ret != Z_OK)
		{
			JLogger.Error("inflateInit fail");
			infp.close();
			return ret;
		}
		
		std::ofstream outfp;
		outfp.open(dest, std::ios_base::binary);
		if (!outfp.is_open())
		{
			JLogger.Error("dest file open fail (%s)", dest);
			deflateEnd(&stream);
			infp.close();
			return Z_ERRNO;
		}

		do
		{
			infp.read((char*)in, FILE_READ_BUF_SIZE);
			stream.avail_in = static_cast<uInt>(infp.gcount());
			if (stream.avail_in == 0)
				break;
			stream.next_in = in;
			do
			{
				stream.avail_out = FILE_READ_BUF_SIZE;
				stream.next_out = out;
				ret = inflate(&stream, Z_NO_FLUSH);
				switch (ret)
				{
					case Z_NEED_DICT:
						ret = Z_DATA_ERROR; // fall through
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						inflateEnd(&stream);
						outfp.close();
						infp.close();
						return ret;
				}
				have = FILE_READ_BUF_SIZE - stream.avail_out;
				outfp.write((char*)out, have);

			} while (stream.avail_out == 0);
		} while (ret != Z_STREAM_END);

		deflateEnd(&stream);

		outfp.close();
		infp.close();
		return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
	}

	bool DeflateFileList(const char* dest, const  char* root_folder, JPAK_FILE_LIST& mlist)
	{
		zipFile zf;
		zf = zipOpen(dest, APPEND_STATUS_CREATE);
		
		zip_fileinfo info;
		memset(&info, 0, sizeof(zip_fileinfo));
		size_t len = strlen(root_folder);

		char in[FILE_READ_BUF_SIZE];
		size_t readsize = 0;

		for (auto& item : mlist)
		{
			SYSTEMTIME& mtime = item.time;
			info.tmz_date.tm_hour = mtime.wHour;
			info.tmz_date.tm_mday = mtime.wDay;
			info.tmz_date.tm_min = mtime.wMinute;
			info.tmz_date.tm_mon = mtime.wMonth - 1;
			info.tmz_date.tm_sec = mtime.wSecond;
			info.tmz_date.tm_year = mtime.wYear;

			const  char* m = item.filePath.c_str();
			zipOpenNewFileInZip(zf, m + len, &info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
			std::ifstream fp(m, std::ios_base::binary);
			if (!fp.is_open())
			{
				zipCloseFileInZip(zf);
				zipClose(zf, "");
				return false;
			}
			do
			{
				memset(in, 0, FILE_READ_BUF_SIZE);
				fp.read(in, FILE_READ_BUF_SIZE);
				readsize = (size_t)fp.gcount();	
				zipWriteInFileInZip(zf, in, static_cast<unsigned int>(readsize));
			} while (!fp.eof());

			fp.close();
			zipCloseFileInZip(zf);
		}
		zipClose(zf, "");
		return true;
	}

	bool InflateFileList(const char* source, const char* outFolderPath, const char* outFolderName)
	{
		zipFile zf;
		zf = unzOpen(source);
		int ret = unzGoToFirstFile(zf);
		if (UNZ_OK != ret)
		{
			unzClose(zf);
			return false;
		}
		CreateFolder(std::string(outFolderPath) + "\\", std::string(outFolderName) + "\\");
		
		unz_file_info info;
		char filename[FILE_NAME_LEN];
		std::string filepath;
		char in[FILE_READ_BUF_SIZE];
		size_t readsize = 0;
		DWORD wsize = 0;

		SYSTEMTIME stime;
		memset(&stime, 0, sizeof(SYSTEMTIME));

		FILETIME ltime, ftime;

		do 
		{
			unzGetCurrentFileInfo(zf, &info, filename, FILE_NAME_LEN, NULL, 0, NULL, 0);

			filepath = std::string(outFolderName) + "\\" + filename;
			CreateFolder(std::string(outFolderPath) + "\\", filepath);
			filepath = std::string(outFolderPath) + "\\" + outFolderName + "\\" + filename;
			
			HANDLE hfile = CreateFileA(filepath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hfile) 
				continue;

			unzOpenCurrentFile(zf);

			do 
			{
				memset(in, 0, FILE_READ_BUF_SIZE);
				readsize = unzReadCurrentFile(zf, in, FILE_READ_BUF_SIZE);
				info.uncompressed_size -= static_cast<uLong>(readsize);
				wsize = 0;
				WriteFile(hfile, in, static_cast<DWORD>(readsize), &wsize, NULL);
			} while (0 != info.uncompressed_size);
			
			unzCloseCurrentFile(zf);

			stime.wHour = info.tmu_date.tm_hour;
			stime.wDay = info.tmu_date.tm_mday;
			stime.wMinute = info.tmu_date.tm_min;
			stime.wMonth = info.tmu_date.tm_mon + 1;
			stime.wSecond = info.tmu_date.tm_sec;
			stime.wYear = info.tmu_date.tm_year;

			SystemTimeToFileTime(&stime, &ltime);
			LocalFileTimeToFileTime(&ltime, &ftime);
			SetFileTime(hfile, &ftime, &ftime, &ftime);

			CloseHandle(hfile);
		} while (UNZ_OK == unzGoToNextFile(zf));

		unzClose(zf);
		return true;
	}

	/*
	source에 있는 파일 중 mergeFile에 중복되는 파일을 덮어씌우고(삭제 후 추가), 새로운 파일은 추가한다.
	1. source 파일 이름 변경 (.jpk > .source)
	2. mergeFile 읽고 리스트를 만들면서 새로운 파일에 압축 진행 (.dest)
	3. source 파일 읽으면서 중복된 파일을 제외하고 새로운 파일 뒤에 붙임 (.dest)
	4. 파일 이름 변경 (.dest > .jpk)
	*/	

	bool MergeFileList(const char* source, const char* mergeFile)
	{
		if (!ValidJPakFilePath(source))
		{
			return false;
		}
		if (!ValidJPakFilePath(mergeFile))
		{
			return false;
		}
		std::string tempFileName = RemoveJPakExtention(source); //확장자 제거
		std::string tempSourceFileName = tempFileName + JPAK_TEMP_SOURCE_FILE_EXTENSION;
		std::string tempDestFileName = tempFileName + JPAK_TEMP_DEST_FILE_EXTENSION;

		rename(source, tempSourceFileName.c_str());
		
		//머지할 파일 읽기
		unzFile mergeZipFile = unzOpen(mergeFile);
		int ret = unzGoToFirstFile(mergeZipFile);
		if (ret != UNZ_OK)
		{
			unzClose(mergeZipFile);
			return false;
		}
		unz_file_info mergeInfo;
		char fileName[FILE_NAME_LEN];
		char in[FILE_READ_BUF_SIZE];
		int readsize = 0;

		zipFile destZipFile = zipOpen(tempDestFileName.c_str(), APPEND_STATUS_CREATE);

		int method = 0;
		int level = 0;
		//머지할 파일 먼저 기록
		std::set<std::string> changedFileList;
		do
		{
			unzGetCurrentFileInfo(mergeZipFile, &mergeInfo, fileName, FILE_NAME_LEN, NULL, 0, NULL, 0);
			changedFileList.insert(fileName);

			unzOpenCurrentFile2(mergeZipFile, &method, &level, 1);
			zipOpenNewFileInZip2(destZipFile, fileName, NULL, NULL, 0, NULL, 0, NULL, method, level, 1);
			int compressedSize = mergeInfo.compressed_size;
			do
			{
				memset(in, 0, FILE_READ_BUF_SIZE);
				readsize = unzReadCurrentFile(mergeZipFile, in, static_cast<unsigned int>(FILE_READ_BUF_SIZE));
				if (readsize < 0)
				{
					zipCloseFileInZip(destZipFile);
					zipClose(destZipFile, "");
					unzClose(mergeZipFile);
					return false;
				}
				compressedSize -= readsize;
				zipWriteInFileInZip(destZipFile, in, static_cast<unsigned int>(readsize));
			} while (0 != compressedSize);

			zipCloseFileInZipRaw(destZipFile, mergeInfo.uncompressed_size, mergeInfo.crc);
			zipCloseFileInZip(destZipFile);
		} while (UNZ_OK == unzGoToNextFile(mergeZipFile));

		unzClose(mergeZipFile);

		//기존 파일 기록
		unzFile sourceZipFile;
		sourceZipFile = unzOpen(tempSourceFileName.c_str());
		ret = unzGoToFirstFile(sourceZipFile);
		if (ret != UNZ_OK)
		{
			unzClose(sourceZipFile);
			zipClose(destZipFile, "");
			return false;
		}
		unz_file_info sourceInfo;
		DWORD wsize = 0;
		memset(fileName, 0, FILE_NAME_LEN);
		memset(in, 0, FILE_READ_BUF_SIZE);
		readsize = 0;
		method = 0;
		level = 0;

		do
		{
			unzGetCurrentFileInfo(sourceZipFile, &sourceInfo, fileName, FILE_NAME_LEN, NULL, 0, NULL, 0);
			auto it = changedFileList.find(fileName);
			if (it != changedFileList.end())
			{
				continue;
			}
			unzOpenCurrentFile2(sourceZipFile, &method, &level, 1);
			zipOpenNewFileInZip2(destZipFile, fileName, NULL, NULL, 0, NULL, 0, NULL, method, level, 1);
			int compressedSize = sourceInfo.compressed_size;
			do
			{
				memset(in, 0, FILE_READ_BUF_SIZE);
				readsize = unzReadCurrentFile(sourceZipFile, in, FILE_READ_BUF_SIZE);
				compressedSize -= readsize;
				zipWriteInFileInZip(destZipFile, in, static_cast<unsigned int>(readsize));
			} while (0 != compressedSize);

			zipCloseFileInZipRaw(destZipFile, sourceInfo.uncompressed_size, sourceInfo.crc);
			zipCloseFileInZip(destZipFile);
		} while (UNZ_OK == unzGoToNextFile(sourceZipFile));

		unzClose(sourceZipFile);
		zipClose(destZipFile, "");
		remove(tempSourceFileName.c_str());
		rename(tempDestFileName.c_str(), source);		
		return true;
	}

	/*
	//딱히 필요없는 예제 테스트 코드들
	voidpf my_fn(voidpf  opaque, uInt  items, uInt  size) {
		std::cout << "my_fn" << std::endl;
		std::cout << "opaque:" << (const  char*)opaque << std::endl;
		std::cout << "items:" << items << ", size:" << size << std::endl;
		return calloc(items, size);
	}

	void my_free(voidpf  opaque, voidpf  address) {
		std::cout << "my_free" << std::endl;
		std::cout << "opaque:" << (const  char*)opaque << std::endl;
		std::cout << "address:" << address << std::endl;
		free(address);
	}
	int deflateFile2()
	{
		do {
			const  char* mop = "my opaque";

			z_stream stream;
			stream.zalloc = my_fn;
			stream.zfree = my_free;
			stream.opaque = (voidpf)mop;

			std::cout << "deflateInit" << std::endl;
			int ret = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
			if (Z_OK != ret) break;

			std::cout << "deflateEnd" << std::endl;
			deflateEnd(&stream);
		} while (false);

		system("pause");

		return 0;
	}

	void ZipFileTest()
	{
		do {
			//zip 파일  생성
			zipFile zf = zipOpen("c.zip", APPEND_STATUS_CREATE);
			if (NULL == zf) break;

			zip_fileinfo info;
			memset(&info, 0, sizeof(zip_fileinfo));

			struct tm tdata = JSharedFunc::GetCurTimeStruct();
			info.tmz_date.tm_hour = tdata.tm_hour;
			info.tmz_date.tm_mday = tdata.tm_mday;
			info.tmz_date.tm_min = tdata.tm_min;
			info.tmz_date.tm_mon = tdata.tm_mon;
			info.tmz_date.tm_sec = tdata.tm_sec;
			info.tmz_date.tm_year = tdata.tm_year;

			// zip 파일  목록  생성
			int ret = zipOpenNewFileInZip(zf, "c.txt", &info, NULL, 0, NULL, 0, "comment",
				Z_DEFLATED, Z_DEFAULT_COMPRESSION);

			///// 기록  시작...
			do {
				std::ifstream fp;
				fp.open("c.txt", std::ios_base::binary);
				if (!fp) break;

				const  int BUF = 1024;
				Bytef in[BUF];

				do {
					fp.read((char*)in, BUF);
					int readsize = (int)fp.gcount();
					zipWriteInFileInZip(zf, (const  void*)in, readsize);
				} while (!fp.eof());

				fp.close();
			} while (false);

			//////기록  완료
			zipCloseFileInZip(zf);//zip 목록  닫기

			zipClose(zf, "global_comment");//zip 파일  닫기

		} while (false);
	}
	void UnZipFileTest()
	{
		do {
			unzFile uf = unzOpen("c.zip");
			if (NULL == uf) break;

			int ret = unzGoToFirstFile(uf);
			if (UNZ_OK != ret) {
				unzClose(uf);
				break;
			}

			/// 현재  방문중인  파일  목록  정보  추출  
			const  int MAX_PATH_LEN = 256;
			const  int MAX_COMMENT = 256;

			char filename[MAX_PATH_LEN];
			char comment[MAX_COMMENT];

			unz_file_info info;
			unzGetCurrentFileInfo(uf, &info, filename, MAX_PATH_LEN, NULL, 0, comment, MAX_COMMENT);

			std::cout << "filename:" << filename << " Comment:" << comment << std::endl;
			std::cout << " compressed_size:" << info.compressed_size << " uncompressed_size:" << info.uncompressed_size << std::endl;


			//현재  파일을  열기.
			ret = unzOpenCurrentFile(uf);

			const  int BUF = 1024;
			Bytef in[BUF];
			int readsize(0);

			std::ofstream op;
			op.open("d.txt", std::ios_base::binary);

			do
			{
				readsize = unzReadCurrentFile(uf, (void*)in, BUF);
				op.write((const  char*)in, readsize);
			} while (0 != readsize);

			op.close();

			unzCloseCurrentFile(uf);
			unzClose(uf);

		} while (false);
	}

	*/
}