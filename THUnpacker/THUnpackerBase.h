#pragma once
#include <string>
using std::string;
#include <vector>
using std::vector;


typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef int BOOL;
const BOOL FALSE = 0;
const BOOL TRUE = !FALSE;

struct Index
{
	string name;
	DWORD address;
	DWORD length;
	DWORD originalLength;
};

const char E_OPEN_FILE[] = "Failed to open the file!";
const int EN_OPEN_FILE = 2;
const char E_UNKNOWN_TYPE[] = "Unknown file type!";
const int EN_UNKNOWN_TYPE = 3;
const char E_FILE_COUNT[] = "Invalid file count!";
const int EN_FILE_COUNT = 4;
const char E_FILE_SIZE[] = "Invalid file size!";
const int EN_FILE_SIZE = 5;
const char E_EXPORT[] = "Failed to export files!";
const int EN_EXPORT = 6;


class THUnpackerBase
{
protected:
	FILE* f;
	int count;
	DWORD indexAddress;
	DWORD originalIndexSize;
	DWORD fileSize;
	vector<Index> index;
	char* dirName;

public:
	THUnpackerBase(FILE* _f);
	virtual ~THUnpackerBase() {}
	virtual int unpack();

	static THUnpackerBase* create(FILE* _f);

protected:
	virtual void readHeader() = 0;
	virtual int checkCountAndSize();
	virtual void readIndex() = 0;
	virtual int exportFiles(FILE* f, const vector<Index>& index, string dirName);
	// called by exportFiles, return true if need to uncompress
	virtual bool onUncompress(const Index& index, BYTE*& buffer, DWORD& size) { return true; }
	// called by exportFiles
	virtual void onExport(const Index& index, BYTE*& buffer, DWORD& size) {}


	static DWORD getFileSize(FILE* f);

	static BYTE* thDecrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6);
	static BYTE* thUncompress(BYTE* buffer, DWORD bufferSize, BYTE* resultBuffer, DWORD originalSize);

	virtual void formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
};
