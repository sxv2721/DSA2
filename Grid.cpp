#include "Grid.h"


void Grid::clearBins()
{
	bins.clear();
}

std::list<std::list<void*>> Grid::insert(glm::vec3 loc, glm::vec3 halfW, void * collider)
{
	std::list<std::list<void*>> ans;
	int imin = (int)(loc.x - halfW.x);
	int imax = (int)(loc.x + halfW.x);
	int jmin = (int)(loc.y - halfW.y);
	int jmax = (int)(loc.y + halfW.y);
	int kmin = (int)(loc.z - halfW.z);
	int kmax = (int)(loc.z + halfW.z);

	for (int i = imin; i < imax; i++)
	{
		for (int j = jmin; j < jmax; j++)
		{
			for (int k = kmin; k < kmax; k++)
			{
				bins[i + row*j + row*col*k].push_front(collider);
				ans.push_front(bins[i + row*j + row*col*k]);
			}
		}
	}
	return ans;
}

Grid::Grid()
{
}

Grid::Grid(float maxS, glm::vec3 halfW)
{
	halfWidth = halfW;
	row = halfW.x * 2 / (maxS*1.1);
	col = halfW.y * 2 / (maxS*1.1);
	depth = halfW.z * 2 / (maxS*1.1);
}


Grid::~Grid()
{
}
