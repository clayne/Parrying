#pragma once
#include "PCH.h"
#include <glm/glm.hpp>

#include <vector>

#include "RE/N/NiMath.h"

#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string to_string(const RE::NiPoint3& p);

class DebugAPILine
{
public:
	DebugAPILine(glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness, unsigned __int64 destroyTickCount);

	glm::vec3 From;
	glm::vec3 To;
	glm::vec4 Color;
	float fColor;
	float Alpha;
	float LineThickness;

	unsigned __int64 DestroyTickCount;
};

class DebugAPI
{
public:
	static void Update();

	static RE::GPtr<RE::IMenu> GetHUD();

	static void DrawLine2D(RE::GPtr<RE::GFxMovieView> movie, glm::vec2 from, glm::vec2 to, float color, float lineThickness, float alpha);
	static void DrawLine2D(RE::GPtr<RE::GFxMovieView> movie, glm::vec2 from, glm::vec2 to, glm::vec4 color, float lineThickness);
	static void DrawLine3D(RE::GPtr<RE::GFxMovieView> movie, glm::vec3 from, glm::vec3 to, float color, float lineThickness, float alpha);
	static void DrawLine3D(RE::GPtr<RE::GFxMovieView> movie, glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness);
	static void ClearLines2D(RE::GPtr<RE::GFxMovieView> movie);

	static void DrawLineForMS(const glm::vec3& from, const glm::vec3& to, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
	static void DrawSphere(glm::vec3, float radius, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
	static void DrawCircle(glm::vec3, float radius, glm::vec3 eulerAngles, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);

	static std::vector<DebugAPILine*> LinesToDraw;

	static bool DEBUG_API_REGISTERED;

	static constexpr int CIRCLE_NUM_SEGMENTS = 32;

	static constexpr float DRAW_LOC_MAX_DIF = 5.0f;

	static glm::vec2 WorldToScreenLoc(RE::GPtr<RE::GFxMovieView> movie, glm::vec3 worldLoc);
	static float RGBToHex(glm::vec3 rgb);

	static void FastClampToScreen(glm::vec2& point);

	// 	static void ClampVectorToScreen(glm::vec2& from, glm::vec2& to);
	// 	static void ClampPointToScreen(glm::vec2& point, float lineAngle);

	static bool IsOnScreen(glm::vec2 from, glm::vec2 to);
	static bool IsOnScreen(glm::vec2 point);

	static void CacheMenuData();

	static bool CachedMenuData;

	static float ScreenResX;
	static float ScreenResY;

private:
	static float ConvertComponentR(float value);
	static float ConvertComponentG(float value);
	static float ConvertComponentB(float value);
	// returns true if there is already a line with the same color at around the same from and to position
	// with some leniency to bundle together lines in roughly the same spot (see DRAW_LOC_MAX_DIF)
	static DebugAPILine* GetExistingLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color, float lineThickness);
};

class DebugOverlayMenu : RE::IMenu
{
public:
	static constexpr const char* MENU_PATH = "BetterThirdPersonSelection/overlay_menu";
	static constexpr const char* MENU_NAME = "HUD Menu";

	DebugOverlayMenu();

	static void Register();

	static void Show();
	static void Hide();

	static RE::stl::owner<RE::IMenu*> Creator() { return new DebugOverlayMenu(); }

	void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;

private:
	class Logger : public RE::GFxLog
	{
	public:
		void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
		{
			std::string fmt(a_fmt ? a_fmt : "");
			while (!fmt.empty() && fmt.back() == '\n') {
				fmt.pop_back();
			}

			std::va_list args;
			va_copy(args, a_argList);
			std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
			std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
			va_end(args);

			logger::info("{}"sv, buf.data());
		}
	};
};

enum EAxis
{
	AxisX,
	AxisY,
	AxisZ
};

struct RayCastData
{
	RayCastData(glm::vec4 startPos, glm::vec4 endPos)
	{
		StartPos = startPos;
		EndPos = endPos;
	}

	glm::vec4 StartPos;
	glm::vec4 EndPos;
};

namespace Util
{
	/*
	vector translation notes

	Euler Angles:
		x = PITCH
		y = ROLL
		z = YAW

	Position:
		x = RIGHT
		y = FORWARD
		z = UP
	*/

	// inserts value into a vector, keeping the vector sorted
	template <typename T, typename Pred>
	typename std::vector<T>::iterator insert_sorted(std::vector<T>& vec, T const& item, Pred pred)
	{
		return vec.insert(std::upper_bound(vec.begin(), vec.end(), item, pred), item);
	}

	glm::highp_mat4 GetRotationMatrix(glm::vec3 eulerAngles);

	glm::vec3 NormalizeVector(glm::vec3 p);
	glm::vec3 RotateVector(glm::quat quatIn, glm::vec3 vecIn);
	glm::vec3 RotateVector(glm::vec3 eulerIn, glm::vec3 vecIn);
	glm::vec3 GetForwardVector(glm::quat quatIn);
	glm::vec3 GetForwardVector(glm::vec3 eulerIn);
	glm::vec3 GetRightVector(glm::quat quatIn);
	glm::vec3 GetRightVector(glm::vec3 eulerIn);

	glm::vec3 ThreeAxisRotation(float r11, float r12, float r21, float r31, float r32);

	glm::vec3 RotMatrixToEuler(RE::NiMatrix3 matrixIn);

	// attempts to find the object's spine node, returns the root node if none is found
	RE::NiAVObject* GetCharacterSpine(RE::TESObjectREFR* object);
	// attempts to find the object's head node, returns the root node if none is found
	RE::NiAVObject* GetCharacterHead(RE::TESObjectREFR* object);

	bool IsRoughlyEqual(float first, float second, float maxDif);

	glm::vec3 QuatToEuler(glm::quat q);

	glm::quat EulerToQuat(glm::vec3 rotIn);

	glm::vec3 GetInverseRotation(glm::vec3 rotIn);
	glm::quat GetInverseRotation(glm::quat rotIn);

	glm::vec3 EulerRotationToVector(glm::vec3);
	glm::vec3 VectorToEulerRotation(glm::vec3);
	glm::vec3 GetCameraPos();
	glm::quat GetCameraRot();

	bool IsPosBehindPlayerCamera(glm::vec3 pos);

	// given a world coordinate and a radius, this returns a point on a rotated circle, with values of i
	// ranging between 0 and maxI to determine the point on the circle
	glm::vec3 GetPointOnRotatedCircle(glm::vec3 origin, float radius, float i, float maxI, glm::vec3 eulerAngles);

	// the Skyrim ref's position doesn't always update with its Havok mesh. This is a workaround for this bug
	// it returns the position of the havok mesh, or the vanilla position if no mesh is present
	glm::vec3 GetObjectAccuratePosition(RE::TESObjectREFR* object);

	constexpr int FOLLOW_PARENT_MAX_RECURSION = 2;

	// for distance checks toward all nearby objects, first a rough check toward the origin is performed, with an added
	// threshhold of this. If this succeeds, a more precise check will be performed against the BB center of the object
	// this avoids making BB calculations for every object in the cell(s)
	constexpr float ROUGH_DISTANCE_ADD = 250.0f;

	// values smaller / larger than this will not be counted as valid by IsValid
	constexpr float POSITIVE_INVALID_THRESHHOLD = 999999.0f;
	constexpr float NEGATIVE_INVALID_THRESHHOLD = -999999.0f;
};
