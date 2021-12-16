#ifndef TER_H
#define TER_H


class BTer
{
public:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> vertexNormals;
	std::vector<float> texCoords;
	unsigned int vertexCount;
	BTer() {

		std::cout << "Loading terrain from file...\n";

		objl::Loader Loader;
		bool loadout = Loader.LoadFile("terrain_small_2.obj");
		if (loadout)
		{

			// Go through each loaded mesh and out its contents
			for (int i = 0; i < Loader.LoadedMeshes.size() ; i++)
			{

				objl::Mesh curMesh = Loader.LoadedMeshes[i];

				for (int j = 0; j < curMesh.Indices.size(); j += 1)
				{
					vertices.push_back(curMesh.Vertices[curMesh.Indices[j]].Position.X);
					vertices.push_back(curMesh.Vertices[curMesh.Indices[j]].Position.Y);
					vertices.push_back(curMesh.Vertices[curMesh.Indices[j]].Position.Z);

					normals.push_back(curMesh.Vertices[curMesh.Indices[j]].Normal.X);
					normals.push_back(curMesh.Vertices[curMesh.Indices[j]].Normal.Y);
					normals.push_back(curMesh.Vertices[curMesh.Indices[j]].Normal.Z);

					texCoords.push_back(curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.X);
					texCoords.push_back(curMesh.Vertices[curMesh.Indices[j]].TextureCoordinate.Y);

				}
				vertexCount = vertices.size() / 3;
			}
		}
		std::cout << "terrain loaded...\n";




	}

private:


};

#endif#pragma once
