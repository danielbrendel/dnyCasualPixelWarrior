/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

string g_szPackagePath = "";
const uint32 PLASMA_BALL_DAMAGE = 25;

/* Plasma ball entity */
class CPlasmaBall : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	float m_fRotation;
	Model m_oModel;
	SpriteHandle m_hSprite;
	int m_iSpriteIndex;
	Timer m_tmrSpriteChange;
	Timer m_tmrMayDamage;
	
	CPlasmaBall()
    {
		this.m_vecSize = Vector(64, 64);
		this.m_iSpriteIndex = 0;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = 0.0f;
		this.m_hSprite = R_LoadSprite(g_szPackagePath + "gfx\\plasmaball.png", 4, 64, 64, 4, false);
		this.m_tmrSpriteChange.SetDelay(50);
		this.m_tmrSpriteChange.Reset();
		this.m_tmrSpriteChange.SetActive(true);
		this.m_tmrMayDamage.SetDelay(2000);
		this.m_tmrMayDamage.Reset();
		this.m_tmrMayDamage.SetActive(true);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		if (CVar_GetBool("game_started", false) == false) {
			return;
		}
		
		this.m_tmrSpriteChange.Update();
		if (this.m_tmrSpriteChange.IsElapsed()) {
			this.m_tmrSpriteChange.Reset();
			
			this.m_iSpriteIndex++;
			if (this.m_iSpriteIndex >= 4) {
				this.m_iSpriteIndex = 0;
			}
			
			Ent_Move(this, 350, MOVE_FORWARD);
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
		
		R_DrawSprite(this.m_hSprite, vOut, this.m_iSpriteIndex, 0.0f, Vector(-1, -1), 0.0f, 0.0f, false, Color(0, 0, 0, 0));
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
	{
		return false;
	}
	
	//Indicate whether this entity is collidable
	bool IsCollidable()
	{
		return true;
	}
	
	//Called when the entity collided with another entity
	void OnCollided(IScriptedEntity@ ref)
	{
		if (ref.GetName() == "player") {
			this.m_tmrMayDamage.Update();
			if (this.m_tmrMayDamage.IsElapsed()) {
				this.m_tmrMayDamage.Reset();
				
				ref.OnDamage(PLASMA_BALL_DAMAGE);
				
				SoundHandle hHit = S_QuerySound(GetPackagePath() + "sound\\plasma_hit.wav");
				S_PlaySound(hHit, S_GetCurrentVolume());
			}
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
		return "plasma_ball";
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


void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	g_szPackagePath = szPath;

	CPlasmaBall @ball = CPlasmaBall();
	Ent_SpawnEntity(szIdent, @ball, vecPos);
}

