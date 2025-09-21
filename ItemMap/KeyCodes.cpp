// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	std::map<std::string, int> NameToKey =
	{
		{ "ESCAPE", KEY_ESCAPE },
		{ "1", KEY_1 },
		{ "2", KEY_2 },
		{ "3", KEY_3 },
		{ "4", KEY_4 },
		{ "5", KEY_5 },
		{ "6", KEY_6 },
		{ "7", KEY_7 },
		{ "8", KEY_8 },
		{ "9", KEY_9 },
		{ "0", KEY_0 },
		{ "MINUS", KEY_MINUS },
		{ "EQUALS", KEY_EQUALS },
		{ "BACK", KEY_BACK },
		{ "TAB", KEY_TAB },
		{ "Q", KEY_Q },
		{ "W", KEY_W },
		{ "E", KEY_E },
		{ "R", KEY_R },
		{ "T", KEY_T },
		{ "Y", KEY_Y },
		{ "U", KEY_U },
		{ "I", KEY_I },
		{ "O", KEY_O },
		{ "P", KEY_P },
		{ "LBRACKET", KEY_LBRACKET },
		{ "RBRACKET", KEY_RBRACKET },
		{ "RETURN", KEY_RETURN },
		{ "LCONTROL", KEY_LCONTROL },
		{ "A", KEY_A },
		{ "S", KEY_S },
		{ "D", KEY_D },
		{ "F", KEY_F },
		{ "G", KEY_G },
		{ "H", KEY_H },
		{ "J", KEY_J },
		{ "K", KEY_K },
		{ "L", KEY_L },
		{ "SEMICOLON", KEY_SEMICOLON },
		{ "APOSTROPHE", KEY_APOSTROPHE },
		{ "GRAVE", KEY_GRAVE },
		{ "LSHIFT", KEY_LSHIFT },
		{ "BACKSLASH", KEY_BACKSLASH },
		{ "Z", KEY_Z },
		{ "X", KEY_X },
		{ "C", KEY_C },
		{ "V", KEY_V },
		{ "B", KEY_B },
		{ "N", KEY_N },
		{ "M", KEY_M },
		{ "COMMA", KEY_COMMA },
		{ "PERIOD", KEY_PERIOD },
		{ "SLASH", KEY_SLASH },
		{ "RSHIFT", KEY_RSHIFT },
		{ "MULTIPLY", KEY_MULTIPLY },
		{ "LMENU", KEY_LMENU },
		{ "SPACE", KEY_SPACE },
		{ "CAPITAL", KEY_CAPITAL },
		{ "F1", KEY_F1 },
		{ "F2", KEY_F2 },
		{ "F3", KEY_F3 },
		{ "F4", KEY_F4 },
		{ "F5", KEY_F5 },
		{ "F6", KEY_F6 },
		{ "F7", KEY_F7 },
		{ "F8", KEY_F8 },
		{ "F9", KEY_F9 },
		{ "F10", KEY_F10 },
		{ "NUMLOCK", KEY_NUMLOCK },
		{ "SCROLL", KEY_SCROLL },
		{ "NUMPAD7", KEY_NUMPAD7 },
		{ "NUMPAD8", KEY_NUMPAD8 },
		{ "NUMPAD9", KEY_NUMPAD9 },
		{ "SUBTRACT", KEY_SUBTRACT },
		{ "NUMPAD4", KEY_NUMPAD4 },
		{ "NUMPAD5", KEY_NUMPAD5 },
		{ "NUMPAD6", KEY_NUMPAD6 },
		{ "ADD", KEY_ADD },
		{ "NUMPAD1", KEY_NUMPAD1 },
		{ "NUMPAD2", KEY_NUMPAD2 },
		{ "NUMPAD3", KEY_NUMPAD3 },
		{ "NUMPAD0", KEY_NUMPAD0 },
		{ "DECIMAL", KEY_DECIMAL },
		{ "OEM_102", KEY_OEM_102 },
		{ "F11", KEY_F11 },
		{ "F12", KEY_F12 },
		{ "F13", KEY_F13 },
		{ "F14", KEY_F14 },
		{ "F15", KEY_F15 },
		{ "KANA", KEY_KANA },
		{ "ABNT_C1", KEY_ABNT_C1 },
		{ "CONVERT", KEY_CONVERT },
		{ "NOCONVERT", KEY_NOCONVERT },
		{ "YEN", KEY_YEN },
		{ "ABNT_C2", KEY_ABNT_C2 },
		{ "NUMPADEQUALS", KEY_NUMPADEQUALS },
		{ "PREVTRACK", KEY_PREVTRACK },
		{ "AT", KEY_AT },
		{ "COLON", KEY_COLON },
		{ "UNDERLINE", KEY_UNDERLINE },
		{ "KANJI", KEY_KANJI },
		{ "STOP", KEY_STOP },
		{ "AX", KEY_AX },
		{ "UNLABELED", KEY_UNLABELED },
		{ "NEXTTRACK", KEY_NEXTTRACK },
		{ "NUMPADENTER", KEY_NUMPADENTER },
		{ "RCONTROL", KEY_RCONTROL },
		{ "MUTE", KEY_MUTE },
		{ "CALCULATOR", KEY_CALCULATOR },
		{ "PLAYPAUSE", KEY_PLAYPAUSE },
		{ "MEDIASTOP", KEY_MEDIASTOP },
		{ "VOLUMEDOWN", KEY_VOLUMEDOWN },
		{ "VOLUMEUP", KEY_VOLUMEUP },
		{ "WEBHOME", KEY_WEBHOME },
		{ "NUMPADCOMMA", KEY_NUMPADCOMMA },
		{ "DIVIDE", KEY_DIVIDE },
		{ "SYSRQ", KEY_SYSRQ },
		{ "RMENU", KEY_RMENU },
		{ "PAUSE", KEY_PAUSE },
		{ "HOME", KEY_HOME },
		{ "UP", KEY_UP },
		{ "PRIOR", KEY_PRIOR },
		{ "LEFT", KEY_LEFT },
		{ "RIGHT", KEY_RIGHT },
		{ "END", KEY_END },
		{ "DOWN", KEY_DOWN },
		{ "NEXT", KEY_NEXT },
		{ "INSERT", KEY_INSERT },
		{ "DELETE", KEY_DELETE },
		{ "LWIN", KEY_LWIN },
		{ "RWIN", KEY_RWIN },
		{ "APPS", KEY_APPS },
		{ "POWER", KEY_POWER },
		{ "SLEEP", KEY_SLEEP },
		{ "WAKE", KEY_WAKE },
		{ "WEBSEARCH", KEY_WEBSEARCH },
		{ "WEBFAVORITES", KEY_WEBFAVORITES },
		{ "WEBREFRESH", KEY_WEBREFRESH },
		{ "WEBSTOP", KEY_WEBSTOP },
		{ "WEBFORWARD", KEY_WEBFORWARD },
		{ "WEBBACK", KEY_WEBBACK },
		{ "MYCOMPUTER", KEY_MYCOMPUTER },
		{ "MAIL", KEY_MAIL },
		{ "MEDIASELECT", KEY_MEDIASELECT },
		{ "BACKSPACE", KEY_BACKSPACE },
		{ "NUMPADSTAR", KEY_NUMPADSTAR },
		{ "LALT", KEY_LALT },
		{ "CAPSLOCK", KEY_CAPSLOCK },
		{ "NUMPADMINUS", KEY_NUMPADMINUS },
		{ "NUMPADPLUS", KEY_NUMPADPLUS },
		{ "NUMPADPERIOD", KEY_NUMPADPERIOD },
		{ "NUMPADSLASH", KEY_NUMPADSLASH },
		{ "RALT", KEY_RALT },
		{ "UPARROW", KEY_UPARROW },
		{ "PGUP", KEY_PGUP },
		{ "LEFTARROW", KEY_LEFTARROW },
		{ "RIGHTARROW", KEY_RIGHTARROW },
		{ "DOWNARROW", KEY_DOWNARROW },
		{ "PGDN", KEY_PGDN },
		{ "CIRCUMFLEX", KEY_CIRCUMFLEX },
		{ "MOUSE_BUTTONLEFT", MOUSE_BUTTONLEFT },
		{ "MOUSE_BUTTONRIGHT", MOUSE_BUTTONRIGHT },
		{ "MOUSE_BUTTONMID", MOUSE_BUTTONMID },
		{ "MOUSE_XBUTTON1", MOUSE_XBUTTON1 },
		{ "MOUSE_XBUTTON2", MOUSE_XBUTTON2 },
		{ "MOUSE_XBUTTON3", MOUSE_XBUTTON3 },
		{ "MOUSE_XBUTTON4", MOUSE_XBUTTON4 },
		{ "MOUSE_XBUTTON5", MOUSE_XBUTTON5 },
		{ "MOUSE_WHEELUP", MOUSE_WHEELUP },
		{ "MOUSE_WHEELDOWN", MOUSE_WHEELDOWN }
	};

	std::map<std::string, int> NameToLogicalKey =
	{
		{ "GAME_LEFT", GAME_LEFT },
		{ "GAME_RIGHT", GAME_RIGHT },
		{ "GAME_UP", GAME_UP },
		{ "GAME_DOWN", GAME_DOWN },
		{ "GAME_ACTION", GAME_ACTION },
		{ "GAME_SLOW", GAME_SLOW },
		{ "GAME_ACTION2", GAME_ACTION2 },
		{ "GAME_WEAPON", GAME_WEAPON },
		{ "GAME_SMOVE", GAME_SMOVE },
		{ "GAME_SMOVE2", GAME_SMOVE2 },
		{ "GAME_SHIFT", GAME_SHIFT },
		{ "GAME_END", GAME_END },
		{ "GAME_INVENTORY", GAME_INVENTORY },
		{ "GAME_LOOK", GAME_LOOK },
		{ "GAME_SNEAK", GAME_SNEAK },
		{ "GAME_STRAFELEFT", GAME_STRAFELEFT },
		{ "GAME_STRAFERIGHT", GAME_STRAFERIGHT },
		{ "GAME_SCREEN_STATUS", GAME_SCREEN_STATUS },
		{ "GAME_SCREEN_LOG", GAME_SCREEN_LOG },
		{ "GAME_SCREEN_MAP", GAME_SCREEN_MAP },
		{ "GAME_LOOK_FP", GAME_LOOK_FP },
		{ "GAME_LOCK_TARGET", GAME_LOCK_TARGET },
		{ "GAME_PARADE", GAME_PARADE },
		{ "GAME_ACTIONLEFT", GAME_ACTIONLEFT },
		{ "GAME_ACTIONRIGHT", GAME_ACTIONRIGHT },
		{ "GAME_LAME_POTION", GAME_LAME_POTION },
		{ "GAME_LAME_HEAL", GAME_LAME_HEAL }
	};

	int GetKey(std::string keyName)
	{
		std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);

		std::string rep("KEY_");
		auto pos = keyName.find(rep);
		if (pos != std::string::npos)
		{
			keyName.replace(pos, rep.size(), "");
		}

		if (auto it = NameToKey.find(keyName); it != NameToKey.end())
		{
			return it->second;
		}

		return -1;
	}

	std::string GetKeyName(int key)
	{
		for (auto& keyPair : NameToKey)
		{
			if (keyPair.second == key)
			{
				return keyPair.first;
			}
		}

		return "";
	}

	int GetLogicalKey(std::string keyName)
	{
		if (auto it = NameToLogicalKey.find(keyName); it != NameToLogicalKey.end())
		{
			return it->second;
		}

		return -1;
	}
}