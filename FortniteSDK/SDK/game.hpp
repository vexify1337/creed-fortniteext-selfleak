#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include "bytes.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;
DWORD processID;
namespace font
{
	ImFont* inter_bold = nullptr;
	ImFont* inter_default = nullptr;
	ImFont* icon = nullptr;
	ImFont* realud = nullptr;

}
#include <string.h>
#include <Windows.h>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <list>
#include <string>
namespace Fortnite_Engine {
	char* wchar_to_char(const wchar_t* pwchar)
	{
		int currentCharIndex = 0;
		char currentChar = pwchar[currentCharIndex];

		while (currentChar != '\0')
		{
			currentCharIndex++;
			currentChar = pwchar[currentCharIndex];
		}

		const int charCount = currentCharIndex + 1;

		char* filePathC = (char*)malloc(sizeof(char) * charCount);

		for (int i = 0; i < charCount; i++)
		{
			char character = pwchar[i];

			*filePathC = character;

			filePathC += sizeof(char);

		}
		filePathC += '\0';

		filePathC -= (sizeof(char) * charCount);

		return filePathC;
	}
	__forceinline std::string platform_player(uint64_t PlayerState)
	{
		DWORD_PTR test_platform = readd<DWORD_PTR>(PlayerState + Offsets::platform); // platform offset
		wchar_t platform[64];
		Driver::read_physical((PVOID)test_platform, (uint8_t*)platform, sizeof(platform));
		std::wstring platform_wstr(platform);
		std::string platform_str(platform_wstr.begin(), platform_wstr.end());
		return platform_str;
	}

	__forceinline std::string grab_weaponname(uint64_t CurrentWeapon) {
		auto ItemData = readd<DWORD_PTR>(CurrentWeapon + 0x568);
		auto DisplayName = readd<uint64_t>(ItemData + 0x40);
		auto WeaponLength = readd<uint32_t>(DisplayName + 0x20);
		wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];
		Driver::read_physical((PVOID)readd<PVOID>(DisplayName + 0x18), (uint8_t*)WeaponName, WeaponLength * sizeof(wchar_t));


		BYTE tier;
		tier = readd<BYTE>(ItemData + 0xa2);
		ImColor Color;

		if (tier == 2)
		{
			Color = IM_COL32(0, 255, 0, 255);
		}
		else if ((tier == 3))
		{
			Color = IM_COL32(0, 0, 255, 255);
		}
		else if ((tier == 4))
		{
			Color = IM_COL32(128, 0, 128, 255); // p
		}
		else if ((tier == 5))
		{
			Color = IM_COL32(255, 255, 0, 255); // p
		}
		else if ((tier == 6))
		{
			Color = IM_COL32(255, 255, 0, 255); // p
		}
		else if ((tier == 0) || (tier == 1))
		{
			Color = IM_COL32(255, 255, 255, 255); // p
		}


		if (!WeaponName || !tier)
		{
			delete[] WeaponName;
			WeaponName = new wchar_t[5];
			wcscpy(WeaponName, L"Pickaxe");
		}

		std::string wep_str = Fortnite_Engine::wchar_to_char(WeaponName);
		return wep_str;




