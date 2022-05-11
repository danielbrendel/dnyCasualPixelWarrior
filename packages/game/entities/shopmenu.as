/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

const int SHOP_CAT_WEAPONS = 1;
const int SHOP_CAT_AMMO = 2;
const int SHOP_CAT_BOOSTER = 3;

class ShopItem {
    size_t uiIdent;
    string szName;
    string szDescription;
    SpriteHandle hIcon;
    int iPrice;
}

/* Shop menu */
class CShopMenu {
    array<ShopItem> m_arrShopItems;
    bool m_bActive;
    Vector m_vecPos;
    Vector m_vecSize;
    Vector m_vecCursorPos;
    SpriteHandle m_hVad;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;
    SpriteHandle m_hCoin;
    int m_iTabSelection;

    CShopMenu()
    {
        this.m_vecSize = Vector(600, 450);
        this.m_bActive = false;
        this.m_hVad = R_LoadSprite(GetPackagePath() + "gfx\\vad.png", 4, 29, 35, 4, false);
        this.m_uiVadIndex = 0;
        this.m_tmrVadAnim.SetDelay(250);
        this.m_tmrVadAnim.Reset();
        this.m_tmrVadAnim.SetActive(true);
        this.m_hCoin = R_LoadSprite(GetPackagePath() + "gfx\\coin.png", 1, 40, 47, 1, false);
        this.m_iTabSelection = 1;
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

        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.title", "Shop"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 50), Color(50, 150, 50, 255));

        R_DrawSprite(this.m_hCoin, Vector(this.m_vecPos[0] + this.m_vecSize[0] - 230, this.m_vecPos[1] + 25), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
        R_DrawString(R_GetDefaultFont(), formatInt(HUD_GetCollectableCount("coins")), Vector(this.m_vecPos[0] + this.m_vecSize[0] - 230 + 40, this.m_vecPos[1] + 40), Color(250, 250, 250, 255));

        Color colCatWeapons = Color(50, 50, 50, 255);
        Color colCatAmmo = Color(50, 50, 50, 255);
        Color colCatBooster = Color(50, 50, 50, 255);

        if (this.m_iTabSelection == SHOP_CAT_WEAPONS) {
            colCatWeapons = Color(100, 100, 100, 255);
        } else if (this.m_iTabSelection == SHOP_CAT_AMMO) {
            colCatAmmo = Color(100, 100, 100, 255);
        } else if (this.m_iTabSelection == SHOP_CAT_BOOSTER) {
            colCatBooster = Color(100, 100, 100, 255);
        }

        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.weapons", "Weapons"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 100), colCatWeapons);
        R_DrawString(R_GetDefaultFont(), " | ", Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10, this.m_vecPos[1] + 100), Color(50, 50, 50, 255));
        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.ammo", "Ammo"), Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3, this.m_vecPos[1] + 100), colCatAmmo);
        R_DrawString(R_GetDefaultFont(), " | ", Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10, this.m_vecPos[1] + 100), Color(50, 50, 50, 255));
        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.booster", "Booster"), Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3, this.m_vecPos[1] + 100), colCatBooster);

        for (size_t i = 0; i < this.m_arrShopItems.length(); i++) {
            
        }

        Color sColor;
        if (this.MouseInsideCloseText()) {
            sColor = Color(35, 140, 35, 255);
        } else {
            sColor = Color(30, 120, 30, 255);
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

    //Indicate if mouse cursor is inside weapons category text
    bool MouseInsideCatWeaponsText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + 111) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + 111 + 20)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside ammo category text
    bool MouseInsideCatAmmoText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + 111) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + 111 + 20)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside booster category text
    bool MouseInsideCatBoosterText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + 111) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3 + _("app.shopmenu.cat.booster", "Booster").length() * 10) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + 111 + 20)) {
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
        } else if (this.MouseInsideCatWeaponsText()) {
            this.m_iTabSelection = SHOP_CAT_WEAPONS;
        } else if (this.MouseInsideCatAmmoText()) {
            this.m_iTabSelection = SHOP_CAT_AMMO;
        } else if (this.MouseInsideCatBoosterText()) {
            this.m_iTabSelection = SHOP_CAT_BOOSTER;
        }
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}