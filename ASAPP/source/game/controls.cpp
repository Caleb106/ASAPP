#include "asapp/game/controls.h"
#include "../core/wrappers.h"
#include <algorithm>

namespace asa::controls
{
	const float PIXELS_PER_DEGREE = 129.f / 90.f;
	const float MAX_LEFT_RIGHT_SENS = 3.2f;
	const float MAX_UP_DOWN_SENS = 3.2f;
	const float MAX_FOV = 1.25f;

	const KeyboardMapping static_keymap = { { "tab", VK_TAB }, { "f1", VK_F1 },
		{ "f2", VK_F2 }, { "f3", VK_F3 }, { "f4", VK_F4 }, { "f5", VK_F5 },
		{ "f6", VK_F6 }, { "f7", VK_F7 }, { "f8", VK_F8 }, { "f9", VK_F9 },
		{ "f10", VK_F10 }, { "delete", VK_DELETE }, { "home", VK_HOME },
		{ "end", VK_END }, { "backspace", VK_BACK }, { "enter", VK_RETURN },
		{ "period", VK_OEM_PERIOD }, { "numpadzero", VK_NUMPAD0 },
		{ "numpadone", VK_NUMPAD1 }, { "numpadtwo", VK_NUMPAD2 },
		{ "numpadthree", VK_NUMPAD3 }, { "numpadfour", VK_NUMPAD4 },
		{ "numpadfive", VK_NUMPAD5 }, { "numpadsix", VK_NUMPAD6 },
		{ "numpadseven", VK_NUMPAD7 }, { "numpadeight", VK_NUMPAD8 },
		{ "NumPadnine", VK_NUMPAD9 }, { "ctrl", VK_CONTROL },
		{ "esc", VK_ESCAPE }, { "space", VK_SPACE }, { "spacebar", VK_SPACE } };

	const auto str_to_button = std::unordered_map<std::string, MouseButton>{
		{ "LeftMouseButton", MouseButton::LEFT },
		{ "RightMouseButton", MouseButton::RIGHT },
		{ "MiddleMouseButton", MouseButton::MIDDLE },
		{ "ThumbMouseButton", MouseButton::MOUSE4 },
		{ "ThumbMouseButton2", MouseButton::MOUSE5 },
	};

	KeyboardMapping GetKeyboardMapping()
	{
		KeyboardMapping mapping = static_keymap;

		for (int i = 32; i < 128; i++) {
			char c = static_cast<char>(i);
			std::string character(1, c);
			SHORT vkCode = VkKeyScanA(i);

			mapping[character] = VkKeyScanA(i);
		}
		return mapping;
	}

	KeyboardMapping mapping = GetKeyboardMapping();

	int constexpr get_mouse_flag(MouseButton button, bool down)
	{
		switch (button) {
		case MouseButton::LEFT:
			return down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
		case MouseButton::RIGHT:
			return down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
		case MouseButton::MIDDLE:
			return down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
		case MouseButton::MOUSE4:
		case MouseButton::MOUSE5:
			return down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
		}
		return -1;
	}

