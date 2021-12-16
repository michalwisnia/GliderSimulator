
#include "Terrain.h"
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
#include <glm/gtx/compatibility.hpp>

float GetPercentage(float value, const float min, const float max)
{
    value = glm::clamp(value, min, max);
    return (value - min) / (max - min);
}

// Get's the size of the file in bytes.
int GetFileLength(std::istream& file)
{
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int length = file.tellg();
    // Restore the position of the get pointer
    file.seekg(pos);
    return length;
}

// Convert data from the char buffer to a floating point value between 0..1
// depending on the storage value of the original data
// NOTE: This only works with (LSB,MSB) data storage.
inline float GetHeightValue(const unsigned char* data, unsigned char numBytes)
{
    switch (numBytes)
    {
    case 1:
    {
        return (unsigned char)(data[0]) / (float)0xff;
    }
    break;
    case 2:
    {
        return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
    }
    break;
    case 4:
    {
        return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
    }
    break;
    default:
    {
        assert(false);  // Height field with non standard pixel sizes
    }
    break;
    }

    return 0.0f;
}

Terrain::Terrain(float heightScale /* = 500.0f */, float blockScale)
    : m_LocalToWorldMatrix(1)
    , m_InverseLocalToWorldMatrix(1)
    , m_HeightmapDimensions(0, 0)
    , m_fHeightScale(heightScale)
    , m_fBlockScale(blockScale)
{
    
}

bool Terrain::LoadHeightmap(const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height)
{

    std::cout << "Loading Terrain...\n";
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::binary);
    if (ifs.fail())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    const unsigned int bytesPerPixel = bitsPerPixel / 8;
    const unsigned int expectedFileSize = (bytesPerPixel * width * height);
    const unsigned int fileSize = GetFileLength(ifs);
    
    if (expectedFileSize != fileSize)
    {
        std::cerr << "Expected file size [" << expectedFileSize << " bytes] differs from actual file size [" << fileSize << " bytes]" << std::endl;
        return false;
    }

    unsigned char* heightMap = new unsigned char[fileSize];
    ifs.read((char*)heightMap, fileSize);
    if (ifs.fail())
    {
        std::cerr << "An error occurred while reading from the height map file: " << filename << std::endl;
        ifs.close();
        delete[] heightMap;
        return false;
    }
    ifs.close();
    
    unsigned int numVerts = width * height;
    m_PositionBuffer.resize(numVerts);
    m_ColorBuffer.resize(numVerts);
    m_NormalBuffer.resize(numVerts);
    m_Tex0Buffer.resize(numVerts);

    m_HeightmapDimensions = glm::uvec2(width, height);
    std::ofstream file5(filename + "_Vertices.txt");
    std::ofstream file1(filename + "_Normals.txt");
    std::ofstream file2(filename + "_TexCoords.txt");

    float terrainWidth = (width - 1) * m_fBlockScale;
    float terrainHeight = (height - 1) * m_fBlockScale;

    float halfTerrainWidth = terrainWidth * 0.5f;
    float halfTerrainHeight = terrainHeight * 0.5f;

    for (unsigned int j = 0; j < height; ++j)
    {
        for (unsigned i = 0; i < width; ++i)
        {
            unsigned int index = (j * width) + i;
            assert(index * bytesPerPixel < fileSize);
            float heightValue = GetHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

            float S = (i / (float)(width - 1));
            float T = (j / (float)(height - 1));

            float X = (S * terrainWidth) - halfTerrainWidth;
            float Y = heightValue * m_fHeightScale;
            float Z = (T * terrainHeight) - halfTerrainHeight;
         
            m_NormalBuffer[index] = glm::vec3(0);
            m_PositionBuffer[index] = glm::vec3(X, Y, Z);
            //file5 << X << ", " << Y << ", " << Z << ",\n";
           
            m_Tex0Buffer[index] = glm::vec2(S, T);
            file2 << S << ", " << T << ",\n";
        }
    }
    
    
    delete[] heightMap;
    
    GenerateIndexBuffer();
    GenerateNormals();

    for (int j = 0; j < m_IndexBuffer.size(); j += 1)
    {
        //file5 << m_PositionBuffer[m_IndexBuffer[j]].x << ", " << m_PositionBuffer[m_IndexBuffer[j]].y << ", " << m_PositionBuffer[m_IndexBuffer[j]].z << ",\n";

        Vertices.push_back(m_PositionBuffer[m_IndexBuffer[j]].x);
        Vertices.push_back(m_PositionBuffer[m_IndexBuffer[j]].y);
        Vertices.push_back(m_PositionBuffer[m_IndexBuffer[j]].z);

        //file1 << m_NormalBuffer[m_IndexBuffer[j]].x << ", " << m_NormalBuffer[m_IndexBuffer[j]].y << ", " << m_NormalBuffer[m_IndexBuffer[j]].z << ",\n ";

        Normals.push_back(m_NormalBuffer[m_IndexBuffer[j]].x);
        Normals.push_back(m_NormalBuffer[m_IndexBuffer[j]].y);
        Normals.push_back(m_NormalBuffer[m_IndexBuffer[j]].z);

        //file2 << m_Tex0Buffer[m_IndexBuffer[j]].x << ", " << m_Tex0Buffer[m_IndexBuffer[j]].y << ",\n";

        TexCoords.push_back(m_Tex0Buffer[m_IndexBuffer[j]].x);
        TexCoords.push_back(m_Tex0Buffer[m_IndexBuffer[j]].y);

    }
    vertexCount = Vertices.size()/3;
    //std::cout << vertexCount << std::endl;
    //GenerateVertexBuffers();
    std::cout << "Terrain has been loaded.." << std::endl;
    return true;
}