		/*std::string wep_str = wchar_to_char(WeaponName);
		ImVec2 TextSize = ImGui::CalcTextSize(wep_str.c_str());
		draw_outlined_text(ImVec2((bottom2d.x) - (TextSize.x / 2), (Headbox.y - 15)), Color, wep_str.c_str());*/

	}
	auto get_player_name(uintptr_t player_state) -> std::string
	{
		auto Name = readd<uintptr_t>(player_state + Offsets::PlayerName); // playername offset
		auto length = readd<int>(Name + 0x10);
		auto v6 = (__int64)length;

		if (length <= 0 || length > 255) return std::string("AI/BOT");

		auto FText = (uintptr_t)readd<__int64>(Name + 0x8);

		wchar_t* Buffer = new wchar_t[length];
		Driver::read_physical(PVOID(static_cast<ULONGLONG>(FText)), Buffer, length * sizeof(wchar_t));

		char v21;
		int v22;
		int i;

		int v25;
		UINT16* v23;

		v21 = v6 - 1;
		if (!(UINT32)v6)
			v21 = 0;
		v22 = 0;
		v23 = (UINT16*)Buffer;
		for (i = (v21) & 3; ; *v23++ += i & 7)
		{
			v25 = v6 - 1;
			if (!(UINT32)v6)
				v25 = 0;
			if (v22 >= v25)
				break;
			i += 3;
			++v22;
		}

		std::wstring PlayerName{ Buffer };
		delete[] Buffer;
		return std::string(PlayerName.begin(), PlayerName.end());
	}




	static auto enemey_visiblecheck(uintptr_t skeletal_mesh, float tolerance = 0.06f) -> bool {

		auto Seconds = readd<double>(cache::uworld + 0x150);
		auto LastRenderTime = readd<float>(skeletal_mesh + Offsets::LastRenderTimeOnScreen);
		return Seconds - LastRenderTime <= 0.06f;
	}

	Vector3 Prediction(Vector3 TargetPosition, Vector3 ComponentVelocity, float player_distance, float ProjectileSpeed = 239)
	{
		float gravity = 3.5;
		float TimeToTarget = player_distance / ProjectileSpeed;
		float bulletDrop = abs(gravity) * (TimeToTarget * TimeToTarget) * 0.5;
		return Vector3
		{
		TargetPosition.x + TimeToTarget * ComponentVelocity.x,
		TargetPosition.y + TimeToTarget * ComponentVelocity.y,
		TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
		};
	}



	__forceinline auto skeleton(uintptr_t mesh, const ImU32& color) -> void
	{  //wait


		Vector3 bonePositions[] = {
			get_entity_bone(mesh, 110),  // HeadBone
			get_entity_bone(mesh, 67),   // Neck
			get_entity_bone(mesh, 7),    // Chest
			get_entity_bone(mesh, 2),    // Pelvis
			get_entity_bone(mesh, 9),    // RightShoulder
			get_entity_bone(mesh, 10),   // RightElbow
			get_entity_bone(mesh, 11),   // RightWrist
			get_entity_bone(mesh, 38),   // LeftShoulder
			get_entity_bone(mesh, 39),   // LeftElbow
			get_entity_bone(mesh, 40),   // LeftWrist
			get_entity_bone(mesh, 71),   // RightHip
			get_entity_bone(mesh, 72),   // RightKnee
			get_entity_bone(mesh, 73),   // RightAnkle
			get_entity_bone(mesh, 78),   // LeftHip
			get_entity_bone(mesh, 79),   // LeftKnee
			get_entity_bone(mesh, 80)    // LeftAnkle
		};

		Vector2 bonePositionsOut[16];
		for (int i = 0; i < 16; ++i) {
			bonePositionsOut[i] = w2s(bonePositions[i]);
		}

		//// Affiche le cercle de la t�te uniquement si globals::headesp est activ�
		//if (globals::headesp) {
		//	float headCircleRadius = 15.0f; // Ajuster pour la taille souhait�e du cercle de la t�te
		//	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(bonePositionsOut[0].x, bonePositionsOut[0].y), headCircleRadius, ImGui::GetColorU32(color), 50, settings::visuals::g_skeletonthickness);
		//}

		// Head to neck
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[0].x, bonePositionsOut[0].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Neck to chest
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Chest to pelvis
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Right arm
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Left arm
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Right leg
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);

		// Left leg
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImVec2(bonePositionsOut[15].x, bonePositionsOut[15].y), ImGui::GetColorU32(color), settings::visuals::g_skeletonthickness);
	}
}



HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = 0;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 1;
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Border] = ImColor(8, 8, 8, 250);
	style->Colors[ImGuiCol_TitleBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_WindowBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_FrameBg] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_Button] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_CheckMark] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_Header] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_PopupBg] = ImColor(38, 38, 38, 250);
	font::realud = io.Fonts->AddFontFromMemoryTTF(&Font, sizeof(Font), 18.0f);

	p_object->Release();
	return S_OK;
}
// overlay (shitty and dtc)
void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"SDK",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"SDK", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}
HWND window_handle;

inline HWND HiJackNotepadWindowH() {
	//window_handle = FindWindowA(E("Afx:00400000:0:00010005:00000000:00000000"), E("RTSS")); //riva tuner
	window_handle = FindWindowA(("MedalOverlayClass"), ("MedalOverlay")); //medal
	//window_handle = FindWindowA_Spoofed(E("GDI+ Hook Window Class"), E("GDI+ Window (obs64.exe)")); //streamlabs & obs
	//window_handle = FindWindowA(("GDI+ Hook Window Class"), ("GDI+ Window (Greenshot.exe)")); //greenshot

	if (!window_handle)
	{
		MessageBoxA(0, ("Overlay failure [Make sure app is open]"), ("Fortnite"), MB_ICONINFORMATION);
		exit(0);
	}

	int WindowWidth = GetSystemMetrics(SM_CXSCREEN);
	int WindowHeight = GetSystemMetrics(SM_CYSCREEN);

	SetMenu(window_handle, NULL);
	SetWindowPos(window_handle, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);



	ShowWindow(window_handle, SW_SHOW);
	SetWindowLongA(window_handle, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	SetWindowLongA(window_handle, GWL_STYLE, WS_VISIBLE);
	SetWindowLongA(window_handle,
		-20,
		static_cast<LONG_PTR>(
			static_cast<int>(GetWindowLongA(window_handle, -20)) | 0x20
			)
	);
	SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, WindowWidth, WindowHeight, SWP_SHOWWINDOW);



	MARGINS Margin = { -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea(
		window_handle,
		&Margin
	);
	SetLayeredWindowAttributes(window_handle, NULL, 0xFF, 0x02);

	ShowWindow(window_handle, SW_SHOW);

	UpdateWindow(window_handle);

	return window_handle;

}

void aimbot(uintptr_t target_mesh)
{
	if (!target_mesh) return;
	if (settings::visuals::VisibleCheck) {
		if (!Fortnite_Engine::enemey_visiblecheck(target_mesh)) return;
	}
	Vector3 head3d = get_entity_bone(target_mesh, 110);
	Vector2 head2d = w2s(head3d);
	Vector2 target{};
	if (head2d.x != 0)
	{
		if (head2d.x > settings::screen_center_x)
		{
			target.x = -(settings::screen_center_x - head2d.x);
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x > settings::screen_center_x * 2) target.x = 0;
		}
		if (head2d.x < settings::screen_center_x)
		{
			target.x = head2d.x - settings::screen_center_x;
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x < 0) target.x = 0;
		}
	}
	if (head2d.y != 0)
	{
		if (head2d.y > settings::screen_center_y)
		{
			target.y = -(settings::screen_center_y - head2d.y);
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y > settings::screen_center_y * 2) target.y = 0;
		}
		if (head2d.y < settings::screen_center_y)
		{
			target.y = head2d.y - settings::screen_center_y;
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y < 0) target.y = 0;
		}
	}
	input::move_mouse(target.x, target.y);
}

void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}



