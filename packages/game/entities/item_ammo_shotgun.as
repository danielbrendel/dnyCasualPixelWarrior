/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "item_ammo.as"

/* Shotgun ammo item entity */
class CItemAmmoShotgun : CItemAmmoBase
{
	CItemAmmoShotgun()
	{
		this.SetSprite("shotgunhud.png");
		this.SetWeapon("shotgun");
		this.SetSupplyCount(10);
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "item_ammo_shotgun";
	}
}

//Create ammo entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CItemAmmoShotgun @ammo = CItemAmmoShotgun();
	Ent_SpawnEntity(szIdent, @ammo, vecPos);
}