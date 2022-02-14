/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "explosion.as"

const uint MISSILE_DAMAGE = 35;

/* Missile weapon entity */
class CMissileEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	Timer m_tmrLifeTime;
	Timer m_tmrMovement;
	SpriteHandle m_hMissile;
	SpriteHandle m_hTrail;
	float m_fRotation;
	bool m_bRemove;
	float m_fSpeed;
	int m_iTrailIndex;

	CMissileEntity()
    {
		this.m_bRemove = false;
		this.m_vecSize = Vector(32, 32);
		this.m_fSpeed = 200;
		this.m_iTrailIndex = 0;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hMissile = R_LoadSprite(GetPackagePath() + "gfx\\missile.png", 1, 80, 60, 1, false);
		this.m_hTrail = R_LoadSprite(GetPackagePath() + "gfx\\trail.png", 64, 128, 128, 8, false);
		this.m_tmrMovement.SetDelay(10);
		this.m_tmrMovement.Reset();
		this.m_tmrMovement.SetActive(true);
		this.m_tmrLifeTime.SetDelay(10000);
		this.m_tmrLifeTime.Reset();
		this.m_tmrLifeTime.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hMissile);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosionEntity @expl = CExplosionEntity();
		Ent_SpawnEntity("explosion", @expl, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrMovement.Update();
		if (this.m_tmrMovement.IsElapsed()) {
			this.m_tmrMovement.Reset();
			
			Ent_Move(this, this.m_fSpeed, MOVE_FORWARD);
		}
		
		this.m_tmrLifeTime.Update();
		if (this.m_tmrLifeTime.IsElapsed()) {
			this.m_tmrLifeTime.SetActive(false);
			this.m_bRemove = true;
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
		if (!R_ShouldDraw(this.m_vecPos, this.m_vecSize))
			return;
			
		Vector vOut;
		R_GetDrawingPosition(this.m_vecPos, this.m_vecSize, vOut);
		
		this.m_iTrailIndex++;
		if (this.m_iTrailIndex >= 64)
			this.m_iTrailIndex = 0;
		
		Vector vecTrailPos = Vector(vOut[0] + 20 - 45, vOut[1] + 15 - 50);
		vecTrailPos[0] += int(sin(this.m_fRotation + 0.014) * -30);
		vecTrailPos[1] -= int(cos(this.m_fRotation + 0.014) * -30);

		R_DrawSprite(this.m_hTrail, vecTrailPos, this.m_iTrailIndex, this.m_fRotation + 6.30 / 2, Vector(-1, -1), 0.3, 0.3, false, Color(0, 0, 0, 0));
		R_DrawSprite(this.m_hMissile, vOut, 0, this.m_fRotation, Vector(-1, -1), 0.5, 0.5, false, Color(0, 0, 0, 0));
	}
	
	//Draw on top
	void OnDrawOnTop()
	{
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return (this.m_tmrLifeTime.IsElapsed()) || (this.m_bRemove);
	}
	
	//Indicate if entity can be collided
	bool IsCollidable()
	{
		return true;
	}
	
	//Indicate if entity can be dormant
	bool CanBeDormant()
	{
		return false;
	}
	
	//Called when the entity recieves damage
	void OnDamage(uint32 damageValue)
	{
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
		this.m_bRemove = true;
	}
	
	//Called for entity collisions
	void OnCollided(IScriptedEntity@ ref)
	{
		if (ref.GetName() == "player") {
			this.m_bRemove = true;
			ref.OnDamage(MISSILE_DAMAGE);
		}
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
	
	//Set position
	void SetPosition(const Vector &in vec)
	{
		this.m_vecPos = vec;
	}
	
	//Return the rotation. 
	float GetRotation()
	{
		return this.m_fRotation;
	}
	
	//Set rotation
	void SetRotation(float fRot)
	{
		this.m_fRotation = fRot;
	}
	
	//Return a name string here, e.g. the class name or instance name. 
	string GetName()
	{
		return "weapon_missile";
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSize()
	{
		return this.m_vecPos;
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return "";
	}
}