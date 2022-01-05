/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "item_ammo.as"

/* Rifle ammo item entity */
class CItemAmmoRifle : CItemAmmoBase
{
	CItemAmmoRifle()
	{
		this.SetSprite("lasergunhud.png");
		this.SetWeapon("laser");
		this.SetSupplyCount(25);
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "item_ammo_rifle";
	}
}

//Create ammo entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CItemAmmoRifle @ammo = CItemAmmoRifle();
	Ent_SpawnEntity(szIdent, @ammo, vecPos);
}