#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#if !defined(MAKEFOURCC)
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    (unsigned(ch0) | (unsigned(ch1) << 8) | \
    (unsigned(ch2) << 16) | (unsigned(ch3) << 24 ))
#endif


struct DDSPixelFormat
{
	unsigned size;
	unsigned flags;
	unsigned fourcc;
	unsigned bitcount;
	unsigned rmask;
	unsigned gmask;
	unsigned bmask;
	unsigned amask;
};

struct DDSHeader
{
	unsigned fourcc;
	unsigned size;
	unsigned flags;
	unsigned height;
	unsigned width;
	unsigned pitch;
	unsigned depth;
	unsigned mipmapcount;
	unsigned reserved[11];
	DDSPixelFormat pf;
	unsigned caps1;
	unsigned caps2;
	unsigned caps3;
	unsigned caps4;
	unsigned notused;
};


// This simple dds loader only handles uncompressed, non-pitched, non-mipmaped, dds files for now.
void* dds_load(const char* fn, int* pwidth, int* pheight, int* pchannels, int* isCube)
{
	FILE *fp = fopen(fn, "rb");
	if (fp==nullptr)
	{
		printf("Failed loading %s\n", fn);
		return nullptr;
	}

	DDSHeader header;
	fread(&header, sizeof(DDSHeader), 1, fp);

	if (header.fourcc != MAKEFOURCC('D', 'D', 'S', ' '))
	{
		printf("not a dds file.\n");
		fclose(fp);
		return nullptr;
	}

	/*if ((header.flags & 0x8A0008) !=0 ||
		(header.pf.flags & 0x20204) !=0)
	{
		printf("feature not supported.\n");
		fclose(fp);
		return nullptr;
	}*/

	int chn = 0;
	if ((header.pf.flags & 0x3) != 0)
		chn++;
	if ((header.pf.flags & 0x40) != 0)
		chn+= 3;
	*pchannels = chn;
	*pwidth = header.width;
	*pheight = header.height;
	*isCube = (header.caps1 & 0x8) == 0x8 && (header.caps2 & 0xFE00) == 0xFE00 ? 1 : 0;

	
	size_t size = *pwidth* (*pheight) * (*pchannels);
	if (*isCube) size *= 6;

	void *buf = malloc(size);
	fread(buf, 1, size, fp);

	fclose(fp);

	return buf;
}


void dds_free(void* data)
{
	free(data);
}

