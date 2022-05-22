/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

string g_szPackagePath = "";

#include "weapon_laser.as"
#include "weapon_laserball.as"
#include "weapon_flame.as"
#include "weapon_gun.as"
#include "weapon_grenade.as"
#include "weapon_missile.as"
#include "explosion.as"
#include "tankcls.as"
#include "infomenu.as"
#include "mapselectmenu.as"
#include "waveinfomenu.as"
#include "shopmenu.as"

/* Player animation manager */
class CPlayerAnimation {
	array<SpriteHandle> m_arrSprites;
	Timer m_tmrSwitch;
	int m_iCurrentIndex;
	Vector m_vecPos;
	float m_fRotation;
	bool m_bDrawCustomColor;
	Color m_sDrawingColor;
	
	CPlayerAnimation()
	{
	}
	
	CPlayerAnimation(string file, int count, const Vector &in dims)
	{
		//Initialize component
	
		this.m_iCurrentIndex = 0;
		this.m_bDrawCustomColor = false;
	
		for (int i = 0; i < count; i++) {
			this.m_arrSprites.insertLast(R_LoadSprite(GetPackagePath() + file + formatInt(i) + ".png", 1, dims[0], dims[1], 1, true));
		}
		
		this.m_tmrSwitch.SetDelay(100);
		this.m_tmrSwitch.Reset();
		this.m_tmrSwitch.SetActive(true);
	}
	
	void Process()
	{
		//Process switching
		
		this.m_tmrSwitch.Update();
		if (this.m_tmrSwitch.IsElapsed()) {
			this.m_tmrSwitch.Reset();
			
			this.m_iCurrentIndex++;
			if (this.m_iCurrentIndex >= int(this.m_arrSprites.length())) {
				this.m_iCurrentIndex = 0;
			}
		}
	}
	
	void Draw()
	{
		//Draw current sprite
		
		R_DrawSprite(this.m_arrSprites[this.m_iCurrentIndex], this.m_vecPos, 0, this.m_fRotation, Vector(-1, -1), 0.0f, 0.0f, this.m_bDrawCustomColor, this.m_sDrawingColor);
	}
	
	void SetPosition(const Vector &in vec)
	{
		//Update current position
		
		this.m_vecPos = vec;
	}
	
	void SetRotation(float fRot)
	{
		//Update rotation
		
		this.m_fRotation = fRot;
	}
	
	void CustomDrawing(bool shallDraw, const Color& in col)
	{
		//Set custom drawing
		
		this.m_bDrawCustomColor = shallDraw;
		this.m_sDrawingColor = col;
	}
}

const int PLAYER_SPEED = 250;
const int BTN_FORWARD = (1 << 0);
const int BTN_BACKWARD = (1 << 1);
const int BTN_MOVELEFT = (1 << 2);
const int BTN_MOVERIGHT = (1 << 3);
const int BTN_TURNLEFT = (1 << 4);
const int BTN_TURNRIGHT = (1 << 5);
const int BTN_SPEED = (1 << 6);
const int BTN_ATTACK = (1 << 7);
const int BTN_THROW = (1 << 8);
const int BTN_DODGE = (1 << 9);
const int WEAPON_HANDGUN = 1;
const int WEAPON_RIFLE = 2;
const int WEAPON_SHOTGUN = 3;
const int WEAPON_FTHROWER = 4;
const int WEAPON_PLASMAGUN = 5;
const int WEAPON_RLAUNCHER = 6;
const uint GAME_COUNTER_MAX = 5;
/* Player entity manager */
class CPlayerEntity : IScriptedEntity, IPlayerEntity, ICollectingEntity
{
	Vector m_vecPos;
	Vector m_vecSize;
	Vector m_vecCursorPos;
	Model m_oModel;
	float m_fRotation;
	uint32 m_uiButtons;
	uint32 m_uiHealth;
	Timer m_tmrMayDamage;
	Timer m_tmrAttack;
	Timer m_tmrFlicker;
	Timer m_tmrSteps;
	array<SoundHandle> m_arrSteps;
	uint32 m_uiFlickerCount;
	CPlayerAnimation@ m_animIdleHandgun;
	CPlayerAnimation@ m_animMoveHandgun;
	CPlayerAnimation@ m_animShootHandgun;
	CPlayerAnimation@ m_animIdleRifle;
	CPlayerAnimation@ m_animMoveRifle;
	CPlayerAnimation@ m_animShootRifle;
	CPlayerAnimation@ m_animIdleShotgun;
	CPlayerAnimation@ m_animMoveShotgun;
	CPlayerAnimation@ m_animShootShotgun;
	bool m_bMoving;
	bool m_bShooting;
	int m_iCurrentWeapon;
	SpriteHandle m_hSprite;
	bool m_bMayThrow;
	int m_iScore;
	SpriteHandle m_hCrosshair;
	Vector m_vecCrosshair;
	Timer m_tmrDodging;
	Timer m_tmrMayDodge;
	MovementDir m_dodgeType;
	uint m_uiDodgeCounter;
	SoundHandle m_hDodge;
	Timer m_tmrShowFlare;
	SpriteHandle m_hMuzzle;
	Timer m_tmrGameCounter;
	uint m_uiGameCounter;
	Timer m_tmrGoInfo;
	FontHandle m_hGameInfoFont;
	bool m_bProcessOnce;
	CInfoMenu m_oInfoMenu;
	SpriteHandle m_hCursor;
	CMapSelectMenu m_oSelectMenu;
	CWaveInfoMenu m_oWaveInfoMenu;
	CShopMenu m_oShopMenu;
	Timer m_tmrEndurance;
	int m_iEnduranceSecs;
	bool m_bPlayTimeAchOnce;
	
	CPlayerEntity()
    {
		this.m_uiButtons = 0;
		this.m_uiHealth = 100;
		this.m_vecSize = Vector(64, 64);
		this.m_uiFlickerCount = 0;
		this.m_bMoving = false;
		this.m_bShooting = false;
		this.m_iCurrentWeapon = WEAPON_HANDGUN;
		this.m_bMayThrow = true;
		this.m_iScore = 0;
		this.m_vecCrosshair = Vector(32, 32);
		this.m_uiDodgeCounter = 0;
		this.m_bProcessOnce = false;
		this.m_iEnduranceSecs = 0;
		this.m_bPlayTimeAchOnce = false;

		CVar_Register("game_started", CVAR_TYPE_BOOL, "0");
		CVar_Register("game_playtime", CVAR_TYPE_INT, "0");
		CVar_Register("match_finished", CVAR_TYPE_BOOL, "0");

		this.m_oInfoMenu = CInfoMenu();
		this.m_oSelectMenu = CMapSelectMenu();
		this.m_oWaveInfoMenu = CWaveInfoMenu();
		this.m_oShopMenu = CShopMenu();
    }

	//Load basis dialog
	void LoadBasisDialog()
	{
		array<string> dialog1;
		dialog1.insertLast("Welcome soldier!");
		dialog1.insertLast("");
		dialog1.insertLast("I am VAD, your personal virtual assistance device!");
		dialog1.insertLast("");
		dialog1.insertLast("This is your basis. Here you can travel to far distanced");
		dialog1.insertLast("worlds to earn coins by defeating waves of enemies.");
		dialog1.insertLast("");
		dialog1.insertLast("With these earned coins you can buy upgrades via the shop.");
		dialog1.insertLast("");
		
		array<string> dialog2;
		dialog2.insertLast("The mission you have is endless: Earn more and more coins,");
		dialog2.insertLast("and invest them in upgrades. The better upgrades you have,");
		dialog2.insertLast("the longer you will last in the waves of enemies.");

		array<string> dialog3;
		dialog3.insertLast("The tower on the left is your shop.");
		dialog3.insertLast("");
		dialog3.insertLast("Use it with your earned coins to buy upgrades.");
		dialog3.insertLast("");
		dialog3.insertLast("Upgrades consist of weapons, ammo and unlocking new worlds.");

		array<string> dialog4;
		dialog4.insertLast("The portal on the right is your way to travel to other worlds.");
		dialog4.insertLast("");
		dialog4.insertLast("In these worlds you will fight endless waves of enemies.");
		dialog4.insertLast("These enemies will drop coins which you then can invest");
		dialog4.insertLast("in new upgrades!");

		this.m_oInfoMenu.AddDialog(dialog1);
		this.m_oInfoMenu.AddDialog(dialog2);
		this.m_oInfoMenu.AddDialog(dialog3);
		this.m_oInfoMenu.AddDialog(dialog4);
	}

