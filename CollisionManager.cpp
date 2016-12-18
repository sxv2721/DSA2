// Written by Cyprian Tayrien, Interactive Games and Media, Rochester Institute of Technology
#include "RaptorEngine.h"
#include <map>
#include <unordered_map>
#include "Grid.h"


struct MyHasher
{
	template<typename t>
	std::size_t operator()(const std::pair<t, t>& key) const
	{
		return (std::size_t)(key.first * 5 + key.second);
	}
};
namespace RaptorEngine 
{
	float timer = 0;
	//Grid myGrid = Grid(1.616,glm::vec3(5)); //Collision scene
	Grid myGrid = Grid(.3, glm::vec3(3));

	void Engine::collisionUpdate()
	{
		int count = 0;
		myGrid.clearBins();
		for (uint32_t i = 0; i < collidableTags.size(); i++)
		{
			/**/
		    std::list<std::list<void*>> gridSpaces;
			Object& obji = *collidableTags[i];
			gridSpaces = myGrid.insert(obji.transform.loc, obji.transform.scale, &obji.collider);
			if (gridSpaces.empty())
			{//if the list is empty
				continue;
				printf("\nnoooooooooo");
			}
			if (gridSpaces.begin()!=gridSpaces.end())
			{
				auto iteri = gridSpaces.begin();
				while (iteri != gridSpaces.end())
				{
					std::list<void*> innerList = (std::list<void*>)(*iteri);

					if (innerList.begin()!=innerList.end())
					{
						auto iteri2 = innerList.begin();
						while (iteri2 != innerList.end())
						{
							Object* objj = (Object*)(*iteri2);
							if (collides(obji, *objj))
							{
								if (obji.collisionScriptMap.count(objj->tag))
									obji.collisionScriptMap[objj->tag](this, &obji, objj);

								if (objj->collisionScriptMap.count(obji.tag))
									objj->collisionScriptMap[obji.tag](this, objj, &obji);
								//printf("\yeeeeeeeeeeeeeeees");
							}
							
							++count;
							++iteri2;
						}
					}
					++iteri;
				}
			}
			else
			{
				std::list<void*> innerList = (std::list<void*>)(*gridSpaces.begin());
				if (innerList.begin()!=innerList.end())
				{
					auto iteri = innerList.begin();
					++iteri;
					Object* obji = (Object*)(*innerList.begin());
					while (iteri != innerList.end())
					{
						Object* objj = (Object*)(*iteri);
						if (collides(*obji, *objj))
						{
							if (obji->collisionScriptMap.count(objj->tag))
								obji->collisionScriptMap[objj->tag](this, obji, objj);

							if (objj->collisionScriptMap.count(obji->tag))
								objj->collisionScriptMap[obji->tag](this, objj, obji);
							//printf("\nnoooooooooo");
						}
						++iteri;
						++count;
					}
				}
			}
			/*
			Object& obji = *collidableTags[i];
			for (uint32_t j = i + 1; j < collidableTags.size(); j++)
			{
				Object& objj = *collidableTags[j];

				if (collides(obji, objj))
				{
					if (obji.collisionScriptMap.count(objj.tag))
						obji.collisionScriptMap[objj.tag](this, &obji, &objj);

					if (objj.collisionScriptMap.count(obji.tag))
						objj.collisionScriptMap[obji.tag](this, &objj, &obji);
				}

				++count;
			}/**/
		}/**/

		timer += engine->getdt();
		if (timer > 1) {
			printf("\n# of Collision Checks: %d\n", count);
			timer -= 1;
		}
	}

	// ICE 1: Declare/Define here...
	//		Collider-pair jump table
	//		testAxis function
	//		radial projection jump table
	//		radial projection functions for AABB and Sphere
	typedef pair<Collider::Type, Collider::Type> ColliderPair;
	typedef bool(Engine::*CollisionFunction)(const Object&, const Object&);

	std::unordered_map<ColliderPair, CollisionFunction, MyHasher> collisionJumpTable;

	typedef glm::vec3(*radialVector)(const Object& o, const glm::vec3& L);

	std::unordered_map<Collider::Type, radialVector> RadialTable;