	int get_virtual_keycode(std::string key)
	{
		std::transform(key.begin(), key.end(), key.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return mapping[key];
	}

	float get_left_right_factor()
	{
		return MAX_LEFT_RIGHT_SENS / settings::sens_x.get();
	}

	float get_up_down_factor()
	{
		return MAX_UP_DOWN_SENS / settings::sens_y.get();
	}

	float get_fov_factor() { return MAX_FOV / settings::fov.get(); }


	bool is_mouse_input(const settings::ActionMapping& input)
	{
		return input.key.find("Mouse") != std::string::npos;
	}

	bool is_key_input(const settings::ActionMapping& input)
	{
		return !is_mouse_input(input);
	}

	void down(
		const settings::ActionMapping& input, std::chrono::milliseconds delay)
	{
		is_mouse_input(input) ? mouse_down(str_to_button.at(input.key), delay)
							  : key_down(input.key, delay);
	}

	void release(
		const settings::ActionMapping& input, std::chrono::milliseconds delay)
	{
		is_mouse_input(input) ? mouse_up(str_to_button.at(input.key), delay)
							  : key_up(input.key, delay);
	}

	void press(
		const settings::ActionMapping& input, std::chrono::milliseconds delay)
	{
		is_mouse_input(input) ? mouse_press(str_to_button.at(input.key), delay)
							  : key_press(input.key, delay);
	}
	void mouse_down(MouseButton button, std::chrono::milliseconds delay)
	{
		INPUT input{ 0 };
		input.type = INPUT_MOUSE;

		input.mi.dwFlags = get_mouse_flag(button, true);

		if (button == MouseButton::MOUSE4) {
			input.mi.mouseData = XBUTTON1;
		}
		else if (button == MouseButton::MOUSE5) {
			input.mi.mouseData = XBUTTON2;
		}

		SendInput(1, &input, sizeof(INPUT));
		sleep_for(delay);
	}

	void mouse_up(MouseButton button, std::chrono::milliseconds delay)
	{
		INPUT input{ 0 };
		input.type = INPUT_MOUSE;

		input.mi.dwFlags = get_mouse_flag(button, false);

		if (button == MouseButton::MOUSE4) {
			input.mi.mouseData = XBUTTON1;
		}
		else if (button == MouseButton::MOUSE5) {
			input.mi.mouseData = XBUTTON2;
		}

		SendInput(1, &input, sizeof(INPUT));
		sleep_for(delay);
	}

	void mouse_press(MouseButton button, std::chrono::milliseconds delay)
	{
		mouse_down(button);
		sleep_for(delay);
		mouse_up(button);
	}

	void mouse_combination_press(MouseButton button, std::string key)
	{
		key_down(key);
		sleep_for(std::chrono::milliseconds(20));
		mouse_press(button);
		sleep_for(std::chrono::milliseconds(20));
		key_up(key);
	}

	void turn_degrees(int x, int y)
	{
		x %= 360;
		y %= 90;

		turn_position(x * PIXELS_PER_DEGREE, y * PIXELS_PER_DEGREE);
	}

	void turn_position(int x, int y)
	{
		INPUT input{ 0 };
		input.type = INPUT_MOUSE;

		input.mi.dx = x * get_left_right_factor() * get_fov_factor();
		input.mi.dy = y * get_up_down_factor() * get_fov_factor();
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;

		SendInput(1, &input, sizeof(INPUT));
	}

	void turn_to(int x, int y)
	{
		INPUT input{ 0 };
		input.type = INPUT_MOUSE;
		int moveX = (x - GetSystemMetrics(SM_CXSCREEN) / 2);
		int moveY = (y - GetSystemMetrics(SM_CYSCREEN) / 2);

		input.mi.dx = moveX;
		input.mi.dy = moveY;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;

		SendInput(1, &input, sizeof(INPUT));
	}

	void key_down(std::string key, std::chrono::milliseconds delay)
	{
		INPUT input{ 0 };
		input.type = INPUT_KEYBOARD;
		int wVk = get_virtual_keycode(key);
		input.ki.wVk = wVk;
		if (HIBYTE(wVk) & 0x01) {
			INPUT shiftInput{ 0 };
			shiftInput.type = INPUT_KEYBOARD;
			shiftInput.ki.wVk = VK_SHIFT;
			SendInput(1, &shiftInput, sizeof(INPUT));
		}

		SendInput(1, &input, sizeof(INPUT));
		sleep_for(delay);
	}

	void key_up(std::string key, std::chrono::milliseconds delay)
	{
		INPUT input{ 0 };
		input.type = INPUT_KEYBOARD;
		int wVk = get_virtual_keycode(key);
		input.ki.wVk = wVk;
		input.ki.dwFlags = KEYEVENTF_KEYUP;

		if (HIBYTE(wVk) & 0x01) {
			INPUT shiftInput{ 0 };
			shiftInput.type = INPUT_KEYBOARD;
			shiftInput.ki.wVk = VK_SHIFT;
			shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &shiftInput, sizeof(INPUT));
		}

		SendInput(1, &input, sizeof(INPUT));
		sleep_for(delay);
	}

	void key_press(std::string key, std::chrono::milliseconds delay)
	{
		key_down(key);
		sleep_for(delay);
		key_up(key);
	}

	void key_combination_press(std::string holdKey, std::string pressKey)
	{
		key_down(holdKey, std::chrono::milliseconds(20));
		key_press(pressKey, std::chrono::milliseconds(20));
		key_up(holdKey);
	}

}