	//Load greenland dialog
	void LoadGreenlandDialog()
	{
		array<string> dialog1;
		dialog1.insertLast("These realms are the so called greenlands.");
		dialog1.insertLast("");
		dialog1.insertLast("It is a fruitful land, with various bioms.");
		dialog1.insertLast("Altough it is currently occupied by a military force.");
		dialog1.insertLast("");
		dialog1.insertLast("You can surely gather some coins from them,");
		dialog1.insertLast("but don't forget to evade their attacks.");

		array<string> dialog2;
		dialog2.insertLast("The military force who has occupied this land");
		dialog2.insertLast("likes to have military from different centuries.");
		dialog2.insertLast("");
		dialog2.insertLast("That's why you will meet ballistas, tanks and laser mechs.");
		dialog2.insertLast("And also they seem to have some tesla towers in place.");

		array<string> dialog3;
		dialog3.insertLast("So, good luck, soldier.");
		dialog3.insertLast("");
		dialog3.insertLast("And don't worry! If you get fragged you will be");
		dialog3.insertLast("resurrected at your basis!");

		this.m_oInfoMenu.AddDialog(dialog1);
		this.m_oInfoMenu.AddDialog(dialog2);
		this.m_oInfoMenu.AddDialog(dialog3);
	}

	//Load snowland dialog
	void LoadSnowlandDialog()
	{
		array<string> dialog1;
		dialog1.insertLast("Welcome to the snowlands!");
		dialog1.insertLast("");
		dialog1.insertLast("It's very cold here, brrr. I hope you are wearing warm clothes!");
		dialog1.insertLast("");
		dialog1.insertLast("It's fascinating what diverse species are out there in the universe.");
		dialog1.insertLast("Tho fascinating and definitely beautiful, these monsters will not");
		dialog1.insertLast("hesitate to show you their power. So keep a distance while fighting");
		dialog1.insertLast("against them.");

		array<string> dialog2;
		dialog2.insertLast("There were some people who wanted to tame these monsters");
		dialog2.insertLast("and keep them as pets. Guess how much luck they had...");
		dialog2.insertLast("");
		dialog2.insertLast("So, don't underestimate their power and beware that some are");
		dialog2.insertLast("close and some are far range attacking monsters.");

		array<string> dialog3;
		dialog3.insertLast("Good luck, soldier!");

		this.m_oInfoMenu.AddDialog(dialog1);
		this.m_oInfoMenu.AddDialog(dialog2);
		this.m_oInfoMenu.AddDialog(dialog3);
	}

	//Load wasteland dialog
	void LoadWastelandDialog()
	{
		array<string> dialog1;
		dialog1.insertLast("The wastelands! Another realm that is occupied by another");
		dialog1.insertLast("military force.");
		dialog1.insertLast("");
		dialog1.insertLast("These guys have caused so much trouble in the universe,");
		dialog1.insertLast("so that we surely can grab some coins from them, you know...");

		array<string> dialog2;
		dialog2.insertLast("I hope you have enough weaponry equipped, because these");
		dialog2.insertLast("guys do serious business.");
		dialog2.insertLast("So be sure to avoid their attacks as much as possible!");
		dialog2.insertLast("");
		dialog2.insertLast("Best of luck, soldier!");

		this.m_oInfoMenu.AddDialog(dialog1);
		this.m_oInfoMenu.AddDialog(dialog2);
	}

	//Load lavaland dialog
	void LoadLavalandDialog()
	{
		array<string> dialog1;
		dialog1.insertLast("The lavalands - an unpleasant environment.");
		dialog1.insertLast("");
		dialog1.insertLast("There is not much lifeform here, except one species:");
		dialog1.insertLast("");
		dialog1.insertLast("The aliens! They have occupied these lands and use it");
		dialog1.insertLast("to build their motherships.");

		array<string> dialog2;
		dialog2.insertLast("These aliens are actually very rich, so we can");
		dialog2.insertLast("get maaaaaany coins from them.");
		dialog2.insertLast("");
		dialog2.insertLast("But beware! They are really strong and heavily armed.");
		dialog2.insertLast("You will need to have a very good equippment to last");
		dialog2.insertLast("long enough to earn a sufficient amount of coins.");

		array<string> dialog3;
		dialog3.insertLast("I hope you got some nice equippment from the shop.");
		dialog3.insertLast("");
		dialog3.insertLast("");
		dialog3.insertLast("");
		dialog3.insertLast("Best of luck, soldier!");

		this.m_oInfoMenu.AddDialog(dialog1);
		this.m_oInfoMenu.AddDialog(dialog2);
		this.m_oInfoMenu.AddDialog(dialog3);
	}
	
	//Aim at screen view position
	void AimAtScreenPoint(const Vector &in vecPos)
	{
		Vector vecScreenPos = Vector(Wnd_GetWindowCenterX(), Wnd_GetWindowCenterY());
		vecScreenPos[0] = vecPos[0] - vecScreenPos[0];
		vecScreenPos[1] = vecPos[1] - vecScreenPos[1];
		
		Vector vecWorldPos = Vector(this.m_vecPos[0] + vecScreenPos[0], this.m_vecPos[1] + vecScreenPos[1]);
	
		float flAngle = atan2(float(vecWorldPos[1] - this.m_vecPos[1]), float(vecWorldPos[0] - this.m_vecPos[0]));
		this.SetRotation(flAngle + 6.30 / 1.36);
	}
	
	//Called when the entity gets spawned. The position in the map is passed as argument
	void OnSpawn(const Vector& in vec)
	{
		this.m_vecPos = vec;
		this.m_fRotation = 0.0f;
		this.m_hSprite = R_LoadSprite(GetPackagePath() + "gfx\\player\\handgun\\idle\\survivor-idle_handgun_0.png", 1, 64, 64, 1, true);
		@this.m_animIdleHandgun = CPlayerAnimation("gfx\\player\\handgun\\idle\\survivor-idle_handgun_", 20, Vector(64, 64));
		@this.m_animMoveHandgun = CPlayerAnimation("gfx\\player\\handgun\\move\\survivor-move_handgun_", 20, Vector(64, 64));
		@this.m_animShootHandgun = CPlayerAnimation("gfx\\player\\handgun\\shoot\\survivor-shoot_handgun_", 3, Vector(64, 64));
		@this.m_animIdleRifle = CPlayerAnimation("gfx\\player\\rifle\\idle\\survivor-idle_rifle_", 20, Vector(64, 64));
		@this.m_animMoveRifle = CPlayerAnimation("gfx\\player\\rifle\\move\\survivor-move_rifle_", 20, Vector(64, 64));
		@this.m_animShootRifle = CPlayerAnimation("gfx\\player\\rifle\\shoot\\survivor-shoot_rifle_", 3, Vector(64, 64));
		@this.m_animIdleShotgun = CPlayerAnimation("gfx\\player\\shotgun\\idle\\survivor-idle_shotgun_", 20, Vector(64, 64));
		@this.m_animMoveShotgun = CPlayerAnimation("gfx\\player\\shotgun\\move\\survivor-move_shotgun_", 20, Vector(64, 64));
		@this.m_animShootShotgun = CPlayerAnimation("gfx\\player\\shotgun\\shoot\\survivor-shoot_shotgun_", 3, Vector(64, 64));
		for (int i = 1; i < 9; i++) {
			this.m_arrSteps.insertLast(S_QuerySound(GetPackagePath() + "sound\\steps\\stepdirt_" + formatInt(i) + ".wav"));
		}
		this.m_hMuzzle = R_LoadSprite(GetPackagePath() + "gfx\\muzzle_turned.png", 1, 256, 256, 1, false);
		this.m_hCursor = R_LoadSprite(GetPackagePath() + "gfx\\menucursor.png", 1, 16, 16, 1, false);
		this.m_hDodge = S_QuerySound(GetPackagePath() + "sound\\swoosh.wav");
		this.m_hGameInfoFont = R_LoadFont("Verdana", 21, 45);
		this.m_tmrMayDamage.SetDelay(2000);
		this.m_tmrMayDamage.Reset();
		this.m_tmrMayDamage.SetActive(true);
		this.m_tmrAttack.SetDelay(400);
		this.m_tmrAttack.Reset();
		this.m_tmrAttack.SetActive(true);
		this.m_tmrFlicker.SetDelay(250);
		this.m_tmrFlicker.Reset();
		this.m_tmrFlicker.SetActive(false);
		this.m_tmrSteps.SetDelay(500);
		this.m_tmrSteps.Reset();
		this.m_tmrSteps.SetActive(false);
		this.m_tmrDodging.SetDelay(10);
		this.m_tmrDodging.Reset();
		this.m_tmrDodging.SetActive(false);
		this.m_tmrMayDodge.SetDelay(1650);
		this.m_tmrMayDodge.Reset();
		this.m_tmrMayDodge.SetActive(true);
		this.m_tmrShowFlare.SetDelay(50);
		this.m_tmrShowFlare.Reset();
		this.m_tmrShowFlare.SetActive(false);
		this.m_tmrGameCounter.SetDelay(1000);
		this.m_tmrGameCounter.Reset();
		this.m_tmrGameCounter.SetActive(true);
		this.m_tmrGoInfo.SetDelay(1500);
		this.m_tmrEndurance.SetDelay(1000);
		this.m_tmrEndurance.Reset();
		this.m_tmrEndurance.SetActive(true);
		CVar_SetBool("game_started", false);
		CVar_SetInt("game_playtime", 0);
		CVar_SetBool("match_finished", false);
		BoundingBox bbox;
		bbox.Alloc();
		bbox.AddBBoxItem(Vector(0, 0), this.m_vecSize);
		this.m_oModel.Alloc();
		this.m_oModel.SetCenter(Vector(32, 32));
		this.m_oModel.Initialize2(bbox, this.m_hSprite);
	}
	
