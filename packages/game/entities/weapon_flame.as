/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "decal.as"

const uint32 FLAME_DAMAGE = 10;

/* FLame entity  */
class CFlameEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	Timer m_oFrameTime;
	SpriteHandle m_hSprite;
	int m_iFrameCounter;
	float m_fRotation;
    Timer m_oAliveTime;
    IScriptedEntity@ m_pOwner;
    float m_fDir;
    Timer m_oMovement;
    float m_fSpeed;
	
	CFlameEntity()
    {
		this.m_iFrameCounter = 0;
		this.m_fRotation = 0.0;
		this.m_vecSize = Vector(48, 48);
        @this.m_pOwner = null;
        this.m_fDir = 0.0;
        this.m_fSpeed = 250.0;
    }

    //Set owner
    void SetOwner(IScriptedEntity@ pOwner)
    {
        @this.m_pOwner = @pOwner;
    }

    //Set fly direction
    void SetDirection(float fDir)
    {
        this.m_fDir = fDir;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\flames.png", 7, 48, 48, 7, false);
		this.m_oFrameTime.SetDelay(10);
		this.m_oFrameTime.Reset();
		this.m_oFrameTime.SetActive(true);
        this.m_oAliveTime.SetDelay(4000);
        this.m_oAliveTime.Reset();
        this.m_oAliveTime.SetActive(true);
        this.m_oMovement.SetDelay(1250);
        this.m_oMovement.Reset();
        this.m_oMovement.SetActive(true);
        BoundingBox bbox;
        bbox.Alloc();
        bbox.AddBBoxItem(Vector(0, 0), Vector(48, 48));
		this.m_oModel.Alloc();
        this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
        CDecalEntity @dcl = CDecalEntity();
		Ent_SpawnEntity("decal", @dcl, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oFrameTime.Update();
		if (this.m_oFrameTime.IsElapsed()) {
			this.m_oFrameTime.Reset();
			this.m_iFrameCounter++;
			if (this.m_iFrameCounter >= 7) {
				this.m_iFrameCounter = 0;
			}
		}
        
        this.m_oAliveTime.Update();

        if (this.m_oMovement.IsActive()) {
            this.m_oMovement.Update();

            Ent_Move(this, this.m_fSpeed, MOVE_FORWARD);

            if (this.m_oMovement.IsElapsed()) {
                this.m_oMovement.SetActive(false);
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
		
		R_DrawSprite(this.m_hSprite, vOut, this.m_iFrameCounter, this.m_fRotation, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_oAliveTime.IsElapsed();
	}
	
	//Indicate whether this entity is collidable
	bool IsCollidable()
	{
		return true;
	}
	
	//Indicate if entity can be dormant
	bool CanBeDormant()
	{
		return false;
	}
	
	//Called when the entity collided with another entity
	void OnCollided(IScriptedEntity@ ref)
	{
        if ((@ref != @this.m_pOwner) && (ref.GetName() != this.m_pOwner.GetName()) && (ref.GetName() != this.GetName()) && (ref.GetName() != "weapon_gun") && (ref.GetName() != "weapon_laser") && (ref.GetName() != "weapon_laserball") && (ref.GetName() != "weapon_missile") && (ref.GetName() != "item_ammo_grenade") && (ref.GetName() != "item_ammo_handgun") && (ref.GetName() != "item_ammo_rifle") && (ref.GetName() != "item_ammo_shotgun") && (ref.GetName() != "item_coin") && (ref.GetName() != "item_health")) {
            ref.OnDamage(FLAME_DAMAGE);
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
		return this.m_fDir;
	}
	
	//Set new rotation
	void SetRotation(float fRot)
	{
		this.m_fRotation = fRot;
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "weapon_flame";
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