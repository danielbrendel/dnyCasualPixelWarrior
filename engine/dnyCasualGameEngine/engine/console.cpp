#include "console.h"
#include "utils.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

/* Console environment */
namespace Console {
	bool CConsole::Initialize(DxRenderer::CDxRenderer* pRenderer, int iWidth, int iHeight, unsigned short wMaxHistory, const ConColor& rColor)
	{
		//Initialize component

		if ((!pRenderer) || (!iWidth) || (!iHeight))
			return false;

		//Temporary constants
		const unsigned int uiFontHeight = 15;
		const unsigned int uiFontLineDist = 3;

		//Register font
		this->m_pFont = pRenderer->LoadFont(L"Arial", 7, uiFontHeight);
		if (!this->m_pFont)
			return false;

		//Save data
		this->m_pRenderer = pRenderer;
		this->m_sOverlayColor = rColor;
		this->m_uiConWidth = iWidth;
		this->m_uiConHeight = iHeight;
		this->m_wMaxLineHistory = wMaxHistory;
		this->m_uiDrawedLines = iHeight / (uiFontHeight + uiFontLineDist) - 1;
		this->m_uiLineHeight = uiFontHeight + uiFontLineDist;

		return true;
	}

	void CConsole::UpdateRect(int iWidth, int iHeight)
	{
		//Update rectangle info

		this->m_uiConWidth = iWidth;
		this->m_uiConHeight = iHeight;
	}

	void CConsole::AddLine(const std::wstring& wszText, const ConColor& rColor)
	{
		//Add line

		//Setup data struct
		console_line_s sLine;
		sLine.wszText = wszText;
		sLine.sColor = rColor;

		//Add to list
		this->m_vLines.push_back(sLine);

		//Check if max history reached and if so remove first element
		if (this->m_vLines.size() >= (size_t)this->m_wMaxLineHistory) {
			this->m_vLines.erase(this->m_vLines.begin() + 0);
		}

		//Scroll to end
		this->ScrollToEnd();
	}

	void CConsole::Toggle(void)
	{
		//Toggle console visibility

		this->m_bVisible = !this->m_bVisible;
	}

	void CConsole::ScrollUp(void)
	{
		//Scroll up

		if (this->m_uiLineOffset > 0)
			this->m_uiLineOffset--;
	}

	void CConsole::ScrollDown(void)
	{
		//Scroll down

		//The limit is reached if the last lines (from offset to end, depends on 'uiDrawedLines' how many do suit) are visible
		size_t uiLimit = 0;
		if (this->m_vLines.size() > this->m_uiDrawedLines)
			uiLimit = this->m_vLines.size() - this->m_uiDrawedLines;

		if (this->m_uiLineOffset < uiLimit)
			this->m_uiLineOffset++;
	}

	void CConsole::ScrollToEnd(void)
	{
		//Scroll to end

		//Only perform scrolling to end if there are enough lines
		if (this->m_vLines.size() < this->m_uiDrawedLines)
			return;

		//Set line offset so that it draws only the last inserted lines which suit into the area
		this->m_uiLineOffset = (unsigned int)this->m_vLines.size() - this->m_uiDrawedLines;
	}

	void CConsole::Draw(void)
	{
		//Draw console

		if (!this->m_bVisible)
			return;

		//Draw overlay
		this->m_pRenderer->DrawFilledBox(0, 0, this->m_uiConWidth, this->m_uiConHeight, this->m_sOverlayColor.r, this->m_sOverlayColor.g, this->m_sOverlayColor.b, 50);
	
		//Calculate limitating conditional value
		size_t uiBorder = (this->m_vLines.size() < this->m_uiDrawedLines) ? this->m_vLines.size() : this->m_uiDrawedLines;

		//Draw text
		for (size_t i = 0; i < uiBorder; i++) {
			this->m_pRenderer->DrawString(this->m_pFont, this->m_vLines[i + this->m_uiLineOffset].wszText.c_str(), 10, (int)(i * this->m_uiLineHeight) + 10, this->m_vLines[i + this->m_uiLineOffset].sColor.r, this->m_vLines[i + this->m_uiLineOffset].sColor.g, this->m_vLines[i + this->m_uiLineOffset].sColor.b, 200);
		}
	}
}
