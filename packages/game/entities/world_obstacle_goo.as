/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "world_obstacle.as"

//Create obstacle entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CWorldObstacle @obstacle = CWorldObstacle();
	obstacle.SetTexture("goo.png");
	obstacle.SetSound("goo.wav");
	obstacle.SetDamage(2);
	
	Ent_SpawnEntity(szIdent, @obstacle, vecPos);
}

