/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "weapon_laserball.as"
#include "explosion.as"

const int C_ALIENVEHICLE_REACT_RANGE = 500;
const int C_ALIENVEHICLE_ATTACK_RANGE = 300;
const float C_ALIENVEHICLE_DEFAULT_SPEED = 65.0;

/* Alien vehicle entity */
class CAlienVehicle : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	float m_fRotation;
	Model m_oModel;
	SpriteHandle m_hMove;
	Timer m_tmrMayDamage;
	Timer m_tmrAttack;
	Timer m_tmrDirChange;
	Timer m_tmrMove;
	Timer m_tmrFlicker;
	bool m_bGotEnemy;
	float m_fSpeed;
	SoundHandle m_hAttackSound;
	bool m_bLastGotEnemy;
	bool m_bInAttackRange;
	uint32 m_uiHealth;
	uint m_uiFlickerCount;

	void LookAt(const Vector &in vPos)
	{
		//Look at position

		float flAngle = atan2(float(vPos[1] - this.m_vecPos[1]), float(vPos[0] - this.m_vecPos[0]));
		this.m_fRotation = flAngle + 6.30 / 4;
	}

	float GetAimRotation(const Vector &in vPos)
	{
		//Get aim rotation
		
		float fAngle = atan2(float(vPos[1] - this.m_vecPos[1]), float(vPos[0] - this.m_vecPos[0]));
		return fAngle - 6.30 / 4;
	}

	void Fire(IScriptedEntity@ pEntity)
	{
		//Fire lightning
		
		if ((@pEntity == null) || (!Ent_IsValid(pEntity)))
			return;
		
		for (int i = 0; i < 5; i++) {
			CLaserBallEntity@ ball = CLaserBallEntity();

			float fBallRot = this.GetRotation();
							
			if (i == 0) {
				fBallRot -= 0.3;
			} else if (i == 1) {
				fBallRot -= 0.2;
			} else if (i == 3) {
				fBallRot += 0.2;
			} else if (i == 2) {
				fBallRot += 0.3;
			}

			ball.SetRotation(fBallRot);
			ball.SetOwner(@this);

			Ent_SpawnEntity("weapon_laserball", ball, this.m_vecPos);
		}

		S_PlaySound(this.m_hAttackSound, S_GetCurrentVolume());
	}
	
	void CheckForEnemiesInRange()
	{
		//Check for enemies in close range and act accordingly
		
		this.m_bGotEnemy = false;
		this.m_bInAttackRange = false;

		IScriptedEntity@ pEntity = Ent_GetPlayerEntity();
		if (this.m_vecPos.Distance(pEntity.GetPosition()) <= C_ALIENVEHICLE_REACT_RANGE) {
			this.m_bGotEnemy = true;
		}
		
		if (this.m_bGotEnemy) {
			if (this.m_fSpeed == C_ALIENVEHICLE_DEFAULT_SPEED)
				this.m_fSpeed *= 2;
				
			this.LookAt(pEntity.GetPosition());

			if (this.m_vecPos.Distance(pEntity.GetPosition()) <= C_ALIENVEHICLE_ATTACK_RANGE) {
				this.m_bInAttackRange = true;
				this.m_tmrAttack.Update();
				if (this.m_tmrAttack.IsElapsed()) {
					this.m_tmrAttack.Reset();
					this.Fire(pEntity);
				}
			}

			this.m_bLastGotEnemy = true;
		}
	}

	CAlienVehicle()
    {
		this.m_vecSize = Vector(76, 97);
		this.m_bGotEnemy = this.m_bLastGotEnemy = false;
		this.m_fSpeed = C_ALIENVEHICLE_DEFAULT_SPEED;
		this.m_uiHealth = 90;
		this.m_uiFlickerCount = 0;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = 0.0f;
		this.m_hMove = R_LoadSprite(GetPackagePath() + "gfx\\alienvehicle.png", 1, 76, 97, 1, false);
		this.m_hAttackSound = S_QuerySound(GetPackagePath() + "sound\\laser.wav");
		this.m_tmrMove.SetDelay(10);
		this.m_tmrMove.Reset();
		this.m_tmrMove.SetActive(true);
		this.m_tmrDirChange.SetDelay(5000);
		this.m_tmrDirChange.Reset();
		this.m_tmrDirChange.SetActive(true);
		this.m_tmrAttack.SetDelay(1500);
		this.m_tmrAttack.Reset();
		this.m_tmrAttack.SetActive(true);
		this.m_tmrFlicker.SetDelay(250);
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(-50, -50), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hMove);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CExplosionEntity @obj = CExplosionEntity();
		Ent_SpawnEntity("explosion", @obj, this.m_vecPos);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		if (this.m_tmrMove.IsActive()) {
			this.m_tmrMove.Update();

			if (this.m_tmrMove.IsElapsed()) {
				this.m_tmrMove.Reset();

				if (!this.m_bInAttackRange) {
					Ent_Move(this, this.m_fSpeed, MOVE_FORWARD);
				}
			}
		}

		if (!this.m_bGotEnemy) {
			this.m_tmrDirChange.Update();
			if (this.m_tmrDirChange.IsElapsed()) {
				this.m_tmrDirChange.Reset();
				
				this.m_fRotation = float(Util_Random(1, 360));
			}
		}

		this.CheckForEnemiesInRange();

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
		
		Color sDrawingColor = (this.m_tmrFlicker.IsActive()) ? Color(255, 0, 0, 150) : Color(0, 0, 0, 0);
		bool bCustomColor = (this.m_tmrFlicker.IsActive()) && (this.m_uiFlickerCount % 2 == 0);

		R_DrawSprite(this.m_hMove, vOut, 0, this.m_fRotation, Vector(-1, -1), 1.0f, 1.0f, bCustomColor, sDrawingColor);
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return this.m_uiHealth == 0;
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
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "mech";
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return Props_CreateProperty("id", formatInt(Ent_GetId(@this))) + 
			Props_CreateProperty("x", formatInt(this.m_vecPos[0])) +
			Props_CreateProperty("y", formatInt(this.m_vecPos[1])) +
			Props_CreateProperty("rot", formatFloat(this.m_fRotation));
	}
}
