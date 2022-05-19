/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* Wave info menu */
class CWaveInfoMenu {
    bool m_bActive;
    Vector m_vecPos;
    Vector m_vecSize;
    Vector m_vecCursorPos;
    FontHandle m_hTitle;
    SpriteHandle m_hBg;
    SpriteHandle m_btnGreen;
    SpriteHandle m_hVad;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;

    CWaveInfoMenu()
    {
        this.m_vecSize = Vector(500, 380);
        this.m_bActive = false;
        this.m_hTitle = R_LoadFont("Verdana", 20, 35);
        this.m_hBg = R_LoadSprite(GetPackagePath() + "gfx\\menubg.png", 1, this.m_vecSize[0] - 2, this.m_vecSize[1] - 2, 1, true);
        this.m_hVad = R_LoadSprite(GetPackagePath() + "gfx\\vad.png", 4, 29, 35, 4, false);
        this.m_btnGreen = R_LoadSprite(GetPackagePath() + "gfx\\btn_green.png", 1, 150, 35, 1, true);
        this.m_uiVadIndex = 0;
        this.m_tmrVadAnim.SetDelay(250);
        this.m_tmrVadAnim.Reset();
        this.m_tmrVadAnim.SetActive(true);
    }

    //Set menu position
    void SetPosition(const Vector &in vecPos)
    {
        this.m_vecPos = vecPos;
    }

    //Start dialog
    void Start()
    {
        this.m_bActive = true;
    }

    //Process menu
    void Process()
    {
        if (!this.m_bActive) {
            return;
        }

        //Process VAD animation
        if (this.m_tmrVadAnim.IsActive()) {
            this.m_tmrVadAnim.Update();
            if (this.m_tmrVadAnim.IsElapsed()) {
                this.m_tmrVadAnim.Reset();

                this.m_uiVadIndex++;
                if (this.m_uiVadIndex >= 4) {
                    this.m_uiVadIndex = 0;
                }
            }
        }
    }

    //Draw the menu
    void Draw()
    {
        if (!this.m_bActive) {
            return;
        }

        R_DrawBox(this.m_vecPos, this.m_vecSize, 2, Color(0, 0, 0, 255));
        
        R_DrawSprite(this.m_hBg, Vector(this.m_vecPos[0] + 2, this.m_vecPos[1] + 2), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));

        R_DrawSprite(this.m_hVad, Vector(this.m_vecPos[0] + this.m_vecSize[0] - 80, this.m_vecPos[1] + 30), this.m_uiVadIndex, 0.0, Vector(-1, -1), 2.0, 2.0, false, Color(0, 0, 0, 0));

        R_DrawString(this.m_hTitle, _("app.waveinfomenu.title", "You got fragged!"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 30), Color(50, 50, 50, 255));

        R_DrawString(R_GetDefaultFont(), "Amount of collected coins: " + CVar_GetInt("player_coins", 0), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 80), Color(50, 50, 50, 255));

        R_DrawString(R_GetDefaultFont(), "Defeated enemies: " + CVar_GetInt("enemies_defeated", 0), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 100), Color(50, 50, 50, 255));

        R_DrawString(R_GetDefaultFont(), "Shots fired: " + CVar_GetInt("shots_fired", 0), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 120), Color(50, 50, 50, 255));

        R_DrawString(R_GetDefaultFont(), "Grenades thrown: " + CVar_GetInt("grenades_thrown", 0), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 140), Color(50, 50, 50, 255));

        R_DrawString(R_GetDefaultFont(), "Dodges: " + CVar_GetInt("dodges_count", 0), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 160), Color(50, 50, 50, 255));

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
        R_DrawString(R_GetDefaultFont(), "Playtime: " + szPlayTimeHours + ":" + szPlayTimeMins + ":" + szPlayTimeSecs, Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 180), Color(50, 50, 50, 255));

        Color sColor;
        if (this.MouseInsideCloseText()) {
            sColor = Color(255, 255, 255, 255);
        } else {
            sColor = Color(230, 230, 230, 255);
        }

        R_DrawSprite(this.m_btnGreen, Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + this.m_vecSize[1] - 45), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
        R_DrawString(R_GetDefaultFont(), _("app.waveinfomenu.gohome", "Go home"), Vector(this.m_vecPos[0] + 50, this.m_vecPos[1] + this.m_vecSize[1] - 35), sColor);
    }

    //Indicate if mouse cursor is inside close-button text
    bool MouseInsideCloseText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 15) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + 150) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 15 + 35)) {
            return true;
        }

        return false;
    }

    //Update cursor position
    void OnUpdateCursorPos(const Vector &in vecPos)
    {
        this.m_vecCursorPos = vecPos;
    }

    //Handle mouse clicks
    void OnMouseClick()
    {
        if (this.MouseInsideCloseText()) {
            CVar_SetString("mapsel_enter_world", "basis");
            this.m_bActive = false;
        }
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}