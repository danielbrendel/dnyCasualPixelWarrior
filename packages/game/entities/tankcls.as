/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "weapon_gun.as"
#include "explosion.as"
#include "item_coin.as"

/* Tank entity */
class CTankEntity : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	float m_fRotation;
	float m_fHeadRot;
	Model m_oModel;
	SpriteHandle m_hBody;
	SpriteHandle m_hHead;
	SpriteHandle m_hMuzzle;
	Timer m_tmrSpriteChange;
	Timer m_tmrAttack;
	Timer m_tmrMuzzle;
	SoundHandle m_hFireSound;
	uint32 m_uiHealth;
	bool m_bRemove;
	Timer m_tmrFlicker;
	uint32 m_uiFlickerCount;
	
	CTankEntity()
    {
		this.m_vecSize = Vector(64, 64);
		this.m_uiHealth = 90;
		this.m_bRemove = false;
    }
	
	void AimTo(const Vector& in vecPos)
	{
		//Init aiming process
		
		//Calculate aim rotation
		float flAngle = atan2(float(vecPos[1] - this.m_vecPos[1]), float(vecPos[0] - this.m_vecPos[0]));
		this.m_fHeadRot = flAngle + 6.30 / 4;
		
		//Activate attack timing if not already
		if (!this.m_tmrAttack.IsActive()) {
			this.m_tmrAttack.SetDelay(Util_Random(1000, 2000));
			this.m_tmrAttack.Reset();
			this.m_tmrAttack.SetActive(true);
		}
	}
	
	void Fire()
	{
		//Fire at target
		
		IScriptedEntity@ player = Ent_GetPlayerEntity();
		if (!Ent_IsValid(@player)) 
			return;
		
		//Play fire sound
		S_PlaySound(this.m_hFireSound, S_GetCurrentVolume());
		
		Vector shotPos = Vector(this.m_vecPos[0] + this.m_vecSize[0] / 2, this.m_vecPos[1] + this.m_vecSize[1] / 2);
		shotPos[0] += int(sin(this.m_fHeadRot + 0.014) * 50);
		shotPos[1] -= int(cos(this.m_fHeadRot + 0.014) * 50);
		
		//Spawn shot
		CGunEntity@ gun = CGunEntity();
		Ent_SpawnEntity("weapon_gun", @gun, shotPos);
		gun.SetRotation(this.m_fHeadRot);
		gun.SetOwner(@this);
		gun.SetExplosionFlag(true);
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = 0.0f;
		this.m_fHeadRot = 0.0f;
		this.m_hBody = R_LoadSprite(GetPackagePath() + "gfx\\tank_body.png", 4, 42, 49, 4, true);
		this.m_hHead = R_LoadSprite(GetPackagePath() + "gfx\\tank_head.png", 4, 65, 65, 4, true);
		this.m_hMuzzle = R_LoadSprite(GetPackagePath() + "gfx\\muzzle.png", 1, 256, 256, 1, false);
		this.m_hFireSound = S_QuerySound(GetPackagePath() + "sound\\tank_fire.wav");
		this.m_tmrSpriteChange.SetDelay(50);
		this.m_tmrSpriteChange.Reset();
		this.m_tmrSpriteChange.SetActive(true);
		this.m_tmrAttack.SetDelay(1000);
		this.m_tmrAttack.Reset();
		this.m_tmrAttack.SetActive(false);
		this.m_tmrMuzzle.SetActive(false);
		this.m_tmrFlicker.SetDelay(250);
		this.m_tmrFlicker.SetActive(false);
		this.m_uiFlickerCount = 0;
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hBody);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosionEntity @expl = CExplosionEntity();
		Ent_SpawnEntity("explosion", @expl, this.m_vecPos);

		for (int i = 0; i < 4; i++) {
			CCoinItem@ coin = CCoinItem();
			coin.SetRandomPos(true);
			Ent_SpawnEntity("item_coin", @coin, this.m_vecPos);
		}
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrSpriteChange.Update();
		if (this.m_tmrSpriteChange.IsElapsed()) {
			this.m_tmrSpriteChange.Reset();
			
			Ent_Move(this, 250, MOVE_FORWARD);
		}
		
		//Process target search
		const int MAX_CHECK_RANGE = 500;
		IScriptedEntity@ player = Ent_GetPlayerEntity();
		if (Ent_IsValid(@player)) {
			if ((this.m_vecPos.Distance(player.GetPosition()) < MAX_CHECK_RANGE)) {
				this.AimTo(player.GetPosition());
			} else {
				this.m_tmrAttack.SetActive(false);
			}
		}
		
		//Process attacking
		if (this.m_tmrAttack.IsActive()) {
			this.m_tmrAttack.Update();
			if (this.m_tmrAttack.IsElapsed()) {
				this.m_tmrAttack.Reset();
				this.Fire();
				this.m_tmrMuzzle.SetDelay(10);
				this.m_tmrMuzzle.Reset();
				this.m_tmrMuzzle.SetActive(true);
			}
		}
		
		//Process muzzle flash
		if (this.m_tmrMuzzle.IsActive()) {
			this.m_tmrMuzzle.Update();
			if (this.m_tmrMuzzle.IsElapsed()) {
				this.m_tmrMuzzle.SetActive(false);
			}
		}
		
		//Process flickering
		if (this.m_tmrFlicker.IsActive()) {
			this.m_tmrFlicker.Update();
			if (this.m_tmrFlicker.IsElapsed()) {
				this.m_tmrFlicker.Reset();
				
				this.m_uiFlickerCount++;
				if (this.m_uiFlickerCount >= 5) {
					this.m_uiFlickerCount = 0;
					this.m_tmrFlicker.SetActive(false);
				}
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
		if (this.m_fRotation == 0.0f) {
			this.m_fRotation = 3.30f;
		} else {
			this.m_fRotation = 0.0f;
		}
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		if (!R_ShouldDraw(this.m_vecPos, this.m_vecSize))
			return;
			
		Vector vOut;
		R_GetDrawingPosition(this.m_vecPos, this.m_vecSize, vOut);
		
		bool bDrawCustomColor = false;
		if ((this.m_tmrFlicker.IsActive()) && (this.m_uiFlickerCount % 2 == 0)) {
			bDrawCustomColor = true;
		}
		
		Color sDrawingColor = (this.m_tmrFlicker.IsActive()) ? Color(255, 0, 0, 150) : Color(0, 0, 0, 0);
		
		R_DrawSprite(this.m_hBody, vOut, 0, 0.0f, Vector(-1, -1), 0.0f, 0.0f, bDrawCustomColor, sDrawingColor);
		R_DrawSprite(this.m_hHead, Vector(vOut[0] - 11, vOut[1] - 3), 0, this.m_fHeadRot, Vector(-1, -1), 0.0, 0.0, bDrawCustomColor, sDrawingColor);
		
		if (this.m_tmrMuzzle.IsActive()) {
			Vector vecMuzzlePos = Vector(this.m_vecPos[0] + this.m_vecSize[0] / 2 - 256 / 2, this.m_vecPos[1] + this.m_vecSize[1] / 2 - 256 / 2);
			vecMuzzlePos[0] += int(sin(this.m_fHeadRot + 0.014) * 90);
			vecMuzzlePos[1] -= int(cos(this.m_fHeadRot + 0.014) * 90);
			
			Vector vMuzzleOut;
			R_GetDrawingPosition(vecMuzzlePos, this.m_vecSize, vMuzzleOut);
			
			R_DrawSprite(this.m_hMuzzle, vMuzzleOut, 0, this.m_fHeadRot, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		}
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_bRemove;
	}
	
	//Indicate whether this entity is collidable
	bool IsCollidable()
	{
		return true;
	}
	
	//Called when the entity collided with another entity
	void OnCollided(IScriptedEntity@ ref)
	{
	}
	
	//Called when entity gets damaged
	void OnDamage(uint32 damageValue)
	{
		if (this.m_uiHealth < damageValue) {
			this.m_uiHealth = 0;
		} else {
			this.m_uiHealth -= damageValue;
		}
		
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(true);
		this.m_uiFlickerCount = 0;
		
		if (this.m_uiHealth == 0) {
			this.m_bRemove = true;
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
	
	//Set new position
	void SetPosition(const Vector &in vecPos)
	{
		this.m_vecPos = vecPos;
	}
	
	//This vector is used for getting the overall drawing size
	Vector& GetSize()
	{
		return this.m_vecSize;
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
	
	//Set health
	void SetHealth(uint health)
	{
		this.m_uiHealth = health;
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "tank";
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return Props_CreateProperty("id", formatInt(Ent_GetId(@this))) +
				Props_CreateProperty("x", formatInt(this.m_vecPos[0])) +
				Props_CreateProperty("y", formatInt(this.m_vecPos[1])) +
				Props_CreateProperty("rot", formatFloat(this.m_fRotation)) +
				Props_CreateProperty("health", formatInt(this.m_uiHealth));
	}
}