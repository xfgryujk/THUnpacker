#pragma once
#include "THUnpackerBase.h"


class TH10To15Unpacker : public THUnpackerBase
{
protected:
	const DWORD magicNumber;
	const BYTE* decParam;

public:
	TH10To15Unpacker(std::ifstream& _f, wchar_t* _dirName, DWORD _magicNumber, const BYTE* _decParam);
	virtual ~TH10To15Unpacker() = default;

protected:
	virtual void ReadHeader() override;
	virtual void ReadIndex() override;
	virtual void FormatIndex(std::vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress) override;
	virtual bool OnUncompress(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) override;
};


class TH10Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH10Unpacker(std::ifstream& _f);
};

class TH11Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH11Unpacker(std::ifstream& _f);
};

class TH12Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH12Unpacker(std::ifstream& _f);
};

class TH13Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH13Unpacker(std::ifstream& _f);
};

class TH14Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH14Unpacker(std::ifstream& _f);
};

class TH15Unpacker final : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH15Unpacker(std::ifstream& _f);
};
