/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* Ammo base item entity */
class CItemAmmoBase : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	float m_fRotation;
	Model m_oModel;
	string m_szSprite;
	string m_szWeapon;
	SpriteHandle m_hSprite;
	float m_fSize;
	Timer m_tmrAnim;
	bool m_bActive;
	Timer m_tmrActive;
	SoundHandle m_hReceive;
	SoundHandle m_hActivate;
	uint m_uiSupplyCount;
	
	CItemAmmoBase()
    {
		this.m_vecSize = Vector(32, 32);
		this.m_bActive = true;
		this.m_fRotation = 0.0;
		this.m_fSize = 0.00;
		this.m_uiSupplyCount = 100;
		this.m_szWeapon = "";
    }
	
	//Set sprite
	void SetSprite(const string &in szSprite)
	{
		this.m_szSprite = szSprite;
	}
	
	//Set amount of supply this entity gives
	void SetSupplyCount(uint uiCount)
	{
		this.m_uiSupplyCount = uiCount;
	}
	
	//Set weapon identifier
	void SetWeapon(const string &in szWeapon)
	{
		this.m_szWeapon = szWeapon;
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\" + this.m_szSprite, 1, this.m_vecSize[0], this.m_vecSize[1], 1, true);
		this.m_hReceive = S_QuerySound(GetPackagePath() + "sound\\ammo_pickup.wav");
		this.m_hActivate = S_QuerySound(GetPackagePath() + "sound\\ammo_activate.wav");
		this.m_tmrAnim.SetDelay(100);
		this.m_tmrAnim.Reset();
		this.m_tmrAnim.SetActive(true);
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
		this.m_tmrAnim.Update();
		if (this.m_tmrAnim.IsElapsed()) {
			this.m_tmrAnim.Reset();
			
			this.m_fRotation += 0.10;
			if (this.m_fRotation >= 6.30) {
				this.m_fRotation = 0.00;
			}
			
			this.m_fSize += 0.10;
			if (this.m_fSize >= 1.20) {
				this.m_fSize = 0.00;
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
			R_DrawSprite(this.m_hSprite, vOut, 0, this.m_fRotation, Vector(-1, -1), 1.00 + this.m_fSize, 1.00 + this.m_fSize, false, Color(0, 0, 0, 0));
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
		if ((this.m_bActive) && (ref.GetName() == "player")) {
			ICollectingEntity@ collectingEntity = cast<ICollectingEntity>(ref);
			collectingEntity.AddAmmo(this.m_szWeapon, this.m_uiSupplyCount);
			
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
		return "item_ammo";
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
