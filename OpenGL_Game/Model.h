#pragma once
#include <vector>
#include "Mesh.h"

class Model
{
private:
	std::vector<Mesh> meshes;
	const char* modelPath;

public:
	 Model(const char* path);

};

