#pragma once

namespace Game
{
#ifdef CLOUD_EDITOR
	void Initialize();
	bool Run(); //Return true if the game is still running
	void Terminate();
#else
	void InitializeWithoutEditor();
	bool RunWithoutEditor(); //Return true if the game is still running
	void TerminateWithoutEditor();
#endif
}