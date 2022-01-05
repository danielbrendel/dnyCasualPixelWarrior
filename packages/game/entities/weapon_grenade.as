/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "explosion.as"

/* Grenade entity */
const uint C_GRENADE_DAMAGE = 64;
class CGrenadeEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	SpriteHandle m_hSprite;
	float m_fRotation;
	float m_fDrawingRotation;
	float m_fSpeed;
	bool m_bRemove;
	Timer m_tmrAlive;
	Timer m_tmrRotate;
	IScriptedEntity@ m_pOwner;
	
	CGrenadeEntity()
    {
		this.m_vecSize = Vector(32, 32);
		this.m_fSpeed = 450.0;
		this.m_bRemove = false;
		@this.m_pOwner = null;
		this.m_fDrawingRotation = 0.00;
    }
	
	//Set owner entity
	void SetOwner(IScriptedEntity@ pOwner)
	{
		@this.m_pOwner = pOwner;
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\grenade.png", 1, this.m_vecSize[0], this.m_vecSize[1], 1, true);
		this.m_tmrAlive.SetDelay(2000);
		this.m_tmrAlive.Reset();
		this.m_tmrAlive.SetActive(true);
		this.m_tmrRotate.SetDelay(10);
		this.m_tmrRotate.Reset();
		this.m_tmrRotate.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(this.m_vecSize[0] / 2, this.m_vecSize[1] / 2));
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosionEntity @expl = CExplosionEntity();
		expl.SetDamageable(true, C_GRENADE_DAMAGE);
		expl.SetOwner(this.m_pOwner);
		Ent_SpawnEntity("explosion", @expl, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		Ent_Move(this, this.m_fSpeed, MOVE_FORWARD);
		
		this.m_tmrAlive.Update();
		if (this.m_tmrAlive.IsElapsed()) {
			this.m_bRemove = true;
		}
		
		this.m_tmrRotate.Update();
		if (this.m_tmrRotate.IsElapsed()) {
			this.m_tmrRotate.Reset();
			this.m_fDrawingRotation += 0.10;
			if (this.m_fDrawingRotation > 6.30) {
				this.m_fDrawingRotation = 0.00;
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		if (!R_ShouldDraw(this.m_vecPos, this.m_vecSize))
			return;
			
		Vector vOut;
		R_GetDrawingPosition(this.m_vecPos, this.m_vecSize, vOut);
		
		R_DrawSprite(this.m_hSprite, vOut, 0, this.m_fDrawingRotation, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
		this.m_bRemove = true;
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_bRemove; //Detonate and dispose grenade when reached position or hit an opponent
	}
	
	//Indicate whether this entity is collidable
	bool IsCollidable()
	{
		return true;
	}
	
	//Called when the entity collided with another entity
	void OnCollided(IScriptedEntity@ ref)
	{
		if ((@ref != @this.m_pOwner) && (ref.GetName() != this.GetName()) && (ref.GetName() != "weapon_gun") && (ref.GetName() != "weapon_laser") && (ref.GetName() != "weapon_laserball") && (ref.GetName() != "weapon_missile") && (ref.GetName() != "item_ammo_grenade") && (ref.GetName() != "item_ammo_handgun") && (ref.GetName() != "item_ammo_rifle") && (ref.GetName() != "item_ammo_shotgun") && (ref.GetName() != "item_coin") && (ref.GetName() != "item_health")) {
			this.m_bRemove = true;
		}
	}
	
	//Called when entity gets damaged
	void OnDamage(uint32 damageValue)
	{
	}
	
	//Called for accessing the model data for this entity.
	Model& GetModel()
	{
		return this.m_oModel;
	}
	
	//Called for recieving the current position. This is useful if the entity shall move.
	Vector& GetPosition()
	{
		return this.m_vecPos;
	}
	
	//Set new position
	void SetPosition(const Vector &in vecPos)
	{
		this.m_vecPos = vecPos;
	}
	
	//Return the rotation.
	float GetRotation()
	{
		return this.m_fRotation;
	}
	
	//Set new rotation
	void SetRotation(float fRot)
	{
		this.m_fRotation = fRot;
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "weapon_grenade";
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSize()
	{
		return this.m_vecSize;
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return "";
	}
}