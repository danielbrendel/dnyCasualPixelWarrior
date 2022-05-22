/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

const int MAPSEL_ITEMS_DISPLAY = 4;

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
    SpriteHandle m_hBg;
    SpriteHandle m_hVad;
    SpriteHandle m_hSliderLeft;
    SpriteHandle m_hSliderRight;
    SpriteHandle m_hBtn;
    FontHandle m_hTitle;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;
    int m_iItemIndex;

    CMapSelectMenu()
    {
        this.m_vecSize = Vector(500, 555);
        this.m_bActive = false;
        this.m_hTitle = R_LoadFont("Verdana", 20, 35);
        this.m_hBg = R_LoadSprite(GetPackagePath() + "gfx\\menubg.png", 1, this.m_vecSize[0] - 2, this.m_vecSize[1] - 2, 1, true);
        this.m_hVad = R_LoadSprite(GetPackagePath() + "gfx\\vad.png", 4, 29, 35, 4, false);
        this.m_hSliderLeft = R_LoadSprite(GetPackagePath() + "gfx\\slider_left.png", 1, 39, 31, 1, true);
        this.m_hSliderRight = R_LoadSprite(GetPackagePath() + "gfx\\slider_right.png", 1, 39, 31, 1, true);
        this.m_hBtn = R_LoadSprite(GetPackagePath() + "gfx\\btn_blue.png", 1, 150, 35, 1, true);
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

    //Scroll to left
    void ScrollLeft()
    {
        if (this.m_iItemIndex > 0) {
            this.m_iItemIndex--;
        }
    }

    //Scroll to right
    void ScrollRight()
    {
        if (this.m_iItemIndex + MAPSEL_ITEMS_DISPLAY < this.m_arrMapInfo.length()) {
            this.m_iItemIndex++;
        }
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

        R_DrawString(this.m_hTitle, _("app.mapselectmenu.title", "Choose a world"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 30), Color(50, 50, 50, 255));

        int iItemLoop = 0;

        for (int i = this.m_iItemIndex; i < this.m_iItemIndex + MAPSEL_ITEMS_DISPLAY; i++) {
            if (this.m_iItemIndex + MAPSEL_ITEMS_DISPLAY <= this.m_arrMapInfo.length()) {
                bool bDrawOverlay = false;
                if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] - 15 >= this.m_vecPos[1] + 100 + iItemLoop * 80) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + this.m_vecSize[0] - 20) && (this.m_vecCursorPos[1] - 15 < this.m_vecPos[1] + 100 + iItemLoop * 80 + 80)) {
                    bDrawOverlay = true;
                }

                R_DrawBox(Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 100 + iItemLoop * 80), Vector(this.m_vecSize[0] - 20, 80), 2, Color(50, 50, 50, 255));

                if (bDrawOverlay) {
                    R_DrawFilledBox(Vector(this.m_vecPos[0] + 12, this.m_vecPos[1] + 102 + iItemLoop * 80), Vector(this.m_vecSize[0] - 20 - 2, 80 - 2), Color(180, 180, 180, 100));
                }

                R_DrawString(R_GetDefaultFont(), this.m_arrMapInfo[i].szMap, Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + 100 + iItemLoop * 80 + 15), Color(50, 50, 50, 255));
                R_DrawString(R_GetDefaultFont(), this.m_arrMapInfo[i].szText, Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + 100 + iItemLoop * 80 + 55), Color(80, 80, 80, 255));

                Color colSign = Color(0, 0, 0, 0);
                if (this.m_arrMapInfo[i].bUnlocked) {
                    colSign = Color(90, 255, 90, 255);
                } else {
                    colSign = Color(243, 115, 123, 255);
                }

                R_DrawSprite(this.m_arrMapInfo[i].hSign, Vector(this.m_vecPos[0] + 10 + this.m_vecSize[0] - 100, this.m_vecPos[1] + 100 + iItemLoop * 80 + 15), 0, 0.0, Vector(-1, -1), 0.0, 0.0, true, colSign);
            
                iItemLoop++;
            }
        }

        Color colScrollLeft = Color(100, 100, 100, 255);
        if (this.MouseInsideScrollLeft()) {
            colScrollLeft = Color(50, 50, 50, 255);
        }
        R_DrawSprite(this.m_hSliderLeft, Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + this.m_vecSize[1] - 100), 0, 0.0, Vector(-1, -1), 0.0, 0.0, true, colScrollLeft);

        Color colScrollRight = Color(100, 100, 100, 255);
        if (this.MouseInsideScrollRight()) {
            colScrollRight = Color(50, 50, 50, 255);
        }
        R_DrawSprite(this.m_hSliderRight, Vector(this.m_vecPos[0] + 65, this.m_vecPos[1] + this.m_vecSize[1] - 100), 0, 0.0, Vector(-1, -1), 0.0, 0.0, true, colScrollRight);

        string szNavInfo = formatInt(this.m_iItemIndex + 1) + " - " + formatInt(this.m_iItemIndex + MAPSEL_ITEMS_DISPLAY) + " / " + formatInt(this.m_arrMapInfo.length());
        R_DrawString(R_GetDefaultFont(), szNavInfo, Vector(this.m_vecPos[0] + 125, this.m_vecPos[1] + this.m_vecSize[1] - 93), Color(100, 100, 100, 255));

        Color sColor;
        if (this.MouseInsideCloseText()) {
            sColor = Color(255, 255, 255, 255);
        } else {
            sColor = Color(215, 215, 215, 255);
        }

        R_DrawSprite(this.m_hBtn, Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + this.m_vecSize[1] - 45), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
        R_DrawString(R_GetDefaultFont(), _("app.mapselectmenu.close", "Close"), Vector(this.m_vecPos[0] + 60, this.m_vecPos[1] + this.m_vecSize[1] - 35), sColor);
    }

    //Indicate if mouse cursor is inside scroll left
    bool MouseInsideScrollLeft()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 30) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 90) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 30 + 39) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 100 + 40)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside scroll right
    bool MouseInsideScrollRight()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 75) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 90) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 75 + 39) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 100 + 40)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside close-button text
    bool MouseInsideCloseText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 35) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + 164) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 35 + 35)) {
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
        } else if (this.MouseInsideScrollLeft()) {
            this.ScrollLeft();
        } else if (this.MouseInsideScrollRight()) {
            this.ScrollRight();
        } else {
            int iItemLoop = 0;
            for (int i = this.m_iItemIndex; i < this.m_iItemIndex + MAPSEL_ITEMS_DISPLAY; i++) {
                if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] - 15 >= this.m_vecPos[1] + 100 + iItemLoop * 80) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + this.m_vecSize[0] - 20) && (this.m_vecCursorPos[1] - 15 < this.m_vecPos[1] + 100 + iItemLoop * 80 + 80)) {
                    if (this.m_arrMapInfo[i].bUnlocked) {
                        CVar_SetString("mapsel_enter_world", this.m_arrMapInfo[i].szMap);
                        this.m_bActive = false;
                    }
                }

                iItemLoop++;
            }
        }
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}