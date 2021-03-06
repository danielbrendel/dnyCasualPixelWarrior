/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

const int C_MUSIC_VOLUME_PERCENT = 90;

/* World theme entity */
class CWorldTheme : IScriptedEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Model m_oModel;
	SoundHandle m_hTheme;
	string m_szTheme;
	Timer m_tmrMusicCheck;
	bool m_bStoppedPreviously;
	
	CWorldTheme()
	{
	}
	
	//Set theme
	void SetTheme(const string &in szTheme)
	{
		this.m_szTheme = szTheme;
	}

	//Play theme
	void PlayTheme()
	{
		int sndPercent = C_MUSIC_VOLUME_PERCENT * S_GetCurrentVolume() / 100;
			
		S_PlaySound(this.m_hTheme, sndPercent, true);
	}
	
	//Called for entity spawn
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_hTheme = S_QuerySound(GetPackagePath() + "sound\\" + this.m_szTheme + ".wav");
		this.m_bStoppedPreviously = true;
		this.m_tmrMusicCheck.SetDelay(500);
		this.m_tmrMusicCheck.Reset();
		this.m_tmrMusicCheck.SetActive(true);
		this.m_oModel.Alloc();
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		S_StopSound(this.m_hTheme);
	}
	
	//Process entity stuff
	void OnProcess()
	{
		this.m_tmrMusicCheck.Update();
		if (this.m_tmrMusicCheck.IsElapsed()) {
			this.m_tmrMusicCheck.Reset();

			if (!CVar_GetBool("snd_playmusic", false)) {
				S_StopSound(this.m_hTheme);
				this.m_bStoppedPreviously = true;
			} else {
				if (this.m_bStoppedPreviously) {
					this.PlayTheme();
					this.m_bStoppedPreviously = false;
				}
			}
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
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
		return false;
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
		return "world_theme";
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
	CWorldTheme @worldTheme = CWorldTheme();
	worldTheme.SetTheme(Props_ExtractValue(szProps, "theme"));
	Ent_SpawnEntity(szIdent, @worldTheme, vecPos);
}