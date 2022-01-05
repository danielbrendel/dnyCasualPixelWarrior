/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "headcrabcls.as"

//Create headcrab entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CHeadcrabEntity @headcrab = CHeadcrabEntity();
	Ent_SpawnEntity(szIdent, @headcrab, vecPos);
	headcrab.SetRotation(fRot);
}
