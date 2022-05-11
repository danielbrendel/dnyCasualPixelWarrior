/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* World shop entity */
class CWorldShop : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
    SpriteHandle m_hShop;
	Timer m_tmrShowMenuAgain;
	bool m_bAgainOnce;
	
	CWorldShop()
	{
        this.m_vecSize = Vector(400, 300);
		this.m_bAgainOnce = false;
	}
	
	//Called for entity spawn
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;

        this.m_hShop = R_LoadSprite(GetPackagePath() + "gfx\\shop.png", 1, 165, 262, 1, false);

		this.m_tmrShowMenuAgain.SetDelay(5000);
		this.m_tmrShowMenuAgain.Reset();
		this.m_tmrShowMenuAgain.SetActive(true);

        BoundingBox bbox;
        bbox.Alloc();
        bbox.AddBBoxItem(Vector(-150, -150), Vector(150, 250));
		this.m_oModel.Alloc();
        this.m_oModel.Initialize2(bbox, this.m_hShop);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrShowMenuAgain.Update();
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
        if (!R_ShouldDraw(this.m_vecPos, this.m_vecSize))
			return;
			
		Vector vOut;
		R_GetDrawingPosition(this.m_vecPos, this.m_vecSize, vOut);

        R_DrawSprite(this.m_hShop, vOut, 0, 0.0, Vector(-1, -1), 0.0f, 0.0f, false, Color(0, 0, 0, 0));
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
			if (!this.m_bAgainOnce) {
				this.m_bAgainOnce = true;
				
				if (!CVar_GetBool("show_shop_menu", false)) {
					CVar_SetBool("show_shop_menu", true);
				}
			} else {
				if (this.m_tmrShowMenuAgain.IsElapsed()) {
					this.m_tmrShowMenuAgain.Reset();
					
					if (!CVar_GetBool("show_shop_menu", false)) {
						CVar_SetBool("show_shop_menu", true);
					}
				}
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
		return "world_shop";
	}
	
	//This vector is used for drawing the selection box
	Vector& GetSize()
	{
		return this.m_vecPos;
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{
		return "";
	}
}

//Create the associated entity here
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CWorldShop @ws = CWorldShop();
    Ent_SpawnEntity(szIdent, @ws, vecPos);
}