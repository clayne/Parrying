#include "Hooks.h"
#include <ParryingControl.h>

namespace Hooks
{
	void MainUpdateHook::Update(RE::Main* a_this, float a2)
	{
		_Update(a_this, a2);

		DebugAPI::Update();

		draw_player_weapon();
	}

	void apply_hooks()
	{
		OnHitHook::Hook();
		MainUpdateHook::Hook();
	}
}
