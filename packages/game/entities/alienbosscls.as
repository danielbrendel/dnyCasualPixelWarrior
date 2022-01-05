/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "weapon_laserball.as"
#include "weapon_laser.as"
#include "weapon_missile.as"
#include "weapon_bolt.as"
#include "bigexplosion.as"
#include "item_coin.as"

const int C_ALIENBOSS_REACT_RANGE = 650;
const int C_ALIENBOSS_ATTACK_RANGE = 500;
const float C_ALIENBOSS_DEFAULT_SPEED = 65.0;
const int C_ALIENBOSS_MAX_HEALTH = 3000;

/* Alien boss entity */
class CAlienBoss : IScriptedEntity
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
	SoundHandle m_hLaserSound;
	SoundHandle m_hMissileSound;
	SoundHandle m_hBoltSound;
	bool m_bLastGotEnemy;
	bool m_bInAttackRange;
	uint32 m_uiHealth;
	uint m_uiFlickerCount;
	FontHandle m_hBossFont;

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

	void FireLaserBall(IScriptedEntity@ pEntity)
	{
		//Fire laser ball

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

		S_PlaySound(this.m_hLaserSound, S_GetCurrentVolume());
	}

	void FireLaser(IScriptedEntity@ pEntity)
	{
		//Fire laser

		if ((@pEntity == null) || (!Ent_IsValid(pEntity)))
			return;
		
		for (int i = 0; i < 5; i++) {
			CLaserEntity@ laser = CLaserEntity();

			float fLaserRot = this.GetRotation();
							
			if (i == 0) {
				fLaserRot -= 0.2;
			} else if (i == 1) {
				fLaserRot -= 0.1;
			} else if (i == 2) {
				fLaserRot += 0.0;
			} else if (i == 3) {
				fLaserRot += 0.1;
			} else if (i == 4) {
				fLaserRot += 0.2;
			}

			laser.SetRotation(fLaserRot);
			laser.SetOwner(@this);
			laser.RandomColor(true);

			Vector vecBulletPos = Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 10);

			Ent_SpawnEntity("weapon_laser", laser, vecBulletPos);
		}

		S_PlaySound(this.m_hLaserSound, S_GetCurrentVolume());
	}

	void FireMissile(IScriptedEntity@ pEntity)
	{
		//Fire missile

		if ((@pEntity == null) || (!Ent_IsValid(pEntity)))
			return;
		
		for (int i = 0; i < 5; i++) {
			CMissileEntity @missile = CMissileEntity();

			float fMissileRot = this.GetRotation();
							
			if (i == 0) {
				fMissileRot -= 0.2;
			} else if (i == 1) {
				fMissileRot -= 0.1;
			} else if (i == 2) {
				fMissileRot += 0.0;
			} else if (i == 3) {
				fMissileRot += 0.1;
			} else if (i == 4) {
				fMissileRot += 0.2;
			}

			missile.SetRotation(fMissileRot);

			Ent_SpawnEntity("weapon_missile", missile, this.m_vecPos);
		}

		S_PlaySound(this.m_hMissileSound, S_GetCurrentVolume());
	}

	void FireBolt(IScriptedEntity@ pEntity)
	{
		//Fire bolt

		if ((@pEntity == null) || (!Ent_IsValid(pEntity)))
			return;
		
		CBoltEntity@ obj = CBoltEntity();
		
		Vector vTargetPos = pEntity.GetPosition();
		Vector vTargetCenter = pEntity.GetModel().GetCenter();
		Vector vAbsTargetPos = Vector(vTargetPos[0] + vTargetCenter[0], vTargetPos[1] + vTargetCenter[1]);
		
		obj.SetRotation(this.GetAimRotation(vAbsTargetPos));
		obj.SetTarget(pEntity);
		
		Ent_SpawnEntity("weapon_bolt", @obj, Vector(this.m_vecPos[0] + 35, this.m_vecPos[1] + 130));

		S_PlaySound(this.m_hBoltSound, 10);
	}

	void Fire(IScriptedEntity@ pEntity)
	{
		//Fire lightning
		
		int iRndValue = Util_Random(1, 5);
		switch (iRndValue) {
			case 1:
				this.FireLaserBall(@pEntity);
				break;
			case 2:
				this.FireLaser(@pEntity);
				break;
			case 3:
				this.FireMissile(@pEntity);
				break;
			case 4:
				this.FireBolt(@pEntity);
				break;
			default:
				this.FireLaserBall(@pEntity);
				break;
		}
	}
	
	void CheckForEnemiesInRange()
	{
		//Check for enemies in close range and act accordingly
		
		this.m_bGotEnemy = false;
		this.m_bInAttackRange = false;

		IScriptedEntity@ pEntity = Ent_GetPlayerEntity();
		if (this.m_vecPos.Distance(pEntity.GetPosition()) <= C_ALIENBOSS_REACT_RANGE) {
			this.m_bGotEnemy = true;
		}
		
		if (this.m_bGotEnemy) {
			if (this.m_fSpeed == C_ALIENBOSS_DEFAULT_SPEED)
				this.m_fSpeed *= 2;
				
			this.LookAt(pEntity.GetPosition());

			if (this.m_vecPos.Distance(pEntity.GetPosition()) <= C_ALIENBOSS_ATTACK_RANGE) {
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

	CAlienBoss()
    {
		this.m_vecSize = Vector(288, 321);
		this.m_bGotEnemy = this.m_bLastGotEnemy = false;
		this.m_fSpeed = C_ALIENBOSS_DEFAULT_SPEED;
		this.m_uiHealth = C_ALIENBOSS_MAX_HEALTH;
		this.m_uiFlickerCount = 0;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = 0.0f;
		this.m_hMove = R_LoadSprite(GetPackagePath() + "gfx\\alienboss.png", 1, 288, 321, 1, false);
		this.m_hLaserSound = S_QuerySound(GetPackagePath() + "sound\\laser.wav");
		this.m_hMissileSound = S_QuerySound(GetPackagePath() + "sound\\missile_launch.wav");
		this.m_hBoltSound = S_QuerySound(GetPackagePath() + "sound\\tesla_attack.wav");
		this.m_hBossFont = R_LoadFont("Arial", 21, 45);
		this.m_tmrMove.SetDelay(10);
		this.m_tmrMove.Reset();
		this.m_tmrMove.SetActive(true);
		this.m_tmrDirChange.SetDelay(5000);
		this.m_tmrDirChange.Reset();
		this.m_tmrDirChange.SetActive(true);
		this.m_tmrAttack.SetDelay(1000);
		this.m_tmrAttack.Reset();
		this.m_tmrAttack.SetActive(true);
		this.m_tmrFlicker.SetDelay(250);
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(-50, -50), Vector(30 * this.m_vecSize[0] / 100, 30 * this.m_vecSize[1] / 100));
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hMove);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		CBigExplosionEntity @obj = CBigExplosionEntity();
		Ent_SpawnEntity("bigexplosion", @obj, this.m_vecPos);

		for (int i = 0; i < 32; i++) {
			CCoinItem@ coin = CCoinItem();
			coin.SetRandomPos(true);
			Ent_SpawnEntity("item_coin", @coin, this.m_vecPos);
		}
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

		R_DrawSprite(this.m_hMove, vOut, 0, this.m_fRotation, Vector(-1, -1), 0.5f, 0.5f, bCustomColor, sDrawingColor);

		const int iHealthMaxWidth = 500;

		if (this.m_uiHealth > 0) {
			int iHealthBarWidthPercent = int(this.m_uiHealth) * 100 / C_ALIENBOSS_MAX_HEALTH;
			int iHealthBarWidthValue = iHealthBarWidthPercent * iHealthMaxWidth / 100;

			Color sBarColor = Color(0, 255, 0, 255);
			if ((iHealthBarWidthPercent < 65) && (iHealthBarWidthPercent >= 25)) {
				sBarColor = Color(150, 150, 0, 150);
			} else if (iHealthBarWidthPercent < 25) {
				sBarColor = Color(250, 0, 0, 150);
			}

			R_DrawFilledBox(Vector(Wnd_GetWindowCenterX() - iHealthMaxWidth / 2 - 100, Wnd_GetWindowCenterY() * 2 - 100), Vector(((iHealthBarWidthValue > 0) ? iHealthBarWidthValue : 1), 50), sBarColor);
			R_DrawString(this.m_hBossFont, _("app.boss", "BOSS"), Vector(Wnd_GetWindowCenterX() - iHealthMaxWidth / 2 - 100 + 2, Wnd_GetWindowCenterY() * 2 - 100), Color(200, 200, 200, 255));
		}
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
		return "alienboss";
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
