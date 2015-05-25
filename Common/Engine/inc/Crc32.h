#pragma once

namespace Crc32
{
	DWORD ScanFile(const char* filePath);
	DWORD ScanFilePart(const char* filePath, DWORD startOffset, DWORD endOffset);
	DWORD ScanData(const void* pData, int dataSize);

	DWORD GenerateHash(const void* dataBuffer, size_t size);

	DWORD CreateID(const char* string);
};