	glm::vec3 SphereRadialVector(const Object& o, const glm::vec3& L)
	{
		return o.transform.getMaxScale()*L;
	}
	glm::vec3 AABBRadialVector(const Object& o, const glm::vec3& L)
	{
		return glm::sign(L)*o.transform.scale;
	}
	glm::vec3 OBBRadialVector(const Object& o, const glm::vec3& L)
	{
		glm::mat3 rotL = glm::mat3(o.transform.getRotMat());
		glm::vec3 newWidth = glm::vec3(glm::sign(L*rotL)*o.transform.scale);
		glm::vec3 ans = glm::vec3 (rotL * newWidth);
		return ans;
	}
	glm::vec3 HalfVolRadialVector(const Object& o, const glm::vec3& L)
	{
		return glm::vec3(0);
	}
	bool testAxis(glm::vec3 axis, glm::vec3 ca, glm::vec3 cb, glm::vec3 ra, glm::vec3 rb)
	{
		if (glm::abs(glm::dot(axis, (ca - cb)))  > (glm::abs(glm::dot(axis, ra)) + glm::abs(glm::dot(axis, rb))))
		{
			return true;
		}
		return false;
	}

	bool Engine::collides(const Object& a, const Object& b)
	{
		if(collisionJumpTable.count({a.collider.type, b.collider.type}))
			return (this->*collisionJumpTable[{a.collider.type, b.collider.type}])(a, b);
		return false;
	}

	

