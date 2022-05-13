/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

class MapInfo {
    string szMap;
    string szText;
    bool bUnlocked;
    SpriteHandle hSign;
}

/* Map selection menu */
class CMapSelectMenu {
    array<MapInfo> m_arrMapInfo;
    bool m_bActive;
    Vector m_vecPos;
    Vector m_vecSize;
    Vector m_vecCursorPos;
    SpriteHandle m_hVad;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;

    CMapSelectMenu()
    {
        this.m_vecSize = Vector(500, 500);
        this.m_bActive = false;
        this.m_hVad = R_LoadSprite(GetPackagePath() + "gfx\\vad.png", 4, 29, 35, 4, false);
        this.m_uiVadIndex = 0;
        this.m_tmrVadAnim.SetDelay(250);
        this.m_tmrVadAnim.Reset();
        this.m_tmrVadAnim.SetActive(true);
    }

    //Add dialog to menu
    void AddMap(string szMap, string szDisplayText, bool bUnlocked)
    {
        MapInfo mapInfo;
        mapInfo.szMap = szMap;
        mapInfo.szText = szDisplayText;
        mapInfo.bUnlocked = bUnlocked;

        if (bUnlocked) {
            mapInfo.hSign = R_LoadSprite(GetPackagePath() + "gfx\\unlocked.png", 1, 50, 50, 1, true);
        } else {
            mapInfo.hSign = R_LoadSprite(GetPackagePath() + "gfx\\locked.png", 1, 50, 50, 1, true);
        }

        this.m_arrMapInfo.insertLast(mapInfo);
    }

    //Unlock specific map
    void UnlockMap(string szMap)
    {
        for (int i = 0; i < this.m_arrMapInfo.length(); i++) {
            if (this.m_arrMapInfo[i].szMap == szMap) {
                this.m_arrMapInfo[i].bUnlocked = true;
                this.m_arrMapInfo[i].hSign = R_LoadSprite(GetPackagePath() + "gfx\\unlocked.png", 1, 50, 50, 1, true);
                break;
            }
        }
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
        R_DrawFilledBox(Vector(this.m_vecPos[0] + 2, this.m_vecPos[1] + 2), Vector(this.m_vecSize[0] - 2, this.m_vecSize[1] - 2), Color(150, 150, 150, 255));

        R_DrawSprite(this.m_hVad, Vector(this.m_vecPos[0] + this.m_vecSize[0] - 80, this.m_vecPos[1] + 30), this.m_uiVadIndex, 0.0, Vector(-1, -1), 2.0, 2.0, false, Color(0, 0, 0, 0));

        R_DrawString(R_GetDefaultFont(), _("app.mapselectmenu.title", "Choose a world to enter"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 50), Color(50, 150, 50, 255));

        for (size_t i = 0; i < this.m_arrMapInfo.length(); i++) {
            bool bDrawOverlay = false;
            if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] - 15 >= this.m_vecPos[1] + 100 + i * 80) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + this.m_vecSize[0] - 20) && (this.m_vecCursorPos[1] - 15 < this.m_vecPos[1] + 100 + i * 80 + 80)) {
                bDrawOverlay = true;
            }

            R_DrawBox(Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 100 + i * 80), Vector(this.m_vecSize[0] - 20, 80), 2, Color(50, 50, 50, 255));

            if (bDrawOverlay) {
                R_DrawFilledBox(Vector(this.m_vecPos[0] + 12, this.m_vecPos[1] + 102 + i * 80), Vector(this.m_vecSize[0] - 20 - 2, 80 - 2), Color(180, 180, 180, 100));
            }

            R_DrawString(R_GetDefaultFont(), this.m_arrMapInfo[i].szMap, Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + 100 + i * 80 + 15), Color(50, 50, 50, 255));
            R_DrawString(R_GetDefaultFont(), this.m_arrMapInfo[i].szText, Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + 100 + i * 80 + 55), Color(80, 80, 80, 255));

            R_DrawSprite(this.m_arrMapInfo[i].hSign, Vector(this.m_vecPos[0] + 10 + this.m_vecSize[0] - 100, this.m_vecPos[1] + 100 + i * 80 + 15), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
        }

        Color sColor;
        if (this.MouseInsideCloseText()) {
            sColor = Color(100, 100, 100, 255);
        } else {
            sColor = Color(50, 50, 50, 255);
        }

        R_DrawString(R_GetDefaultFont(), _("app.mapselectmenu.close", "Close"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + this.m_vecSize[1] - 35), sColor);
    }

    //Indicate if mouse cursor is inside close-button text
    bool MouseInsideCloseText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 25) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + 45) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 25 + 25)) {
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
            this.m_bActive = false;
        } else {
            for (size_t i = 0; i < this.m_arrMapInfo.length(); i++) {
                if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] - 15 >= this.m_vecPos[1] + 100 + i * 80) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + this.m_vecSize[0] - 20) && (this.m_vecCursorPos[1] - 15 < this.m_vecPos[1] + 100 + i * 80 + 80)) {
                    if (this.m_arrMapInfo[i].bUnlocked) {
                        CVar_SetString("mapsel_enter_world", this.m_arrMapInfo[i].szMap);
                        this.m_bActive = false;
                    }
                }
            }
        }
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}