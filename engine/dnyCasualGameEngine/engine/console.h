#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "shared.h"
#include "renderer.h"

#define CON_DEFAULT_MAXHISTORY 512

/* Console environment */
namespace Console {
	struct ConColor {
		ConColor() {}
		ConColor(byte r, byte g, byte b) : r(r), g(g), b(b) {}

		byte r, g, b;

		byte GetR(void) { return r; }
		byte GetG(void) { return g; }
		byte GetB(void) { return b; }

		//AngelScript interface methods
		void Constr_Bytes(byte cr, byte cg, byte cb) { r = cr; g = cg; b = cb; }
		void Construct(void* pMemory) { new (pMemory) ConColor(); }
		void Destruct(void* pMemory) { ((ConColor*)pMemory)->~ConColor(); }
	};

	class CConsole { //Console manager
	private:
		struct console_line_s {
			std::wstring wszText;
			ConColor sColor;
		};

		DxRenderer::CDxRenderer* m_pRenderer;
		DxRenderer::d3dfont_s* m_pFont;
		std::vector<console_line_s> m_vLines;
		unsigned short m_wMaxLineHistory;
		unsigned int m_uiConWidth;
		unsigned int m_uiConHeight;
		unsigned int m_uiLineHeight;
		unsigned int m_uiDrawedLines;
		unsigned int m_uiLineOffset;
		ConColor m_sOverlayColor;
		ConColor m_sDefaultColor;
		bool m_bVisible;
	public:
		CConsole() : m_pFont(nullptr), m_uiLineOffset(0), m_bVisible(false) { m_sDefaultColor.r = m_sDefaultColor.g = m_sDefaultColor.b = 220;  }
		CConsole(DxRenderer::CDxRenderer* pRenderer, int iWidth, int iHeight, unsigned short wMaxHistory, const ConColor& rColor) : CConsole() { this->Initialize(pRenderer, iWidth, iHeight, wMaxHistory, rColor); }
		~CConsole() { this->Clear(); }

		bool Initialize(DxRenderer::CDxRenderer* pRenderer, int iWidth, int iHeight, unsigned short wMaxHistory, const ConColor& rColor);
		void Clear(void) { this->m_vLines.clear(); }

		void UpdateRect(int iWidth, int iHeight);
		void AddLine(const std::wstring& wszText, const ConColor& rColor);
		void AddLine(const std::wstring& wszText) { this->AddLine(wszText, this->m_sDefaultColor); }
		void Toggle(void);
		void ScrollUp(void);
		void ScrollDown(void);
		void ScrollToEnd(void);
		void Draw(void);

		inline bool IsVisible(void) const { return this->m_bVisible; }
	};
}

