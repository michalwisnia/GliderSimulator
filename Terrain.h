#pragma once;
#include <filesystem>
#include <fstream>
#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <cstdint>
#include <memory>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/std_based_type.hpp>

class Terrain
{
public:

    Terrain(float heightScale = 4000.0f, float blockScale = 4.0f);

    bool LoadHeightmap(const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height);

    float GetHeightAt(const glm::vec3& position);

    std::vector<float> Vertices = {};
    std::vector<float> Normals = {};
    std::vector<float> TexCoords = {};
    unsigned int vertexCount;

protected:
    void GenerateIndexBuffer();
    void GenerateNormals();
 
private:
    
    std::vector<glm::vec3>  m_PositionBuffer;
    std::vector<glm::vec4>     m_ColorBuffer;
    std::vector<glm::vec3>    m_NormalBuffer;
    std::vector<glm::vec2>  m_Tex0Buffer;
    std::vector<GLuint>     m_IndexBuffer;

    static const unsigned int m_uiNumTextures = 3;
    GLuint  m_GLTextures[m_uiNumTextures];

    glm::mat4x4 m_LocalToWorldMatrix;
    glm::mat4x4 m_InverseLocalToWorldMatrix;

    glm::uvec2 m_HeightmapDimensions;

    float   m_fHeightScale;
    
    float   m_fBlockScale;
};