	//Called when the entity gets released
	void OnRelease()
	{
		int coins = HUD_GetCollectableCount("coins");
		int basisHint = (CVar_GetBool("basis_hint", false)) ? 1 : 0;
		int snowlandUnlocked = (CVar_GetBool("snowland_unlocked", false)) ? 1 : 0;
		int wastelandUnlocked = (CVar_GetBool("wasteland_unlocked", false)) ? 1 : 0;
		int lavalandUnlocked = (CVar_GetBool("lavaland_unlocked", false)) ? 1 : 0;
		int shotgunUnlocked = (CVar_GetBool("weapon_shotgun", false)) ? 1 : 0;
		int lasergunUnlocked = (CVar_GetBool("weapon_laser", false)) ? 1 : 0;
		int fthrowerUnlocked = (CVar_GetBool("weapon_fthrower", false)) ? 1 : 0;
		int plasmagunUnlocked = (CVar_GetBool("weapon_plasma", false)) ? 1 : 0;
		int rlauncherUnlocked = (CVar_GetBool("weapon_rlauncher", false)) ? 1 : 0;

		Props_SaveToFile("coins:" + formatInt(coins) + ";" + 
			"basishint:" + formatInt(basisHint) + ";" + 
			"snowland:" + formatInt(snowlandUnlocked) + ";" + 
			"wasteland:" + formatInt(wastelandUnlocked) + ";" + 
			"lavaland:" + formatInt(lavalandUnlocked) + ";" +
			"lasergun:" + formatInt(lasergunUnlocked) + ";" +
			"shotgun:" + formatInt(shotgunUnlocked) + ";" +
			"fthrower:" + formatInt(fthrowerUnlocked) + ";" +
			"plasmagun:" + formatInt(plasmagunUnlocked) + ";" +
			"rlauncher:" + formatInt(rlauncherUnlocked) + ";"
		, "player.props");
	}
	
