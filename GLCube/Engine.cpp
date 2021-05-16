#include "Engine.h"
#include "GL/glew.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "dds_reader.hpp"

#pragma comment(lib, "glew.lib")

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    (unsigned(ch0) | (unsigned(ch1) << 8) | \
    (unsigned(ch2) << 16) | (unsigned(ch3) << 24 ))
#endif


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


GLBuffer::GLBuffer(size_t size, unsigned target)
{
	m_target = target;
	m_size = size;
	glGenBuffers(1, &m_id);
	glBindBuffer(m_target, m_id);
	glBufferData(m_target, m_size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(m_target, 0);
}

GLBuffer::~GLBuffer()
{
	if (m_id != -1)
		glDeleteBuffers(1, &m_id);
}

void GLBuffer::upload(const void* data)
{
	glBindBuffer(m_target, m_id);
	glBufferData(m_target, m_size, data, GL_STATIC_DRAW);
	glBindBuffer(m_target, 0);
}

const GLBuffer& GLBuffer::operator = (const GLBuffer& in)
{
	glBindBuffer(GL_COPY_READ_BUFFER, in.m_id);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_id);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_size);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	return *this;
}


GLShader::GLShader(unsigned type, const char* code)
{
	m_type = type;
	m_id = glCreateShader(type);
	glShaderSource(m_id, 1, &code, nullptr);
	glCompileShader(m_id);

	GLint compileResult;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == 0)
	{
		GLint infoLogLength;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		glGetShaderInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader compilation failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}
}

GLShader::~GLShader()
{
	if (m_id != -1)
		glDeleteShader(m_id);
}

GLProgram::GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader)
{
	m_id = glCreateProgram();
	glAttachShader(m_id, vertexShader.m_id);
	glAttachShader(m_id, fragmentShader.m_id);
	glLinkProgram(m_id);

	GLint linkResult;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
	if (linkResult == 0)
	{
		GLint infoLogLength;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader link failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}
}

GLProgram::GLProgram(const GLShader& computeShader)
{
	m_id = glCreateProgram();
	glAttachShader(m_id, computeShader.m_id);
	glLinkProgram(m_id);

	GLint linkResult;
	glGetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
	if (linkResult == 0)
	{
		GLint infoLogLength;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(m_id, (GLsizei)infoLog.size(), NULL, infoLog.data());

		printf("Shader link failed: %s", std::string(infoLog.begin(), infoLog.end()).c_str());
	}
}

GLProgram::~GLProgram()
{
	if (m_id != -1)
		glDeleteProgram(m_id);
}

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	clean();
}

void Mesh::clean()
{
	delete this->pos_buf;
	delete this->norm_buf;
	delete this->ind_buf;

	this->num_pos = 0;
	this->num_normal = 0;
	this->pos_buf = nullptr;
	this->norm_buf = nullptr;

	this->num_face = 0;
	this->ind_buf = nullptr;	
}

void Mesh::load_obj(const char* filename)
{
	clean();

	tinyobj::attrib_t                attrib;
	std::vector<tinyobj::shape_t>    shapes;
	std::vector<tinyobj::material_t> materials;
	std::string                      err;

	tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);

	std::vector<glm::vec4> positions(attrib.vertices.size() / 3);
	for (size_t i = 0; i < positions.size(); i++)
	{
		float* vp = &attrib.vertices[3 * i];
		positions[i] = glm::vec4(vp[0], vp[1], vp[2], 1.0f);		
	}

	std::vector<glm::vec4> normals(attrib.normals.size() / 3);
	for (size_t i = 0; i < normals.size(); i++)
	{
		float* np = &attrib.normals[3 * i];
		normals[i] = glm::vec4(np[0], np[1], np[2], 0.0f);
	}

	std::vector<glm::ivec4> indices;

	for (size_t i = 0; i < shapes.size(); i++)
	{
		tinyobj::shape_t& shape = shapes[i];
		for (size_t j = 0; j < shape.mesh.indices.size(); j++)
		{			
			int material_id = shape.mesh.material_ids[j / 3];			
			tinyobj::index_t& t_index = shape.mesh.indices[j];
			indices.push_back({ t_index.vertex_index, t_index.normal_index, i, material_id });
		}
	}

	this->num_pos = (int)positions.size();
	this->num_normal = (int)normals.size();
	this->num_face = (int)indices.size() / 3;	

	this->pos_buf = new GLBuffer(sizeof(glm::vec4)*positions.size());
	this->pos_buf->upload(positions.data());

	this->norm_buf = new GLBuffer(sizeof(glm::vec4)*normals.size());
	this->norm_buf->upload(normals.data());

	this->ind_buf = new GLBuffer(sizeof(glm::ivec4)*indices.size());
	this->ind_buf->upload(indices.data());	

	std::vector<glm::mat4> transforms(26, glm::identity<glm::mat4>());
	
	/*glm::mat4 trans = glm::identity<glm::mat4>();
	trans = glm::rotate(trans, DegToRad(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	transforms[3] = trans;
	transforms[7] = trans;
	transforms[11] = trans;
	transforms[12] = trans;
	transforms[15] = trans;
	transforms[19] = trans;
	transforms[20] = trans;
	transforms[23] = trans;
	transforms[24] = trans;*/

	this->trans_buf = new  GLBuffer(sizeof(glm::mat4)*26);
	this->trans_buf->upload(transforms.data());
}

