#pragma once
#include <string>
#include <vector>
#include <memory>


struct Index
{
	std::string name;
	DWORD address;
	DWORD length;
	DWORD originalLength;
};

class THUnpackerBase
{
protected:
	FILE* f = NULL;
	int count = 0;
	DWORD indexAddress = 0;
	DWORD originalIndexSize = 0;
	DWORD fileSize = 0;
	std::vector<Index> index;
	std::wstring dirName = L"th";

public:
	static std::shared_ptr<THUnpackerBase> Create(FILE* _f);

protected:
	static DWORD GetFileSize(FILE* f);
	static std::wstring StringToWstring(const std::string& src);

	static void THDecrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6);
	static std::unique_ptr<BYTE[]> THUncompress(const BYTE* buffer, DWORD bufferSize, DWORD originalSize);


public:
	THUnpackerBase(FILE* _f);
	virtual ~THUnpackerBase() = default;

	virtual int Unpack();

protected:
	virtual void ReadHeader() = 0;
	virtual int CheckCountAndSize();
	virtual void ReadIndex() = 0;
	// Called by ExportFiles. Return true if need to uncompress
	virtual bool OnUncompress(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) { return true; }
	// Called by ExportFiles
	virtual void OnExport(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) { }
	virtual void FormatIndex(std::vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
	virtual bool ExportFiles(FILE* f, const std::vector<Index>& index, std::wstring dirName);
};
