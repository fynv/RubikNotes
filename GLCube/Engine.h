#pragma once

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

struct GLBuffer
{
	unsigned m_id = -1;
	unsigned m_target = 0x8892;
	size_t m_size = 0;
	GLBuffer(size_t size, unsigned target = 0x8892 /*GL_ARRAY_BUFFER*/);
	~GLBuffer();
	void upload(const void* data);
	const GLBuffer& operator = (const GLBuffer& in);
};

struct GLShader
{
	unsigned m_type = 0;
	unsigned m_id = -1;
	GLShader(unsigned type, const char* code);
	~GLShader();

};

struct GLProgram
{
	unsigned m_id = -1;
	GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader);
	GLProgram(const GLShader& computeShader);
	~GLProgram();

};

struct Mesh
{
	Mesh();
	~Mesh();

	void clean();
	void load_obj(const char* filename);

	int num_pos = 0;
	int num_normal = 0;
	GLBuffer* pos_buf = nullptr;
	GLBuffer* norm_buf = nullptr;

	int num_face = 0;
	GLBuffer* ind_buf = nullptr;

	GLBuffer* trans_buf = nullptr;
};

class Renderer
{
public:
	Renderer(const char* fn_sky_dds, const char* fn_rad_dds, const char* fn_irrad_dds);
	~Renderer();
	
	void DrawSky(const glm::mat4& matProj, const glm::mat4& matView);
	void DrawMesh(const Mesh& mesh, const glm::mat4& matProj, const glm::mat4& matView, const glm::mat4& matModel);

	glm::mat4 m_mat_sky;
	unsigned m_tex_sky;
	unsigned m_tex_radiance;
	unsigned m_tex_irradiance;
};