void Terrain::GenerateIndexBuffer()
{
    
    const unsigned int terrainWidth = m_HeightmapDimensions.x;
    const unsigned int terrainHeight = m_HeightmapDimensions.y;

    // trojkaty
    const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

    // 3 indices for each triangle in the terrain mesh
    m_IndexBuffer.resize(numTriangles * 3);
    //std::ofstream file3("ter_Indices.txt");
    unsigned int index = 0; // Index in the index buffer
    for (unsigned int j = 0; j < (terrainHeight - 1); ++j)
    {
        for (unsigned int i = 0; i < (terrainWidth - 1); ++i)
        {
            int vertexIndex = (j * terrainWidth) + i;
            // Top triangle (T0)
            m_IndexBuffer[index++] = vertexIndex;                           // V0
            //file3 << vertexIndex << ", ";
            m_IndexBuffer[index++] = vertexIndex + terrainWidth + 1;    // V3
            //file3 << vertexIndex + terrainWidth + 1 << ", ";
            m_IndexBuffer[index++] = vertexIndex + 1;                       // V1
            //file3 << vertexIndex + 1 << ",\n";

            // Bottom triangle (T1)
            m_IndexBuffer[index++] = vertexIndex;                           // V0
            //file3 << vertexIndex << ", ";
            m_IndexBuffer[index++] = vertexIndex + terrainWidth;            // V2
            //file3 << vertexIndex + terrainWidth + 1 << ", ";
            m_IndexBuffer[index++] = vertexIndex + terrainWidth + 1;        // V3
            //file3 << vertexIndex + 1 << ",\n";
        }
    }
}

void Terrain::GenerateNormals()
{
    //std::ofstream file1("_Normals.txt");
    for (unsigned int i = 0; i < m_IndexBuffer.size(); i += 3)
    {
        glm::vec3 v0 = m_PositionBuffer[m_IndexBuffer[i + 0]];
        glm::vec3 v1 = m_PositionBuffer[m_IndexBuffer[i + 1]];
        glm::vec3 v2 = m_PositionBuffer[m_IndexBuffer[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        m_NormalBuffer[m_IndexBuffer[i + 0]] += normal;
        m_NormalBuffer[m_IndexBuffer[i + 1]] += normal;
        m_NormalBuffer[m_IndexBuffer[i + 2]] += normal;
    }

    const glm::vec3 UP(0.0f, 1.0f, 0.0f);
    for (unsigned int i = 0; i < m_NormalBuffer.size(); ++i)
    {
        m_NormalBuffer[i] = glm::normalize(m_NormalBuffer[i]);
        //file1 << m_NormalBuffer[i].x << ", " << m_NormalBuffer[i].y << ", " << m_NormalBuffer[i].z << ",\n";
        float fTexture0Contribution = glm::saturate(glm::dot(m_NormalBuffer[i], UP) - 0.1f);
        m_Tex0Buffer[i] = glm::vec2(fTexture0Contribution, m_Tex0Buffer[i].y);
    }
}

float Terrain::GetHeightAt(const glm::vec3& position)
{
    float height = -FLT_MAX;
    if (m_HeightmapDimensions.x < 2 || m_HeightmapDimensions.y < 2) return height;

    float terrainWidth = (m_HeightmapDimensions.x - 1) * m_fBlockScale;
    float terrainHeight = (m_HeightmapDimensions.y - 1) * m_fBlockScale;
    float halfWidth = terrainWidth * 0.5f;
    float halfHeight = terrainHeight * 0.5f;

    glm::vec3 terrainPos = glm::vec3(m_InverseLocalToWorldMatrix * glm::vec4(position, 1.0f));
    glm::vec3 invBlockScale(1.0f / m_fBlockScale, 0.0f, 1.0f / m_fBlockScale);

    glm::vec3 offset(halfWidth, 0.0f, halfHeight);

    glm::vec3 vertexIndices = (terrainPos + offset) * invBlockScale;

    int u0 = (int)floorf(vertexIndices.x);
    int u1 = u0 + 1;
    int v0 = (int)floorf(vertexIndices.z);
    int v1 = v0 + 1;

    if (u0 >= 0 && u1 < (int)m_HeightmapDimensions.x && v0 >= 0 && v1 < (int)m_HeightmapDimensions.y)
    {
        glm::vec3 p00 = m_PositionBuffer[(v0 * m_HeightmapDimensions.x) + u0];    // Top-left vertex
        glm::vec3 p10 = m_PositionBuffer[(v0 * m_HeightmapDimensions.x) + u1];    // Top-right vertex
        glm::vec3 p01 = m_PositionBuffer[(v1 * m_HeightmapDimensions.x) + u0];    // Bottom-left vertex
        glm::vec3 p11 = m_PositionBuffer[(v1 * m_HeightmapDimensions.x) + u1];    // Bottom-right vertex

        // triangle choice
        float percentU = vertexIndices.x - u0;
        float percentV = vertexIndices.z - v0;

        glm::vec3 dU, dV;
        if (percentU > percentV)
        {   // Top triangle
            dU = p10 - p00;
            dV = p11 - p10;
        }
        else
        {   // Bottom triangle
            dU = p11 - p01;
            dV = p01 - p00;
        }

        glm::vec3 heightPos = p00 + (dU * percentU) + (dV * percentV);

        heightPos = glm::vec3(m_LocalToWorldMatrix * glm::vec4(heightPos, 1));
        height = heightPos.y;
    }

    return height;
}