void Box(int X, int Y, int W, int H, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, color, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void draw_line(Vector2 target, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(Vector2 location, float distance, const ImColor color)
{
	char dist[64];
	sprintf_s(dist, "%.fm", distance);
	ImVec2 text_size = ImGui::CalcTextSize(dist);
	ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

#include <chrono>

using namespace std::chrono;

inline std::wstring MBytesToWString(const char* lpcszString)
{

	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}
inline std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}
inline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	ImGui::PushFont(font::realud);

	va_list va_alist;
	char buf[128] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
 		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetBackgroundDrawList()->AddText(font::realud, fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(font::realud, fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(font::realud, fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(font::realud, fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());

		ImGui::GetBackgroundDrawList()->AddText(font::realud, fontSize, ImVec2(x, y), ImColor(color), text.c_str());
	}
}



ImVec2 GetWatermarkSize()
{
	ImVec2 position = ImVec2(30, 30);
	const char* text = "SofMain FN Base [Private Build]";

	//ImVec2 position = ImVec2(30, 30);
	//const char* text = "Fortnite [Public]";

	ImVec2 textSize = ImGui::CalcTextSize(text);
	return ImVec2(position.x, position.y + textSize.y);
}
#include "Windows.h"
#include <iostream>

//void Debug_Loop() {
//	std::cout << "Debug -> Cached Uworld" << cache::uworld << std::endl;
//	std::cout << "Debug -> Cached game_instance" << cache::game_instance << std::endl;
//	std::cout << "Debug -> Cached local_players" << cache::local_players << std::endl;
//	std::cout << "Debug -> Cached player_controller" << cache::player_controller << std::endl;
//	std::cout << "Debug -> Cached local_pawn" << cache::local_pawn << std::endl;
//	std::cout << "Debug -> Cached root_component" << cache::root_component << std::endl;
//	std::cout << "Debug -> Cached root_component" << cache::root_component << std::endl;
//	std::cout << "Debug -> Cached player_state" << cache::player_state << std::endl;
//	std::cout << "Debug -> Cached my_team_id" << cache::my_team_id << std::endl;
//	std::cout << "Debug -> Cached game_state" << cache::game_state << std::endl;
//	std::cout << "Debug -> Cached player_array" << cache::player_array << std::endl;
//	std::cout << "Debug -> Cached player_count" << cache::player_count << std::endl;
//
//
//}
// 


inline void draw_outlined_text(ImVec2 pos, ImColor color, std::string text)
{
	ImGui::PushFont(font::realud);

	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = ImGui::CalcTextSize(line.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
}

// Inside your rendering or update loop
void Debugg() 
{
	ImGui::Begin("debugger", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(10, 10)); // Position on the left side
	ImGui::SetWindowSize(ImVec2(300, 300)); // Fixed size for better appearance

	ImVec4 purpleColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f); // RGBA for purple

	ImGui::TextColored(purpleColor, "Debug -> Cached Uworld: %p", cache::uworld);
	ImGui::TextColored(purpleColor, "Debug -> Cached game_instance: %p", cache::game_instance);
	ImGui::TextColored(purpleColor, "Debug -> Cached local_players: %p", cache::local_players);
	ImGui::TextColored(purpleColor, "Debug -> Cached player_controller: %p", cache::player_controller);
	ImGui::TextColored(purpleColor, "Debug -> Cached local_pawn: %p", cache::local_pawn);
	ImGui::TextColored(purpleColor, "Debug -> Cached root_component: %p", cache::root_component);
	ImGui::TextColored(purpleColor, "Debug -> Cached player_state: %p", cache::player_state);
	ImGui::TextColored(purpleColor, "Debug -> Cached my_team_id: %d", cache::my_team_id);
	ImGui::TextColored(purpleColor, "Debug -> Cached game_state: %p", cache::game_state);
	ImGui::TextColored(purpleColor, "Debug -> Cached player_array: %p", cache::player_array);
	ImGui::TextColored(purpleColor, "Debug -> Cached player_count: %d", cache::player_count);

	ImGui::End();
}
// readd, is a pointer we define in our "Comms/Communications.hpp", this will read memory from the designated game.

void game_loop()
{
	cache::uworld = readd<uintptr_t>(Base + UWORLD);
	cache::game_instance = readd<uintptr_t>(cache::uworld + GAME_INSTANCE);
	cache::local_players = readd<uintptr_t>(readd<uintptr_t>(cache::game_instance + LOCAL_PLAYERS	));
	cache::player_controller = readd<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
	cache::local_pawn = readd<uintptr_t>(cache::player_controller + LOCAL_PAWN);
	if (cache::local_pawn != 0)
	{
		cache::root_component = readd<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
		cache::relative_location = readd<Vector3>(cache::root_component + RELATIVE_LOCATION);
		cache::player_state = readd<uintptr_t>(cache::local_pawn + PLAYER_STATE);
		cache::my_team_id = readd<int>(cache::player_state + TEAM_INDEX);
	}
	cache::game_state = readd<uintptr_t>(cache::uworld + GAME_STATE);
	cache::player_array = readd<uintptr_t>(cache::game_state + PLAYER_ARRAY);
	cache::player_count = readd<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
	cache::closest_distance = FLT_MAX;
	cache::closest_mesh = NULL;
	for (int i = 0; i < cache::player_count; i++)
	{
		uintptr_t player_state = readd<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
		if (!player_state) continue;
		int player_team_id = readd<int>(player_state + TEAM_INDEX);
		if (player_team_id == cache::my_team_id) continue;
		uintptr_t pawn_private = readd<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private) continue;
		if (pawn_private == cache::local_pawn) continue;
		uintptr_t mesh = readd<uintptr_t>(pawn_private + MESH);
		if (!mesh) continue;
		Vector3 head3d = get_entity_bone(mesh, 110); // defining the head bone.
		Vector2 head2d = w2s(head3d);
		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = w2s(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = cache::relative_location.distance(bottom3d) / 100;

		ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
		ImColor col1pasted;
		static float g_Color_Filled[3] = { 0.0f, 255.0f, 0.0f }; // Blanc
		col1pasted = ImVec4(g_Color_Filled[0], g_Color_Filled[1], g_Color_Filled[2], 1.0f);

		// col 2 = transpacey colour visibleColor
		ImU32 col1 = ImColor(255, 0, 0, 128); // Rot mit 50% Transparenz
		ImU32 col2 = ImColor(0, 0, 0, 128);   // Schwarz mit 50% Transparenz

		ImVec2 top_left = ImVec2(head2d.x - (box_width / 2), head2d.y);
		ImVec2 top_right = ImVec2(bottom2d.x + (box_width / 2), head2d.y);
		ImVec2 bottom_left = ImVec2(head2d.x - (box_width / 2), bottom2d.y);
		ImVec2 bottom_right = ImVec2(bottom2d.x + (box_width / 2), bottom2d.y);
		if (settings::visuals::enable)
		{
			if (settings::visuals::Box)
			{
				if (settings::visuals::VisibleCheck) {
					if (Fortnite_Engine::enemey_visiblecheck(mesh))
					{
						Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
					}
					else
					{
						Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
					}
				}
				else {
					Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1); // render normally, if visible check is false.

				}
				if (settings::visuals::fill_box) {
					 // i dont see the point of doing visible check on this, if you want vis check. just do similar what i did before.
						//draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
				

					draw_list->AddRectFilledMultiColor(top_left, bottom_right, col1pasted, col2, col2, col1);
					
				}
			}

			if (settings::visuals::CorneredBox)
			{
				if (settings::visuals::VisibleCheck) {
					if (Fortnite_Engine::enemey_visiblecheck(mesh))
					{
						draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
					}
					else
					{
						draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
					}
				}
				else {
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);

				}
			
			}
			if (settings::visuals::Platform) {
				auto platform_str = Fortnite_Engine::platform_player(player_state);
				int offset;
				if (settings::visuals::Platform)
					offset = 30;
				else
					offset = 15;

				ImVec2 textPosition(head2d.x, head2d.y - offset);

				draw_outlined_text(textPosition, ImColor(255, 255, 255), platform_str.c_str());
			}
			if (settings::visuals::name_Esp) {
				std::string username_str = Fortnite_Engine::get_player_name(player_state);
				ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
				DrawString(3.0f, head2d.x - (text_size.x / 2), head2d.y - 20, ImColor(255,255,255), false, true, username_str.c_str());
			
			}
			if (settings::visuals::skeleton) {
				Fortnite_Engine::skeleton(mesh, ImColor(250, 250, 250, 250)); // skeleton
			}
			if (settings::visuals::line)
			{
				if (settings::visuals::VisibleCheck) {
					if (Fortnite_Engine::enemey_visiblecheck(mesh))
					{
						draw_line(bottom2d, ImColor(250, 250, 250, 250));
					}
					else
					{
						draw_line(bottom2d, ImColor(250, 0, 0, 250));
					}
				}
				else {
					draw_line(bottom2d, ImColor(250, 250, 250, 250));

				}
			}
			if (settings::visuals::distance)
			{
				draw_distance(bottom2d, distance, ImColor(250, 250, 250, 250));
			}
		}
		
		double dx = head2d.x - settings::screen_center_x;
		double dy = head2d.y - settings::screen_center_y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist <= settings::aimbot::fov && dist < cache::closest_distance)
		{
			cache::closest_distance = dist;
			cache::closest_mesh = mesh;
		}
	}
	if (settings::aimbot::enable)
	{
		if (GetAsyncKeyState(settings::aimbot::current_key)) aimbot(cache::closest_mesh);
	}
	if (settings::Crosshair)
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImVec2 screen_size = ImGui::GetIO().DisplaySize;
		ImVec2 center = ImVec2(screen_size.x / 2, screen_size.y / 2);

		ImVec2 horizontal_start = ImVec2(center.x - settings::crosshair_length, center.y);
		ImVec2 horizontal_end = ImVec2(center.x + settings::crosshair_length, center.y);

		ImVec2 vertical_start = ImVec2(center.x, center.y - settings::crosshair_length);
		ImVec2 vertical_end = ImVec2(center.x, center.y + settings::crosshair_length);

		//draw_list->AddLine(horizontal_start, horizontal_end, settings::crosshair_color, settings::crosshair_thickness);
		//draw_list->AddLine(vertical_start, vertical_end, settings::crosshair_color, settings::crosshair_thickness);
	}
	if (settings::Features::Watermark) {


		DrawString(19, 13, 13, ImColor(255, 255, 255), false, true, "hypnotic.lol");

	}
	if (settings::Features::Debug) {


		Debugg();

	}
}