	bool Engine::collidesNone(const Object & a, const Object & b)
	{
		return false;
	}
	bool Engine::collidesHalfVol(const Object & a, const Object & b)
	{
		if (b.collider.type == Collider::HALFVOL)
		{
			return collidesHalfVol(b, a);
		}
		glm::vec3 axis = (glm::vec3)a.transform.getRotMat()[2];

		if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::HALFVOL](a, axis), RadialTable[b.collider.type](b, axis))
			&& glm::sign(glm::dot(axis, (b.transform.loc - a.transform.loc)))<0) 
		{
			return false;
		}
		return true;
	}

	// ICE 1: Replace this innards of this function with a call test axis
	bool Engine::collidesAABBvAABB(const Object & a, const Object & b)
	{
		/*vec3 al = a.t.loc;
		vec3 as = a.t.scale;
		vec3 bl = b.t.loc;
		vec3 bs = b.t.scale;

		//		distance	 >	half-width 1  + half-width 2
		if(fabs(al.x - bl.x) > fabs(as.x) + fabs(bs.x)) return false;
		if(fabs(al.y - bl.y) > fabs(as.y) + fabs(bs.y)) return false;
		if(fabs(al.z - bl.z) > fabs(as.z) + fabs(bs.z)) return false;
		/**/
		for (int i = 0; i < 3; i++)
		{
			glm::vec3 axis;
			axis[i] = 1;
			axis[(i + 1) % 3] = 0;
			axis[(i + 2) % 3] = 0;

			if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::AABB](a, axis) , RadialTable[Collider::AABB](b, axis)))
			{
				return false;
			}
		}
		
		
		return true;
	}

	// ICE 1: Replace this innards of this function with a call test axis
	bool Engine::collidesSpherevSphere(const Object & a, const Object & b)
	{
		/*if (glm::distance(a.t.loc, b.t.loc) > (a.t.maxScale( ) + b.t.maxScale( ))) 
		{
			return false;
		}/**/
		glm::vec3 axis = glm::normalize(a.transform.loc - b.transform.loc);
		if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::SPHERE](a, axis), RadialTable[Collider::SPHERE](b, axis)))
		{
			return false;
		}
		//printf("\n sphere sphere");
		return true;
	}

	// ICE 1 Challenge: Replace this innards of this function with a call test axis
	bool Engine::collidesAABBvSphere(const Object & a, const Object & b)
	{
		/*float distSqrd = 0.f;
		for (int i = 0; i < 3; i++) {
			float min = a.t.loc[i] - a.t.scale[i];
			float max = a.t.loc[i] + a.t.scale[i];
			float v = b.t.loc[i];

			if (v < min) distSqrd += (min - v) * (min - v);
			if (v > max) distSqrd += (v - max) * (v - max);
		}

		if (distSqrd > b.t.maxScale() * b.t.maxScale()) return false;
		/**/
		/*
		glm::vec3 d = a.t.loc - b.t.loc;
		glm::clamp(d.x, -a.t.scale.x, a.t.scale.x);
		glm::clamp(d.y, -a.t.scale.y, a.t.scale.y);
		glm::clamp(d.z, -a.t.scale.z, a.t.scale.z);
		glm::vec3 AABBPoint = d + a.t.loc;/**/

		//glm::vec3 axis = glm::normalize(AABBPoint);
			
		for (int i = 0; i < 3; i++)
		{
			glm::vec3 axis;
			axis[i] = 1;
			axis[(i + 1) % 3] = 0;
			axis[(i + 2) % 3] = 0;

			if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::AABB](a, axis), RadialTable[Collider::SPHERE](b, axis)))
			{
				return false;
			}
		}

		return true;
	}

	bool Engine::collidesSpherevAABB(const Object & a, const Object & b)
	{
		return collidesAABBvSphere(b, a);
	}
	
	bool Engine::collidesOBBvAABB(const Object & a, const Object & b)
	{
		for (int i = 0; i < 3; i++)
		{
			glm::vec3 axis;
			axis[i] = 1;
			axis[(i + 1) % 3] = 0;
			axis[(i + 2) % 3] = 0;

			if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::OBB](a, axis), RadialTable[Collider::AABB](b, axis)))
			{
				return false;
			}
		}
		return true;
		//return collidesAABBvSphere(b, a);
	}
	bool Engine::collidesOBBvSphere(const Object & a, const Object & b)
	{
		glm::vec3 d = b.transform.loc-a.transform.loc;
		
		//d = glm::vec3(glm::vec4(rotd, 1)*a.t.matrix);
		glm::mat3 rotd = glm::mat3(a.transform.getRotMat());

		glm::vec3 OBBPoint = a.transform.loc + rotd*glm::clamp(d*rotd, -a.transform.scale, a.transform.scale);
		glm::vec3 axis = glm::normalize(OBBPoint - b.transform.loc);

		if (testAxis(axis, a.transform.loc, b.transform.loc, RadialTable[Collider::OBB](a, axis), RadialTable[Collider::SPHERE](b, axis)))
		{
			return false;
		}

		return true;
		//return collidesAABBvSphere(b, a);
	}
	bool Engine::collidesSpherevOBB(const Object & a, const Object & b)
	{
		return collidesOBBvSphere(b, a);
	}
	bool Engine::collidesAABBvOBB(const Object & a, const Object & b)
	{
		return collidesOBBvAABB(b, a);
	}
	bool Engine::collidesOBBvOBB(const Object & a, const Object & b)
	{
		return false;
	}
	// ICE 1: Populate the two jump-tables in the init function
	void Engine::collisionInit()
	{
		collisionJumpTable[{Collider::AABB, Collider::AABB}] = &Engine::collidesAABBvAABB;
		collisionJumpTable[{Collider::AABB, Collider::SPHERE}] = &Engine::collidesAABBvSphere;
		collisionJumpTable[{Collider::SPHERE, Collider::AABB}] = &Engine::collidesSpherevAABB;
		collisionJumpTable[{Collider::SPHERE, Collider::SPHERE}] = &Engine::collidesSpherevSphere;
		collisionJumpTable[{Collider::OBB, Collider::AABB}] = &Engine::collidesOBBvAABB;
		collisionJumpTable[{Collider::OBB, Collider::SPHERE}] = &Engine::collidesOBBvSphere;
		collisionJumpTable[{Collider::OBB, Collider::OBB}] = &Engine::collidesAABBvOBB;
		collisionJumpTable[{Collider::SPHERE, Collider::OBB}] = &Engine::collidesSpherevOBB;
		collisionJumpTable[{Collider::HALFVOL, Collider::AABB}] = &Engine::collidesHalfVol;
		collisionJumpTable[{Collider::HALFVOL, Collider::SPHERE}] = &Engine::collidesHalfVol;
		collisionJumpTable[{Collider::HALFVOL, Collider::OBB}] = &Engine::collidesHalfVol;
		collisionJumpTable[{Collider::AABB, Collider::HALFVOL}] = &Engine::collidesHalfVol;
		collisionJumpTable[{Collider::SPHERE, Collider::HALFVOL}] = &Engine::collidesHalfVol;
		collisionJumpTable[{Collider::OBB, Collider::HALFVOL}] = &Engine::collidesHalfVol;


		RadialTable[Collider::SPHERE] = &SphereRadialVector;
		RadialTable[Collider::AABB] = &AABBRadialVector;
		RadialTable[Collider::OBB] = &OBBRadialVector;
		RadialTable[Collider::HALFVOL] = &HalfVolRadialVector;
	}
}