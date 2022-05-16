#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include <d3d9.h>
#include <d3dx9core.h>
#include <DxErr.h>
#include "utils.h"

/* Renderer management component */
namespace DxRenderer {
	#define GFX_INVALID_LIST_ID std::wstring::npos
	#define GFX_INVALID_SPRITE_ID nullptr

	typedef struct d3dimage_s* HD3DIMAGE;
	typedef LPDIRECT3DTEXTURE9 HD3DSPRITE;

	struct d3dfont_s {
		LPD3DXFONT pFont; //Pointer to font object
		std::wstring wszFontName; //Font name string
		BYTE ucFontSizeW, ucFontSizeH; //Font size
	};

	struct d3dimage_s {
		std::wstring wszFile; //Full file name
		LPDIRECT3DSURFACE9 pSurface; //Surface data pointer
		D3DXIMAGE_INFO d3dImageInfo; //Image info
	};

	struct d3dsprite_s {
		std::wstring wszFile; //File name
		LPDIRECT3DTEXTURE9 pTexture; //Pointer to the sprite texture
		int iFrameCount; //Total amount of frames
		int iFramesPerLine; //Amount of frames per line
		int iFrameWidth; //Single frame width
		int iFrameHeight; //Single frame height
	};

	struct drawsprite_s {
		HD3DSPRITE hSprite;
		int x, y;
		int iFrame;
		float fRot;
		int rotx;
		int roty;
		float fScale1, fScale2;
		bool bUseDefaultColor;
		byte r, g, b, a;
	};

	class CDxRenderer {
	private:
		HWND m_hWnd;
		INT m_iWidth, m_iHeight;

		IDirect3D9* m_pInterface;
		IDirect3DDevice9* m_pDevice;
		IDirect3DSurface9* m_pBackBuffer;
		IDirect3DSurface9* m_pImageSurface;
		LPD3DXSPRITE m_pSpriteMgr;

		D3DCOLOR m_d3dcClearColor;

		std::vector<d3dfont_s*> m_vFonts;
		std::vector<d3dimage_s*> m_vImages;
		std::vector<d3dsprite_s> m_vSprites;
		std::vector<drawsprite_s> m_vDrawnSprites;

		d3dfont_s* FindFont(const std::wstring& wszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH)
		{
			//Get font data pointer

			if (!wszFontName.length())
				return nullptr;

			//Search in list
			for (size_t i = 0; i < this->m_vFonts.size(); i++) {
				if ((this->m_vFonts[i]->ucFontSizeW == ucFontSizeW) && (this->m_vFonts[i]->ucFontSizeH == ucFontSizeH) && (this->m_vFonts[i]->wszFontName == wszFontName)) { //Compare data
					return this->m_vFonts[i];
				}
			}

			return nullptr;
		}

		const size_t FindSprite(const HD3DSPRITE hSprite)
		{
			//Get list ID of sprite handle if exists

			for (size_t i = 0; i < this->m_vSprites.size(); i++) {
				if (this->m_vSprites[i].pTexture == hSprite)
					return i;
			}

			return GFX_INVALID_LIST_ID;
		}

		HD3DSPRITE FindSprite(const std::wstring& wszTexture, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine)
		{
			//Find existing sprite

			for (size_t i = 0; i < this->m_vSprites.size(); i++) {
				if ((this->m_vSprites[i].wszFile == wszTexture) && (this->m_vSprites[i].iFrameCount == iFrameCount) && (this->m_vSprites[i].iFrameWidth == iFrameWidth) && (this->m_vSprites[i].iFrameHeight == iFrameHeight) && (this->m_vSprites[i].iFramesPerLine == iFramesPerLine)) {
					return this->m_vSprites[i].pTexture;
				}
			}

			return GFX_INVALID_SPRITE_ID;
		}
	public:
		CDxRenderer() : m_hWnd(0), m_iWidth(0), m_iHeight(0), m_pInterface(nullptr), m_pDevice(nullptr), m_pBackBuffer(nullptr), m_pImageSurface(nullptr), m_pSpriteMgr(nullptr) {}
		CDxRenderer(HWND hWnd, bool bWindowed, int iWidth, int iHeight, BYTE r, BYTE g, BYTE b, BYTE a) : CDxRenderer() { this->Initialize(hWnd, bWindowed, iWidth, iHeight, r, g, b, a); }
		~CDxRenderer() { this->Release(); }

