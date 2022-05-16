#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include "vars.h"
#include "utils.h"
#include "scriptint.h"

/* Entity environment */
namespace Entity {
	enum MovementDir { MOVE_FORWARD, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT, MOVE_NORTH, MOVE_SOUTH, MOVE_WEST, MOVE_EAST };

	struct Color {
		Color() {}
		Color(byte cr, byte cg, byte cb, byte ca) : r(cr), g(cg), b(cb), a(ca) {}
		~Color() {}

		byte r, g, b, a;

		byte GetR(void) { return r; }
		byte GetG(void) { return g; }
		byte GetB(void) { return b; }
		byte GetA(void) { return a; }

		void operator=(const Color& ref) { this->r = ref.r; this->g = ref.g; this->b = ref.b; this->a = ref.a; }

		//AngelScript interface methods
		void Constr_Bytes(byte cr, byte cg, byte cb, byte ca) { r = cr; g = cg; b = cb; a = ca; }
		void Construct(void* pMemory) { new (pMemory) Color(); }
		void Destruct(void* pMemory) { ((Color*)pMemory)->~Color(); }
	};

	/* Timer utility class */
	class CTimer {
	private:
		bool m_bStarted;
		DWORD m_dwInitial;
		DWORD m_dwCurrent;
		DWORD m_dwDelay;
	public:
		CTimer() {}
		//CTimer() : m_bStarted(false) {}
		CTimer(DWORD dwDelay) { this->SetDelay(dwDelay); this->SetActive(true); }
		~CTimer() {}

		void Reset(void)
		{
			//Reset timer

			this->m_dwCurrent = this->m_dwInitial = GetTickCount();
		}

		void Update(void)
		{
			//Update current value

			this->m_dwCurrent = GetTickCount();
		}

		//Setters
		void SetActive(bool bStatus) { this->m_bStarted = bStatus; }
		void SetDelay(DWORD dwDelay) { this->m_dwDelay = dwDelay; }

		//Getters
		const bool Started(void) const { return this->m_bStarted; }
		const DWORD Delay(void) const { return this->m_dwDelay; }
		const bool Elapsed(void) const { return this->m_dwCurrent >= this->m_dwInitial + this->m_dwDelay; }

		//AngelScript interface methods
		void Constr_Delay(DWORD dwDelay) { this->SetDelay(dwDelay); this->SetActive(true); }
		void Construct(void* pMemory) { new (pMemory) CTimer(); }
		void Destruct(void* pMemory) { ((CTimer*)pMemory)->~CTimer(); }
	};

	/* Entity screen position manager */
	struct Vector {
		//Constructors
		Vector() {}
		Vector(int x, int y) { elem[0] = x; elem[1] = y; }
		Vector(const Vector& ref) { elem[0] = ref.GetX(); elem[1] = ref.GetY(); }

		//Getters
		inline int GetX(void) const { return elem[0]; }
		inline int GetY(void) const { return elem[1]; }

		//Setters
		inline void SetX(int x) { elem[0] = x; }
		inline void SetY(int y) { elem[1] = y; }

		//Overriden operators
		inline int& operator[](int i) { return elem[i]; }
		inline int operator[](int i) const { return elem[i]; }
		inline bool operator==(const Vector& ref) { return (elem[0] == ref[0]) && (elem[1] == ref[1]); }
		inline void operator=(const Vector& ref) { elem[0] = ref.GetX(); elem[1] = ref.GetY(); }
		inline void operator++(int iIndex) { this->elem[iIndex]++; }
		inline void operator--(int iIndex) { this->elem[iIndex]--; }
		inline Vector operator+(const Vector& v) { Vector res; res[0] = this->elem[0] + v[0]; res[1] = this->elem[1] + v[1]; return res; }
		inline Vector operator-(const Vector& v) { Vector res; res[0] = this->elem[0] - v[0]; res[1] = this->elem[1] - v[1]; return res; }
		inline Vector operator*(const Vector& v) { Vector res; res[0] = this->elem[0] * v[0]; res[1] = this->elem[1] * v[1]; return res; }
		inline Vector operator/(const Vector& v) { Vector res; res[0] = this->elem[0] / v[0]; res[1] = this->elem[1] / v[1]; return res; }

		//Utils
		int Distance(const Vector& ref)
		{
			//Get distance from this to reference vector
			Vector vDiff;
			vDiff[0] = ref[0] - elem[0];
			vDiff[1] = ref[1] - elem[1];
			return (int)(sqrt(vDiff[0] * vDiff[0] + vDiff[1] * vDiff[1]));
		}
		void Zero(void)
		{
			//Zero values
			elem[0] = elem[1] = 0;
		}

		//AngelScript interface methods
		void Constr_Class(const Vector& src) { this->SetX(src.GetX()); this->SetY(src.GetY()); }
		void Constr_Ints(int x, int y) { this->SetX(x); this->SetY(y); }
		void Construct(void* pMemory) { new (pMemory) Vector(); }
		void Destruct(void* pMemory) { ((Vector*)pMemory)->~Vector(); }
	private:
		int elem[2];
	};

	/* Sprite info data container */
	struct SpriteInfo {
		SpriteInfo() {}
		SpriteInfo(const Vector& v, int d, int f) : res(v), depth(d), format(f) {}
		~SpriteInfo() {}

		Vector res;
		int depth;
		int format;

