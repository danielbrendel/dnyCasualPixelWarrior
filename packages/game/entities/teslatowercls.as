/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "weapon_bolt.as"
#include "explosion.as"
#include "item_coin.as"

/* Tesla tower entity */
class CTeslaTower : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	SpriteHandle m_hSprite;
	SoundHandle m_hAttack;
	SoundHandle m_hCharge;
	IScriptedEntity@ m_pTarget;
	uint8 m_ucAlpha;
	Timer m_oAttacking;
	Timer m_oAlpha;
	uint m_uiHealth;
	uint m_uiFlickerCount;
	Timer m_tmrFlicker;
	
	CTeslaTower()
    {
		this.m_vecSize = Vector(64, 64);
		this.m_uiHealth = 100;
		this.m_uiFlickerCount = 0;
    }
	
	float GetAimRotation(const Vector &in vPos)
	{
		//Get aim rotation
		
		float fAngle = atan2(float(vPos[1] - this.m_vecPos[1]), float(vPos[0] - this.m_vecPos[0]));
		return fAngle - 6.30 / 4;
	}
	
	void CheckForEnemiesInRange()
	{
		//Check for enemies in close range and act accordingly
		
		const int TESLATOWER_ATTACK_RANGE = 550;
		
		@this.m_pTarget = null;
		
		for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
			IScriptedEntity@ pEntity = @Ent_GetEntityHandle(i);
			if ((@pEntity != null) && (@pEntity != @this) && (pEntity.GetName() == "player")) {
				if (this.m_vecPos.Distance(pEntity.GetPosition()) <= TESLATOWER_ATTACK_RANGE) {
					@this.m_pTarget = @pEntity;
					break;
				}
			}
		}
		
		if (@this.m_pTarget != null) {
			if (!this.m_oAttacking.IsActive()) {
				this.m_oAttacking.SetDelay(2500);
				this.m_oAttacking.Reset();
				this.m_oAttacking.SetActive(true);
				this.m_oAlpha.SetDelay(10);
				this.m_oAlpha.Reset();
				this.m_oAlpha.SetActive(true);
				this.m_ucAlpha = 255;
				S_PlaySound(this.m_hCharge, 9);
			}
		} else {
			if (this.m_oAttacking.IsActive())
				this.m_oAttacking.SetActive(false);
			
			if (this.m_oAlpha.IsActive())
				this.m_oAlpha.SetActive(false);
				
			this.m_ucAlpha = 255;
		}
	}
	
	void Fire()
	{
		//Fire tesla lightning
		
		if (@this.m_pTarget == null)
			return;
		
		CBoltEntity@ obj = CBoltEntity();
		
		Vector vTargetPos = this.m_pTarget.GetPosition();
		Vector vTargetCenter = this.m_pTarget.GetModel().GetCenter();
		Vector vAbsTargetPos = Vector(vTargetPos[0] + vTargetCenter[0], vTargetPos[1] + vTargetCenter[1]);
		
		obj.SetRotation(this.GetAimRotation(vAbsTargetPos));
		obj.SetTarget(this.m_pTarget);
		
		Ent_SpawnEntity("weapon_bolt", @obj, Vector(this.m_vecPos[0] - 15, this.m_vecPos[1] + 100));

		S_PlaySound(this.m_hAttack, 10);
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\teslatower.png", 1, 32, 55, 1, false);
		this.m_hAttack = S_QuerySound(GetPackagePath() + "sound\\tesla_attack.wav");
		this.m_hCharge = S_QuerySound(GetPackagePath() + "sound\\tesla_charge.wav");
		this.m_oAttacking.SetActive(false);
		this.m_tmrFlicker.SetDelay(250);
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), Vector(32, 55));
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(16, 38));
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosionEntity @expl = CExplosionEntity();
		Ent_SpawnEntity("explosion", @expl, this.m_vecPos);

		for (int i = 0; i < 2; i++) {
			CCoinItem@ coin = CCoinItem();
			coin.SetRandomPos(true);
			Ent_SpawnEntity("item_coin", @coin, this.m_vecPos);
		}
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.CheckForEnemiesInRange();
		
		if (this.m_oAttacking.IsActive()) {
			this.m_oAttacking.Update();
			if (this.m_oAttacking.IsElapsed()) {
				this.m_oAttacking.Reset();
				this.Fire();
				this.m_ucAlpha = 255;
			}
		}
		
		if (this.m_oAlpha.IsActive()) {
			this.m_oAlpha.Update();
			if (this.m_oAlpha.IsElapsed()) {
				this.m_oAlpha.Reset();
				
				this.m_ucAlpha -= 2;
			}
		}
		
		if (this.m_tmrFlicker.IsActive()) {
			this.m_tmrFlicker.Update();
			if (this.m_tmrFlicker.IsElapsed()) {
				this.m_tmrFlicker.Reset();
				
				this.m_uiFlickerCount++;
				if (this.m_uiFlickerCount >= 6) {
					this.m_tmrFlicker.SetActive(false);
					this.m_uiFlickerCount = 0;
				}
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
		if (!R_ShouldDraw(this.m_vecPos, this.m_vecSize))
			return;
			
		Vector vOut;
		R_GetDrawingPosition(this.m_vecPos, this.m_vecSize, vOut);
		
		Color sDrawingColor = (this.m_tmrFlicker.IsActive()) ? Color(255, 0, 0, this.m_ucAlpha) : Color(105, 201, 223, this.m_ucAlpha);
		
		R_DrawSprite(this.m_hSprite, vOut, 0, 0.0, Vector(-1, -1), 0.0, 0.0, true, sDrawingColor);
	}
	
	//Draw on top
	void OnDrawOnTop()
	{
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_uiHealth == 0;
	}
	
	//Indicate if entity can be dormant
	bool CanBeDormant()
	{
		return false;
	}
	
	//Indicate if entity can be collided
	bool IsCollidable()
	{
		return true;
	}
	
	//Called when the entity recieves damage
	void OnDamage(uint32 damageValue)
	{
		if (damageValue >= this.m_uiHealth) {
			this.m_uiHealth = 0;
		} else {
			this.m_uiHealth -= damageValue;
		}
		
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(true);
		this.m_uiFlickerCount = 0;
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Called for entity collisions
	void OnCollided(IScriptedEntity@ ref)
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
	
	//Set position
	void SetPosition(const Vector &in vec)
	{
		this.m_vecPos = vec;
	}
	
	//Return the rotation.
	float GetRotation()
	{
		return 0.0;
	}
	
	//Set rotation
	void SetRotation(float fRot)
	{
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "teslatower";
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSize()
	{
		return this.m_vecPos;
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return Props_CreateProperty("x", formatInt(this.m_vecPos[0])) +
			Props_CreateProperty("y", formatInt(this.m_vecPos[1])) +
			Props_CreateProperty("rot", formatFloat(this.GetRotation()));
	}
}
