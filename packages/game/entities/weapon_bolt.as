/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "explosion.as"

const uint32 BOLT_SHOT_DAMAGE = 30;

/* Bolt entity  */
class CBoltEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	Timer m_oFrameTime;
	SpriteHandle m_hSprite;
	int m_iFrameCounter;
	float m_fRotation;
	float m_fRange;
	IScriptedEntity@ m_pTarget;
	
	CBoltEntity()
    {
		this.m_iFrameCounter = 0;
		this.m_fRotation = 0.0;
		this.m_vecSize = Vector(32, 256);
		@m_pTarget = null;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		Vector vTargetPos = this.m_pTarget.GetPosition();
		Vector vTargetCenter = this.m_pTarget.GetModel().GetCenter();
		Vector vAbsTargetPos = Vector(vTargetPos[0] + vTargetCenter[0], vTargetPos[1] + vTargetCenter[1]);
		this.m_fRange = float(Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 5).Distance(vAbsTargetPos)) / 300;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\lightning.png", 8, 32, 256, 8, false);
		this.m_oFrameTime.SetDelay(10);
		this.m_oFrameTime.Reset();
		this.m_oFrameTime.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_oFrameTime.Update();
		if (this.m_oFrameTime.IsElapsed()) {
			this.m_oFrameTime.Reset();
			this.m_iFrameCounter++;
			if (this.m_iFrameCounter >= 8) {
				this.m_oFrameTime.SetActive(false);
				
				this.m_pTarget.OnDamage(BOLT_SHOT_DAMAGE);
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
		
		R_DrawSprite(this.m_hSprite, vOut, this.m_iFrameCounter, this.m_fRotation, Vector(16, 0), 0.75, this.m_fRange, false, Color(0, 0, 0, 0));
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_iFrameCounter >= 8;
	}
	
	//Indicate whether this entity is collidable
	bool IsCollidable()
	{
		return false;
	}
	
	//Called when the entity collided with another entity
	void OnCollided(IScriptedEntity@ ref)
	{
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
		return "weapon_bolt";
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
	
	//Set target entity
	void SetTarget(IScriptedEntity@ pEntity)
	{
		@this.m_pTarget = pEntity;
	}
}