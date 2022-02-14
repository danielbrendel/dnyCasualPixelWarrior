/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* World obstacle entity */
class CWorldObstacle : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	SpriteHandle m_hSprite;
	SoundHandle m_hDamage;
	string m_szTexture;
	string m_szSound;
	uint m_uiDamageValue;
	bool m_bPlaySound;
	Timer m_tmrPlaySound;
	
	CWorldObstacle()
    {
		this.m_vecSize = Vector(64, 64);
		this.m_bPlaySound = true;
    }
	
	//Set texture
	void SetTexture(const string &in szTexture)
	{
		this.m_szTexture = szTexture;
	}
	
	//Set damage sound
	void SetSound(const string &in szSound)
	{
		this.m_szSound = szSound;
	}
	
	//Set damage
	void SetDamage(uint uiDamageValue)
	{
		this.m_uiDamageValue = uiDamageValue;
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\" + this.m_szTexture, 1, this.m_vecSize[0], this.m_vecSize[1], 1, true);
		this.m_hDamage = S_QuerySound(GetPackagePath() + "sound\\" + this.m_szSound);
		this.m_tmrPlaySound.SetDelay(2000);
		this.m_tmrPlaySound.Reset();
		this.m_tmrPlaySound.SetActive(false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(10, 10), Vector(22, 22));
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
		if (this.m_tmrPlaySound.IsActive()) {
			this.m_tmrPlaySound.Update();
			if (this.m_tmrPlaySound.IsElapsed()) {
				this.m_tmrPlaySound.SetActive(false);
				
				this.m_bPlaySound = true;
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
		
		R_DrawSprite(this.m_hSprite, vOut, 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
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
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Called for entity collisions
	void OnCollided(IScriptedEntity@ ref)
	{
		if (ref.GetName() == "player") {
			ref.OnDamage(this.m_uiDamageValue);
			
			if (this.m_bPlaySound) {
				S_PlaySound(this.m_hDamage, S_GetCurrentVolume());
			
				this.m_bPlaySound = false;
				
				this.m_tmrPlaySound.Reset();
				this.m_tmrPlaySound.SetActive(true);
			}
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
		return "world_obstacle";
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