/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* Health item entity */
const int ITEM_HEALTH_ADDITION = 25;
class CItemHealth : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	SpriteHandle m_hSprite;
	array<SpriteHandle> m_arrHeart;
	int m_iSpriteIndex;
	Timer m_tmrSpriteSwitch;
	bool m_bActive;
	Timer m_tmrActive;
	SoundHandle m_hReceive;
	SoundHandle m_hActivate;
	
	CItemHealth()
    {
		this.m_vecSize = Vector(32, 32);
		this.m_iSpriteIndex = 0;
		this.m_bActive = true;
    }
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\health\\frame-1.png", 1, this.m_vecSize[0], this.m_vecSize[1], 1, true);
		for (int i = 1; i < 9; i++) {
			this.m_arrHeart.insertLast(R_LoadSprite(GetPackagePath() + "gfx\\health\\frame-" + formatInt(i) + ".png", 1, this.m_vecSize[0], this.m_vecSize[1], 1, true));
		}
		this.m_hReceive = S_QuerySound(GetPackagePath() + "sound\\health_pickup.wav");
		this.m_hActivate = S_QuerySound(GetPackagePath() + "sound\\health_activate.wav");
		this.m_tmrSpriteSwitch.SetDelay(100);
		this.m_tmrSpriteSwitch.Reset();
		this.m_tmrSpriteSwitch.SetActive(true);
		this.m_tmrActive.SetDelay(30000);
		this.m_tmrActive.Reset();
		this.m_tmrActive.SetActive(false);
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
		//Process sprite switching
		this.m_tmrSpriteSwitch.Update();
		if (this.m_tmrSpriteSwitch.IsElapsed()) {
			this.m_tmrSpriteSwitch.Reset();
			
			this.m_iSpriteIndex++;
			if (this.m_iSpriteIndex >= 8) {
				this.m_iSpriteIndex = 0;
			}
		}
		
		//Process activation
		if (this.m_tmrActive.IsActive()) {
			this.m_tmrActive.Update();
			if (this.m_tmrActive.IsElapsed()) {
				this.m_tmrActive.SetActive(false);
				this.m_bActive = true;
				S_PlaySound(this.m_hActivate, S_GetCurrentVolume());
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
		
		if (this.m_bActive) {
			R_DrawSprite(this.m_arrHeart[this.m_iSpriteIndex], vOut, 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		}
	}
	
	//Draw on top
	void OnDrawOnTop()
	{
	}
	
	//Indicate whether this entity shall be removed by the game
	bool NeedsRemoval()
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
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Called for entity collisions
	void OnCollided(IScriptedEntity@ ref)
	{
		if ((this.m_bActive) && (ref.GetName() == "player")) {
			ICollectingEntity@ collectingEntity = cast<ICollectingEntity>(ref);
			collectingEntity.AddHealth(ITEM_HEALTH_ADDITION);
			
			this.m_bActive = false;
			this.m_tmrActive.Reset();
			this.m_tmrActive.SetActive(true);
			
			S_PlaySound(this.m_hReceive, S_GetCurrentVolume());
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
		return 0.0;
	}
	
	//Set rotation
	void SetRotation(float fRot)
	{
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "item_health";
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

//Create health entity
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CItemHealth @health = CItemHealth();
	Ent_SpawnEntity(szIdent, @health, vecPos);
}