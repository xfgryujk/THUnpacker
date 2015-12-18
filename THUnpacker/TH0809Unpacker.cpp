#include "stdafx.h"
#include "TH0809Unpacker.h"


TH0809Unpacker::TH0809Unpacker(FILE* _f) 
: THUnpackerBase(_f)
{
	dirName = "th0809";
}

void TH0809Unpacker::readHeader()
{
	DWORD header[3];
	fread(header, 1, 12, f);
	// decrypt
	thDecrypt((BYTE*)header, 12, 27, 55, 12, 1024);
	count = header[0] - 123456;
	indexAddress = header[1] - 345678;
	originalIndexSize = header[2] - 567891;
}

void TH0809Unpacker::readIndex()
{
	fseek(f, indexAddress, SEEK_SET);
	DWORD indexSize = fileSize - indexAddress;
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// decrypt
	BYTE* result = thDecrypt(indexBuffer, indexSize, 62, -101, 128, 1024);
	delete indexBuffer;
	indexBuffer = result;
	// uncompress
	result = thUncompress(indexBuffer, indexSize, NULL, originalIndexSize);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = originalIndexSize;

	// format index
	formatIndex(index, indexBuffer, count, indexAddress);
	delete indexBuffer;
}

// see th09.exe.0042BAF0, not used in th08
void TH0809Unpacker::onExport(const Index& index, BYTE*& buffer, DWORD& size)
{
	// see th09.exe.00498E60
	static const BYTE decParam[] = {
		0x5D, 0x1B, 0x37, 0xAA, 0x40, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x74, 0x51, 0xE9, 0xBB,
		0x40, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x71, 0xC1, 0x51, 0xCC, 0x00, 0x04, 0x00, 0x00,
		0x00, 0x04, 0x00, 0x00, 0x8A, 0x03, 0x19, 0xDD, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
		0x95, 0xAB, 0xCD, 0xEE, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0xB7, 0x12, 0x34, 0xFF,
		0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x9D, 0x35, 0x97, 0x11, 0x80, 0x00, 0x00, 0x00,
		0x00, 0x28, 0x00, 0x00, 0xAA, 0x99, 0x37, 0x77, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00
	};

	if (buffer[0] != 'e' || buffer[1] != 'd' || buffer[2] != 'z')
		return;

	int magicNumber = buffer[3];
	int i = 0;
	if (magicNumber != 'M')
	{
		DWORD offset = 0;
		while (offset < 0x60)
		{
			i++;
			if (magicNumber == (decParam + 0xC)[offset] - 16 * (i + 1))
			{
				if (i >= 8)
					return;
				break;
			}
			offset += 0xC;
		}
	}

	DWORD newSize = size - 4;
	BYTE* result = new BYTE[newSize];
	memcpy(result, &buffer[4], newSize);
	delete buffer;
	buffer = result;
	thDecrypt(
		result,
		newSize,
		(decParam + 0x1)[12 * i],
		(decParam + 0x2)[12 * i],
		((int*)(decParam + 0x4))[3 * i],
		((int*)(decParam + 0x8))[3 * i]);
}
