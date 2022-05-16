#pragma once

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#pragma warning(disable : 4996)

#include "shared.h"
#include "utils.h"
#include <steam_api.h>

namespace Achievements {
	class CSteamAchievements {
	public:
		STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
		STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);

		CSteamAchievements() : m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived), m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored)
		{
			//Construct object

			SteamUserStats()->RequestCurrentStats();
		}

		~CSteamAchievements() {}

		void UnlockAchievement(const char* pName)
		{
			//Unlock Steam achievement

			SteamUserStats()->SetAchievement(pName);
		}

		void SetStat(const char* pName, int32 iCount)
		{
			//Set stat value

			SteamUserStats()->SetStat(pName, iCount);
		}

		void SetStat(const char* pName, float fCount)
		{
			//Set stat value

			SteamUserStats()->SetStat(pName, fCount);
		}

		bool IsAchievementUnlocked(const char *pName)
		{
			//Indicate if achievement got unlocked

			bool bReached = false;

			SteamUserStats()->GetAchievement(pName, &bReached);

			return bReached;
		}

		int32 GetStatInt(const char* pName)
		{
			//Get stat value

			int32 iValue = 0;

			SteamUserStats()->GetStat(pName, &iValue);

			return iValue;
		}

		float GetStatFloat(const char* pName)
		{
			//Get stat value

			float fValue = 0.0f;

			SteamUserStats()->GetStat(pName, &fValue);

			return fValue;
		}

		void PublishAchievementAndStatProgress(void)
		{
			//Publish achievement/stat progress

			SteamUserStats()->StoreStats();
		}
	};
}