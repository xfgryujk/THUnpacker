#pragma once
#include "THUnpackerBase.h"


class TH10To15Unpacker : public THUnpackerBase
{
protected:
	const DWORD magicNumber;
	const BYTE* decParam;

public:
	TH10To15Unpacker(FILE* _f, char* _dirName, DWORD _magicNumber, const BYTE* _decParam);

protected:
	void readHeader();
	void readIndex();
	void formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
	bool onUncompress(const Index& index, BYTE*& buffer, DWORD& size);
};


class TH10Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH10Unpacker(FILE* _f);
};

class TH11Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH11Unpacker(FILE* _f);
};

class TH12Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH12Unpacker(FILE* _f);
};

class TH13Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH13Unpacker(FILE* _f);
};

class TH14Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH14Unpacker(FILE* _f);
};

class TH15Unpacker : public TH10To15Unpacker
{
private:
	static const BYTE _decParam[];
public:
	TH15Unpacker(FILE* _f);
};
