#include "stdafx.h"
#include "common.h"


int unpackTH07(FILE* f)
{
	// read header
	DWORD header[3]; // file count, index address, original index size
	fread(header, 1, 12, f);

	// check
	if (header[0] <= 0)
	{
		puts("Invalid file count!");
		return 3;
	}

	DWORD fileSize = getFileSize(f);
	if (fileSize <= header[1])
	{
		puts("Invalid file size!");
		return 4;
	}

	// read index
	fseek(f, header[1], SEEK_SET);
	DWORD indexSize = fileSize - header[1];
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// uncompress
	BYTE* result = uncompress(indexBuffer, indexSize, NULL, header[2]);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = header[2];

	// format index
	vector<Index>* index = formatIndex(indexBuffer, header[0], header[1]);
	delete indexBuffer;

	// export files
	if (!exportFiles(f, index, "th07"))
	{
		delete index;
		puts("Failed to export files!");
		return 5;
	}
	delete index;

	puts("done.");
	return 0;
}

int unpackTH08(FILE* f)
{
	// read header
	DWORD header[3]; // file count, index address, original index size
	fread(header, 1, 12, f);
	// decrypt
	BYTE* result = decrypt((BYTE*)header, 12, 27, 55, 12, 1024);
	memcpy(header, result, 12);
	delete result;
	header[0] -= 123456;
	header[1] -= 345678;
	header[2] -= 567891;

	// check
	if (header[0] <= 0)
	{
		puts("Invalid file count!");
		return 3;
	}

	DWORD fileSize = getFileSize(f);
	if (fileSize <= header[1])
	{
		puts("Invalid file size!");
		return 4;
	}

	// read index
	fseek(f, header[1], SEEK_SET);
	DWORD indexSize = fileSize - header[1];
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// decrypt
	result = decrypt(indexBuffer, indexSize, 62, -101, 128, 1024);
	delete indexBuffer;
	indexBuffer = result;
	// uncompress
	result = uncompress(indexBuffer, indexSize, NULL, header[2]);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = header[2];

	// format index
	vector<Index>* index = formatIndex(indexBuffer, header[0], header[1]);
	delete indexBuffer;

	// export files
	if (!exportFiles(f, index, "th08"))
	{
		delete index;
		puts("Failed to export files!");
		return 5;
	}
	delete index;

	puts("done.");
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		puts("Usage: THUnpacker filename");
		return 1;
	}

	// open file
	FILE* f;
	fopen_s(&f, argv[1], "rb");
	if (f == NULL)
	{
		puts("Failed to open the file!");
		return 1;
	}

	// check magic number
	DWORD magicNumber = 0;
	fread(&magicNumber, 4, 1, f);
	int result = 0;
	switch (magicNumber)
	{
	case 0x34474250: // "PBG4" for TH07
		result = unpackTH07(f);
		break;
	case 0x5A474250: // "PBGZ" for TH08
		result = unpackTH08(f);
		break;
	default:
		puts("Unknown file type!");
		result = 2;
		break;
	}

	fclose(f);
	return result;
}
