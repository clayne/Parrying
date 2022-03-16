#pragma once

#include "PCH.h"
#include "SKSE/Trampoline.h"
#include <xbyak\xbyak.h>

#include "ParryingControl.h"

namespace Hooks
{
	class MainUpdateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(35551) };

			auto& trampoline = SKSE::GetTrampoline();
			_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);  // SkyrimSE.exe+5AF4EF
		}

	private:
		static void Update(RE::Main* a_this, float a2);
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class OnHitHook
	{
	public:
		static void Hook()
		{
			// SkyrimSE.exe+627CBB
			_OnHit = SKSE::GetTrampoline().write_call<5>(REL::ID(37650).address() + 0x38b, OnHit);
		}

	private:
		static void OnHit(RE::Actor* attacker, RE::Actor* victim, RE::Projectile* projectile, char aleft)
		{
			if (attacker && victim && !projectile && Control::OnHit(attacker, victim, aleft))
				return;

			_OnHit(attacker, victim, projectile, aleft);
		}
		static inline REL::Relocation<decltype(OnHit)> _OnHit;
	};

	void apply_hooks();
}