void Crosshair()
{
	if (ImGui::Checkbox("Crosshair", &settings::Crosshair))
	{
		//ImVec4 color = ImGui::ColorConvertU32ToFloat4(settings::crosshair_color);
		//if (ImGui::ColorEdit4("Crosshair Color", (float*)&color))
	//	{
			//settings::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
	//	}
		ImGui::SliderFloat("Thickness", &settings::crosshair_thickness, 1.0f, 10.0f);

		if (settings::Crosshair) // Only show the combo box if the crosshair is enabled
		{

		}
	}

}
void SetupImGuiStyle()
{
	// Hazy Dark style by kaitabuchi314 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(5.5f, 8.300000190734863f);
	style.WindowRounding = 4.5f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 3.200000047683716f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 2.700000047683716f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 2.400000095367432f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 3.200000047683716f;
	style.TabRounding = 3.5f;
	style.TabBorderSize = 1.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1372549086809158f, 0.1725490242242813f, 0.2274509817361832f, 0.5400000214576721f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2117647081613541f, 0.2549019753932953f, 0.3019607961177826f, 0.4000000059604645f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04313725605607033f, 0.0470588244497776f, 0.0470588244497776f, 0.6700000166893005f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0784313753247261f, 0.08235294371843338f, 0.09019608050584793f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7176470756530762f, 0.7843137383460999f, 0.843137264251709f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47843137383461f, 0.5254902243614197f, 0.572549045085907f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2901960909366608f, 0.3176470696926117f, 0.3529411852359772f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.1490196138620377f, 0.1607843190431595f, 0.1764705926179886f, 0.4000000059604645f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1372549086809158f, 0.1450980454683304f, 0.1568627506494522f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.09019608050584793f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.196078434586525f, 0.2156862765550613f, 0.239215686917305f, 0.3100000023841858f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1647058874368668f, 0.1764705926179886f, 0.1921568661928177f, 0.800000011920929f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.07450980693101883f, 0.08235294371843338f, 0.09019608050584793f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.239215686917305f, 0.3254902064800262f, 0.4235294163227081f, 0.7799999713897705f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.2745098173618317f, 0.3803921639919281f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2901960909366608f, 0.3294117748737335f, 0.3764705955982208f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.239215686917305f, 0.2980392277240753f, 0.3686274588108063f, 0.6700000166893005f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.1647058874368668f, 0.1764705926179886f, 0.1882352977991104f, 0.949999988079071f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1176470592617989f, 0.125490203499794f, 0.1333333402872086f, 0.8619999885559082f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3294117748737335f, 0.407843142747879f, 0.501960813999176f, 0.800000011920929f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.2431372553110123f, 0.2470588237047195f, 0.2549019753932953f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
int currentTab = 0;

void render_menu()
{
	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
	}
	if (settings::aimbot::show_fov) ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(250, 250, 250, 250), 100, 1.0f);
	if (GetAsyncKeyState(VK_INSERT) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)
	{
		ImGui::SetNextWindowSize({ 620, 350 });
		ImGui::Begin("SDK paste", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		if (ImGui::Button("Aimbot", { 196, 20 })) settings::tab = 0;
		ImGui::SameLine();
		if (ImGui::Button("Visuals", { 196, 20 })) settings::tab = 1;
		ImGui::SameLine();
		if (ImGui::Button("Misc", { 196, 20 })) settings::tab = 2;
		switch (settings::tab)
		{
			case 0:
			{
				ImGui::Checkbox("Enable", &settings::aimbot::enable);
				ImGui::Checkbox("Show Fov", &settings::aimbot::show_fov);
				ImGui::SliderFloat("##Fov", &settings::aimbot::fov, 50, 300, "Fov: %.2f");
				ImGui::SliderFloat("##Smoothness", &settings::aimbot::smoothness, 1, 10, "Smoothness: %.2f");
				ImGui::Combo("##Aimkey", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, sizeof(settings::aimbot::aimkey) / sizeof(*settings::aimbot::aimkey));
				break;
			}
			case 1:
			{
				ImGui::Checkbox("Enable", &settings::visuals::enable);
				ImGui::Checkbox("Cornered Box", &settings::visuals::CorneredBox);
				ImGui::SameLine();
				ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
				ImGui::Checkbox("Box", &settings::visuals::Box);
				ImGui::Checkbox("VisibleCheck", &settings::visuals::VisibleCheck);
				ImGui::Checkbox("Line", &settings::visuals::line);
				ImGui::Checkbox("Distance", &settings::visuals::distance);
				break;
			}
			case 2:
			{
				
				//if (ImGui::Checkbox("Crosshair", &settings::Crosshair))
				//{



					//if (settings::Crosshair) // Only show the combo box if the crosshair is enabled
				//{


					//}
			//	}
				ImGui::Checkbox("WaterMark", &settings::Features::Watermark);
				ImGui::Checkbox("Debug", &settings::Features::Debug);
				if (ImGui::Button("Unload", { 120, 20 })) exit(0);
				break;
			}
		}
		ImGui::End();
	}
}

HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

WPARAM render_loop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		if (game_wnd == NULL) exit(0);
		HWND active_wnd = GetForegroundWindow();
		if (active_wnd == game_wnd)
		{
			HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
			SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			game_wnd = get_process_wnd(processID);
			Sleep(250);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		game_loop();
		RenderMenu();
		ImGui::EndFrame();
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != 0) p_object->Release();
	DestroyWindow(my_wnd);
	return messager.wParam;
}
