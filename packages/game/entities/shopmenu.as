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
const int SHOP_CAT_WORLDS = 3;
const int SHOP_ITEMS_DISPLAY = 4;
const int SHOP_ITEM_CHARACTER_MAXLEN = 13;

class ShopItem {
    string szIdent;
    string szName;
    string szDescription;
    SpriteHandle hIcon;
    int iPrice;
    int iCategory;
}

/* Shop menu */
class CShopMenu {
    array<ShopItem> m_arrShopItemWeapons;
    array<ShopItem> m_arrShopItemAmmo;
    array<ShopItem> m_arrShopItemWorlds;
    bool m_bActive;
    Vector m_vecPos;
    Vector m_vecSize;
    Vector m_vecCursorPos;
    SpriteHandle m_hVad;
    size_t m_uiVadIndex;
    Timer m_tmrVadAnim;
    SpriteHandle m_hCoin;
    int m_iTabSelection;
    int m_iItemIndex;
    int m_iSelectedItem;

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
        this.m_iItemIndex = 0;
        this.m_iSelectedItem = -1;

        this.AddItem("item_test", "Weapon Item - Start", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);
        this.AddItem("item_test", "Weapon Item - Last", "This is a test item", "missile.png", 20, SHOP_CAT_WEAPONS);

        this.AddItem("ammo_pistol_x50", "Pistol x50", "", "handgunhud.png", 50, SHOP_CAT_AMMO);
        this.AddItem("ammo_shotgun_x25", "Shotgun x25", "", "shotgunhud.png", 100, SHOP_CAT_AMMO);
        this.AddItem("ammo_lasergun_x25", "Lasergun x25", "", "lasergunhud.png", 100, SHOP_CAT_AMMO);
        this.AddItem("ammo_grenade_x1", "Grenade x1", "", "grenade.png", 100, SHOP_CAT_AMMO);
        this.AddItem("ammo_grenade_x5", "Grenade x5", "", "grenade.png", 350, SHOP_CAT_AMMO);
        this.AddItem("ammo_grenade_x10", "Grenade x10", "", "grenade.png", 500, SHOP_CAT_AMMO);
        this.AddItem("ammo_grenade_x20", "Grenade x20", "", "grenade.png", 1000, SHOP_CAT_AMMO);