Renderer::Renderer(const char* fn_sky_dds, const char* fn_rad_dds, const char* fn_irrad_dds)
{
	{
		int width, height, channel, isCube;
		unsigned char* data = (unsigned char*)dds_load(fn_sky_dds, &width, &height, &channel, &isCube);

		glGenTextures(1, &m_tex_sky);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_sky);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int f = GL_TEXTURE_CUBE_MAP_POSITIVE_X; f <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; f++)
			glTexImage2D(f, 0, GL_SRGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data + (size_t)width*height*channel* ((size_t)f - GL_TEXTURE_CUBE_MAP_POSITIVE_X));
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		dds_free(data);
	}

	{
		int width, height, channel, isCube;
		unsigned char* data = (unsigned char*)dds_load(fn_rad_dds, &width, &height, &channel, &isCube);

		glGenTextures(1, &m_tex_radiance);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_radiance);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int f = GL_TEXTURE_CUBE_MAP_POSITIVE_X; f <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; f++)
			glTexImage2D(f, 0, GL_SRGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data + (size_t)width * height * channel * ((size_t)f - GL_TEXTURE_CUBE_MAP_POSITIVE_X));
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		dds_free(data);
	}

	{
		int width, height, channel, isCube;
		unsigned char* data = (unsigned char*)dds_load(fn_irrad_dds, &width, &height, &channel, &isCube);
		glGenTextures(1, &m_tex_irradiance);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_irradiance);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (int f = GL_TEXTURE_CUBE_MAP_POSITIVE_X; f <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; f++)
			glTexImage2D(f, 0, GL_SRGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data + (size_t)width * height * channel * ((size_t)f - GL_TEXTURE_CUBE_MAP_POSITIVE_X));
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		dds_free(data);
	}

	m_mat_sky = glm::identity<glm::mat4>();
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_tex_irradiance);
	glDeleteTextures(1, &m_tex_radiance);
	glDeleteTextures(1, &m_tex_sky);
}

static const char* g_vertex_shader_sky =
"#version 430\n"
"layout (location = 0) uniform mat4 uInvViewMatrix;\n"
"layout (location = 1) uniform mat4 uInvProjectionMatrix;\n"
"layout (location = 0) out vec3 vCubeMapCoord;\n"
"void main()\n"
"{\n"
"    vec2 grid = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);\n"
"    vec2 pos_proj = grid * vec2(2.0, 2.0) + vec2(-1.0, -1.0);\n"
"    gl_Position = vec4(pos_proj, 1.0, 1.0);\n"
"    vec4 pos_view = uInvProjectionMatrix * gl_Position;\n"
"    pos_view = pos_view/pos_view.w;\n"
"    vCubeMapCoord = (mat3(uInvViewMatrix) * pos_view.xyz);\n"
"    vCubeMapCoord.z = - vCubeMapCoord.z;\n"
"}\n";

static const char* g_frag_shader_sky =
"#version 430\n"
"layout (location = 0) in vec3 vCubeMapCoord;\n"
"layout (location = 0) out vec4 outColor;\n"
"layout (location = 8) uniform samplerCube uCubeSky;\n"
"void main()\n"
"{\n"
"    outColor = texture(uCubeSky, vCubeMapCoord);\n"
"}\n";


void Renderer::DrawSky(const glm::mat4& matProj, const glm::mat4& matView)
{
	static GLShader s_vertex_shader(GL_VERTEX_SHADER, g_vertex_shader_sky);
	static GLShader s_frag_shader(GL_FRAGMENT_SHADER, g_frag_shader_sky);
	static GLProgram s_program(s_vertex_shader, s_frag_shader);	

	glm::mat4 inv_proj = glm::inverse(matProj);
	glm::mat4 inv_view = glm::inverse(matView * m_mat_sky);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glUseProgram(s_program.m_id);
	glUniformMatrix4fv(0, 1, false, (float*)&inv_view);
	glUniformMatrix4fv(1, 1, false, (float*)&inv_proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_sky);
	glUniform1i(8, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);
}

