#pragma once
#include <string>
#include <fstream>
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
	std::ifstream& f;
	int count = 0;
	DWORD indexAddress = 0;
	DWORD originalIndexSize = 0;
	DWORD fileSize = 0;
	std::vector<Index> index;
	std::wstring dirName = L"th";

public:
	static std::shared_ptr<THUnpackerBase> Create(std::ifstream& _f);

protected:
	static DWORD GetFileSize(std::ifstream& f);
	static std::wstring StringToWstring(const std::string& src);

	static void THDecrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6);
	static std::unique_ptr<BYTE[]> THUncompress(const BYTE* buffer, DWORD bufferSize, DWORD originalSize);


public:
	THUnpackerBase(std::ifstream& _f);
	virtual ~THUnpackerBase() = default;

	virtual bool Unpack();

protected:
	virtual void ReadHeader() = 0;
	virtual bool CheckCountAndSize();
	virtual void ReadIndex() = 0;
	// Called by ExportFiles. Return true if need to uncompress
	virtual bool OnUncompress(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) { return true; }
	// Called by ExportFiles
	virtual void OnExport(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) { }
	virtual void FormatIndex(std::vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
	virtual bool ExportFiles(std::ifstream& f, const std::vector<Index>& index, std::wstring dirName);
};
