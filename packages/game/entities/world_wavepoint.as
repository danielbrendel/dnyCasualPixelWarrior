/*
	Casual Game Engine: Casual Pixel Warrior
	
	A sample and test game for Casual Game Engine
	
	(C) 2021 - 2022 by Daniel Brendel

	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#include "world_wavepointcls.as"

//Create the associated entity here
void CreateEntity(const Vector &in vecPos, float fRot, const string &in szIdent, const string &in szPath, const string &in szProps)
{
	CWavePoint @wavePoint = CWavePoint();
	wavePoint.SetTarget(Props_ExtractValue(szProps, "target"));
	wavePoint.SetEntityCount(parseInt(Props_ExtractValue(szProps, "entcount")));
	wavePoint.SetWaveCount(parseInt(Props_ExtractValue(szProps, "wavecount")));
	wavePoint.SetWaveDelay(parseInt(Props_ExtractValue(szProps, "wavedelay")));
	Ent_SpawnEntity(szIdent, @wavePoint, vecPos);
}