		Vector& GetResolution(void) { return res; }
		int GetDepth(void) { return depth; }
		int GetFormat(void) { return format; }

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) Color(); }
		void Destruct(void* pMemory) { ((Color*)pMemory)->~Color(); }
	};

	/* Bounding Box handler */
	class CBoundingBox {
	private:
		struct bbox_item_s {
			Vector pos;
			Vector size;
		};

		std::vector<bbox_item_s>* m_pvBBoxItems;

		void Release(void)
		{
			//Clear data

			if (!this->m_pvBBoxItems)
				return;

			this->m_pvBBoxItems->clear();

			delete this->m_pvBBoxItems;
		}

		bool IsItemCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const bbox_item_s& rBBoxItem) const
		{
			//Check if bbox item collides with any of own items

			if (!this->m_pvBBoxItems)
				return false;

			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Iterate through items
				bbox_item_s item = (*this->m_pvBBoxItems)[i];
				//Check if bbox item is inside current iterated item
				if (((vRefAbsPos[0] + rBBoxItem.pos[0] >= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0]) && (vRefAbsPos[0] + rBBoxItem.pos[0] <= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0] + (*this->m_pvBBoxItems)[i].size[0])) && ((vRefAbsPos[1] + rBBoxItem.pos[1] >= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1]) && (vRefAbsPos[1] + rBBoxItem.pos[1] <= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1] + (*this->m_pvBBoxItems)[i].size[1]))) {
					return true;
				}
			}

			return false;
		}
	public:
		CBoundingBox() {}
		~CBoundingBox() { this->Release(); }

		bool Alloc(void)
		{
			//Allocate memory for vector

			this->m_pvBBoxItems = new std::vector<bbox_item_s>;

			return this->m_pvBBoxItems != nullptr;
		}

		void AddBBoxItem(const Vector& pos, const Vector& size)
		{
			//Add bounding box item

			if (!this->m_pvBBoxItems)
				return;

			//Setup data
			bbox_item_s sItem;
			sItem.pos = pos;
			sItem.size = size;

			//Add to list
			this->m_pvBBoxItems->push_back(sItem);
		}

		bool IsCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const CBoundingBox& roBBox)
		{
			//Check if bounding boxes collide with each other

			if (!this->m_pvBBoxItems)
				return false;

			if (roBBox.IsEmpty())
				return false;

			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Iterate through own bbox list
				//Check if current item collides ref bbox
				if (roBBox.IsItemCollided(vRefAbsPos, vMyAbsPos, (*this->m_pvBBoxItems)[i]))
					return true;
			}

			return false;
		}

		bool IsInside(const Vector& vMyAbsPos, const Vector& vPosition)
		{
			//Check if position is inside bbox

			if (!this->m_pvBBoxItems)
				return false;

			if (this->IsEmpty())
				return false;

			for (size_t i = 0; i < this->m_pvBBoxItems->size(); i++) { //Loop through all bbox i tems
				//Check if position is inside
				if ((vPosition[0] >= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0]) && (vPosition[0] <= vMyAbsPos[0] + (*this->m_pvBBoxItems)[i].pos[0] + (*this->m_pvBBoxItems)[i].size[0]) && (vPosition[1] >= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1]) && (vPosition[1] <= vMyAbsPos[1] + (*this->m_pvBBoxItems)[i].pos[1] + (*this->m_pvBBoxItems)[i].size[1])) {
					return true;
				}
			}

			return false;
		}

		inline bool IsEmpty(void) const { return (this->m_pvBBoxItems) ? (this->m_pvBBoxItems->size() == 0) : true; }
		inline void Clear(void) { if (this->m_pvBBoxItems) this->m_pvBBoxItems->clear(); }
		inline size_t Count(void) const { return this->m_pvBBoxItems->size(); }
		inline const bbox_item_s& Item(const size_t uiItem) const { return (*this->m_pvBBoxItems)[uiItem]; }

		void operator=(const CBoundingBox& ref)
		{
			//Copy data from other bbox

			if (!this->m_pvBBoxItems) {
				this->Alloc();
			}

			this->m_pvBBoxItems->clear();

			for (size_t i = 0; i < ref.Count(); i++) {
				this->m_pvBBoxItems->push_back(ref.Item(i));
			}
		}

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) CBoundingBox(); }
		void Destruct(void* pMemory) { ((CBoundingBox*)pMemory)->~CBoundingBox(); }
	};

	/* Sprite component */
	class CTempSprite {
	private:
		bool m_bInfinite;
		CTimer m_oTimer;
		DxRenderer::HD3DSPRITE m_hSprite;
		int m_iCurrentFrame;
		int m_iMaxFrames;
	public:
		CTempSprite() {}
		CTempSprite(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize) { this->Initialize(szTexture, dwSwitchDelay, bInfinite, iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize); }
		~CTempSprite() {}

		bool Initialize(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize)
		{
			//Initialize sprite object

			//Load sprite
			this->m_hSprite = pRenderer->LoadSprite(Utils::ConvertToWideString(szTexture), iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize);
			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return false;

			//Initialize timer
			this->m_oTimer = CTimer(dwSwitchDelay);

			//Store indicator
			this->m_bInfinite = bInfinite;

			//Init frame values
			this->m_iCurrentFrame = 0;
			this->m_iMaxFrames = iFrameCount;

			return true;
		}

		void Draw(const Vector& pos, float fRotation, float fScale1, float fScale2)
		{
			//Draw sprite items

			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return;

			//Draw current sprite frame
			pRenderer->DrawSprite(this->m_hSprite, pos[0], pos[1], this->m_iCurrentFrame, fRotation, fScale1, fScale2);

			//Update timer
			this->m_oTimer.Update();

			//Handle frame change
			if (this->m_oTimer.Elapsed()) {
				this->m_iCurrentFrame++;
				if (this->m_iCurrentFrame >= this->m_iMaxFrames) {
					this->m_iCurrentFrame = 0;

					if (!this->m_bInfinite) {
						this->Release();
					}
				}
			}
		}

		void Release(void)
		{
			//Release resources

			if (this->m_hSprite == GFX_INVALID_SPRITE_ID)
				return;

			//Free sprite
			pRenderer->FreeSprite(this->m_hSprite);

			//Stop timer
			this->m_oTimer.SetActive(false);

			//Clear value
			this->m_hSprite = GFX_INVALID_SPRITE_ID;
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szTexture, DWORD dwSwitchDelay, bool bInfinite, int iFrameCount, int iFrameWidth, int iFrameHeight, int iFramesPerLine, const bool bForceCustomSize) { this->Initialize(szTexture, dwSwitchDelay, bInfinite, iFrameCount, iFrameWidth, iFrameHeight, iFramesPerLine, bForceCustomSize); }
		void Construct(void* pMemory) { new (pMemory) CTempSprite(); }
		void Destruct(void* pMemory) { ((CTempSprite*)pMemory)->~CTempSprite(); }
	};

	/* Model component */
	class CModel {
	private:
		bool m_bReady;
		DxRenderer::HD3DSPRITE m_hSprite;
		CBoundingBox m_oBBox;
		Vector m_vCenter;
	public:
		CModel() {}
		//CModel(const std::string& szInputFile) { this->Initialize(szInputFile); }
		//CModel(const std::string& szInputFile, bool bForceCustomSize) { this->Initialize(szInputFile, bForceCustomSize); }
		~CModel() { this->Release(); }

		bool Initialize(const std::string& szMdlFile, bool bForceCustomSize = false)
		{
			//Initialize model by using source file as input

			std::ifstream hFile;
			std::string szSpriteFile;
			int iaFrameInfos[4];

			//Allocate bounding box
			//if (!this->m_oBBox.Alloc())
			//	return false;

			//Attempt to open file for reading
			hFile.open(szMdlFile, std::ifstream::in);
			if (!hFile.is_open())
				return false;

			char szCurLine[MAX_PATH];

			//Read first line to get the model sprite file
			hFile.getline(szCurLine, sizeof(szCurLine), '\n');
			szSpriteFile = szCurLine;

			//Read second line to get the sprite infos
			hFile.getline(szCurLine, sizeof(szCurLine), '\n');
			std::vector<std::string> vSplit = Utils::Split(szCurLine, " ");
			if (vSplit.size() != 4) { hFile.close(); return false; }
			iaFrameInfos[0] = atoi(vSplit[0].c_str());
			iaFrameInfos[1] = atoi(vSplit[1].c_str());
			iaFrameInfos[2] = atoi(vSplit[2].c_str());
			iaFrameInfos[3] = atoi(vSplit[3].c_str());

			//Read remaining lines to get the bbox infos
			while (!hFile.eof()) {
				hFile.getline(szCurLine, sizeof(szCurLine), '\n');

				Vector vPos, vSize;

				vSplit = Utils::Split(szCurLine, " ");
				if (vSplit.size() != 4) { hFile.close(); return false; }
				vPos[0] = atoi(vSplit[0].c_str());
				vPos[1] = atoi(vSplit[1].c_str());
				vSize[0] = atoi(vSplit[2].c_str());
				vSize[1] = atoi(vSplit[3].c_str());

				//Add to bbox
				this->m_oBBox.AddBBoxItem(vPos, vSize);
			}

			//Close file
			hFile.close();

			//Load sprite
			this->m_hSprite = pRenderer->LoadSprite(Utils::ConvertToWideString(szSpriteFile), iaFrameInfos[0], iaFrameInfos[1], iaFrameInfos[2], iaFrameInfos[3], bForceCustomSize);
			if (!this->m_hSprite)
				return false;

			//Return set indicator
			return this->m_bReady = true;
		}

		bool Initialize2(const CBoundingBox& bbox, const DxRenderer::HD3DSPRITE hSprite)
		{
			//Initialize model by using pre-initialized data objects

			//Copy data
			this->m_hSprite = hSprite;
			this->m_oBBox = bbox;

			//Return set indicator
			return this->m_bReady = true;
		}

		void Release(void)
		{
			//Release resources

			if (!this->m_bReady)
				return;

			//Free sprite
			pRenderer->FreeSprite(this->m_hSprite);

			//Clear bbox data
			this->m_oBBox.Clear();

			//Clear indicator
			this->m_bReady = false;
		}

		bool IsCollided(const Vector& vMyAbsPos, const Vector& vRefAbsPos, const CModel& rRefMdl)
		{
			//Check for collision

			return this->m_oBBox.IsCollided(vMyAbsPos, vRefAbsPos, rRefMdl.BBox());
		}

		bool IsInside(const Vector& vMyAbsPos, const Vector& vPosition)
		{
			//Check if inside

			return this->m_oBBox.IsInside(vMyAbsPos, vPosition);
		}

		inline bool Alloc(void) { return this->m_oBBox.Alloc(); }
		inline void SetCenter(const Vector& vCenter) { this->m_vCenter = vCenter; }

		//Getters
		inline bool IsValid(void) const { return this->m_bReady; }
		inline const CBoundingBox& BBox(void) const { return this->m_oBBox; }
		inline const DxRenderer::HD3DSPRITE Sprite(void) const { return this->m_hSprite; }
		inline const Vector& Center(void) const { return this->m_vCenter; }

		//Assign operator
		void operator=(const CModel& ref)
		{
			this->m_hSprite = ref.Sprite();
			this->m_oBBox = ref.BBox();
			this->m_vCenter = ref.Center();
		}

		//AngelScript interface methods
		void Constr_IntputStr(const std::string& szInputFile) { this->Initialize(szInputFile); }
		void Constr_IntputStrBool(const std::string& szInputFile, bool bForceCustomSize) { this->Initialize(szInputFile, bForceCustomSize); }
		void Construct(void* pMemory) { new (pMemory) CModel(); }
		void Destruct(void* pMemory) { ((CModel*)pMemory)->~CModel(); }
	};

	/* Managed entity component */
	class CScriptedEntity {
	public:
		typedef byte DamageValue;
	private:
		std::string m_szClassName;
		Scripting::HSISCRIPT m_hScript;
		asIScriptObject* m_pScriptObject;

		void Release(void)
		{
			//Release resources

			this->m_pScriptObject->Release();
			this->m_pScriptObject = nullptr;
		}
	public:
		CScriptedEntity(const Scripting::HSISCRIPT hScript, asIScriptObject* pObject) : m_pScriptObject(pObject), m_hScript(hScript) {}
		CScriptedEntity(const Scripting::HSISCRIPT hScript, const std::string& szClassName) : m_szClassName(szClassName) { this->Initialize(hScript, szClassName); }
		~CScriptedEntity() { this->Release(); }

		bool Initialize(const Scripting::HSISCRIPT hScript, const std::string& szClassName)
		{
			//Initialize scripted entity

			if (!szClassName.length())
				return false;

			//Allocate class instance
			this->m_pScriptObject = pScriptingInt->AllocClass(hScript, szClassName);

			//Store data
			this->m_hScript = hScript;

			return this->m_pScriptObject != nullptr;
		}

		void OnSpawn(const Vector& vAtPos)
		{
			//Inform class instance of event

			Vector v(vAtPos);

			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&v);

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnSpawn(const Vector& in)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		void OnRelease(void)
		{
			//Inform class instance of event
			
			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnRelease()", nullptr, nullptr);
		}

		void OnProcess(void)
		{
			//Inform class instance of event

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnProcess()", nullptr, nullptr);
		}

		void OnDraw(void)
		{
			//Inform class instance of event

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDraw()", nullptr, nullptr);
		}

		void OnDrawOnTop(void)
		{
			//Inform class instance of event

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDrawOnTop()", nullptr, nullptr);
		}

		void OnWallCollided(void)
		{
			//Inform class instance of event

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnWallCollided()", nullptr, nullptr);
		}

		bool IsCollidable(void)
		{
			//Query if entity is collidable

			bool bResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool IsCollidable()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		void OnCollided(asIScriptObject* ref)
		{
			//Inform of being collided
			
			BEGIN_PARAMS(vArgs);
			PUSH_POINTER(ref);

			ref->AddRef();

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnCollided(IScriptedEntity@)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		void OnDamage(unsigned int damageValue)
		{
			//Inform of being collided
			
			BEGIN_PARAMS(vArgs);
			PUSH_DWORD(damageValue);

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void OnDamage(uint32)", &vArgs, nullptr);

			END_PARAMS(vArgs);
		}

		CModel* GetModel(void)
		{
			//Query model object pointer

			CModel* pResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Model& GetModel()", nullptr, &pResult, Scripting::FA_OBJECT);

			return pResult;
		}

		Vector GetPosition(void)
		{
			//Query position

			Vector* pResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Vector& GetPosition()", nullptr, &pResult, Scripting::FA_OBJECT);

			return *pResult;
		}

		void SetPosition(const Vector& vec)
		{
			//Set position

			Vector vPos = vec;

			BEGIN_PARAMS(vArgs);
			PUSH_OBJECT(&vPos);

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void SetPosition(const Vector &in)", &vArgs, nullptr, Scripting::FA_VOID);

			END_PARAMS(vArgs);
		}

		float GetRotation(void)
		{
			//Query rotation

			float flResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "float GetRotation()", nullptr, &flResult, Scripting::FA_FLOAT);
			
			return flResult;
		}

		void SetRotation(float fRot)
		{
			//Set rotation

			BEGIN_PARAMS(vArgs);
			PUSH_FLOAT(fRot);

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "void SetRotation(float)", &vArgs, nullptr, Scripting::FA_VOID);

			END_PARAMS(vArgs);
		}

		Vector GetSize(void)
		{
			//Query selection size

			Vector* pResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "Vector& GetSize()", nullptr, &pResult, Scripting::FA_OBJECT);

			return *pResult;
		}

		bool NeedsRemoval(void)
		{
			//Indicate if entity needs removal

			bool bResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool NeedsRemoval()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		bool CanBeDormant(void)
		{
			//Indicate if entity can be dormant

			bool bResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "bool CanBeDormant()", nullptr, &bResult, Scripting::FA_BYTE);

			return bResult;
		}

		std::string GetName(void)
		{
			//Get name from class instance

			std::string szResult;

			pScriptingInt->CallScriptMethod(this->m_hScript, this->m_pScriptObject, "string GetName()", nullptr, &szResult, Scripting::FA_STRING);

			return szResult;
		}

		//Getters
		inline bool IsReady(void) const { return (this->m_pScriptObject != nullptr); }
		inline asIScriptObject* Object(void) const { return this->m_pScriptObject; }
	};

	/* Scripted entity manager */
	class CScriptedEntsMgr {
	public:
		struct playerentity_s {
			Scripting::HSISCRIPT hScript;
			asIScriptObject* pObject;
		};
	private:
		std::vector<CScriptedEntity*> m_vEnts;
		playerentity_s m_sPlayerEntity;

		bool IsEntityDormant(CScriptedEntity* pEntity)
		{
			//Check if entity is considered being dormant

			const int C_DISTANCE_ADDITION = 200;

			//Get player position
			Vector* vecPlayerPos = nullptr;
			pScriptingInt->CallScriptMethod(this->m_sPlayerEntity.hScript, this->m_sPlayerEntity.pObject, "Vector& GetPosition()", nullptr, &vecPlayerPos, Scripting::FA_OBJECT);

			if (vecPlayerPos) {
				Vector vecEntityPos = pEntity->GetPosition();
	
				//Calculate distance between player and entity
				int iDistance = vecPlayerPos->Distance(vecEntityPos);
				int iDormantDistance = 0;

				//Select greater resolution dimension
				if (pWindow->GetResolutionX() > pWindow->GetResolutionY()) {
					iDormantDistance = pWindow->GetResolutionX() / 2;
				} else {
					iDormantDistance = pWindow->GetResolutionY() / 2;
				}

				//Add constant value to let max value be outside of screen
				iDormantDistance += C_DISTANCE_ADDITION;

				//Return indication if player to entity distance is greater than dormant range
				return iDistance > iDormantDistance;
			}

			return true;
		}
	public:
		CScriptedEntsMgr() {}
		~CScriptedEntsMgr() { this->Release(); }

		bool Spawn(const std::wstring& wszIdent, asIScriptObject* pObject, const Vector& vAtPos);

		void Process(void);

		void Draw(void)
		{
			//Inform entities

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->CanBeDormant()) {
					if (this->IsEntityDormant(this->m_vEnts[i])) {
						continue;
					}
				}

				this->m_vEnts[i]->OnDraw();
			}
		}

		void DrawOnTop(void)
		{
			//Inform entities

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->CanBeDormant()) {
					if (this->IsEntityDormant(this->m_vEnts[i])) {
						continue;
					}
				}

				this->m_vEnts[i]->OnDrawOnTop();
			}
		}

		void Release(void)
		{
			//Release resources
			
			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				this->m_vEnts[i]->OnRelease();
				delete this->m_vEnts[i];
			}

			this->m_vEnts.clear();

			this->m_sPlayerEntity.hScript = 0;
			this->m_sPlayerEntity.pObject = nullptr;
		}

		//Entity querying
		size_t GetEntityCount() { return this->m_vEnts.size(); }
		size_t GetEntityNameCount(const std::string& szName)
		{
			size_t uiResult = 0;

			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->GetName() == szName) {
					uiResult++;
				}
			}

			return uiResult;
		}
		CScriptedEntity* GetEntity(size_t uiEntityId)
		{
			if (uiEntityId >= this->m_vEnts.size())
				return nullptr;

			return this->m_vEnts[uiEntityId];
		}
		asIScriptObject* GetEntityHandle(size_t uiEntityId)
		{
			if (uiEntityId >= this->m_vEnts.size())
				return nullptr;

			if (!this->m_vEnts[uiEntityId])
				return nullptr;

			return this->m_vEnts[uiEntityId]->Object();
		}
		bool IsValidEntity(asIScriptObject* pEntity)
		{
			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->Object() == pEntity)
					return true;
			}

			return false;
		}
		size_t GetEntityId(asIScriptObject* pEntity)
		{
			for (size_t i = 0; i < this->m_vEnts.size(); i++) {
				if (this->m_vEnts[i]->Object() == pEntity)
					return i;
			}

			return (size_t)-1;
		}
		const playerentity_s& GetPlayerEntity(void) const { return this->m_sPlayerEntity; }
	};

	extern CScriptedEntsMgr oScriptedEntMgr;

	class CEntityTrace { //Entity tracing utility class
	private:
		struct tracedata_s {
			Vector vStart;
			Vector vEnd;
			asIScriptObject* pIgnoreEnt;
		};

		std::vector<CScriptedEntity*> m_vEntities;
		tracedata_s m_sTraceData;

		void IncreaseVec(Vector& vVector)
		{
			//Increase vector value to move to the next position in direction to the end position

			if (vVector[0] != this->m_sTraceData.vEnd[0]) { (this->m_sTraceData.vEnd[0] - vVector[0] > 0) ? vVector[0]++ : vVector[0]--; }
			if (vVector[1] != this->m_sTraceData.vEnd[1]) { (this->m_sTraceData.vEnd[1] - vVector[1] > 0) ? vVector[1]++ : vVector[1]--; }
		}

		bool EndReached(const Vector& vCurPos)
		{
			//Indicate whether end has been reached

			return (vCurPos[0] == this->m_sTraceData.vEnd[0]) && (vCurPos[1] == this->m_sTraceData.vEnd[1]);
		}

		void EnumerateEntitiesAtPosition(const Vector& vPosition, asIScriptObject* pIgnoreEnt)
		{
			//Create a list with all entities at the given position

			//Clear list
			this->m_vEntities.clear();

			for (size_t i = 0; i < oScriptedEntMgr.GetEntityCount(); i++) { //Loop through all entities
				CScriptedEntity* pEntity = oScriptedEntMgr.GetEntity(i); //Get entity pointer
				if ((pEntity) && (pEntity->Object() != pIgnoreEnt) && (pEntity->IsCollidable())) { //Use only valid collidable entities
					//Obtain model pointer
					CModel* pModel = pEntity->GetModel();
					if (pModel) {
						if (pModel->IsInside(pEntity->GetPosition(), vPosition)) { //Check if position is inside
							//Add to list
							this->m_vEntities.push_back(pEntity);
						}
					}
				}
			}
		}
	public:
		CEntityTrace() {}
		CEntityTrace(const Vector& vStart, const Vector& vEnd, asIScriptObject* pIgnoreEnt)
		{
			this->SetStart(vStart);
			this->SetEnd(vEnd);
			this->SetIgnoredEnt(pIgnoreEnt);
			this->Run();
		}
		~CEntityTrace() { this->m_vEntities.clear(); }

		//Action
		void Run(void)
		{
			//Run trace computation

			//Initialize with start position
			Vector vCurrentPosition = this->m_sTraceData.vStart;

			while (!this->EndReached(vCurrentPosition)) { //Loop as long as end position has not yet been reached
				//Enumerate all entities at current position
				this->EnumerateEntitiesAtPosition(vCurrentPosition, this->m_sTraceData.pIgnoreEnt);
				if (!this->IsEmpty()) { //Break out if at least one entity has been found
					break;
				}

				//Update current position
				this->IncreaseVec(vCurrentPosition);
			}
		}

		//Setters
		inline void SetStart(const Vector& vVector) { this->m_sTraceData.vStart = vVector; }
		inline void SetEnd(const Vector& vVector) { this->m_sTraceData.vEnd = vVector; }
		inline void SetIgnoredEnt(asIScriptObject* pIgnoreEnt) { this->m_sTraceData.pIgnoreEnt = pIgnoreEnt; }

		//Getters
		inline const bool IsEmpty(void) const { return this->m_vEntities.size() == 0; }
		inline asIScriptObject* EntityObject(const size_t uiId) { if (uiId >= this->m_vEntities.size()) return nullptr; return this->m_vEntities[uiId]->Object(); }
		inline const size_t EntityCount(void) const { return this->m_vEntities.size(); }
	};

	/* Solid sprite class */
	class CSolidSprite {
	private:
		std::vector<DxRenderer::HD3DSPRITE> m_vSprites;
		Vector m_vecPos;
		Vector m_vecSize;
		int m_iDir;
		float m_fRotation;
		bool m_bWall;
	public:
		CSolidSprite() {}
		~CSolidSprite() {}

		bool Initialize(int x, int y, int w, int h, const std::wstring& wszFile, int repeat, int dir, float rot, bool wall)
		{
			//Initialize entity
			
			//Set default value
			if (!repeat) {
				repeat = 1;
			}

			//Store data
			this->m_vecPos = Vector(x, y);
			this->m_vecSize = Vector(w, h);
			this->m_iDir = dir;
			this->m_fRotation = rot;
			this->m_bWall = wall;

			//Load repated sprites if any
			for (size_t i = 0; i < repeat; i++) {
				DxRenderer::HD3DSPRITE hSprite = pRenderer->LoadSprite(wszFile, 1, w, h, 1, false);
				if (hSprite == GFX_INVALID_SPRITE_ID) {
					return false;
				}
				
				this->m_vSprites.push_back(hSprite);
			}

			return true;
		}

		void Draw(void);
		void Process(void);

		bool IsVectorFieldCollided(const Vector& vecPos, const Vector& vecSize)
		{
			//Check if vector field is collided with entity

			Vector vecEntireSize;

			//Calculate entire size according to direction
			if (this->m_iDir == 0) {
				vecEntireSize[0] = (int)this->m_vSprites.size() * this->m_vecSize[0];
				vecEntireSize[1] = this->m_vecSize[1];
			} else {
				vecEntireSize[0] = this->m_vecSize[0];
				vecEntireSize[1] = (int)this->m_vSprites.size() * this->m_vecSize[1];
			}

			//Check collision
			if (((vecPos[0] > this->m_vecPos[0]) && (vecPos[0] < this->m_vecPos[0] + vecEntireSize[0]) && (vecPos[1] > this->m_vecPos[1]) && (vecPos[1] < this->m_vecPos[1] + vecEntireSize[1]))
				|| ((vecPos[0] + vecSize[0] > this->m_vecPos[0]) && (vecPos[0] + vecSize[0] < this->m_vecPos[0] + vecEntireSize[0]) && (vecPos[1] + vecSize[1] > this->m_vecPos[1]) && (vecPos[1] + vecSize[1] < this->m_vecPos[1] + vecEntireSize[1]))) {
				return true;
			}

			return false;
		}

		void Release(void)
		{
			//Free resources

			for (size_t i = 0; i < this->m_vSprites.size(); i++) {
				pRenderer->FreeSprite(this->m_vSprites[i]);
			}

			this->m_vSprites.clear();
		}

		//Getters
		const bool IsWall(void) const { return this->m_bWall; }
		const Vector& GetPosition(void) { return this->m_vecPos; }
	};

	/* Goal entity class */
	const size_t GOAL_ENTITY_MAX_SPRITES = 64;
	class CGoalEntity {
	private:
		std::vector<DxRenderer::HD3DSPRITE> m_vSprites;
		Vector m_vecPosition;
		Vector m_vecSize;
		int m_iCurrentFrame;
		CTimer m_tmrFrameChange;
		std::wstring m_wszGoal;
		bool m_bGoalReached;
		bool m_bActivated;
	public:
		CGoalEntity()
		{
			//Initialize entity

			this->m_bGoalReached = false;
			this->m_bActivated = true;
			this->m_vecSize = Vector(128, 128);
			this->m_iCurrentFrame = 0;

			for (size_t i = 1; i < GOAL_ENTITY_MAX_SPRITES + 1; i++) {
				std::wstring wszFileName = L"portal" + ((i < 10) ? L"0" + std::to_wstring(i) : std::to_wstring(i)) + L".png";
				DxRenderer::HD3DSPRITE hSprite = pRenderer->LoadSprite(wszBasePath + L"media\\gfx\\portal\\" + wszFileName, 1, this->m_vecSize[0], this->m_vecSize[1], 1, true);
				this->m_vSprites.push_back(hSprite);
			}

			this->m_tmrFrameChange.SetDelay(50);
			this->m_tmrFrameChange.Reset();
			this->m_tmrFrameChange.SetActive(true);
		}

		~CGoalEntity()
		{
			//Free sprites and clear list

			for (size_t i = 0; i < this->m_vSprites.size(); i++) {
				pRenderer->FreeSprite(this->m_vSprites[i]);
			}

			this->m_vSprites.clear();
		}

		void SetPosition(int x, int y)
		{
			//Set position

			this->m_vecPosition = Vector(x, y);
		}

		void SetGoal(const std::wstring& wszGoal)
		{
			//Set goal

			this->m_wszGoal = wszGoal;
		}

		void SetActivationStatus(bool value)
		{
			//Set activation status

			this->m_bActivated = value;
		}

		//Indicate if goal reached
		bool IsGoalReached(void) const { return this->m_bGoalReached; }

		//Get goal type/map
		const std::wstring& GetGoal(void) const { return this->m_wszGoal; }

		void Draw(void);
		void Process(void)
		{
			//Process entity

			if (!this->m_bActivated)
				return;

			this->m_tmrFrameChange.Update();
			if (this->m_tmrFrameChange.Elapsed()) {
				this->m_tmrFrameChange.Reset();
				
				this->m_iCurrentFrame++;
				if (this->m_iCurrentFrame >= GOAL_ENTITY_MAX_SPRITES) {
					this->m_iCurrentFrame = 0;
				}
			}

			Vector* vecPosition = nullptr;
			Vector* vecSize = nullptr;

			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetPosition()", nullptr, &vecPosition, Scripting::FA_OBJECT);
			pScriptingInt->CallScriptMethod(oScriptedEntMgr.GetPlayerEntity().hScript, oScriptedEntMgr.GetPlayerEntity().pObject, "Vector& GetSize()", nullptr, &vecSize, Scripting::FA_OBJECT);

			if ((!vecPosition) || (!vecSize)) {
				return;
			}

			const int C_GOAL_POS_ADDITION = 10;

			//Check if player is collided with goal entity
			if (((((*vecPosition)[0] >= this->m_vecPosition[0] + this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2) && ((*vecPosition)[0] <= this->m_vecPosition[0] + this->m_vecSize[0] - (this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2)) && ((*vecPosition)[1] >= this->m_vecPosition[1] + this->m_vecSize[1] / 2 - (this->m_vecSize[1] / 2) / 2) && ((*vecPosition)[1] <= this->m_vecPosition[1] + this->m_vecSize[1] - (this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2))))
				|| (((*vecPosition)[0] + (*vecSize)[0] - ((*vecSize)[0] / 2 - ((*vecSize)[0] / 2) / 2) >= this->m_vecPosition[0] + this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2) && ((*vecPosition)[0] + (*vecSize)[0] - ((*vecSize)[0] / 2 - ((*vecSize)[0] / 2) / 2) <= this->m_vecPosition[0] + this->m_vecSize[0] - (this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2)) && ((*vecPosition)[1] + (*vecSize)[1] - ((*vecSize)[1] / 2 - ((*vecSize)[1] / 2) / 2) >= this->m_vecPosition[1] + this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2) && ((*vecPosition)[1] + (*vecSize)[1] - ((*vecSize)[1] / 2 - ((*vecSize)[1] / 2) / 2) <= this->m_vecPosition[1] + this->m_vecSize[1] - (this->m_vecSize[0] / 2 - (this->m_vecSize[0] / 2) / 2)))) {
				if (!this->m_bGoalReached) {
					this->m_bGoalReached = true;
				}
			}
		}
	};

	/* File reader class */
	class CFileReader {
	public:
		enum SeekWay_e {
			SEEKW_BEGIN = std::ios_base::beg,
			SEEKW_CURRENT = std::ios_base::cur,
			SEEKW_END = std::ios_base::end
		};
	private:
		std::ifstream* m_poFile;

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CFileReader() {}
		~CFileReader() { /*this->Release();*/ }

		bool Open(const std::string& szFile)
		{
			//Allocate object and open stream to file

			//if (this->m_poFile)
			//	return false;

			this->m_poFile = new std::ifstream();
			if (!this->m_poFile)
				return false;

			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "packages\\" + szFile, std::ifstream::in);

			return this->m_poFile->is_open();
		}

		bool IsOpen(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return false;

			return this->m_poFile->is_open();
		}

		bool Eof(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return true;

			return this->m_poFile->eof();
		}

		void Seek(int from, int offset)
		{
			//Set file pointer

			if (!this->m_poFile)
				return;

			this->m_poFile->seekg(offset, from);
		}

		std::string GetLine(void)
		{
			//Return current line content

			if (!this->m_poFile)
				return "";

			char szLineBuffer[1024 * 4] = { 0 };

			this->m_poFile->getline(szLineBuffer, sizeof(szLineBuffer), '\n');

			return std::string(szLineBuffer);
		}

		std::string GetEntireContent(bool bSkipNLChar)
		{
			//Return entire file content

			if (!this->m_poFile)
				return "";

			std::string szAllLines;
			this->m_poFile->seekg(0, std::ios_base::beg);
			while (!this->m_poFile->eof()) {
				szAllLines += this->GetLine() + ((bSkipNLChar) ? "" : "\n");
			}

			return szAllLines;
		}

		void Close(void)
		{
			//Close stream and free memory

			this->Release();
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szFile) { this->Open(szFile); }
		void Construct(void* pMemory) { new (pMemory) CFileReader(); }
		void Destruct(void* pMemory) { ((CFileReader*)pMemory)->~CFileReader(); }
	};

	/* File writer class */
	class CFileWriter {
	public:
		enum SeekWay_e {
			SEEKW_BEGIN = std::ios_base::beg,
			SEEKW_CURRENT = std::ios_base::cur,
			SEEKW_END = std::ios_base::end
		};
	private:
		std::ofstream* m_poFile;

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CFileWriter() {}
		~CFileWriter() { /*this->Release();*/ }

		bool Open(const std::string& szFile, bool bAppend = true)
		{
			//Allocate object and open stream to file

			//if (this->m_poFile)
			//	return false;

			this->m_poFile = new std::ofstream();
			if (!this->m_poFile)
				return false;

			int openMode = std::ifstream::out;
			if (bAppend) openMode |= std::ifstream::app;

			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "packages\\" + szFile, openMode);

			return this->m_poFile->is_open();
		}

		bool IsOpen(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return false;

			return this->m_poFile->is_open();
		}

		bool Eof(void)
		{
			//Indicate status

			if (!this->m_poFile)
				return true;

			return this->m_poFile->eof();
		}

		void Seek(int from, int offset)
		{
			//Set file pointer

			if (!this->m_poFile)
				return;

			this->m_poFile->seekp(offset, from);
		}

		void Write(const std::string& szText)
		{
			//Write text to file

			if (!this->m_poFile)
				return;

			this->m_poFile->write(szText.data(), szText.length());
		}

		void WriteLine(const std::string& szText)
		{
			//Write text line to file

			this->Write(szText + "\n");
		}

		void Close(void)
		{
			//Close stream and free memory

			this->Release();
		}

		//AngelScript interface methods
		void Constr_Init(const std::string& szFile) { this->Open(szFile); }
		void Construct(void* pMemory) { new (pMemory) CFileWriter(); }
		void Destruct(void* pMemory) { ((CFileWriter*)pMemory)->~CFileWriter(); }
	};

	/* Save game writer */
	class CSaveGameWriter {
	private:
		std::ofstream* m_poFile;
		std::string* m_pszFileName;

		bool IsValidHandle(void)
		{
			//Check if file is opened 

			return (this->m_poFile != nullptr) && (this->m_poFile->is_open());
		}

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CSaveGameWriter() : m_poFile(nullptr), m_pszFileName(nullptr) {}
		~CSaveGameWriter() {}

		bool BeginSaveGame(void)
		{
			//Begin writing save game

			this->m_poFile = new std::ofstream();
			if (!this->m_poFile)
				return false;

			this->m_pszFileName = new std::string("");
			if (!this->m_pszFileName)
				return false;

			tm time;

			time_t t = std::time(nullptr);
			localtime_s(&time, &t);

			std::ostringstream oss;
			oss << std::put_time(&time, "%d-%m-%Y_%H-%M-%S");
			
			*this->m_pszFileName = std::string("savegame_" + oss.str() + ".sav");

			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "saves\\" + *this->m_pszFileName, std::ifstream::out);
			if (!this->m_poFile->is_open()) {
				delete this->m_poFile;
				return false;
			}

			return true;
		}

		bool WritePackage(const std::string& szPackage)
		{
			//Write package name

			return this->WriteAttribute("package", szPackage);
		}

		bool WriteFromPath(const std::string& szPath)
		{
			//Write frompath attribute

			return this->WriteAttribute("frompath", szPath);
		}

		bool WriteMap(const std::string& szMap)
		{
			//Write map name

			return this->WriteAttribute("map", szMap);
		}

		bool WriteAttribute(const std::string& szName, const std::string& szValue)
		{
			//Write save game attribute

			if (!this->IsValidHandle()) {
				return false;
			}

			std::string szLine = szName + " " + szValue + "\n";

			this->m_poFile->write(szLine.c_str(), szLine.length());

			return true;
		}

		void EndSaveGame(void);

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) CSaveGameWriter(); }
		void Destruct(void* pMemory) { ((CSaveGameWriter*)pMemory)->~CSaveGameWriter(); }
	};

	/* Save game reader */
	class CSaveGameReader {
	public:
		struct save_game_entry_s {
			std::string szIdent;
			std::string szValue;
		};
	private:
		std::ifstream* m_poFile;
		std::vector<save_game_entry_s> m_vData;

		bool IsValidFileHandle(void)
		{
			//Indicate if valid file handle

			return (this->m_poFile != nullptr) && (this->m_poFile->is_open());
		}

		void Release(void)
		{
			//Release resources

			if (this->m_poFile) {
				if (this->m_poFile->is_open()) {
					this->m_poFile->close();
				}

				delete this->m_poFile;
				this->m_poFile = nullptr;
			}
		}
	public:
		CSaveGameReader() : m_poFile(nullptr) {}
		~CSaveGameReader() {}

		bool OpenSaveGameFile(const std::string& szFile)
		{
			//Open save game file

			this->m_poFile = new std::ifstream();
			if (!this->m_poFile)
				return false;

			this->m_poFile->open(Utils::ConvertToAnsiString(wszBasePath) + "saves\\" + szFile, std::ifstream::in);

			return this->m_poFile->is_open();
		}

		void AcquireSaveGameData(void)
		{
			//Acquire all save game data

			if (!this->IsValidFileHandle()) {
				return;
			}

			this->m_poFile->seekg(0, std::ios_base::beg);

			while (!this->m_poFile->eof()) {
				char szLineBuffer[1024 * 4] = { 0 };
				this->m_poFile->getline(szLineBuffer, sizeof(szLineBuffer), '\n');

				std::string szLine = szLineBuffer;

				size_t uiFirstSpace = szLine.find(' ');
				if (uiFirstSpace != std::string::npos) {
					std::string szIdent;
					std::string szValue;

					szIdent = szLine.substr(0, uiFirstSpace);
					szValue = szLine.substr(uiFirstSpace + 1);

					save_game_entry_s sItem;
					sItem.szIdent = szIdent;
					sItem.szValue = szValue;
					this->m_vData.push_back(sItem);
				}
			}
		}

		std::string GetDataItem(const std::string& szIdent)
		{
			//Get data item value

			for (size_t i = 0; i < this->m_vData.size(); i++) {
				if (this->m_vData[i].szIdent == szIdent) {
					return this->m_vData[i].szValue;
				}
			}

			return "";
		}

		void Close(void)
		{
			//Finish file operation

			this->m_vData.clear();
			this->Release();
		}

		//Getter
		const std::vector<save_game_entry_s>& GetDataVector(void) const { return this->m_vData; }

		//AngelScript interface methods
		void Construct(void* pMemory) { new (pMemory) CSaveGameReader(); }
		void Destruct(void* pMemory) { ((CSaveGameReader*)pMemory)->~CSaveGameReader(); }
	};

	/* HUD info messages */
	const int HUDMSG_DEFAULDURATION = 3000;
	const int HUDMSG_FONTSIZE_W = 14;
	const int HUDMSG_FONTSIZE_H = 30;
	const int HUDMSG_SPRITE_SIZE = 50;
	enum HudMessageColor { HM_GREEN = 1, HM_RED, HM_YELLOW, HM_BLUE };
	class CHudInfoMessages {
	private:
		struct message_s {
			std::wstring wszMsg;
			CTimer oTimer;
			Color sColor;
		};

		std::vector<message_s> m_vMessages;
		DxRenderer::HD3DSPRITE m_hExclamation;
		DxRenderer::d3dfont_s* m_pFont;
		DxSound::HDXSOUND m_hSound;
	public:
		CHudInfoMessages() {}
		~CHudInfoMessages() {}

		bool Initialize(void)
		{
			//Initialize component

			this->m_hExclamation = pRenderer->LoadSprite(wszBasePath + L"media\\gfx\\exclamation.png", 1, HUDMSG_SPRITE_SIZE, HUDMSG_SPRITE_SIZE, 1, true);
			if (this->m_hExclamation == GFX_INVALID_SPRITE_ID) {
				return false;
			}

			this->m_pFont = pRenderer->LoadFont(L"Arial", HUDMSG_FONTSIZE_W, HUDMSG_FONTSIZE_H);
			if (!this->m_pFont) {
				return false;
			}

			this->m_hSound = pSound->QuerySound(wszBasePath + L"media\\sound\\hint.wav");

			return true;
		}

		void AddMessage(const std::wstring& wszMessage, const HudMessageColor eColor, int iDuration = HUDMSG_DEFAULDURATION)
		{
			//Add new message to list

			message_s sMessage;
			sMessage.wszMsg = wszMessage;

			switch (eColor) {
			case HM_GREEN:
				sMessage.sColor = Color(50, 130, 0, 150);
				break;
			case HM_RED:
				sMessage.sColor = Color(235, 29, 36, 150);
				break;
			case HM_YELLOW:
				sMessage.sColor = Color(135, 135, 0, 150);
				break;
			case HM_BLUE:
				sMessage.sColor = Color(0, 130, 255, 150);
				break;
			default:
				sMessage.sColor = Color(50, 50, 50, 150);
				break;
			}

			sMessage.oTimer.SetDelay(iDuration);
			sMessage.oTimer.Reset();
			sMessage.oTimer.SetActive(true);

			this->m_vMessages.push_back(sMessage);

			pSound->Play(this->m_hSound, pSndVolume->iValue, 0);
		}

		void Process(void)
		{
			//Process messages

			for (size_t i = 0; i < this->m_vMessages.size(); i++) {
				this->m_vMessages[i].oTimer.Update();
				if (this->m_vMessages[i].oTimer.Elapsed()) {
					this->m_vMessages.erase(this->m_vMessages.begin() + i);
					break;
				}
			}
		}

		void Draw(void)
		{
			//Draw messages

			for (size_t i = 0; i < this->m_vMessages.size(); i++) {
				int iBoxWidth = HUDMSG_SPRITE_SIZE + (int)this->m_vMessages[i].wszMsg.length() * HUDMSG_FONTSIZE_W + 50;
				int iBoxHeight = HUDMSG_SPRITE_SIZE + 2;
				int drawx = pWindow->GetResolutionX() / 2 - iBoxWidth / 2;
				int drawy = pWindow->GetResolutionY() - 100 - (iBoxHeight + 3) * (int)i;

				pRenderer->DrawBox(drawx, drawy, iBoxWidth, iBoxHeight, 1, 255, 255, 255, 150);
				pRenderer->DrawFilledBox(drawx + 1, drawy + 1, iBoxWidth - 1, iBoxHeight - 1, this->m_vMessages[i].sColor.r, this->m_vMessages[i].sColor.g, this->m_vMessages[i].sColor.b, this->m_vMessages[i].sColor.a);
				pRenderer->DrawSprite(this->m_hExclamation, drawx + 3, drawy + 1, 0, 0.0f);
				pRenderer->DrawString(this->m_pFont, this->m_vMessages[i].wszMsg, drawx + 5 + 50, drawy + 10, 200, 200, 200, 255);
			}
		}
	};

	/* Player HUD manager */
	const int HUD_FONT_SIZE_WIDTH = 15;
	const int HUD_FONT_SIZE_HEIGHT = 20;
	class CHud {
	private:
		struct ammo_item_s {
			std::wstring wszIdent;
			D3DXIMAGE_INFO sInfo;
			DxRenderer::HD3DSPRITE hSprite;
			size_t uiCurAmmo;
			size_t uiMaxAmmo;
		};

		struct collectable_s {
			std::wstring wszIdent;
			D3DXIMAGE_INFO sInfo;
			DxRenderer::HD3DSPRITE hSprite;
			size_t uiCurCount;
			bool bDrawAlways;
		};

		size_t m_uiHealth;
		std::vector<ammo_item_s> m_vAmmoItems;
		std::vector< collectable_s> m_vCollectables;
		size_t m_uiDisplayItem;
		DxRenderer::d3dfont_s* m_pFont;
		DxRenderer::HD3DSPRITE m_hBar;
		bool m_bEnable;

		bool AmmoItemExists(const std::wstring& wszIdent)
		{
			//Check if ammo item already exists

			for (size_t i = 0; i < this->m_vAmmoItems.size(); i++) {
				if (this->m_vAmmoItems[i].wszIdent == wszIdent) {
					return true;
				}
			}

			return false;
		}

		bool CollectableExists(const std::wstring& wszIdent)
		{
			//Check if collectable already exists

			for (size_t i = 0; i < this->m_vCollectables.size(); i++) {
				if (this->m_vCollectables[i].wszIdent == wszIdent) {
					return true;
				}
			}

			return false;
		}
	public:
		CHud() : m_uiDisplayItem(std::string::npos), m_bEnable(true)
		{
			this->m_pFont = pRenderer->LoadFont(L"Consolas", HUD_FONT_SIZE_WIDTH, HUD_FONT_SIZE_HEIGHT);
			this->m_hBar = pRenderer->LoadSprite(wszBasePath + L"media\\gfx\\healthbar.png", 1, 128, 33, 1, false);
		}

		~CHud() {}

		void UpdateHealth(size_t value)
		{
			//Update health

			this->m_uiHealth = value;
		}

		void AddAmmoItem(const std::wstring& wszIdent, const std::wstring& wszSprite)
		{
			//Add new ammo item

			if (this->AmmoItemExists(wszIdent)) {
				return;
			}

			ammo_item_s sItem;

			pRenderer->GetSpriteInfo(wszSprite, sItem.sInfo);

			sItem.wszIdent = wszIdent;
			sItem.hSprite = pRenderer->LoadSprite(wszSprite, 1, sItem.sInfo.Width, sItem.sInfo.Height, 1, false);

			this->m_vAmmoItems.push_back(sItem);
		}

		void AddCollectable(const std::wstring& wszIdent, const std::wstring& wszSprite, bool bDrawAlways)
		{
			//Add a collectable

			if (this->CollectableExists(wszIdent)) {
				return;
			}

			collectable_s sItem;

			pRenderer->GetSpriteInfo(wszSprite, sItem.sInfo);

			sItem.wszIdent = wszIdent;
			sItem.hSprite = pRenderer->LoadSprite(wszSprite, 1, sItem.sInfo.Width, sItem.sInfo.Height, 1, false);
			sItem.uiCurCount = 0;
			sItem.bDrawAlways = bDrawAlways;

			this->m_vCollectables.push_back(sItem);
		}

		void UpdateAmmoItem(const std::wstring& wszIdent, size_t uiCurAmmo, size_t uiMaxAmmo)
		{
			//Update ammo item

			for (size_t i = 0; i < this->m_vAmmoItems.size(); i++) {
				if (this->m_vAmmoItems[i].wszIdent == wszIdent) {
					this->m_vAmmoItems[i].uiCurAmmo = uiCurAmmo;
					this->m_vAmmoItems[i].uiMaxAmmo = uiMaxAmmo;

					break;
				}
			}
		}

		void UpdateCollectable(const std::wstring& wszIdent, size_t uiCurCount)
		{
			//Update collectable item

			for (size_t i = 0; i < this->m_vCollectables.size(); i++) {
				if (this->m_vCollectables[i].wszIdent == wszIdent) {
					this->m_vCollectables[i].uiCurCount = uiCurCount;

					break;
				}
			}
		}

		void SetAmmoDisplayItem(const std::wstring& wszIdent)
		{
			//Set ammo display item

			this->m_uiDisplayItem = std::string::npos;

			for (size_t i = 0; i < this->m_vAmmoItems.size(); i++) {
				if (this->m_vAmmoItems[i].wszIdent == wszIdent) {
					this->m_uiDisplayItem = i;
					break;
				}
			}
		}

		void Draw(void)
		{
			//Draw HUD component

			if (!this->m_bEnable)
				return;

			//Draw health text
			int iHealthXPos = 10;
			if ((this->m_uiHealth < 100) && (this->m_uiHealth >= 10)) {
				iHealthXPos += HUD_FONT_SIZE_WIDTH;
			} else if (this->m_uiHealth < 10) {
				iHealthXPos += HUD_FONT_SIZE_WIDTH * 2;
			}
			pRenderer->DrawString(this->m_pFont, std::to_wstring(this->m_uiHealth), iHealthXPos, 19, 200, 200, 200, 255);

			//Draw health bar
			#define HUD_SHIELDBAR_LENGTH 128
			#define HUD_SHIELDBAR_HEIGHT 33
			Entity::Color sBarColor = Entity::Color(0, 255, 0, 255);
			if ((this->m_uiHealth < 75) && (this->m_uiHealth >= 35)) {
				sBarColor = Entity::Color(150, 150, 0, 150);
			} else if (this->m_uiHealth < 35) {
				sBarColor = Entity::Color(250, 0, 0, 150);
			}
			int iShieldBarWidth = (int)this->m_uiHealth * HUD_SHIELDBAR_LENGTH / 100;
			const int iShieldBarPadding = 5;
			pRenderer->DrawSprite(this->m_hBar, 65, 15, 0, 0.0f);
			int iDrawingBarWidth = iShieldBarWidth - (iShieldBarPadding * 2);
			if (iDrawingBarWidth > 0) {
				pRenderer->DrawFilledBox(65 + iShieldBarPadding, 15 + iShieldBarPadding, iDrawingBarWidth, HUD_SHIELDBAR_HEIGHT - iShieldBarPadding * 2 - 1, sBarColor.r, sBarColor.g, sBarColor.b, sBarColor.a);
			}

			//Draw ammo info
			if ((this->m_uiDisplayItem != std::string::npos) && (this->m_uiDisplayItem < this->m_vAmmoItems.size())) {
				std::wstring wszCurAmmo = std::to_wstring(this->m_vAmmoItems[this->m_uiDisplayItem].uiCurAmmo);
				std::wstring wszMaxAmmo = std::to_wstring(this->m_vAmmoItems[this->m_uiDisplayItem].uiMaxAmmo);

				int iAmmoInfoStartX = pWindow->GetResolutionX() - this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Width - (int)wszCurAmmo.length() * HUD_FONT_SIZE_WIDTH - HUD_FONT_SIZE_WIDTH - (int)wszMaxAmmo.length() * HUD_FONT_SIZE_WIDTH - 20;

				pRenderer->DrawSprite(this->m_vAmmoItems[this->m_uiDisplayItem].hSprite, iAmmoInfoStartX, 5, 0, 0.0f);
				
				pRenderer->DrawString(this->m_pFont, wszCurAmmo, iAmmoInfoStartX + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Width + 5, 5 + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Height / 2 - HUD_FONT_SIZE_HEIGHT / 2, 200, 200, 200, 255);

				if (this->m_vAmmoItems[this->m_uiDisplayItem].uiMaxAmmo > 0) {
					pRenderer->DrawString(this->m_pFont, L"/", iAmmoInfoStartX + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Width + 5 + (int)wszCurAmmo.length() * HUD_FONT_SIZE_WIDTH, 5 + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Height / 2 - HUD_FONT_SIZE_HEIGHT / 2, 200, 200, 200, 150);
					pRenderer->DrawString(this->m_pFont, wszMaxAmmo, iAmmoInfoStartX + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Width + 5 + ((int)wszCurAmmo.length() + 1) * HUD_FONT_SIZE_WIDTH, 5 + this->m_vAmmoItems[this->m_uiDisplayItem].sInfo.Height / 2 - HUD_FONT_SIZE_HEIGHT / 2, 200, 200, 200, 150);
				}
			}

			//Draw collectables
			for (size_t i = 0; i < this->m_vCollectables.size(); i++) {
				if (this->m_vCollectables[i].bDrawAlways) {
					const int C_ADDED_GAP = 20;
					std::wstring wszStrCurCount = std::to_wstring(this->m_vCollectables[i].uiCurCount);
					
					pRenderer->DrawSprite(this->m_vCollectables[i].hSprite, ((int)i * (this->m_vCollectables[i].sInfo.Width + ((int)wszStrCurCount.length() * iDefaultFontSize[0]) + C_ADDED_GAP)), pWindow->GetResolutionY() - 43, 0, 0.0f);
					pRenderer->DrawString(pDefaultFont, wszStrCurCount, ((int)i * (this->m_vCollectables[i].sInfo.Width + ((int)wszStrCurCount.length() * iDefaultFontSize[0]) + C_ADDED_GAP) + this->m_vCollectables[i].sInfo.Width), pWindow->GetResolutionY() - 43, 200, 200, 200, 255);
				}
			}
		}

		size_t GetAmmoItemCurrent(const std::wstring& wszIdent)
		{
			//Get current ammo value of item

			for (size_t i = 0; i < this->m_vAmmoItems.size(); i++) {
				if (this->m_vAmmoItems[i].wszIdent == wszIdent) {
					return this->m_vAmmoItems[i].uiCurAmmo;
				}
			}

			return 0;
		}

		size_t GetAmmoItemMax(const std::wstring& wszIdent)
		{
			//Get max ammo value of item

			for (size_t i = 0; i < this->m_vAmmoItems.size(); i++) {
				if (this->m_vAmmoItems[i].wszIdent == wszIdent) {
					return this->m_vAmmoItems[i].uiMaxAmmo;
				}
			}

			return 0;
		}

		size_t GetCollectableCount(const std::wstring& wszIdent)
		{
			//Get amount of current items of a collectable

			for (size_t i = 0; i < this->m_vCollectables.size(); i++) {
				if (this->m_vCollectables[i].wszIdent == wszIdent) {
					return this->m_vCollectables[i].uiCurCount;
				}
			}

			return 0;
		}

		void SetEnableStatus(bool value)
		{
			//Set enable status value

			this->m_bEnable = value;
		}

		//Getter
		bool IsEnabled(void) { return this->m_bEnable; }
	};

	bool Initialize(void);
}