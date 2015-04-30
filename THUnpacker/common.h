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


DWORD getFileSize(FILE* f);

BYTE* decrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6);
BYTE* uncompress(BYTE* buffer, DWORD bufferSize, BYTE* resultBuffer, DWORD originalSize);

struct Index
{
	string name;
	DWORD address;
	DWORD length;
	DWORD originalLength;
};
vector<Index>* formatIndex(const BYTE* indexBuffer, DWORD fileCount, DWORD indexAddress);
BOOL exportFiles(FILE* f, const vector<Index>* index, string dirName);

const char E_OPEN_FILE[]		= "Failed to open the file!";
const int EN_OPEN_FILE			= 2;
const char E_UNKNOWN_TYPE[]		= "Unknown file type!";
const int EN_UNKNOWN_TYPE		= 3;
const char E_FILE_COUNT[]		= "Invalid file count!";
const int EN_FILE_COUNT			= 4;
const char E_FILE_SIZE[]		= "Invalid file size!";
const int EN_FILE_SIZE			= 5;
const char E_EXPORT[]			= "Failed to export files!";
const int EN_EXPORT				= 6;
