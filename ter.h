#ifndef TER_H
#define TER_H
#include "model.h"

namespace Models {
	namespace TerInternal {
		extern float vertices[];
		extern float normals[];
		extern float vertexNormals[];
		extern float texCoords[];
		extern unsigned int vertexCount;
		extern unsigned int indexes[];
	}

	class Ter : public Model {
	public:
		Ter();
		virtual ~Ter();
	private:


	};
}
#endif#pragma once