		bool Initialize(HWND hWnd, bool bWindowed, int iWidth, int iHeight, BYTE r, BYTE g, BYTE b, BYTE a)
		{
			//Initialize DirectX

			if ((!hWnd) || (!iWidth) || (!iHeight))
				return false;

			//Create D3D9 interface
			this->m_pInterface = Direct3DCreate9(D3D_SDK_VERSION);
			if (!this->m_pInterface)
				return false;

			//Setup data
			D3DPRESENT_PARAMETERS d3dppParameters;
			memset(&d3dppParameters, 0x00, sizeof(d3dppParameters));
			d3dppParameters.Windowed = bWindowed; //Windowed or fullscreened target window
			d3dppParameters.SwapEffect = D3DSWAPEFFECT_DISCARD; //Driver shall show backbuffer via the most fast way
			d3dppParameters.hDeviceWindow = hWnd; //Handle of target window
			d3dppParameters.BackBufferFormat = D3DFMT_A8R8G8B8; //Use 32-bit color depth (RGBA)
			d3dppParameters.BackBufferWidth = iWidth; //Width of backbuffer
			d3dppParameters.BackBufferHeight = iHeight; //Height of backbuffer

			//Create device
			if (FAILED(this->m_pInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dppParameters, &this->m_pDevice))) {
				this->m_pInterface->Release();
				this->m_pInterface = nullptr;
				return false;
			}

			//Get pointer to backbuffer
			if (FAILED(this->m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->m_pBackBuffer))) {
				this->m_pDevice->Release();
				this->m_pInterface->Release();
				this->m_pDevice = nullptr;
				this->m_pInterface = nullptr;
				return false;
			}

			//Create sprite
			if (FAILED(D3DXCreateSprite(this->m_pDevice, &this->m_pSpriteMgr)))
				return false;

			//Save data
			this->m_hWnd = hWnd;
			this->m_iWidth = iWidth;
			this->m_iHeight = iHeight;
			this->m_d3dcClearColor = D3DCOLOR_ARGB(a, r, g, b);
			
