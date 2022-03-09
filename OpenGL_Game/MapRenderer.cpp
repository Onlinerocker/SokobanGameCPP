#include "MapRenderer.h"
#include <iostream>

MapRenderer::MapRenderer(Map* map, GLuint texture) : mesh(), meshTop(), meshDynamic(), meshParticle()
{
	this->map = map;
	this->texture = texture;
	refresh();
    
	glm::vec2 pos2D = glm::vec2(0, 0);
	unsigned long di = 0;
}

void MapRenderer::addQuad(Mesh* mesh, float xInd, float& x, float y, glm::vec2 pos, float tileSize, glm::vec2 uvMin, glm::vec2 uvMax, unsigned long& ind)
{
	x = pos.x + xInd * tileSize;
    
	Vertex v[4];
	GLuint i[6] = { 0 + ind, 1 + ind, 2 + ind, 0 + ind, 2 + ind, 3 + ind };
    
	v[0].vertex = glm::vec2(x, y);
	v[0].uv = uvMin;
    
	v[1].vertex = glm::vec2(x, y + tileSize);
	v[1].uv = glm::vec2(uvMin.x, uvMax.y);
    
	v[2].vertex = glm::vec2(x + tileSize, v[1].vertex.y);
	v[2].uv = uvMax;
    
	v[3].vertex = glm::vec2(v[2].vertex.x, y);
	v[3].uv = glm::vec2(uvMax.x, uvMin.y);
    
	for (int z = 0; z < 4; z++) mesh->vertices.push_back(v[z]);
	for (int z = 0; z < 6; z++) mesh->indices.push_back(i[z]);
    
	ind += 4;
}

void MapRenderer::addQuadMap(Mesh* mesh, float xInd, float& x, float y, glm::vec2 pos, float tileSize, unsigned long& ind, float id, glm::vec3 color)
{
	x = pos.x + xInd * tileSize;
    
	VertexMap v[4];
	GLuint i[6] = { 0 + ind, 1 + ind, 2 + ind, 0 + ind, 2 + ind, 3 + ind };
    
	v[0].vertex = glm::vec2(x, y);
	v[0].uv = glm::vec2(0, 0);
	v[0].id = id;
    v[0].color = color;
    
	v[1].vertex = glm::vec2(x, y + tileSize);
	v[1].uv = glm::vec2(0, 1);
	v[1].id = id;
    v[1].color = color;
    
	v[2].vertex = glm::vec2(x + tileSize, v[1].vertex.y);
	v[2].uv = glm::vec2(1, 1);
	v[2].id = id;
    v[2].color = color;
    
	v[3].vertex = glm::vec2(v[2].vertex.x, y);
	v[3].uv = glm::vec2(1, 0);
	v[3].id = id;
    v[3].color = color;
    
	for (int z = 0; z < 4; z++) mesh->verticesMap.push_back(v[z]);
	for (int z = 0; z < 6; z++) mesh->indices.push_back(i[z]);
    
	ind += 4;
}

void MapRenderer::addQuadInstance(Mesh* mesh, float xInd, float& x, float y, glm::vec2 pos, float tileSize, unsigned long& ind)
{
	x = pos.x + xInd * tileSize;
    
	VertexInstance v[4];
	GLuint i[6] = { 0 + ind, 1 + ind, 2 + ind, 0 + ind, 2 + ind, 3 + ind };
    
	v[0].vertex = glm::vec2(x, y);
	v[1].vertex = glm::vec2(x, y + tileSize);
	v[2].vertex = glm::vec2(x + tileSize, v[1].vertex.y);
	v[3].vertex = glm::vec2(v[2].vertex.x, y);
    
	for (int z = 0; z < 4; z++) mesh->verticesInstance.push_back(v[z]);
	for (int z = 0; z < 6; z++) mesh->indices.push_back(i[z]);
    
	ind += 4;
}