	//Process entity stuff
	void OnProcess()
	{
		//First call processings
		if (!this.m_bProcessOnce) {
			this.m_bProcessOnce = true;

			if (GetCurrentMap() == "basis.cfg") {
				this.LoadBasisDialog();
			} else if (GetCurrentMap() == "greenland.cfg") {
				this.LoadGreenlandDialog();
			} else if (GetCurrentMap() == "snowland.cfg") {
				this.LoadSnowlandDialog();
			} else if (GetCurrentMap() == "wasteland.cfg") {
				this.LoadWastelandDialog();
			} else if (GetCurrentMap() == "lavaland.cfg") {
				this.LoadLavalandDialog();
			}

			this.m_oInfoMenu.SetPosition(Vector(Wnd_GetWindowCenterX() - 250, Wnd_GetWindowCenterY() - 250));
			this.m_oSelectMenu.SetPosition(Vector(Wnd_GetWindowCenterX() - 250, Wnd_GetWindowCenterY() - 250));
			this.m_oWaveInfoMenu.SetPosition(Vector(Wnd_GetWindowCenterX() - 250, Wnd_GetWindowCenterY() - 250));
			this.m_oShopMenu.SetPosition(Vector(Wnd_GetWindowCenterX() - 250, Wnd_GetWindowCenterY() - 250));

			if (GetCurrentMap() == "basis.cfg") {
				if (CVar_GetBool("basis_hint", true)) {
					this.m_oInfoMenu.Start();
					CVar_SetBool("basis_hint", false);
				}
			} else {
				this.m_oInfoMenu.Start();
			}
			
			if (GetCurrentMap() == "basis.cfg") {
				this.m_oSelectMenu.AddMap("greenland", "Green meadow lands", true);
				this.m_oSelectMenu.AddMap("snowland", "The snowy frozen neverlands", CVar_GetBool("snowland_unlocked", false));
				this.m_oSelectMenu.AddMap("wasteland", "The intoxicated wastelands", CVar_GetBool("wasteland_unlocked", false));
				this.m_oSelectMenu.AddMap("lavaland", "A very unpleasant environment", CVar_GetBool("lavaland_unlocked", false));

				this.m_uiGameCounter = GAME_COUNTER_MAX;
				this.m_tmrGameCounter.SetActive(false);
				this.m_tmrGoInfo.SetActive(false);
			}

			if (GetCurrentMap() == "snowland.cfg") {
				this.m_hCrosshair = R_LoadSprite(GetPackagePath() + "gfx\\crosshair_red.png", 1, this.m_vecCrosshair[0], this.m_vecCrosshair[1], 1, false);
			} else {
				this.m_hCrosshair = R_LoadSprite(GetPackagePath() + "gfx\\crosshair.png", 1, this.m_vecCrosshair[0], this.m_vecCrosshair[1], 1, false);
			}
		}

		//Process game counter
		if (!this.m_oInfoMenu.IsActive()) {
			if (this.m_tmrGameCounter.IsActive()) {
				this.m_tmrGameCounter.Update();
				if (this.m_tmrGameCounter.IsElapsed()) {
					this.m_uiGameCounter++;
					if (this.m_uiGameCounter >= GAME_COUNTER_MAX) {
						this.m_tmrGameCounter.SetActive(false);
						this.m_tmrGoInfo.Reset();
						this.m_tmrGoInfo.SetActive(true);
						this.m_iEnduranceSecs = 0;
					}
				}
			}
		}

		this.m_oInfoMenu.Process();
		this.m_oSelectMenu.Process();
		this.m_oWaveInfoMenu.Process();
		this.m_oShopMenu.Process();

		if (this.m_uiGameCounter < GAME_COUNTER_MAX) {
			return;
		}

		if (this.m_tmrGoInfo.IsActive()) {
			this.m_tmrGoInfo.Update();
			if (this.m_tmrGoInfo.IsElapsed()) {
				this.m_tmrGoInfo.SetActive(false);
				CVar_SetBool("game_started", true);
			}
		}

		//Endurance calculation
		if ((GetCurrentMap() != "basis.cfg") && (!this.m_oWaveInfoMenu.IsActive())) {
			this.m_tmrEndurance.Update();
			if (this.m_tmrEndurance.IsElapsed()) {
				this.m_tmrEndurance.Reset();
				this.m_iEnduranceSecs++;
				CVar_SetInt("game_playtime", this.m_iEnduranceSecs);
			}
		}

		//Process movement

		this.m_bMoving = false;
		this.m_bShooting = false;

		//Handle button flags

		if ((this.m_uiButtons & BTN_FORWARD) == BTN_FORWARD) {
			Ent_Move(this, PLAYER_SPEED, MOVE_NORTH);
			this.m_bMoving = true;
		} 
		
		if ((this.m_uiButtons & BTN_BACKWARD) == BTN_BACKWARD) {
			Ent_Move(this, PLAYER_SPEED, MOVE_SOUTH);
			this.m_bMoving = true;
		} 
		
		if ((this.m_uiButtons & BTN_MOVELEFT) == BTN_MOVELEFT) {
			float fSpeed;

			/*if ((this.m_fRotation > 4.725f) || (this.m_fRotation < 1.575f)) {
				fSpeed = -PLAYER_SPEED;
			} else {
				fSpeed = PLAYER_SPEED;
			}*/
			
			Ent_Move(this, PLAYER_SPEED, MOVE_WEST);

			this.m_bMoving = true;
		}
		
		if ((this.m_uiButtons & BTN_MOVERIGHT) == BTN_MOVERIGHT) {
			float fSpeed;
			
			/*if ((this.m_fRotation > 4.725f) || (this.m_fRotation < 1.575f)) {
				fSpeed = -PLAYER_SPEED;
			} else {
				fSpeed = PLAYER_SPEED;
			}*/
			
			Ent_Move(this, PLAYER_SPEED, MOVE_EAST);

			this.m_bMoving = true;
		}

		if ((this.m_uiButtons & BTN_TURNLEFT) == BTN_TURNLEFT) {
			this.m_fRotation += 0.05f;
		} 

		if ((this.m_uiButtons & BTN_TURNRIGHT) == BTN_TURNRIGHT) {
			this.m_fRotation -= 0.05f;
		}
		
		//Activate or deactivate step sound timer depending on movement status
		if (this.m_bMoving) {
			if (!this.m_tmrSteps.IsActive()) {
				this.m_tmrSteps.Reset();
				this.m_tmrSteps.SetActive(true);
			}
		} else {
			if (this.m_tmrSteps.IsActive()) {
				this.m_tmrSteps.SetActive(false);
			}
		}
		
		HUD_UpdateHealth(this.m_uiHealth);

		//Process attacking
		if (((this.m_uiButtons & BTN_ATTACK) == BTN_ATTACK) && (GetCurrentMap() != "basis.cfg")) {
			this.m_bShooting = true;
			this.m_tmrAttack.Update();
			if (this.m_tmrAttack.IsElapsed()) {
				this.m_tmrAttack.Reset();
				
				Vector vecBulletPos = this.m_vecPos;
				vecBulletPos[0] += int(sin(this.GetRotation()) * 50);
				vecBulletPos[1] -= int(cos(this.GetRotation()) * 50);
				vecBulletPos[0] -= int(sin(this.GetRotation() + 80.0) * 20);
				vecBulletPos[1] += int(cos(this.GetRotation() + 80.0) * 20);
				
				if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
					if (HUD_GetAmmoItemCurrent("handgun") > 0) {
						CGunEntity @gun = CGunEntity();
						
						gun.SetRotation(this.GetRotation());
						gun.SetOwner(@this);
						
						Ent_SpawnEntity("weapon_gun", @gun, vecBulletPos);
						
						HUD_UpdateAmmoItem("handgun", HUD_GetAmmoItemCurrent("handgun") - 1, HUD_GetAmmoItemMax("handgun"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\handgun.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						this.m_tmrShowFlare.Reset();
						this.m_tmrShowFlare.SetActive(true);

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					} else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				} else if (this.m_iCurrentWeapon == WEAPON_RIFLE) {
					if (HUD_GetAmmoItemCurrent("laser") > 0) {
						CLaserEntity @laser = CLaserEntity();

						laser.SetRotation(this.GetRotation());
						laser.SetOwner(@this);
						
						Ent_SpawnEntity("weapon_laser", @laser, vecBulletPos);
						
						HUD_UpdateAmmoItem("laser", HUD_GetAmmoItemCurrent("laser") - 1, HUD_GetAmmoItemMax("laser"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\laser.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					}  else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
					if (HUD_GetAmmoItemCurrent("shotgun") > 0) {
						for (int i = 0; i < 3; i++) {
							CGunEntity @gun = CGunEntity();
						
							float fGunRot = this.GetRotation();
							
							if (i == 0) {
								fGunRot -= 0.2;
							} else if (i == 2) {
								fGunRot += 0.2;
							}
							
							gun.SetRotation(fGunRot);
							gun.SetOwner(@this);
							
							Ent_SpawnEntity("weapon_gun", @gun, vecBulletPos);
						}
						
						HUD_UpdateAmmoItem("shotgun", HUD_GetAmmoItemCurrent("shotgun") - 1, HUD_GetAmmoItemMax("shotgun"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\shotgun.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						this.m_tmrShowFlare.Reset();
						this.m_tmrShowFlare.SetActive(true);

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					} else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				} else if (this.m_iCurrentWeapon == WEAPON_FTHROWER) {
					if (HUD_GetAmmoItemCurrent("fthrower") > 0) {
						CFlameEntity @flame = CFlameEntity();

						flame.SetDirection(this.GetRotation());
						flame.SetOwner(@this);
						
						Ent_SpawnEntity("weapon_flame", @flame, vecBulletPos);
						
						HUD_UpdateAmmoItem("fthrower", HUD_GetAmmoItemCurrent("fthrower") - 1, HUD_GetAmmoItemMax("fthrower"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\flame_shot.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					}  else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				} else if (this.m_iCurrentWeapon == WEAPON_PLASMAGUN) {
					if (HUD_GetAmmoItemCurrent("plasma") > 0) {
						for (int i = 0; i < 3; i++) {
							CLaserBallEntity@ ball = CLaserBallEntity();

							float fBallRot = this.GetRotation();
											
							if (i == 0) {
								fBallRot -= 0.2;
							} else if (i == 2) {
								fBallRot += 0.2;
							}

							ball.SetRotation(fBallRot);
							ball.SetOwner(@this);

							Vector vecBulletPos = Vector(this.m_vecPos[0], this.m_vecPos[1]);

							Ent_SpawnEntity("weapon_laserball", ball, vecBulletPos);
						}
						
						HUD_UpdateAmmoItem("plasma", HUD_GetAmmoItemCurrent("plasma") - 1, HUD_GetAmmoItemMax("plasma"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\laser.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					}  else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				} else if (this.m_iCurrentWeapon == WEAPON_RLAUNCHER) {
					if (HUD_GetAmmoItemCurrent("rlauncher") > 0) {
						CMissileEntity@ missile = CMissileEntity();

						missile.SetRotation(this.GetRotation());
						missile.SetOwner(@this);

						Vector vecBulletPos = Vector(this.m_vecPos[0], this.m_vecPos[1]);

						Ent_SpawnEntity("weapon_missile", missile, vecBulletPos);
						
						HUD_UpdateAmmoItem("rlauncher", HUD_GetAmmoItemCurrent("rlauncher") - 1, HUD_GetAmmoItemMax("rlauncher"));
						
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\missile_launch.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());

						CVar_SetInt("shots_fired", CVar_GetInt("shots_fired", 0) + 1);
					}  else {
						SoundHandle hSound = S_QuerySound(g_szPackagePath + "sound\\empty.wav");
						S_PlaySound(hSound, S_GetCurrentVolume());
					}
				}
			}
		}
		
		//Process throwing
		if (((this.m_uiButtons & BTN_THROW) == BTN_THROW) && (GetCurrentMap() != "basis.cfg")) {
			if (this.m_bMayThrow) {
				this.m_bMayThrow = false;
				
				if (HUD_GetCollectableCount("grenade") > 0) {
					CGrenadeEntity @grenade = CGrenadeEntity();
					grenade.SetOwner(this);
					grenade.SetRotation(this.GetRotation());
					
					Ent_SpawnEntity("weapon_grenade", @grenade, this.m_vecPos);
					
					HUD_UpdateCollectable("grenade", HUD_GetCollectableCount("grenade") - 1);

					CVar_SetInt("grenades_thrown", CVar_GetInt("grenades_thrown", 0) + 1);
				}
			}
		}

		//Proess dodging

		if ((this.m_uiButtons & BTN_DODGE) == BTN_DODGE) {
			if ((this.m_uiButtons & BTN_FORWARD) == BTN_FORWARD) {
				if (this.m_tmrMayDodge.IsElapsed()) {
					this.m_tmrDodging.Reset();
					this.m_tmrDodging.SetActive(true);
					this.m_tmrMayDodge.Reset();
					this.m_dodgeType = MOVE_NORTH;
					S_PlaySound(this.m_hDodge, S_GetCurrentVolume());
				}
			} else if ((this.m_uiButtons & BTN_BACKWARD) == BTN_BACKWARD) {
				if (this.m_tmrMayDodge.IsElapsed()) {
					this.m_tmrDodging.Reset();
					this.m_tmrDodging.SetActive(true);
					this.m_tmrMayDodge.Reset();
					this.m_dodgeType = MOVE_SOUTH;
					S_PlaySound(this.m_hDodge, S_GetCurrentVolume());
				}
			} else if ((this.m_uiButtons & BTN_MOVELEFT) == BTN_MOVELEFT) {
				if (this.m_tmrMayDodge.IsElapsed()) {
					this.m_tmrDodging.Reset();
					this.m_tmrDodging.SetActive(true);
					this.m_tmrMayDodge.Reset();
					this.m_dodgeType = MOVE_WEST;
					S_PlaySound(this.m_hDodge, S_GetCurrentVolume());
				}
			} else if ((this.m_uiButtons & BTN_MOVERIGHT) == BTN_MOVERIGHT) {
				if (this.m_tmrMayDodge.IsElapsed()) {
					this.m_tmrDodging.Reset();
					this.m_tmrDodging.SetActive(true);
					this.m_tmrMayDodge.Reset();
					this.m_dodgeType = MOVE_EAST;
					S_PlaySound(this.m_hDodge, S_GetCurrentVolume());
				}
			}

			this.m_uiButtons &= ~BTN_DODGE;
		}

		if (this.m_tmrMayDodge.IsActive()) {
			this.m_tmrMayDodge.Update();
		}

		if (this.m_tmrDodging.IsActive()) {
			this.m_tmrDodging.Update();
			if (this.m_tmrDodging.IsElapsed()) {
				Ent_Move(this, PLAYER_SPEED * 3, this.m_dodgeType);
				this.m_uiDodgeCounter++;
				if (this.m_uiDodgeCounter >= 5) {
					this.m_tmrDodging.SetActive(false);
					this.m_uiDodgeCounter = 0;
					CVar_SetInt("dodges_count", CVar_GetInt("dodges_count", 0) + 1);
				}
			}
		}
		
		//Update damage permission timer
		this.m_tmrMayDamage.Update();
		
		//Process flickering
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
		
		//Process step sound handling
		if (this.m_tmrSteps.IsActive()) {
			this.m_tmrSteps.Update();
			if (this.m_tmrSteps.IsElapsed()) {
				this.m_tmrSteps.Reset();
				
				S_PlaySound(this.m_arrSteps[Util_Random(1, 8)], S_GetCurrentVolume());
			}
		}
		
		//Process animation
		if (this.m_bShooting) {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animShootHandgun.Process();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN)) {
				this.m_animShootRifle.Process();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animShootShotgun.Process();
			}
		} else if (this.m_bMoving) {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animMoveHandgun.Process();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN)) {
				this.m_animMoveRifle.Process();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animMoveShotgun.Process();
			}
		} else {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animIdleHandgun.Process();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN)) {
				this.m_animIdleRifle.Process();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animIdleShotgun.Process();
			}
		}

		//Muzzle flare
		if (this.m_tmrShowFlare.IsActive()) {
			this.m_tmrShowFlare.Update();
			if (this.m_tmrShowFlare.IsElapsed()) {
				this.m_tmrShowFlare.SetActive(false);
			}
		}

		//Map selection menu
		if (CVar_GetBool("show_mapsel_menu", false)) {
			this.m_oSelectMenu.Start();
			CVar_SetBool("show_mapsel_menu", false);
			this.m_uiButtons = 0;
		}

		//Shop menu
		if (CVar_GetBool("show_shop_menu", false)) {
			this.m_oShopMenu.Start();
			CVar_SetBool("show_shop_menu", false);
			this.m_uiButtons = 0;
		}

		//Enter world if requested
		string szShallLoadMap = CVar_GetString("mapsel_enter_world", "");
		if (szShallLoadMap != "") {
			CVar_SetString("mapsel_enter_world", "");
			LoadMap(szShallLoadMap);
		}

		//Show wave info menu
		if (this.m_uiHealth == 0) {
			this.m_uiButtons = 0;
			this.m_oWaveInfoMenu.Start();

			if (!this.m_bPlayTimeAchOnce) {
				this.m_bPlayTimeAchOnce = true;

				CVar_SetBool("match_finished", true);

				int iMins = CVar_GetInt("game_playtime", 0) / 60;

				if ((iMins >= 15) && (iMins < 30)) {
					if (GetCurrentMap() == "greenland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_15_GREENLAND")) {
							Steam_SetAchievement("ENDURED_15_GREENLAND");
						}
					} else if (GetCurrentMap() == "snowland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_15_SNOWLAND")) {
							Steam_SetAchievement("ENDURED_15_SNOWLAND");
						}
					} else if (GetCurrentMap() == "wasteland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_15_WASTELAND")) {
							Steam_SetAchievement("ENDURED_15_WASTELAND");
						}
					} else if (GetCurrentMap() == "lavaland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_15_LAVALAND")) {
							Steam_SetAchievement("ENDURED_15_LAVALAND");
						}
					}
				} else if ((iMins >= 30) && (iMins < 60)) {
					if (GetCurrentMap() == "greenland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_30_GREENLAND")) {
							Steam_SetAchievement("ENDURED_30_GREENLAND");
						}
					} else if (GetCurrentMap() == "snowland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_30_SNOWLAND")) {
							Steam_SetAchievement("ENDURED_30_SNOWLAND");
						}
					} else if (GetCurrentMap() == "wasteland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_30_WASTELAND")) {
							Steam_SetAchievement("ENDURED_30_WASTELAND");
						}
					} else if (GetCurrentMap() == "lavaland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_30_LAVALAND")) {
							Steam_SetAchievement("ENDURED_30_LAVALAND");
						}
					}
				} else if (iMins >= 60) {
					if (GetCurrentMap() == "greenland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_60_GREENLAND")) {
							Steam_SetAchievement("ENDURED_60_GREENLAND");
						}
					} else if (GetCurrentMap() == "snowland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_60_SNOWLAND")) {
							Steam_SetAchievement("ENDURED_60_SNOWLAND");
						}
					} else if (GetCurrentMap() == "wasteland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_60_WASTELAND")) {
							Steam_SetAchievement("ENDURED_60_WASTELAND");
						}
					} else if (GetCurrentMap() == "lavaland.cfg") {
						if (!Steam_IsAchievementUnlocked("ENDURED_60_LAVALAND")) {
							Steam_SetAchievement("ENDURED_60_LAVALAND");
						}
					}
				}
				
			}
		}

		//Process shop commands
		string szShopCmd = CVar_GetString("shop_command", "");
		if (szShopCmd.length() > 0) {
			string szUnlock = Props_ExtractValue(szShopCmd, "unlock");
			if (szUnlock.length() > 0) {
				this.m_oSelectMenu.UnlockMap(szUnlock);
			}

			CVar_SetString("shop_command", "");
		}
	}
	
	//Entity can draw everything in default order here
	void OnDraw()
	{
	}
	
	//Entity can draw on-top stuff here
	void OnDrawOnTop()
	{
		bool bDrawCustomColor = false;
		
		if ((this.m_tmrFlicker.IsActive()) && (this.m_uiFlickerCount % 2 == 0)) {
			bDrawCustomColor = true;
		}
		
		Color sDrawingColor = (this.m_tmrFlicker.IsActive()) ? Color(255, 0, 0, 150) : Color(0, 0, 0, 0);
		
		if (this.m_bShooting) {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animShootHandgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animShootHandgun.SetRotation(this.m_fRotation);
				this.m_animShootHandgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animShootHandgun.Draw();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN) || (this.m_iCurrentWeapon == WEAPON_RLAUNCHER)) {
				this.m_animShootRifle.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animShootRifle.SetRotation(this.m_fRotation);
				this.m_animShootRifle.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animShootRifle.Draw();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animShootShotgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animShootShotgun.SetRotation(this.m_fRotation);
				this.m_animShootShotgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animShootShotgun.Draw();
			}
		} else if (this.m_bMoving) {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animMoveHandgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animMoveHandgun.SetRotation(this.m_fRotation);
				this.m_animMoveHandgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animMoveHandgun.Draw();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN) || (this.m_iCurrentWeapon == WEAPON_RLAUNCHER)) {
				this.m_animMoveRifle.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animMoveRifle.SetRotation(this.m_fRotation);
				this.m_animMoveRifle.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animMoveRifle.Draw();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animMoveShotgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animMoveShotgun.SetRotation(this.m_fRotation);
				this.m_animMoveShotgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animMoveShotgun.Draw();
			}
		} else {
			if (this.m_iCurrentWeapon == WEAPON_HANDGUN) {
				this.m_animIdleHandgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animIdleHandgun.SetRotation(this.m_fRotation);
				this.m_animIdleHandgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animIdleHandgun.Draw();
			} else if ((this.m_iCurrentWeapon == WEAPON_RIFLE) || (this.m_iCurrentWeapon == WEAPON_FTHROWER) || (this.m_iCurrentWeapon == WEAPON_PLASMAGUN) || (this.m_iCurrentWeapon == WEAPON_RLAUNCHER)) {
				this.m_animIdleRifle.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animIdleRifle.SetRotation(this.m_fRotation);
				this.m_animIdleRifle.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animIdleRifle.Draw();
			} else if (this.m_iCurrentWeapon == WEAPON_SHOTGUN) {
				this.m_animIdleShotgun.SetPosition(Vector(Wnd_GetWindowCenterX() - 64 / 2, Wnd_GetWindowCenterY() - 64 / 2));
				this.m_animIdleShotgun.SetRotation(this.m_fRotation);
				this.m_animIdleShotgun.CustomDrawing(bDrawCustomColor, sDrawingColor);
				this.m_animIdleShotgun.Draw();
			}
		}
		
		if ((this.m_tmrShowFlare.IsActive()) && (!this.m_tmrShowFlare.IsElapsed())) {
			Vector vecForward = Vector(Wnd_GetWindowCenterX() - 128, Wnd_GetWindowCenterY() - 128);
			vecForward[0] += int(sin(this.GetRotation()) * 92);
			vecForward[1] -= int(cos(this.GetRotation()) * 92);

			vecForward[0] -= int(sin(this.GetRotation() + 80.0) * 20);
			vecForward[1] += int(cos(this.GetRotation() + 80.0) * 20);

			R_DrawSprite(this.m_hMuzzle, vecForward, 0, this.m_fRotation, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		}

		if (this.m_tmrGameCounter.IsActive()) {
			R_DrawString(this.m_hGameInfoFont, formatInt(GAME_COUNTER_MAX - this.m_uiGameCounter), Vector(Wnd_GetWindowCenterX() - 10, Wnd_GetWindowCenterY() - 100), Color(100, 0, 0, 255));
		} else if (this.m_tmrGoInfo.IsActive()) {
			R_DrawString(this.m_hGameInfoFont, _("app.go", "GO!"), Vector(Wnd_GetWindowCenterX() - 30, Wnd_GetWindowCenterY() - 100), Color(100, 0, 0, 255));
		}

		if (GetCurrentMap() != "basis.cfg") {
			int iPlayTime = CVar_GetInt("game_playtime", 0);
			int iPlayTimeHours = iPlayTime / 60 / 60;
			int iPlayTimeMins = iPlayTime / 60;
			int iPlayTimeSecs = iPlayTime % 60;
			string szPlayTimeHours = formatInt(iPlayTimeHours);
			string szPlayTimeMins = formatInt(iPlayTimeMins);
			string szPlayTimeSecs = formatInt(iPlayTimeSecs);
			if (iPlayTimeHours < 10) { szPlayTimeHours = "0" + szPlayTimeHours; }
			if (iPlayTimeMins < 10) { szPlayTimeMins = "0" + szPlayTimeMins; }
			if (iPlayTimeSecs < 10) { szPlayTimeSecs = "0" + szPlayTimeSecs; }
			string szPlayTimeInfo = szPlayTimeHours + ":" + szPlayTimeMins + ":" + szPlayTimeSecs;
			R_DrawString(R_GetDefaultFont(), szPlayTimeHours + ":" + szPlayTimeMins + ":" + szPlayTimeSecs, Vector(Wnd_GetWindowCenterX() - (szPlayTimeInfo.length() * 10 / 2) , Wnd_GetWindowCenterY() * 2 - 20), Color(130, 0, 20, 255));
		}

		this.m_oInfoMenu.Draw();
		this.m_oSelectMenu.Draw();
		this.m_oWaveInfoMenu.Draw();
		this.m_oShopMenu.Draw();

		if ((this.m_oInfoMenu.IsActive()) || (this.m_oSelectMenu.IsActive()) || (this.m_oWaveInfoMenu.IsActive()) || (this.m_oShopMenu.IsActive())) {
			R_DrawSprite(this.m_hCursor, Vector(this.m_vecCursorPos[0] - this.m_vecCrosshair[0] / 2, this.m_vecCursorPos[1] - this.m_vecCrosshair[1] / 2), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		} else {
			R_DrawSprite(this.m_hCrosshair, Vector(this.m_vecCursorPos[0] - this.m_vecCrosshair[0] / 2, this.m_vecCursorPos[1] - this.m_vecCrosshair[1] / 2), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
		}
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
		if (this.m_tmrMayDamage.IsElapsed()) {
			if (this.m_uiHealth < damageValue) {
				this.m_uiHealth = 0;
			} else {
				this.m_uiHealth -= damageValue;
			}
			
			this.m_tmrMayDamage.Reset();
			
			this.m_tmrFlicker.Reset();
			this.m_tmrFlicker.SetActive(true);
			this.m_uiFlickerCount = 0;
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
		return this.m_fRotation + 6.30 / 4;
	}
	
	//Set new rotation
	void SetRotation(float fRot)
	{
		this.m_fRotation = fRot + 6.30 / 4;
	}
	
	//Set health
	void SetHealth(uint health)
	{
		this.m_uiHealth = health;
	}
	
	//Get health
	uint GetHealth()
	{
		return this.m_uiHealth;
	}
	
	//Return a name string here, e.g. the class name or instance name.
	string GetName()
	{
		return "player";
	}
	
	//Called for wall collisions
	void OnWallCollided()
	{
	}
	
	//Called for key presses
	void OnKeyPress(int vKey, bool bDown)
	{
		if ((this.m_tmrGameCounter.IsActive()) || (this.m_oInfoMenu.IsActive()) || (this.m_oSelectMenu.IsActive()) || (this.m_oWaveInfoMenu.IsActive()) || (this.m_oShopMenu.IsActive())) {
			return;
		}
		
		if (vKey == GetKeyBinding("TURN_LEFT")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_TURNLEFT) == BTN_TURNLEFT)) {
					this.m_uiButtons |= BTN_TURNLEFT;
				}
			} else {
				if ((this.m_uiButtons & BTN_TURNLEFT) == BTN_TURNLEFT) {
					this.m_uiButtons &= ~BTN_TURNLEFT;
				}
			}
		} else if (vKey == GetKeyBinding("TURN_RIGHT")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_TURNRIGHT) == BTN_TURNRIGHT)) {
					this.m_uiButtons |= BTN_TURNRIGHT;
				}
			} else {
				if ((this.m_uiButtons & BTN_TURNRIGHT) == BTN_TURNRIGHT) {
					this.m_uiButtons &= ~BTN_TURNRIGHT;
				}
			}
		} 
		
		if (vKey == GetKeyBinding("MOVE_FORWARD")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_FORWARD) == BTN_FORWARD)) {
					this.m_uiButtons |= BTN_FORWARD;
				}
			} else {
				if ((this.m_uiButtons & BTN_FORWARD) == BTN_FORWARD) {
					this.m_uiButtons &= ~BTN_FORWARD;
				}
			}
		} else if (vKey == GetKeyBinding("MOVE_BACKWARD")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_BACKWARD) == BTN_BACKWARD)) {
					this.m_uiButtons |= BTN_BACKWARD;
				}
			} else {
				if ((this.m_uiButtons & BTN_BACKWARD) == BTN_BACKWARD) {
					this.m_uiButtons &= ~BTN_BACKWARD;
				}
			}
		} else if (vKey == GetKeyBinding("MOVE_LEFT")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_MOVELEFT) == BTN_MOVELEFT)) {
					this.m_uiButtons |= BTN_MOVELEFT;
				}
			} else {
				if ((this.m_uiButtons & BTN_MOVELEFT) == BTN_MOVELEFT) {
					this.m_uiButtons &= ~BTN_MOVELEFT;
				}
			}
		} else if (vKey == GetKeyBinding("MOVE_RIGHT")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_MOVERIGHT) == BTN_MOVERIGHT)) {
					this.m_uiButtons |= BTN_MOVERIGHT;
				}
			} else {
				if ((this.m_uiButtons & BTN_MOVERIGHT) == BTN_MOVERIGHT) {
					this.m_uiButtons &= ~BTN_MOVERIGHT;
				}
			}
		}
		
		if (vKey == GetKeyBinding("DODGE")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_DODGE) == BTN_DODGE)) {
					this.m_uiButtons |= BTN_DODGE;
				}
			} else {
				if ((this.m_uiButtons & BTN_DODGE) == BTN_DODGE) {
					this.m_uiButtons &= ~BTN_DODGE;
				}
			}
		}
		
		if (vKey == GetKeyBinding("THROW")) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_THROW) == BTN_THROW)) {
					this.m_uiButtons |= BTN_THROW;
				}
			} else {
				if ((this.m_uiButtons & BTN_THROW) == BTN_THROW) {
					this.m_uiButtons &= ~BTN_THROW;
				}
				
				this.m_bMayThrow = true;
			}
		}
		
		if (vKey == GetKeyBinding("SLOT1")) {
			this.m_iCurrentWeapon = WEAPON_HANDGUN;
			HUD_SetAmmoDisplayItem("handgun");
		} else if (vKey == GetKeyBinding("SLOT2")) {
			if ((CVar_GetBool("weapon_laser", false)) || (CVar_GetBool("weapon_temp_laser", false))) {
				this.m_iCurrentWeapon = WEAPON_RIFLE;
				HUD_SetAmmoDisplayItem("laser");
			}
		} else if (vKey == GetKeyBinding("SLOT3")) {
			if ((CVar_GetBool("weapon_shotgun", false)) || (CVar_GetBool("weapon_temp_shotgun", false))) {
				this.m_iCurrentWeapon = WEAPON_SHOTGUN;
				HUD_SetAmmoDisplayItem("shotgun");
			}
		} else if (vKey == GetKeyBinding("SLOT4")) {
			if ((CVar_GetBool("weapon_fthrower", false)) || (CVar_GetBool("weapon_temp_fthrower", false))) {
				this.m_iCurrentWeapon = WEAPON_FTHROWER;
				HUD_SetAmmoDisplayItem("fthrower");
			}
		} else if (vKey == GetKeyBinding("SLOT5")) {
			if ((CVar_GetBool("weapon_plasma", false)) || (CVar_GetBool("weapon_temp_plasma", false))) {
				this.m_iCurrentWeapon = WEAPON_PLASMAGUN;
				HUD_SetAmmoDisplayItem("plasma");
			}
		} else if (vKey == GetKeyBinding("SLOT6")) {
			if ((CVar_GetBool("weapon_rlauncher", false)) || (CVar_GetBool("weapon_temp_rlauncher", false))) {
				this.m_iCurrentWeapon = WEAPON_RLAUNCHER;
				HUD_SetAmmoDisplayItem("rlauncher");
			}
		}
	}
	
	//Called for mouse presses
	void OnMousePress(int key, bool bDown)
	{
		if (this.m_oInfoMenu.IsActive()) {
			if ((key == 1) && (!bDown)) {
				this.m_oInfoMenu.OnMouseClick();
			}
		}

		if (this.m_oSelectMenu.IsActive()) {
			if ((key == 1) && (!bDown)) {
				this.m_oSelectMenu.OnMouseClick();
			}
		}

		if (this.m_oWaveInfoMenu.IsActive()) {
			if ((key == 1) && (!bDown)) {
				this.m_oWaveInfoMenu.OnMouseClick();
			}
		}

		if (this.m_oShopMenu.IsActive()) {
			if ((key == 1) && (!bDown)) {
				this.m_oShopMenu.OnMouseClick();
			}
		}

		if ((this.m_tmrGameCounter.IsActive()) || (this.m_oInfoMenu.IsActive()) || (this.m_oWaveInfoMenu.IsActive()) || (this.m_oShopMenu.IsActive())) {
			return;
		}

		if (key == 1) {
			if (bDown) {
				if (!((this.m_uiButtons & BTN_ATTACK) == BTN_ATTACK)) {
					this.m_uiButtons |= BTN_ATTACK;
				}
			} else {
				if ((this.m_uiButtons & BTN_ATTACK) == BTN_ATTACK) {
					this.m_uiButtons &= ~BTN_ATTACK;
				}
			}
		}
	}
	
	//Called for getting current cursor position
	void OnUpdateCursor(const Vector &in pos)
	{
		//Store cursor pos
		this.m_vecCursorPos = pos;
		
		//Aim at cursor position
		this.AimAtScreenPoint(this.m_vecCursorPos);

		//Inform info menu
		this.m_oInfoMenu.OnUpdateCursorPos(pos);

		//Inform selection menu
		this.m_oSelectMenu.OnUpdateCursorPos(pos);

		//Inform wave info menu
		this.m_oWaveInfoMenu.OnUpdateCursorPos(pos);

		//Inform shop menu
		this.m_oShopMenu.OnUpdateCursorPos(pos);	
	}
	
	//Return save game properties
	string GetSaveGameProperties()
	{	
		return Props_CreateProperty("id", formatInt(Ent_GetId(@this))) + 
			Props_CreateProperty("x", formatInt(this.m_vecPos[0])) +
			Props_CreateProperty("y", formatInt(this.m_vecPos[1])) +
			Props_CreateProperty("rot", formatFloat(this.m_fRotation)) +
			Props_CreateProperty("health", formatInt(this.m_uiHealth)) +
			Props_CreateProperty("score", formatInt(this.m_iScore));
	}
	
	//Add to player score
	void AddPlayerScore(int amount)
	{
		this.m_iScore += amount;
	}
	
	//Called for returning the current score
	int GetPlayerScore()
	{
		return this.m_iScore;
	}
	
	//Add health
	void AddHealth(uint health)
	{
		this.m_uiHealth += health;
		if (this.m_uiHealth > 100) {
			this.m_uiHealth = 100;
		}
	}
	
	//Add ammo
	void AddAmmo(const string &in ident, uint amount)
	{
		if (ident != "grenade") {
			HUD_UpdateAmmoItem(ident, HUD_GetAmmoItemCurrent(ident) + amount, HUD_GetAmmoItemMax(ident));
		} else {
			HUD_UpdateCollectable(ident, HUD_GetCollectableCount(ident) + amount);
		}
	}
}