			return true;
		}

		void Release(void)
		{
			//Clear data and free memory

			if (!this->m_pInterface)
				return;

			//Clear fonts

			for (size_t i = 0; i < this->m_vFonts.size(); i++) {
				if (this->m_vFonts[i]) {
					this->m_vFonts[i]->pFont->Release(); //Release object
					delete this->m_vFonts[i]; //Free memory
				}
			}

			this->m_vFonts.clear(); //Clear list

			//Clear images
			for (size_t i = 0; i < this->m_vImages.size(); i++) {
				if (this->m_vImages[i]) {
					this->m_vImages[i]->pSurface->Release(); //Release object
					delete this->m_vImages[i]; //Free memory
				}
			}

			this->m_vImages.clear(); //Clear list

			//Clear sprites
			for (size_t i = 0; i < this->m_vSprites.size(); i++) {
				if (this->m_vSprites[i].pTexture) {
					this->m_vSprites[i].pTexture->Release(); //Release object
				}
			}

			this->m_vSprites.clear(); //Clear list

			//Release sprite manager
			if (this->m_pSpriteMgr)
				this->m_pSpriteMgr->Release();

			//Release device
			if (this->m_pDevice)
				this->m_pDevice->Release();

			//Release interface
			this->m_pInterface->Release();

			//Clear data
			this->m_pInterface = nullptr;
			this->m_pDevice = nullptr;
			this->m_hWnd = 0;
			this->m_iWidth = this->m_iHeight = 0;
		}

		bool DrawBegin(void)
		{
			//Begin scene drawing
			
			if ((!this->m_pInterface) || (!this->m_pDevice))
				return false;

			//Clear backbuffer
			if (FAILED(this->m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, this->m_d3dcClearColor, 0, 0)))
				return false;
			
			//Begin scene
			if (FAILED(this->m_pDevice->BeginScene()))
				return false;
			
			BOOL bResult = TRUE;

			//Check for background image
			if (this->m_pImageSurface) {
				//Set RECT data
				RECT rSrc;
				rSrc.left = 0;
				rSrc.top = 0;
				rSrc.right = this->m_iWidth;
				rSrc.bottom = this->m_iHeight;
				bResult = SUCCEEDED(this->m_pDevice->StretchRect(this->m_pImageSurface, &rSrc, this->m_pBackBuffer, nullptr, D3DTEXF_NONE)); //Copy surface into backbuffer
			}

			//Begin drawing sprites
			if (FAILED(this->m_pSpriteMgr->Begin(D3DXSPRITE_ALPHABLEND)))
				return false;
			
			return bResult == TRUE;
		}

		bool DrawEnd(void)
		{
			//End scene drawing

			if ((!this->m_pInterface) || (!this->m_pDevice))
				return false;

			//End drawing sprites
			if (FAILED(this->m_pSpriteMgr->End()))
				return false;

			//End scene
			if (FAILED(this->m_pDevice->EndScene()))
				return false;

			//Copy backbuffer into frontbuffer
			return SUCCEEDED(this->m_pDevice->Present(nullptr, nullptr, this->m_hWnd, nullptr));
		}

		d3dfont_s* LoadFont(const std::wstring& wszFontName, BYTE ucFontSizeW, BYTE ucFontSizeH)
		{
			//Create new font

			if (!wszFontName.length())
				return nullptr;

			d3dfont_s* pFont = nullptr;

			//Check if already exists
			pFont = this->FindFont(wszFontName, ucFontSizeW, ucFontSizeH);
			if (pFont)
				return pFont;

			//Allocate memory font font structure
			pFont = new d3dfont_s;
			if (!pFont)
				return nullptr;

			//Create font object
			if (FAILED(D3DXCreateFont(this->m_pDevice, (INT)ucFontSizeH, (UINT)(ucFontSizeW), FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, wszFontName.c_str(), &pFont->pFont))) {
				delete[] pFont;
				return nullptr;
			}

			//Set data
			pFont->wszFontName = wszFontName;
			pFont->ucFontSizeW = ucFontSizeW;
			pFont->ucFontSizeH = ucFontSizeH;

			//Add to list
			this->m_vFonts.push_back(pFont);

			return pFont;
		}

		HD3DIMAGE LoadImage(const std::wstring& wszImageFile)
		{
			//Load image

			if (!wszImageFile.length())
				return nullptr;

			//Allocate memory
			d3dimage_s* pImageData = new d3dimage_s;
			if (!pImageData)
				return nullptr;

			//Get image info
			if (FAILED(D3DXGetImageInfoFromFile(wszImageFile.c_str(), &pImageData->d3dImageInfo))) {
				delete pImageData;
				return nullptr;
			}

			//Create surface for image
			if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(pImageData->d3dImageInfo.Width, pImageData->d3dImageInfo.Height, pImageData->d3dImageInfo.Format, D3DPOOL_DEFAULT, &pImageData->pSurface, nullptr))) {
				delete pImageData;
				return nullptr;
			}

			//Load image into surface
			if (FAILED(D3DXLoadSurfaceFromFile(pImageData->pSurface, nullptr, nullptr, wszImageFile.c_str(), nullptr, D3DX_DEFAULT, 0, &pImageData->d3dImageInfo))) {
				delete pImageData;
				return nullptr;
			}

			//Save file name
			pImageData->wszFile = wszImageFile;

			//Add to list
			this->m_vImages.push_back(pImageData);

			return pImageData; //Return item handle
		}

		bool GetSpriteInfo(const std::wstring& wszFile, D3DXIMAGE_INFO& rOut)
		{
			//Get sprite info

			D3DXIMAGE_INFO sImageInfo = { 0 };

			if (FAILED(D3DXGetImageInfoFromFile(wszFile.c_str(), &sImageInfo)))
				return false;

			rOut = sImageInfo;

			return true;
		}

		HD3DSPRITE LoadSprite(const std::wstring& wszTexture, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize = false)
		{
			//Load sprite

			if (!wszTexture.length())
				return GFX_INVALID_SPRITE_ID;

			//Handle case if sprite with that texture already exists
			/*HD3DSPRITE hExists = this->FindSprite(wszTexture, iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine);
			if (hExists != GFX_INVALID_SPRITE_ID) {
				return hExists;
			}*/

			d3dsprite_s sSpriteData;	

			//Load texture from file
			if (FAILED(D3DXCreateTextureFromFileEx(this->m_pDevice, wszTexture.c_str(), (bForceCustomSize) ? iFrameWidth : D3DX_DEFAULT_NONPOW2, (bForceCustomSize) ? iFrameHeight : D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, nullptr, nullptr, &sSpriteData.pTexture))) {
				return GFX_INVALID_SPRITE_ID;
			}

			//Save further data
			sSpriteData.wszFile = wszTexture;
			sSpriteData.iFrameCount = iFrameCount;
			sSpriteData.iFramesPerLine = iFramesPerLine;
			sSpriteData.iFrameWidth = iFrameWidth;
			sSpriteData.iFrameHeight = iFrameHeight;

			//Add to list
			this->m_vSprites.push_back(sSpriteData);

			//Return handle
			return sSpriteData.pTexture;
		}

		bool FreeSprite(HD3DSPRITE hSprite)
		{
			//Free the sprite resources

			if (!hSprite)
				return false;

			//Find sprite
			size_t uiSprite = this->FindSprite(hSprite);
			if (uiSprite == GFX_INVALID_LIST_ID)
				return false;

			//Free texture
			if (this->m_vSprites[uiSprite].pTexture)
				this->m_vSprites[uiSprite].pTexture->Release();

			//Remove from list
			this->m_vSprites.erase(this->m_vSprites.begin() + uiSprite);

			return true;
		}

		bool DrawString(const d3dfont_s* pFont, const std::wstring& wszText, int x, int y, BYTE r, BYTE g, BYTE b, BYTE a)
		{
			//Draw a string on backbuffer

			if ((!this->m_pDevice) || (!wszText.length()) || (!pFont))
				return false;

			//End drawing sprites
			if (FAILED(this->m_pSpriteMgr->End()))
				return false;

			//Set data
			RECT rect = { x, y, x, y };
			D3DCOLOR d3dcColor = D3DCOLOR_ARGB(a, r, g, b);

			//Calculate font rect
			INT iResult = pFont->pFont->DrawText(nullptr, wszText.c_str(), (int)wszText.length(), &rect, DT_NOCLIP, d3dcColor);

			//Begin drawing sprites
			if (FAILED(this->m_pSpriteMgr->Begin(D3DXSPRITE_ALPHABLEND)))
				return false;

			return iResult != 0;
		}

		bool DrawBox(int x, int y, int w, int h, int iThickness, BYTE r, BYTE g, BYTE b, BYTE a)
		{
			//Draw a box skeleton on backbuffer

			if (!this->m_pDevice)
				return false;

			if (!DrawFilledBox(x, y, w, iThickness, r, g, b, a))
				return false;

			if (!DrawFilledBox(x, y, iThickness, h, r, g, b, a))
				return false;

			if (!DrawFilledBox(x + w, y, iThickness, h + iThickness, r, g, b, a))
				return false;

			return DrawFilledBox(x, y + h, w, iThickness, r, g, b, a);
		}

		bool DrawFilledBox(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a)
		{
			//Draw a filled box on backbuffer

			if (!this->m_pDevice)
				return false;

			IDirect3DSurface9* pSurface = nullptr;

			//End drawing sprites
			if (FAILED(this->m_pSpriteMgr->End()))
				return false;

			//Create surface
			if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(w, h, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pSurface, nullptr)))
				return false;

			//Fill surface with color
			if (FAILED(this->m_pDevice->ColorFill(pSurface, nullptr, D3DCOLOR_ARGB(a, r, g, b)))) {
				pSurface->Release();
				return false;
			}

			//Set destination area
			RECT rTargetRect;
			rTargetRect.left = x;
			rTargetRect.top = y;
			rTargetRect.right = x + w;
			rTargetRect.bottom = y + h;

			//Copy surface into backbuffer at desired position
			bool bResult = SUCCEEDED(this->m_pDevice->StretchRect(pSurface, nullptr, this->m_pBackBuffer, &rTargetRect, D3DTEXF_NONE));

			//Release surface
			pSurface->Release();

			//Begin drawing sprites
			if (FAILED(this->m_pSpriteMgr->Begin(D3DXSPRITE_ALPHABLEND)))
				return false;

			return bResult;
		}

		bool DrawLine(int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, BYTE a)
		{
			//Draw a line on backbuffer

			if (!this->m_pDevice)
				return false;

			//End drawing sprites
			if (FAILED(this->m_pSpriteMgr->End()))
				return false;

			//Define custom vertex type
			struct D3DVERTEX {
				float x, y, z, rhw;
				D3DCOLOR d3dColor;
			};

			//Setup vertex buffer
			D3DVERTEX vVertices[2] = {
				{ (float)x1, (float)y1, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b) }, //Line source point
				{ (float)x2, (float)y2, 0.0f, 0.0f, D3DCOLOR_ARGB(a, r, g, b) } //Line destination point
			};

			//Set custom vertex format (Vertex has x, y, z, rhw and color member)
			if (FAILED(this->m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE)))
				return false;

			//Draw line primitive
			bool result = this->m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, sizeof(vVertices) / sizeof(D3DVERTEX), vVertices, sizeof(D3DVERTEX)) == D3D_OK;

			//Begin drawing sprites
			if (FAILED(this->m_pSpriteMgr->Begin(D3DXSPRITE_ALPHABLEND)))
				return false;

			return result;
		}

		bool DrawImage(const HD3DIMAGE hImage, int x, int y)
		{
			//Draw image from list

			//Check image handle for validity
			if (!hImage)
				return false;

			//End drawing sprites
			if (FAILED(this->m_pSpriteMgr->End()))
				return false;

			//Setup rect data
			RECT rDstRect;
			rDstRect.top = y;
			rDstRect.left = x;
			rDstRect.right = x + hImage->d3dImageInfo.Width;
			rDstRect.bottom = y + hImage->d3dImageInfo.Height;

			//Copy image surface to backbuffer
			bool result = SUCCEEDED(this->m_pDevice->StretchRect(hImage->pSurface, nullptr, this->m_pBackBuffer, &rDstRect, D3DTEXF_NONE));

			//Begin drawing sprites
			if (FAILED(this->m_pSpriteMgr->Begin(D3DXSPRITE_ALPHABLEND)))
				return false;

			return result;
		}

		bool DrawSprite(const HD3DSPRITE hSprite, int x, int y, int iFrame, float fRotation, int rotx, int roty, float fScale1, float fScale2, const bool bUseCustomColorMask, byte r, byte g, byte b, byte a)
		{
			//Add sprite to drawing list
			
			if (!hSprite)
				return false;

			//Find sprite
			size_t uiSprite = this->FindSprite(hSprite);
			if (uiSprite == GFX_INVALID_LIST_ID)
				return false;
			
			if ((!this->m_vSprites[uiSprite].pTexture) || (!this->m_pSpriteMgr))
				return false;
			
			//Calculate horizontal line ID
			int iFrameLineId = (this->m_vSprites[uiSprite].iFramesPerLine > 0) ? iFrame / this->m_vSprites[uiSprite].iFramesPerLine : 0;
			//Calculate vertical frame ID
			int iFrameVerticalId = (this->m_vSprites[uiSprite].iFramesPerLine > 0) ? iFrame % this->m_vSprites[uiSprite].iFramesPerLine : 0;

			//Calculate x position of frame
			int iFrameXPos = iFrameVerticalId * this->m_vSprites[uiSprite].iFrameWidth;
			//Calculate y position of frame
			int iFrameYPos = iFrameLineId * this->m_vSprites[uiSprite].iFrameHeight;

			//Setup rectangle info
			RECT sRect;
			sRect.top = iFrameYPos;
			sRect.left = iFrameXPos;
			sRect.bottom = iFrameYPos + this->m_vSprites[uiSprite].iFrameHeight;
			sRect.right = iFrameXPos + this->m_vSprites[uiSprite].iFrameWidth;

			//Setup vector with position
			D3DXVECTOR2 vPosition = { (float)x, (float)y };

			//Setup vector with sprite center
			D3DXVECTOR2 vRotCenter;
			if ((rotx != -1) && (roty != -1))
				vRotCenter = { (float)(rotx), (float)(roty) };
			else
				vRotCenter = { (float)(this->m_vSprites[uiSprite].iFrameWidth / 2) , (float)(this->m_vSprites[uiSprite].iFrameHeight / 2) };

			D3DXMATRIX vNewMatrix;

			//Setup scale vector
			D3DXVECTOR2 vScale(fScale1, fScale2);

			//Calculate transformation matrix
			if (!D3DXMatrixTransformation2D(&vNewMatrix, ((fScale1 != 0.0f) && (fScale2 != 0.0f)) ? &vRotCenter : nullptr, 0.0f, ((fScale1 != 0.0f) && (fScale2 != 0.0f)) ? &vScale : nullptr, (D3DXVECTOR2*)&vRotCenter, fRotation, &vPosition))
				return false;

			//Apply transformation
			if (FAILED(this->m_pSpriteMgr->SetTransform(&vNewMatrix)))
				return false;
			
			//Add sprite to batch list
			return (SUCCEEDED(this->m_pSpriteMgr->Draw(this->m_vSprites[uiSprite].pTexture, &sRect, nullptr, nullptr, (bUseCustomColorMask) ? D3DCOLOR_RGBA(r, g, b, a) : 0xFFFFFFFF)));
		}
		bool DrawSprite(const HD3DSPRITE hSprite, int x, int y, int iFrame, float fRotation) { return this->DrawSprite(hSprite, x, y, iFrame, fRotation, 0.0f, 0.0f); }
		bool DrawSprite(const HD3DSPRITE hSprite, int x, int y, int iFrame, float fRotation, float fScale1, float fScale2) { return this->DrawSprite(hSprite, x, y, iFrame, fRotation, fScale1, fScale2, false, 0, 0, 0, 0); }
		bool DrawSprite(const HD3DSPRITE hSprite, int x, int y, int iFrame, float fRotation, float fScale1, float fScale2, const bool bUseCustomColorMask, byte r, byte g, byte b, byte a) { return this->DrawSprite(hSprite, x, y, iFrame, fRotation, -1, -1, fScale1, fScale2, false, 0, 0, 0, 0); }
		
		bool StoreScreenshotToDisk(const std::wstring& wszTargetFile, D3DXIMAGE_FILEFORMAT fmt = D3DXIFF_PNG)
		{
			//Store a screenshot file with content of the frontbuffer

			if ((!wszTargetFile.length()) || (!this->m_pInterface) || (!this->m_pDevice))
				return false;

			IDirect3DSurface9* pSurface = nullptr;
			D3DDISPLAYMODE sDisplayMode;

			//Get display mode
			if (FAILED(this->m_pDevice->GetDisplayMode(0, &sDisplayMode)))
				return false;

			//Create surface
			if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(sDisplayMode.Width, sDisplayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, nullptr)))
				return false;

			//Copy frontbuffer content into surface
			if (FAILED(this->m_pDevice->GetFrontBufferData(0, pSurface))) {
				pSurface->Release();
				return false;
			}
			
			//Save surface content to file with Bitmap format
			if (FAILED(D3DXSaveSurfaceToFile(wszTargetFile.c_str(), fmt, pSurface, nullptr, nullptr))) {
				pSurface->Release();
				return false;
			}

			//Release surface
			pSurface->Release();

			return true;
		}

		void SetClearColor(D3DCOLOR d3dcColor) { this->m_d3dcClearColor = d3dcColor; }
		D3DCOLOR GetClearColor(void) { return this->m_d3dcClearColor; }
		
		bool SetBackgroundPicture(const std::wstring& wszPictureFile)
		{
			//Set background image

			if (!wszPictureFile.length()) { //Clear picture
				if (this->m_pImageSurface) {
					this->m_pImageSurface->Release(); //Release surface
					this->m_pImageSurface = nullptr; //Set to nullptr
				}
			}
			else { //Create new surface with picture
				D3DXIMAGE_INFO d3dimgInfo;

				if (FAILED(D3DXGetImageInfoFromFile(wszPictureFile.c_str(), &d3dimgInfo))) //Get image info
					return false;
				
				if (FAILED(this->m_pDevice->CreateOffscreenPlainSurface(this->GetWindowWidth(), this->GetWindowHeight(), D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &this->m_pImageSurface, nullptr))) //Create surface for image
					return false;
				
				if (FAILED(D3DXLoadSurfaceFromFile(this->m_pImageSurface, nullptr, nullptr, wszPictureFile.c_str(), nullptr, D3DX_DEFAULT, 0, &d3dimgInfo))) { //Load image into surface
					this->m_pImageSurface->Release();
					this->m_pImageSurface = nullptr;
					return false;
				}
			}

			return true;
		}

		inline INT GetWindowWidth(void) { return this->m_iWidth; }
		inline INT GetWindowHeight(void) { return this->m_iHeight; }
		inline HWND GetOwnerWindow(void) { return this->m_hWnd; }
	};
}