void MapRenderer::refresh()
{
	mesh.verticesMap.clear();
	mesh.indices.clear();
    
	mesh1.verticesMap.clear();
	mesh1.indices.clear();
    
	meshTop.verticesMap.clear();
	meshTop.indices.clear();
    
	meshDynamic.verticesInstance.clear();
	meshDynamic.indices.clear();
    
	glm::vec3 mp = map->pos;
	glm::vec2 pos2D = glm::vec2(mp.x, mp.y);
	float tileSize = map->tileSize;
	float curX = pos2D.x;
	float curY = pos2D.y;
    
	unsigned long indOffset = 0;
	unsigned long indOffset1 = 0;
	unsigned long indOffsetTop = 0;
    
	for (unsigned int y = 0; y < map->height; y++)
	{
		curY = pos2D.y + y * (tileSize); //TODO: overlap tiles 0.5f?
		for (unsigned int x = 0; x < map->width; x++)
		{
			TileData d = map->getTileStatic(x, y, 0);
			TileData d2 = map->getTileStatic(x, y, 1);
            
			TileData d1 = map->getTileStaticTop(x, y);
			glm::vec4 uvs;
			glm::vec2 b;
			glm::vec2 t;
            
            size_t p = y * map->width + x;
            
			if (d.id != 0)
			{
				uvs = map->tileManager->getTileUV(d.id);
				b = glm::vec2(uvs.x, uvs.y);
				t = glm::vec2(uvs.z, uvs.w);
				//addQuad(&mesh, x, curX, curY, pos2D, tileSize, b, t, indOffset);
				addQuadMap(&mesh, x, curX, curY, pos2D, tileSize, indOffset, d.id, map->staticDataColors[p]);
			}
            
            //250 is the invisible wall ID for enemies (don't need to render anything)
			if (d2.id != 0 && d2.id != 250)
			{
				addQuadMap(&mesh1, x, curX, curY, pos2D, tileSize, indOffset1, d2.id, map->staticData1Colors[p]);
			}
            
			if (d1.id != 0)
			{
				uvs = map->tileManager->getTileUV(d1.id);
                
				if (uvs != glm::vec4(0))
				{
					b = glm::vec2(uvs.x, uvs.y);
					t = glm::vec2(uvs.z, uvs.w);
					//addQuad(&meshTop, x, curX, curY, pos2D, tileSize, b, t, indOffsetTop);
					addQuadMap(&meshTop, x, curX, curY, pos2D, tileSize, indOffsetTop, d1.id, glm::vec3(1));
				}
				else
				{
					map->setTileStaticTop(x, y, 0);
				}
			}
		}
	}
    
	unsigned long di = 0;
	addQuadInstance(&meshDynamic, 0, pos2D.x, pos2D.y, pos2D, tileSize, di);
    
	Texture t;
	t.id = texture;
	t.type = diffuse;
	mesh.textures = { t };
	mesh1.textures = { t };
	meshTop.textures = { t };
	meshDynamic.textures = { t };
    
	//mesh.setupMesh();
	//meshTop.setupMesh();
	mesh.setupMeshMap();
	mesh1.setupMeshMap();
	meshTop.setupMeshMap();
	meshDynamic.setupInstancedMesh(map->dynamicOffsets, map->dynamicUvs, map->dynamicGlowing);
}

void MapRenderer::draw(ShaderProgram& program)
{
	mesh.drawArray(program);
	mesh1.drawArray(program);
}

void MapRenderer::drawTop(ShaderProgram& program)
{
	meshTop.drawArray(program);
}

void MapRenderer::drawDynamic(ShaderProgram& program)
{
	meshDynamic.setupInstancedMesh(map->dynamicOffsets, map->dynamicUvs, map->dynamicGlowing); //TODO: only setup on change?
	meshDynamic.drawInstanced(program, map->dynamicOffsets.size());
}

void MapRenderer::initMeshParticle(float size) 
{
	glm::vec2 pos2D = glm::vec2(0, 0);
	unsigned long di = 0;
    
	addQuadInstance(&meshParticle, 0, pos2D.x, pos2D.y, pos2D, size, di);
    
	Texture t;
	t.id = texture;
	t.type = diffuse;
	meshParticle.textures = { t };
}

/*void MapRenderer::setupMeshParticle(const std::vector<glm::vec2>& offsets, const std::vector<glm::vec4>& uvs, const std::vector<float>& glows)
{
	meshParticle.setupInstancedMesh(offsets, uvs, glows);
}*/

void MapRenderer::setupMeshParticle(const glm::vec2* const offsets, const glm::vec4* const uvs, const float* const sizes, const float* const times, const glm::vec3* colors, size_t count)
{
	meshParticle.setupParticleMesh(offsets, uvs, sizes, times, colors, count);
}