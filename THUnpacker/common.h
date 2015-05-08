#pragma once

#include <string>
using std::string;
#include <vector>
using std::vector;


// type ////////////////////////////////////////////////////////////////////////////

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

// common function /////////////////////////////////////////////////////////////////

DWORD getFileSize(FILE* f);

BYTE* decrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6);
BYTE* uncompress(BYTE* buffer, DWORD bufferSize, BYTE* resultBuffer, DWORD originalSize);

void formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
BOOL exportFiles(FILE* f, const vector<Index>& index, string dirName);

// unpack //////////////////////////////////////////////////////////////////////////

class THUnpacker
{
protected:
	FILE* f;
	int count;
	DWORD indexAddress;
	DWORD originalIndexSize;
	DWORD fileSize;
	vector<Index> index;

public:
	THUnpacker(FILE* _f)
	{
		f = _f;
		fileSize = getFileSize(_f);
		count = indexAddress = originalIndexSize = 0;
	}
	int unpack();
protected:
	virtual void readHeader() = 0;
	int checkCountAndSize();
	virtual void readIndex() = 0;
	virtual int exportFiles() = 0;
};
