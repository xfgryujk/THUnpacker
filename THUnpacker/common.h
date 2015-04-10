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
