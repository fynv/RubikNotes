#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Engine.h"
#include "RubiksCube.h"
#include "Pusher.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "glew.lib")


#ifndef PI
#define PI 3.1415926f
inline float DegToRad(float deg)
{
	return deg * (PI / 180.0f);
}

inline float RadToDeg(float rad)
{
	return rad * (180.0f / PI);
}
#endif


#include <cstdint>
#include <chrono>

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

std::vector<glm::mat4> get_transforms(const RubiksCube& cube, float radius, const std::string& op = "", float progress= 0.0f)
{
	static glm::vec3 s_origins[6 * 9] =
	{
		{ 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f, 0.0f}, { 1.0f, 1.0f, -1.0f},
		{ 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f, -1.0f},
		{ 1.0f, -1.0f, 1.0f}, { 1.0f, -1.0f, 0.0f}, { 1.0f, -1.0f, -1.0f},

		{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 1.0f},
		{-1.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, 1.0f},

		{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
		{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},

		{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, -1.0f, 0.0f},
		{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},

		{-1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
		{-1.0f, 0.0f, 1.0f }, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f }, {0.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},

		{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
		{1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, -1.0f},
		{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f},
	};

	static glm::vec3 s_dirs[6] = { {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} };
	static glm::vec3 s_dirs2[6 * 4] =
	{
		{ 0.0f, 0.0f, -1.0f}, { 0.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f, 0.0f},
		{ 0.0f, 0.0f, 1.0f}, { 0.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, -1.0f}, { 0.0f, 1.0f, 0.0f},
		{ 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, -1.0f},
		{ 1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, -1.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f},
		{ 1.0f, 0.0f, 0.0f}, { 0.0f, -1.0f, 0.0f}, { -1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f},
		{ -1.0f, 0.0f, 0.0f}, { 0.0f, -1.0f, 0.0f}, { 1.0f, 0.0f, 0.0f}, { 0.0f, 1.0f, 0.0f}
	};

	static int ids_center[6] = { 31, 22, 13, 4, 40, 49 };
	
	static glm::ivec2 ids_edge[12] = {
		{ 16, 30 }, {7, 32}, { 43, 28}, { 52, 34},
		{ 14, 39}, {41, 3}, { 5, 48}, { 50, 12},
		{ 10, 21}, {1, 23}, { 37, 25}, { 46, 19}
	};	

	static glm::ivec3 ids_corner[8] = {
		{ 27, 17, 42 }, { 29, 44, 6}, { 35, 8, 51 }, { 33, 53, 15},
		{ 24, 36, 11 }, { 26, 0, 38}, { 20, 45, 2}, { 18, 9, 47}
	};


	RubiksCube reverse;
	reverse /= cube;

	int moving_face = -1;
	glm::mat4 trans_moving = glm::identity<glm::mat4>();

	if (op == "R")
	{
		moving_face = 0;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (op == "R'")
	{
		moving_face = 0;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (op == "L")
	{
		moving_face = 1;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (op == "L'")
	{
		moving_face = 1;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (op == "U")
	{
		moving_face = 2;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (op == "U'")
	{
		moving_face = 2;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (op == "D")
	{
		moving_face = 3;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (op == "D'")
	{
		moving_face = 3;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (op == "F")
	{
		moving_face = 4;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (op == "F'")
	{
		moving_face = 4;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (op == "B")
	{
		moving_face = 5;
		trans_moving = glm::rotate(trans_moving, DegToRad(90.0f)*progress, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (op == "B'")
	{
		moving_face = 5;
		trans_moving = glm::rotate(trans_moving, -DegToRad(90.0f)*progress, glm::vec3(0.0f, 0.0f, 1.0f));
	}


	std::vector<glm::mat4> res(26, glm::identity<glm::mat4>());

	for (int i = 0; i < 6; i++)
	{
		int face_in = ids_center[i];		
		glm::vec3 origin_in = s_origins[face_in] * radius;
		glm::vec3 dir1_in = s_dirs[face_in / 9];
		glm::vec3 dir2_in = s_dirs2[face_in / 9 * 4];
		glm::vec3 dir3_in = glm::normalize(glm::cross(dir1_in, dir2_in));
		glm::mat4 mat_in;
		mat_in[0] = glm::vec4(dir1_in, 0.0f);
		mat_in[1] = glm::vec4(dir2_in, 0.0f);
		mat_in[2] = glm::vec4(dir3_in, 0.0f);
		mat_in[3] = glm::vec4(origin_in, 1.0f);

		int face_out = reverse.map[face_in];
		glm::vec3 origin_out = s_origins[face_out] * radius;
		glm::vec3 dir1_out = s_dirs[face_out / 9];
		glm::vec3 dir2_out = s_dirs2[face_out / 9 * 4 + reverse.dirs[face_out]];
		glm::vec3 dir3_out = glm::normalize(glm::cross(dir1_out, dir2_out));
		glm::mat4 mat_out;
		mat_out[0] = glm::vec4(dir1_out, 0.0f);
		mat_out[1] = glm::vec4(dir2_out, 0.0f);
		mat_out[2] = glm::vec4(dir3_out, 0.0f);
		mat_out[3] = glm::vec4(origin_out, 1.0f);
		
		res[i] = mat_out * glm::inverse(mat_in);

		if (face_out / 9 == moving_face)
		{
			res[i] = trans_moving * res[i];
		}
	}

	for (int i = 0; i < 12; i++)
	{
		glm::ivec2 face_in = ids_edge[i];
		glm::vec3 origin_in = s_origins[face_in.x] * radius;
		glm::vec3 dir1_in = s_dirs[face_in.x / 9];
		glm::vec3 dir2_in = s_dirs[face_in.y / 9];
		glm::vec3 dir3_in = glm::normalize(glm::cross(dir1_in, dir2_in));
		glm::mat4 mat_in;
		mat_in[0] = glm::vec4(dir1_in, 0.0f);
		mat_in[1] = glm::vec4(dir2_in, 0.0f);
		mat_in[2] = glm::vec4(dir3_in, 0.0f);
		mat_in[3] = glm::vec4(origin_in, 1.0f);

		glm::ivec2 face_out = { reverse.map[face_in.x], reverse.map[face_in.y] };
		glm::vec3 origin_out = s_origins[face_out.x] * radius;
		glm::vec3 dir1_out = s_dirs[face_out.x / 9];
		glm::vec3 dir2_out = s_dirs[face_out.y / 9];
		glm::vec3 dir3_out = glm::normalize(glm::cross(dir1_out, dir2_out));
		glm::mat4 mat_out;
		mat_out[0] = glm::vec4(dir1_out, 0.0f);
		mat_out[1] = glm::vec4(dir2_out, 0.0f);
		mat_out[2] = glm::vec4(dir3_out, 0.0f);
		mat_out[3] = glm::vec4(origin_out, 1.0f);

		res[i+6] = mat_out * glm::inverse(mat_in);

		if (face_out.x /9 == moving_face || face_out.y/9 == moving_face)
		{
			res[i + 6] = trans_moving * res[i + 6];
		}
	}

	for (int i = 0; i < 8; i++)
	{
		glm::ivec3 face_in = ids_corner[i];
		glm::vec3 origin_in = s_origins[face_in.x] * radius;
		
		glm::vec3 dir1_in = s_dirs[face_in.x / 9];
		glm::vec3 dir2_in = s_dirs[face_in.y / 9];
		glm::vec3 dir3_in = s_dirs[face_in.z / 9];
		glm::mat4 mat_in;
		mat_in[0] = glm::vec4(dir1_in, 0.0f);
		mat_in[1] = glm::vec4(dir2_in, 0.0f);
		mat_in[2] = glm::vec4(dir3_in, 0.0f);
		mat_in[3] = glm::vec4(origin_in, 1.0f);

		glm::ivec3 face_out = { reverse.map[face_in.x], reverse.map[face_in.y], reverse.map[face_in.z] };
		glm::vec3 origin_out = s_origins[face_out.x] * radius;
		glm::vec3 dir1_out = s_dirs[face_out.x / 9];
		glm::vec3 dir2_out = s_dirs[face_out.y / 9];
		glm::vec3 dir3_out = s_dirs[face_out.z / 9];
		glm::mat4 mat_out;
		mat_out[0] = glm::vec4(dir1_out, 0.0f);
		mat_out[1] = glm::vec4(dir2_out, 0.0f);
		mat_out[2] = glm::vec4(dir3_out, 0.0f);
		mat_out[3] = glm::vec4(origin_out, 1.0f);

		res[i + 18] = mat_out * glm::inverse(mat_in);

		if (face_out.x / 9 == moving_face || face_out.y / 9 == moving_face || face_out.z / 9 == moving_face)
		{
			res[i + 18] = trans_moving * res[i + 18];
		}
	}

	return res;
}

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1200, 900, "GLCube", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();

	int width_video = 1200;
	int height_video = 900;
	unsigned tex_video, fbo_video, tex_msaa, rbo_msaa, fbo_msaa;

	glGenFramebuffers(1, &fbo_video);
	glGenTextures(1, &tex_video);
	glGenFramebuffers(1, &fbo_msaa);
	glGenTextures(1, &tex_msaa);
	glGenRenderbuffers(1, &rbo_msaa);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_video);

	glBindTexture(GL_TEXTURE_2D, tex_video);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width_video, height_video, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_video, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex_msaa);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_SRGB8_ALPHA8, width_video, height_video, true);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex_msaa, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo_msaa);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, width_video, height_video);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_msaa);

	float fovy = DegToRad(45.0f);
	float near_plane = 10.0f;
	float far_plane = 1000.0f;

	Mesh mesh;
	mesh.load_obj("mirror.obj");

	RubiksCube::s_Initialize();
	RubiksCube cube_start;	
	cube_start.exec_seq("U'F2UB2D'R2UF2L2D'F2L'R'F'DBDU2B2R2");
	std::string solve = "F'LF2'BUR'U'B2R'U2'R'URU2R'URUBU'B'B'UBUFU'F'U2F'UFU2F'U'FULU'L'U'B'UBU2FU'F'U'L'ULRU2R'U'RU'R'R2URUR'U'R'U'R'UR'U2";
	std::vector<const RubiksCube*> operations;
	std::vector<std::string> notes;
	std::vector<int> groups;
	RubiksCube::parse_seq(solve, operations, notes, groups);	
	
	Renderer renderer("sky_skybox.dds", "sky_pmrem.dds", "sky_iem.dds");
	renderer.m_mat_sky = glm::rotate(renderer.m_mat_sky, DegToRad(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 look_from = { 50.0f, 50.0f, 100.0f };
	glm::vec3 look_at = { 0.0f, -10.0f, 0.0f };

	Pusher pusher;

	double t0 = time_sec();
	RubiksCube cube = cube_start;
	size_t pos = 0;
	float prog = 0.0f;
	float speed = 3.0f;

	double camera_t0 = time_sec();	
	float camera_angle = 0.0f;
	float camera_speed = DegToRad(10.0f);

	bool rotating = false;

	while (glfwWindowShouldClose(window) == 0)
	{
		std::vector<glm::mat4> trans = get_transforms(cube, 6.0f, notes[pos], prog);
		mesh.trans_buf->upload(trans.data());

		glfwPollEvents();
		int width_wnd, height_wnd;
		glfwGetFramebufferSize(window, &width_wnd, &height_wnd);

		look_from = { 110.0f* sinf(camera_angle), 50.0f, 110.0f* cosf(camera_angle) };

		glm::mat4 proj = glm::perspective(fovy, (float)width_video / (float)height_video, near_plane, far_plane);
		glm::mat4 view = glm::lookAt(look_from, look_at, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 model = glm::identity<glm::mat4>();

		double camera_t1 = time_sec();
		camera_angle += camera_speed * (camera_t1 - camera_t0);
		camera_t0 = camera_t1;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_FRAMEBUFFER_SRGB);

		glViewport(0, 0, width_video, height_video);

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 4xMSAA		
		renderer.DrawSky(proj, view);
		renderer.DrawMesh(mesh, proj, view, model);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_video);
		glBlitFramebuffer(0, 0, width_video, height_video, 0, 0, width_video, height_video, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_video);
		
		// No MSAA
		pusher.Push(width_video, height_video);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glViewport(0, 0, width_wnd, height_wnd);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// To Window
		if (width_wnd < width_video || height_wnd < height_video)
		{
			// scale down
			int dst_w = width_video * height_wnd / height_video;
			if (dst_w <= width_wnd)
			{
				int dst_offset = (width_wnd - dst_w) / 2;
				glBlitFramebuffer(0, 0, width_video, height_video, dst_offset, 0, dst_offset + dst_w, height_wnd, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			}
			else
			{
				int dst_h = height_video * width_wnd / width_video;
				int dst_offset = (height_wnd - dst_h) / 2;
				glBlitFramebuffer(0, 0, width_video, height_video, 0, dst_offset, width_wnd, dst_offset + dst_h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			}
		}
		else
		{
			// center
			int offset_x = (width_wnd - width_video) / 2;
			int offset_y = (height_wnd - height_video) / 2;
			glBlitFramebuffer(0, 0, width_video, height_video, offset_x, offset_y, offset_x + width_video, offset_y + height_video, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glfwSwapBuffers(window);

		if (rotating)
		{
			double t1 = time_sec();
			double dt = t1 - t0;
			t0 = t1;

			prog += speed * dt;
			while (prog > 1.0f)
			{
				prog -= 1.0f;
				cube *= *operations[pos];
				pos++;
				if (pos >= notes.size())
				{
					//pos = 0;
					//cube = cube_start;
					rotating = false;
				}
			}
		}
		else
		{
			double t1 = time_sec();
			if (t1 - t0 > 3.0f)
			{
				pos = 0;
				cube = cube_start;
				rotating = true;
				t0 = t1;
			}
		}
	}

	glDeleteRenderbuffers(1, &rbo_msaa);
	glDeleteTextures(1, &tex_msaa);
	glDeleteFramebuffers(1, &fbo_msaa);
	glDeleteTextures(1, &tex_video);
	glDeleteFramebuffers(1, &fbo_video);

	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}