//Helper to determine if a file exists
bool Util_FileExists(const string &in szFileName)
{
	FileReader fr;
	fr.Open(szFileName);
	if (fr.IsOpen()) {
		fr.Close();
		return true;
	}

	return false;
}

//Create the associated entity here
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	g_szPackagePath = szPath;
	
	Ent_SetGoalActivationStatus(false);

	CVar_Register("show_mapsel_menu", CVAR_TYPE_BOOL, "0");
	CVar_Register("mapsel_enter_world", CVAR_TYPE_STRING, "");
	CVar_Register("show_shop_menu", CVAR_TYPE_BOOL, "0");
	CVar_Register("player_coins", CVAR_TYPE_INT, "0");
	CVar_Register("snowland_unlocked", CVAR_TYPE_BOOL, "0");
	CVar_Register("wasteland_unlocked", CVAR_TYPE_BOOL, "0");
	CVar_Register("lavaland_unlocked", CVAR_TYPE_BOOL, "0");
	CVar_Register("basis_hint", CVAR_TYPE_BOOL, "1");
	CVar_Register("shop_command", CVAR_TYPE_STRING, "");
	CVar_Register("ammo_max_pistol", CVAR_TYPE_INT, "0");
	CVar_Register("ammo_max_shotgun", CVAR_TYPE_INT, "200");
	CVar_Register("ammo_max_lasergun", CVAR_TYPE_INT, "200");
	CVar_Register("ammo_max_fthrower", CVAR_TYPE_INT, "100");
	CVar_Register("ammo_max_plasmagun", CVAR_TYPE_INT, "100");
	CVar_Register("ammo_max_rlauncher", CVAR_TYPE_INT, "100");
	CVar_Register("weapon_laser", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_shotgun", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_fthrower", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_plasma", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_rlauncher", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_temp_laser", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_temp_shotgun", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_temp_fthrower", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_temp_plasma", CVAR_TYPE_BOOL, "0");
	CVar_Register("weapon_temp_rlauncher", CVAR_TYPE_BOOL, "0");
	CVar_Register("enemies_defeated", CVAR_TYPE_INT, "0");
	CVar_Register("shots_fired", CVAR_TYPE_INT, "0");
	CVar_Register("grenades_thrown", CVAR_TYPE_INT, "0");
	CVar_Register("dodges_count", CVAR_TYPE_INT, "0");

	CVar_SetString("mapsel_enter_world", "");
	CVar_SetInt("enemies_defeated", 0);
	CVar_SetInt("dodges_count", 0);
	CVar_SetInt("shots_fired", 0);
	CVar_SetInt("grenades_thrown", 0);
	CVar_SetInt("player_coins", 0);

	CPlayerEntity @player = CPlayerEntity();
	Ent_SpawnEntity(szIdent, @player, vecPos);
	player.SetRotation(fRot);

	if (!Util_FileExists("game\\props\\player.props")) {
		FileWriter fw;
		fw.Open("game\\props\\player.props");
		fw.WriteLine("coins:100");
		fw.WriteLine("basishint:1");
		fw.WriteLine("snowland:0");
		fw.WriteLine("wasteland:0");
		fw.WriteLine("lavaland:0");
		fw.WriteLine("lasergun:0");
		fw.WriteLine("shotgun:0");
		fw.WriteLine("fthrower:0");
		fw.WriteLine("plasmagun:0");
		fw.WriteLine("rlauncher:0");
		fw.Close();
	}

	string props = Props_GetFromFile("player.props");
	int coins = parseInt(Props_ExtractValue(props, "coins"));
	int snowlandUnlocked = parseInt(Props_ExtractValue(props, "snowland"));
	int wastelandUnlocked = parseInt(Props_ExtractValue(props, "wasteland"));
	int lavalandUnlocked = parseInt(Props_ExtractValue(props, "lavaland"));
	int basisHint = parseInt(Props_ExtractValue(props, "basishint"));
	int lasergun = parseInt(Props_ExtractValue(props, "lasergun"));
	int shotgun = parseInt(Props_ExtractValue(props, "shotgun"));
	int fthrower = parseInt(Props_ExtractValue(props, "fthrower"));
	int plasmagun = parseInt(Props_ExtractValue(props, "plasmagun"));
	int rlauncher = parseInt(Props_ExtractValue(props, "rlauncher"));

	CVar_SetBool("snowland_unlocked", snowlandUnlocked > 0);
	CVar_SetBool("wasteland_unlocked", wastelandUnlocked > 0);
	CVar_SetBool("lavaland_unlocked", lavalandUnlocked > 0);
	CVar_SetBool("basis_hint", basisHint > 0);
	CVar_SetBool("weapon_laser", lasergun > 0);
	CVar_SetBool("weapon_shotgun", shotgun > 0);
	CVar_SetBool("weapon_fthrower", fthrower > 0);
	CVar_SetBool("weapon_plasma", plasmagun > 0);
	CVar_SetBool("weapon_rlauncher", rlauncher > 0);
	CVar_SetBool("weapon_temp_laser", false);
	CVar_SetBool("weapon_temp_shotgun", false);
	CVar_SetBool("weapon_temp_fthrower", false);
	CVar_SetBool("weapon_temp_plasma", false);
	CVar_SetBool("weapon_temp_rlauncher", false);

	HUD_AddAmmoItem("handgun", GetPackagePath() + "gfx\\handgunhud.png");
	HUD_UpdateAmmoItem("handgun", 125, CVar_GetInt("ammo_max_pistol", 0));
	HUD_SetAmmoDisplayItem("handgun");

	HUD_AddAmmoItem("laser", GetPackagePath() + "gfx\\lasergunhud.png");
	HUD_UpdateAmmoItem("laser", 35, CVar_GetInt("ammo_max_lasergun", 0));

	HUD_AddAmmoItem("shotgun", GetPackagePath() + "gfx\\shotgunhud.png");
	HUD_UpdateAmmoItem("shotgun", 40, CVar_GetInt("ammo_max_shotgun", 0));

	HUD_AddAmmoItem("fthrower", GetPackagePath() + "gfx\\fthrowerhud.png");
	HUD_UpdateAmmoItem("fthrower", 20, CVar_GetInt("ammo_max_fthrower", 0));

	HUD_AddAmmoItem("plasma", GetPackagePath() + "gfx\\plasmagunhud.png");
	HUD_UpdateAmmoItem("plasma", 15, CVar_GetInt("ammo_max_plasmagun", 0));

	HUD_AddAmmoItem("rlauncher", GetPackagePath() + "gfx\\rlauncherhud.png");
	HUD_UpdateAmmoItem("rlauncher", 10, CVar_GetInt("ammo_max_rlauncher", 0));
	
	HUD_AddCollectable("grenade", GetPackagePath() + "gfx\\grenade.png", true);
	HUD_UpdateCollectable("grenade", 10);
	
	HUD_AddCollectable("coins", GetPackagePath() + "gfx\\coin.png", true);
	HUD_UpdateCollectable("coins", coins);
}

