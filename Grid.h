#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <list>

class Grid
{
private:
	int row = 1, col = 1, depth = 1;
	glm::vec3 halfWidth = { 10,10,10 };
	std::unordered_map<int, std::list<void*>> bins;
public:
	std::unordered_map<int, std::list<void*>>& getBins() { return bins; }
	void clearBins();
	//void insert(glm::vec3 loc, glm::vec3 halfW, void* collider);
	std::list<std::list<void*>> insert(glm::vec3 loc, glm::vec3 halfW, void* collider);
	Grid();
	Grid(float maxS, glm::vec3 halfW);//max scale of the colliders and the grid's half width 
	~Grid();
};

