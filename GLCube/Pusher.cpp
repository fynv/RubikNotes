#include "Pusher.h"
#include "GL/glew.h"
#include <cstdint>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <vector>

inline uint64_t time_micro_sec()
{
	std::chrono::time_point<std::chrono::system_clock> tpSys = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> tpMicro
		= std::chrono::time_point_cast<std::chrono::microseconds>(tpSys);
	return tpMicro.time_since_epoch().count();
}

inline double time_sec()
{
	return (double)time_micro_sec() / 1000000.0;
}


#define LEN_QUEUE 3
static const char MAPPING_NAME[] = "LiveKitVCam";

struct Header
{
	int width;
	int height;
	int chn;
	int cur_frame;
};

struct FrameHeader
{
	int is_flipped;
	uint64_t timestamp;
};

Pusher::Pusher(){}

Pusher::~Pusher()
{
	Close();
}

void Pusher::Close()
{
	if (m_hMapFile != NULL)
	{
		UnmapViewOfFile(m_data);
		CloseHandle(m_hMapFile);
	}
}

void Pusher::Push(int width, int height)
{
	size_t data_size = width * height * 3;
	size_t total_size = sizeof(Header) + (sizeof(FrameHeader) + data_size) * LEN_QUEUE;

	if (width != m_width || height != m_height)
	{
		Close();		
		m_hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)(total_size), MAPPING_NAME);			
		m_data = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int) * 2 + data_size * LEN_QUEUE + sizeof(uint64_t)*LEN_QUEUE);
		memset(m_data, 0, total_size);
		*(Header*)m_data = { width, height, 3, 2 };	

		m_width = width;
		m_height = height;
	}

	{

		Header* header = (Header*)m_data;
		int cur_frame = header->cur_frame;

		size_t frame_size = sizeof(FrameHeader) + width * height * 3;
		unsigned char* p_frames = (unsigned char*)(header + 1);
		int write_frame = (cur_frame + 1) % 3;

		FrameHeader* frame_header = (FrameHeader*)(p_frames + frame_size * write_frame);
		frame_header->is_flipped = 1;
		frame_header->timestamp = time_micro_sec();

		unsigned char* p_data_out = (unsigned char*)(frame_header + 1);
		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, p_data_out);
		header->cur_frame = write_frame;

#if 0
		static double s_t0 = time_sec();
		static int s_count = 0;

		double t1 = time_sec();
		if (t1 - s_t0 >= 0.5)
		{
			s_t0+=0.5;
			s_count++;

			char fn[1024];
			sprintf(fn, ".\\dump\\%d.png", s_count);

			std::vector<uint8_t> rgb(data_size);
			for (size_t i = 0; i < width * height; i++)
			{
				const uint8_t* p_bgr = p_data_out + i * 3;
				uint8_t* p_rgb = &rgb[((height - i/ width -1)*width +  i % width) * 3];
				p_rgb[0] = p_bgr[2];
				p_rgb[1] = p_bgr[1];
				p_rgb[2] = p_bgr[0];
			}
			stbi_write_png(fn, width, height, 3, rgb.data(), width * 3);
		}

#endif
	}
}

