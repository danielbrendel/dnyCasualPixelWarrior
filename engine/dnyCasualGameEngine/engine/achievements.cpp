#include "achievements.h"

/*
	Casual Game Engine (dnyCasualGameEngine) developed by Daniel Brendel

	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

namespace Achievements {
	void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
	{
		//Callback for received stats result
	}

	void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
	{
		//Callback for stored stats result
	}
}