//Restore game state
void RestoreState(const string &in szIdent, const string &in szValue)
{
	Ent_SetGoalActivationStatus(true);
	IScriptedEntity@ ent = Ent_GetPlayerEntity();
	ent.SetPosition(Vector(950, 700));
	/*string id = Props_ExtractValue(szValue, "id");
	if (id != "") {
		IScriptedEntity@ ent = Ent_GetEntityHandle(parseInt(id));
		if (@ent != null) {
			int x = parseInt(Props_ExtractValue(szValue, "x"));
			int y = parseInt(Props_ExtractValue(szValue, "y"));
			float rot = parseFloat(Props_ExtractValue(szValue, "rot"));
			
			ent.SetPosition(Vector(x, y));
			ent.SetRotation(rot);
			
			string health = Props_ExtractValue(szValue, "health");
			if (health != "") {
				if (ent.GetName() == "player") {
					CPlayerEntity@ casted = cast<CPlayerEntity>(ent);
					casted.SetHealth(parseInt(health));
				} else if (ent.GetName() == "tank") {
					CTankEntity@ casted = cast<CTankEntity>(ent);
					casted.SetHealth(parseInt(health));
				}
			}
			
			string score = Props_ExtractValue(szValue, "score");
			if (score != "") {
				if (ent.GetName() == "player") {
					CPlayerEntity@ casted = cast<CPlayerEntity>(ent);
					casted.AddPlayerScore(parseInt(score));
				}
			}
		}
	} else {
		int x = parseInt(Props_ExtractValue(szValue, "x"));
		int y = parseInt(Props_ExtractValue(szValue, "y"));
		float rot = parseFloat(Props_ExtractValue(szValue, "rot"));
	
		if (szIdent == "decal") {
			CDecalEntity @dcl = CDecalEntity();
			dcl.SetRotation(rot);
			Ent_SpawnEntity("decal", @dcl, Vector(x, y));
		} else if (szIdent == "explosion") {
			CExplosionEntity @expl = CExplosionEntity();
			expl.SetRotation(rot);
			Ent_SpawnEntity("explosion", @expl, Vector(x, y));
		} else {
			Print("Unknown spawnable entity ident: " + szIdent);
		}
	}*/
}

//Save game state to disk
bool SaveGame()
{
	Print("Saving game...");
	
	SaveGameWriter writer;
	writer.BeginSaveGame();
	writer.WritePackage(GetPackageName());
	writer.WriteMap(GetCurrentMap());
	
	for (size_t i = 0; i < Ent_GetEntityCount(); i++) {
		IScriptedEntity@ ent = Ent_GetEntityHandle(i);
		if (@ent != null) {
			writer.WriteAttribute(ent.GetName(), ent.GetSaveGameProperties());
		}
	}

	writer.EndSaveGame();
	
	Print("Done!");
	
	return true;
}
