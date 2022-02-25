/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* Informational menu */
class CInfoMenu {
    array<array<string>> m_arrDialogs;
    size_t m_uiCurrentIndex;
    uint8 m_uiAlphaValue;
    bool m_bActive;
    Vector m_vecPos;
    Vector m_vecSize;
    Vector m_vecCursorPos;
    Timer m_tmrUpdateAlpha;
    SpriteHandle m_hVad;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;

    CInfoMenu()
    {
        this.m_vecSize = Vector(500, 380);
        this.m_bActive = false;
        this.m_hVad = R_LoadSprite(GetPackagePath() + "gfx\\vad.png", 4, 29, 35, 4, false);
        this.m_uiVadIndex = 0;
        this.m_tmrUpdateAlpha.SetDelay(10);
        this.m_tmrVadAnim.SetDelay(250);
        this.m_tmrVadAnim.Reset();
        this.m_tmrVadAnim.SetActive(true);
    }

    //Add dialog to menu
    void AddDialog(array<string> dialog)
    {
        this.m_arrDialogs.insertLast(dialog);
    }

    //Set menu position
    void SetPosition(const Vector &in vecPos)
    {
        this.m_vecPos = vecPos;
    }

    //Start dialog
    void Start()
    {
        this.m_uiCurrentIndex = 0;
        this.m_bActive = true;
        this.m_uiAlphaValue = 0;

        this.m_tmrUpdateAlpha.Reset();
        this.m_tmrUpdateAlpha.SetActive(true);
    }

    //Go to next dialog entry
    void Next()
    {
        //Go to next dialog or last dialog page
        if (this.m_uiCurrentIndex < this.m_arrDialogs.length() - 1) {
            this.m_uiCurrentIndex++;
        } else {
            this.m_bActive = false;
        }

        this.m_uiAlphaValue = 0;

        this.m_tmrUpdateAlpha.Reset();
        this.m_tmrUpdateAlpha.SetActive(true);
    }

    //Skip dialog
    void Skip()
    {
        this.m_uiCurrentIndex = this.m_arrDialogs.length();
        this.m_bActive = false;
    }

    //Clear dialogues
    void Clear()
    {
        this.m_arrDialogs.resize(0);
    }

    //Process menu
    void Process()
    {
        if (!this.m_bActive) {
            return;
        }

        //Increase alpha value for each current dialog
        if (this.m_tmrUpdateAlpha.IsActive()) {
            this.m_tmrUpdateAlpha.Update();
            if (this.m_tmrUpdateAlpha.IsElapsed()) {
                this.m_tmrUpdateAlpha.Reset();

                this.m_uiAlphaValue += 5;
                if (this.m_uiAlphaValue >= 255) {
                    this.m_tmrUpdateAlpha.SetActive(false);
                }
            }
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

        for (size_t i = 0; i < this.m_arrDialogs[this.m_uiCurrentIndex].length(); i++) {
            R_DrawString(R_GetDefaultFont(), this.m_arrDialogs[this.m_uiCurrentIndex][i], Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 10 + i * 30), Color(50, 50, 50, this.m_uiAlphaValue));
        }

        string szCurrent = "";
        if (this.m_uiCurrentIndex < this.m_arrDialogs.length() - 1) {
            szCurrent = _("app.infomenu.next", "Next");
        } else {
            szCurrent = _("app.infomenu.go", "Go!");
        }

        Color sColor;
        if (this.MouseInsideNextText()) {
            sColor = Color(35, 140, 35, 255);
        } else {
            sColor = Color(30, 120, 30, 255);
        }

        R_DrawString(R_GetDefaultFont(), szCurrent, Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + this.m_vecSize[1] - 35), sColor);

        if (this.MouseInsideSkipText()) {
            sColor = Color(35, 140, 35, 255);
        } else {
            sColor = Color(30, 120, 30, 255);
        }

        R_DrawString(R_GetDefaultFont(), _("app.infomenu.skip", "Skip"), Vector(this.m_vecPos[0] + this.m_vecSize[0] - 80, this.m_vecPos[1] + this.m_vecSize[1] - 35), sColor);
    }

    //Indicate if mouse cursor is inside next-button text
    bool MouseInsideNextText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 25) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + 45) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 25 + 25)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside skip-button text
    bool MouseInsideSkipText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + this.m_vecSize[0] - 80) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 35) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + this.m_vecSize[0] - 80 + 45) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 35 + 25)) {
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
        if (this.MouseInsideNextText()) {
            this.Next();
        } else if (this.MouseInsideSkipText()) {
            this.Skip();
        }
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}