static const char* g_vertex_shader_mesh =
"#version 430\n"
"layout (location = 0) in ivec4 aInd;\n"
"layout (std430, binding = 0) buffer Position\n"
"{\n"
"	vec4 positions[];\n"
"}; \n"
"layout (std430, binding = 1) buffer Normal\n"
"{\n"
"	vec4 normals[];\n"
"}; \n"
"layout (std430, binding = 2) buffer Transform\n"
"{\n"
"	mat4 obj_trans[];\n"
"}; \n"
"layout (location = 0) uniform mat4 uProjMat;\n"
"layout (location = 1) uniform mat4 uViewMat;\n"
"layout (location = 2) uniform mat4 uModelMat;\n"
"layout (location = 3) uniform vec3 uEyePos;\n"
"layout (location = 0) out vec3 vNorm;\n"
"layout (location = 1) out vec3 vIncidentVector;\n"
"layout (location = 2) flat out int vObjId;\n"
"layout (location = 3) flat out int vMatId;\n"
"void main()\n"
"{\n"
"	vec4 model_pos = obj_trans[aInd.z] * vec4(positions[aInd.x].xyz, 1.0);\n"
"	vec4 world_pos = uModelMat * model_pos;\n"
"	vec4 view_pos = uViewMat * world_pos;\n"
"	vec4 model_norm = obj_trans[aInd.z] * vec4(normals[aInd.y].xyz, 0.0);\n"
"	vec4 world_norm = uModelMat * model_norm;\n"
"	gl_Position = uProjMat*view_pos;\n"
"	vNorm = normalize(world_norm.xyz);\n"
"	vIncidentVector = world_pos.xyz - uEyePos;\n"
"	vObjId = aInd.z;\n"
"	vMatId = aInd.w;\n"
"}\n";

static const char* g_frag_shader_mesh =
"#version 430\n"
"layout (location = 0) in vec3 vNorm;\n"
"layout (location = 1) in vec3 vIncidentVector;\n"
"layout (location = 2) flat in int vObjId;\n"
"layout (location = 3) flat in int vMatId;\n"
"layout (location = 10) uniform samplerCube uRadianceMap;\n"
"layout (location = 11) uniform samplerCube uIrradianceMap;\n"
"layout (location = 12) uniform mat4 uInvSkyMatrix;\n"
"out vec4 outColor;"
"vec3 to_sky(in vec3 dir_world)\n"
"{\n"
"	vec3 sky = (uInvSkyMatrix * vec4(dir_world, 0.0)).xyz;\n"
"	sky.z = -sky.z;\n"
"	return sky;\n"
"}\n"
"void main()\n"
"{\n"
"   vec3 norm = normalize(vNorm);\n"
"   vec3 I = normalize(vIncidentVector);\n"
"   vec3 R = reflect(I, norm);\n"
"   vec3 l_diffuse = texture(uIrradianceMap, to_sky(norm)).rgb;\n "
"   vec3 l_specular = texture(uRadianceMap, to_sky(R)).rgb;\n "
"   if (vMatId == 0)\n"
"   {\n"
"       vec3 col = l_diffuse*0.2 + l_specular*0.8;\n"
"       outColor = vec4(col, 1.0);\n"
"   }\n"
"   else\n"
"   {\n"
"       vec3 col = l_diffuse*0.1;\n"
"       outColor = vec4(col, 1.0);\n"
"   }\n"
"}\n";

void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& matProj, const glm::mat4& matView, const glm::mat4& matModel)
{
	static GLShader s_vertex_shader_mesh(GL_VERTEX_SHADER, g_vertex_shader_mesh);
	static GLShader s_frag_shader_mesh(GL_FRAGMENT_SHADER, g_frag_shader_mesh);
	static GLProgram s_program_mesh(s_vertex_shader_mesh, s_frag_shader_mesh);
	
	glm::mat4 matViewInv = glm::inverse(matView);
	glm::vec3 eye_pos = matViewInv[3];
	glm::mat4 matSkyInv = glm::inverse(m_mat_sky);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(s_program_mesh.m_id);
	glUniformMatrix4fv(0, 1, false, (float*)&matProj);
	glUniformMatrix4fv(1, 1, false, (float*)&matView);
	glUniformMatrix4fv(2, 1, false, (float*)&matModel);
	glUniform3fv(3, 1, (float*)&eye_pos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_radiance);
	glUniform1i(10, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_irradiance);
	glUniform1i(11, 2);

	glUniformMatrix4fv(12, 1, false, (float*)&matSkyInv);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.ind_buf->m_id);
	glVertexAttribIPointer(0, 4, GL_INT, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mesh.pos_buf->m_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh.norm_buf->m_id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mesh.trans_buf->m_id);

	glDrawArrays(GL_TRIANGLES, 0, mesh.num_face*3);		
	

	glUseProgram(0);

}


