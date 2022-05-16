/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "item_ammo.as"

/* Flame thrower ammo item entity */
class CItemAmmoFThrower : CItemAmmoBase
{
	CItemAmmoFThrower()
	{
		this.SetSprite("fthrowerhud.png");
		this.SetWeapon("fthrower");
		this.SetSupplyCount(25);
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "item_ammo_fthrower";
	}
}

//Create ammo entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CItemAmmoFThrower @ammo = CItemAmmoFThrower();
	Ent_SpawnEntity(szIdent, @ammo, vecPos);
}