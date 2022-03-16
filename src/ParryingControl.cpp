#include "DebugAPI.h"
#include "Util.h"
#include "Geom.h"
#include <UselessFenixUtils.h>

namespace DrawDebug
{
	namespace Colors
	{
		static constexpr glm::vec4 RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		static constexpr glm::vec4 GRN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		static constexpr glm::vec4 BLU = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	template <glm::vec4 Color = Colors::RED, int time = 3000>
	void draw_line(const RE::NiPoint3& _from, const RE::NiPoint3& _to, float size = 5.0f)
	{
		glm::vec3 from(_from.x, _from.y, _from.z);
		glm::vec3 to(_to.x, _to.y, _to.z);
		DebugAPI::DrawLineForMS(from, to, time, Color, size);
	}
}
using namespace DrawDebug::Colors;
using DrawDebug::draw_line;

bool get_weapon(RE::Actor* a, RE::NiPoint3& from, RE::NiPoint3& to)
{
	float reach = FenixUtils::PlayerCharacter__get_reach(a) * 0.75f;
	auto root = netimmerse_cast<RE::BSFadeNode*>(a->Get3D());
	if (!root)
		return false;
	auto bone = netimmerse_cast<RE::NiNode*>(root->GetObjectByName("WEAPON"));
	if (!bone)
		return false;
	from = bone->world.translate;
	auto weaponDirection = RE::NiPoint3{ bone->world.rotate.entry[0][1], bone->world.rotate.entry[1][1], bone->world.rotate.entry[2][1] };
	to = from + weaponDirection * reach;
	return true;
}

void draw_player_weapon()
{
	RE::NiPoint3 startPos, endPos;
	if (!get_weapon(RE::PlayerCharacter::GetSingleton(), startPos, endPos))
		return;
	auto hud = DebugAPI::GetHUD();
	if (!hud || !hud->uiMovie)
		return;
	glm::vec3 from = { startPos.x, startPos.y, startPos.z };
	glm::vec3 to = { endPos.x, endPos.y, endPos.z };
	DebugAPI::DrawLine3D(hud->uiMovie, from, to, BLU, 5.0f);
}

RE::TESObjectWEAP* get_weapon(RE::Actor* a, bool left)
{
	auto _weap = a->GetEquippedObject(left);
	if (!_weap)
		return nullptr;

	auto weap = _weap->As<RE::TESObjectWEAP>();
	return weap;
}

namespace Control
{
	void parry(RE::Actor* attacker, RE::Actor* victim)
	{
		// TODO: play sound

		victim->NotifyAnimationGraph("recoilStop");
		victim->NotifyAnimationGraph("AttackStop");
		attacker->NotifyAnimationGraph("recoilStop");
		attacker->NotifyAnimationGraph("AttackStop");

		attacker->NotifyAnimationGraph("recoilLargeStart");
		victim->NotifyAnimationGraph("recoilLargeStart");
	}

	bool is_attacking(RE::ATTACK_STATE_ENUM state_a)
	{
		return state_a == RE::ATTACK_STATE_ENUM::kHit || state_a == RE::ATTACK_STATE_ENUM::kSwing;
	}

	bool close_attack_(RE::Actor* attacker, RE::Actor* victim)
	{
		RE::NiPoint3 attacker_from, attacker_to, victim_from, victim_to;
		if (!get_weapon(attacker, attacker_from, attacker_to) || !get_weapon(victim, victim_from, victim_to))
			return false;

		float d = dist(attacker_from, attacker_to, victim_from, victim_to);

		logger::info("d={}", d);
		draw_line(attacker_from, attacker_to);
		draw_line(victim_from, victim_to);

		return d <= 100.0f;
	}

	bool close_attack(RE::Actor* attacker, RE::Actor* victim) {
		return is_attacking(attacker->GetAttackState()) && is_attacking(victim->GetAttackState()) && close_attack_(attacker, victim);
	}

	bool OnHit(RE::Actor* attacker, RE::Actor* victim, char aleft)
	{
		auto attacker_weap = get_weapon(attacker, aleft);
		auto victim_weap = get_weapon(victim, false);
		if (attacker_weap && victim_weap) {
			if (close_attack(attacker, victim)) {
				parry(attacker, victim);
				return true;
			}
		}
		return false;
	}
}