        this.AddItem("world_greenland", "Greenland", "", "sym_greenland.png", 0, SHOP_CAT_WORLDS);
        this.AddItem("world_snowland", "Snowland", "", "sym_snowland.png", 500, SHOP_CAT_WORLDS);
        this.AddItem("world_wasteland", "Wasteland", "", "sym_wasteland.png", 1250, SHOP_CAT_WORLDS);
        this.AddItem("world_lavaland", "Lavaland", "", "sym_lavaland.png", 1800, SHOP_CAT_WORLDS);
    }

    //Add shop item
    void AddItem(string szIdent, string szName, string szDescription, string szIcon, int iPrice, int iCategory)
    {
        ShopItem item = ShopItem();
        item.szIdent = szIdent;
        item.szName = szName;
        item.szDescription = szDescription;
        item.iPrice = iPrice;
        item.hIcon = R_LoadSprite(GetPackagePath() + "gfx\\" + szIcon, 1, 64, 64, 1, false);
        item.iCategory = iCategory;

        if (iCategory == SHOP_CAT_WEAPONS) {
            this.m_arrShopItemWeapons.insertLast(item);
        } else if (iCategory == SHOP_CAT_AMMO) {
            this.m_arrShopItemAmmo.insertLast(item);
        } else if (iCategory == SHOP_CAT_WORLDS) {
            this.m_arrShopItemWorlds.insertLast(item);
        }
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
        if (this.m_iTabSelection == SHOP_CAT_WEAPONS) {
            if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY < this.m_arrShopItemWeapons.length()) {
                this.m_iItemIndex++;
            }
        } else if (this.m_iTabSelection == SHOP_CAT_AMMO) {
            if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY < this.m_arrShopItemAmmo.length()) {
                this.m_iItemIndex++;
            }
        } else if (this.m_iTabSelection == SHOP_CAT_WORLDS) {
            if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY < this.m_arrShopItemWorlds.length()) {
                this.m_iItemIndex++;
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

        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.title", "Shop"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 50), Color(50, 150, 50, 255));

        R_DrawSprite(this.m_hCoin, Vector(this.m_vecPos[0] + this.m_vecSize[0] - 230, this.m_vecPos[1] + 25), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));
        R_DrawString(R_GetDefaultFont(), formatInt(HUD_GetCollectableCount("coins")), Vector(this.m_vecPos[0] + this.m_vecSize[0] - 230 + 40, this.m_vecPos[1] + 40), Color(250, 250, 250, 255));

        Color colCatWeapons = Color(50, 50, 50, 255);
        Color colCatAmmo = Color(50, 50, 50, 255);
        Color colCatWorlds = Color(50, 50, 50, 255);

        if (this.m_iTabSelection == SHOP_CAT_WEAPONS) {
            colCatWeapons = Color(100, 100, 100, 255);
        } else if (this.m_iTabSelection == SHOP_CAT_AMMO) {
            colCatAmmo = Color(100, 100, 100, 255);
        } else if (this.m_iTabSelection == SHOP_CAT_WORLDS) {
            colCatWorlds = Color(100, 100, 100, 255);
        }

        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.weapons", "Weapons"), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 100), colCatWeapons);
        R_DrawString(R_GetDefaultFont(), " | ", Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10, this.m_vecPos[1] + 100), Color(50, 50, 50, 255));
        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.ammo", "Ammo"), Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3, this.m_vecPos[1] + 100), colCatAmmo);
        R_DrawString(R_GetDefaultFont(), " | ", Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10, this.m_vecPos[1] + 100), Color(50, 50, 50, 255));
        R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.worlds", "Worlds"), Vector(this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3, this.m_vecPos[1] + 100), colCatWorlds);

        int iItemLoop = 0;
        this.m_iSelectedItem = -1;

        if (this.m_iTabSelection == SHOP_CAT_WEAPONS) {
            for (int i = this.m_iItemIndex; i < this.m_iItemIndex + SHOP_ITEMS_DISPLAY; i++) {
                if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY <= this.m_arrShopItemWeapons.length()) {
                    int iSpacing = iItemLoop * 10 * SHOP_ITEM_CHARACTER_MAXLEN + 10;

                    if (this.MouseInsideProductItem(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130))) {
                        R_DrawBox(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130), 2, Color(100, 100, 100, 255));
                        this.m_iSelectedItem = i;
                    }

                    R_DrawString(R_GetDefaultFont(), this.m_arrShopItemWeapons[i].szName, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 150), Color(100, 100, 100, 255));
                    R_DrawSprite(this.m_arrShopItemWeapons[i].hIcon, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 183), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));

                    Color colAffordable = Color(200, 30, 30, 255);
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemWeapons[i].iPrice >= 0) {
                        colAffordable = Color(30, 200, 30, 255);
                    }

                    R_DrawString(R_GetDefaultFont(), "$" + formatInt(this.m_arrShopItemWeapons[i].iPrice), Vector(this.m_vecPos[0] + 10 + iSpacing + 15, this.m_vecPos[1] + 255), colAffordable);

                    iItemLoop++;
                }
            }
        } else if (this.m_iTabSelection == SHOP_CAT_AMMO) {
            R_DrawString(R_GetDefaultFont(), _("app.shopmenu.cat.ammo.temporary", "Note: Additional ammo is only temporary."), Vector(this.m_vecPos[0] + 10, this.m_vecPos[1] + 123), Color(50, 50, 50, 255));

            for (int i = this.m_iItemIndex; i < this.m_iItemIndex + SHOP_ITEMS_DISPLAY; i++) {
                if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY <= this.m_arrShopItemAmmo.length()) {
                    int iSpacing = iItemLoop * 10 * SHOP_ITEM_CHARACTER_MAXLEN + 10;

                    if (this.MouseInsideProductItem(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130))) {
                        R_DrawBox(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130), 2, Color(100, 100, 100, 255));
                        this.m_iSelectedItem = i;
                    }

                    R_DrawString(R_GetDefaultFont(), this.m_arrShopItemAmmo[i].szName, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 150), Color(100, 100, 100, 255));
                    R_DrawSprite(this.m_arrShopItemAmmo[i].hIcon, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 183), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));

                    Color colAffordable = Color(200, 30, 30, 255);
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemAmmo[i].iPrice >= 0) {
                        colAffordable = Color(30, 200, 30, 255);
                    }

                    R_DrawString(R_GetDefaultFont(), "$" + formatInt(this.m_arrShopItemAmmo[i].iPrice), Vector(this.m_vecPos[0] + 10 + iSpacing + 15, this.m_vecPos[1] + 255), colAffordable);

                    iItemLoop++;
                }
            }
        } else if (this.m_iTabSelection == SHOP_CAT_WORLDS) {
            for (int i = this.m_iItemIndex; i < this.m_iItemIndex + SHOP_ITEMS_DISPLAY; i++) {
                if (this.m_iItemIndex + SHOP_ITEMS_DISPLAY <= this.m_arrShopItemWorlds.length()) {
                    int iSpacing = iItemLoop * 10 * SHOP_ITEM_CHARACTER_MAXLEN + 10;

                    if (this.MouseInsideProductItem(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130))) {
                        R_DrawBox(Vector(this.m_vecPos[0] + iSpacing, this.m_vecPos[1] + 140), Vector(10 * SHOP_ITEM_CHARACTER_MAXLEN, 130), 2, Color(100, 100, 100, 255));
                        this.m_iSelectedItem = i;
                    }

                    R_DrawString(R_GetDefaultFont(), this.m_arrShopItemWorlds[i].szName, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 150), Color(100, 100, 100, 255));
                    R_DrawSprite(this.m_arrShopItemWorlds[i].hIcon, Vector(this.m_vecPos[0] + 10 + iSpacing, this.m_vecPos[1] + 183), 0, 0.0, Vector(-1, -1), 0.0, 0.0, false, Color(0, 0, 0, 0));

                    Color colAffordable = Color(200, 30, 30, 255);
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemWorlds[i].iPrice >= 0) {
                        colAffordable = Color(30, 200, 30, 255);
                    }

                    R_DrawString(R_GetDefaultFont(), "$" + formatInt(this.m_arrShopItemWorlds[i].iPrice), Vector(this.m_vecPos[0] + 10 + iSpacing + 15, this.m_vecPos[1] + 255), colAffordable);

                    iItemLoop++;
                }
            }
        }

        Color colScrollLeft = Color(50, 50, 50, 255);
        if (this.MouseInsideScrollLeftText()) {
            colScrollLeft = Color(100, 100, 100, 255);
        }
        R_DrawString(R_GetDefaultFont(), "<<", Vector(this.m_vecPos[0] + 20, this.m_vecPos[1] + this.m_vecSize[1] - 100), colScrollLeft);

        Color colScrollRight = Color(50, 50, 50, 255);
        if (this.MouseInsideScrollRightText()) {
            colScrollRight = Color(100, 100, 100, 255);
        }
        R_DrawString(R_GetDefaultFont(), ">>", Vector(this.m_vecPos[0] + 50, this.m_vecPos[1] + this.m_vecSize[1] - 100), colScrollRight);

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

    //Indicate if mouse cursor is inside worlds category text
    bool MouseInsideCatWorldsText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + 111) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 10 + _("app.shopmenu.cat.weapons", "Weapons").length() * 10 + 10 * 3 + _("app.shopmenu.cat.ammo", "Ammo").length() * 10 + 10 * 3 + _("app.shopmenu.cat.worlds", "Worlds").length() * 10) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + 111 + 20)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside scroll left text
    bool MouseInsideScrollLeftText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 20) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 100) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 20 + 45) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 100 + 25)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside scroll right text
    bool MouseInsideScrollRightText()
    {
        if ((this.m_vecCursorPos[0] >= this.m_vecPos[0] + 50) && (this.m_vecCursorPos[1] >= this.m_vecPos[1] + this.m_vecSize[1] - 100) && (this.m_vecCursorPos[0] < this.m_vecPos[0] + 50 + 45) && (this.m_vecCursorPos[1] < this.m_vecPos[1] + this.m_vecSize[1] - 100 + 25)) {
            return true;
        }

        return false;
    }

    //Indicate if mouse cursor is inside product item
    bool MouseInsideProductItem(const Vector &in vecPos, const Vector &in vecSize)
    {
        if ((this.m_vecCursorPos[0] >= vecPos[0]) && (this.m_vecCursorPos[1] >= vecPos[1]) && (this.m_vecCursorPos[0] < vecPos[0] + vecSize[0]) && (this.m_vecCursorPos[1] < vecPos[1] + vecSize[1])) {
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
            this.m_iItemIndex = 0;
        } else if (this.MouseInsideCatAmmoText()) {
            this.m_iTabSelection = SHOP_CAT_AMMO;
            this.m_iItemIndex = 0;
        } else if (this.MouseInsideCatWorldsText()) {
            this.m_iTabSelection = SHOP_CAT_WORLDS;
            this.m_iItemIndex = 0;
        } else if (this.MouseInsideScrollLeftText()) {
            this.ScrollLeft();
        } else if (this.MouseInsideScrollRightText()) {
            this.ScrollRight();
        } else {
            if (this.m_iSelectedItem != -1) {
                if (this.m_iTabSelection == SHOP_CAT_WEAPONS) {
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemWeapons[this.m_iSelectedItem].iPrice >= 0) {
                        //Todo: Apply item
                    } else {
                        HUD_AddMessage(_("app.shopmenu.purchase.insufficientfunds", "Insufficient funds!"), HUD_MSG_COLOR_RED);
                    }
                } else if (this.m_iTabSelection == SHOP_CAT_AMMO) {
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice >= 0) {
                        if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_pistol_x50") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateAmmoItem("handgun", HUD_GetAmmoItemCurrent("handgun") + 50, HUD_GetAmmoItemMax("handgun"));
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_shotgun_x25") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateAmmoItem("shotgun", HUD_GetAmmoItemCurrent("shotgun") + 25, HUD_GetAmmoItemMax("shotgun"));
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_lasergun_x25") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateAmmoItem("laser", HUD_GetAmmoItemCurrent("laser") + 25, HUD_GetAmmoItemMax("laser"));
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_grenade_x1") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateCollectable("grenade", HUD_GetCollectableCount("grenade") + 1);
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_grenade_x5") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateCollectable("grenade", HUD_GetCollectableCount("grenade") + 5);
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_grenade_x10") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateCollectable("grenade", HUD_GetCollectableCount("grenade") + 10);
                        } else if (this.m_arrShopItemAmmo[this.m_iSelectedItem].szIdent == "ammo_grenade_x20") {
                            this.PerformPurchase(this.m_arrShopItemAmmo[this.m_iSelectedItem].iPrice);
                            HUD_UpdateCollectable("grenade", HUD_GetCollectableCount("grenade") + 20);
                        }
                    } else {
                        HUD_AddMessage(_("app.shopmenu.purchase.insufficientfunds", "Insufficient funds!"), HUD_MSG_COLOR_RED);
                    }
                } else if (this.m_iTabSelection == SHOP_CAT_WORLDS) {
                    if (HUD_GetCollectableCount("coins") - this.m_arrShopItemWorlds[this.m_iSelectedItem].iPrice >= 0) {
                        if (this.m_arrShopItemWorlds[this.m_iSelectedItem].szIdent == "world_greenland") {
                            HUD_AddMessage(_("app.shopmenu.purchase.alreadyowned", "You already own this item!"), HUD_MSG_COLOR_BLUE);
                        } else if (this.m_arrShopItemWorlds[this.m_iSelectedItem].szIdent == "world_snowland") {
                            if (!CVar_GetBool("snowland_unlocked", false)) {
                                this.PerformPurchase(this.m_arrShopItemWorlds[this.m_iSelectedItem].iPrice);
                                CVar_SetBool("snowland_unlocked", true);
                                CVar_SetString("shop_command", "unlock:snowland;");
                            } else {
                                HUD_AddMessage(_("app.shopmenu.purchase.alreadyowned", "You already own this item!"), HUD_MSG_COLOR_BLUE);
                            }
                        } else if (this.m_arrShopItemWorlds[this.m_iSelectedItem].szIdent == "world_wasteland") {
                            if (!CVar_GetBool("wasteland_unlocked", false)) {
                                this.PerformPurchase(this.m_arrShopItemWorlds[this.m_iSelectedItem].iPrice);
                                CVar_SetBool("wasteland_unlocked", true);
                                CVar_SetString("shop_command", "unlock:wasteland;");
                            } else {
                                HUD_AddMessage(_("app.shopmenu.purchase.alreadyowned", "You already own this item!"), HUD_MSG_COLOR_BLUE);
                            }
                        } else if (this.m_arrShopItemWorlds[this.m_iSelectedItem].szIdent == "world_lavaland") {
                            if (!CVar_GetBool("lavaland_unlocked", false)) {
                                this.PerformPurchase(this.m_arrShopItemWorlds[this.m_iSelectedItem].iPrice);
                                CVar_SetBool("lavaland_unlocked", true);
                                CVar_SetString("shop_command", "unlock:lavaland;");
                            } else {
                                HUD_AddMessage(_("app.shopmenu.purchase.alreadyowned", "You already own this item!"), HUD_MSG_COLOR_BLUE);
                            }
                        }
                    } else {
                        HUD_AddMessage(_("app.shopmenu.purchase.insufficientfunds", "Insufficient funds!"), HUD_MSG_COLOR_RED);
                    }
                }
            }
        }
    }

    //Perform the purchasing
    void PerformPurchase(int iPrice)
    {
        HUD_UpdateCollectable("coins", HUD_GetCollectableCount("coins") - iPrice);
        HUD_AddMessage(_("app.shopmenu.purchase.itempurchased", "Purchase successful!"), HUD_MSG_COLOR_GREEN);
    }

    //Indicate if menu is active
    bool IsActive()
    {
        return this.m_bActive;
    }
}