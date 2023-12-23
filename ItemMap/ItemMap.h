// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
	static constexpr std::string_view PluginName = "ItemMap";
	static constexpr std::string_view PluginVersion = "1.0.0";
	
	static constexpr float sin90 = 1.0f; // 90 degree sinus
	static constexpr float cos90 = 0.0f; // 90 degree cosinus

	enum class HookType : int
	{
		Normal,
		CoM,
		NoHook
	};

	enum class ItemMapMode : int
	{
		ITEMS,
		NPCS
	};

	enum class ItemMapFilterItems : int
	{
		PLANT,
		MELEE,
		RANGED,
		ARMOR,
		DOC,
		SPELL,
		MAGICITEM,
		POTION,
		FOOD,
		NONE,
		ALL
	};
	static constexpr auto ColorsItemsMax = static_cast<size_t>(ItemMapFilterItems::ALL);

	static constexpr std::string_view FilterItemsNames[ColorsItemsMax + 1] = {
		"Plants",
		"Melee",
		"Ranged",
		"Armors",
		"Docs",
		"Spells",
		"MagicItems",
		"Potions",
		"Food",
		"None",
		"All"
	};

	static constexpr std::string_view DefaultColorsItems[ColorsItemsMax] = {
		"#00FF00",
		"#FF0000",
		"#FF0000",
		"#800080",
		"#FFFF7F",
		"#0080FF",
		"#FF00FF",
		"#00FFFF",
		"#FF8000",
		"#808080"
	};

	enum class ItemMapFilterNpcs : int
	{
		DEAD,
		ANGRY,
		FRIENDLY,
		PARTY,
		HOSTILEHUMAN,
		HOSTILEMONSTER,
		TRADER,
#if ENGINE >= Engine_G2
		PICKPOCKET,
#endif
		ALL
	};
	static constexpr auto ColorsNpcsMax = static_cast<size_t>(ItemMapFilterNpcs::ALL);

	static constexpr std::string_view FilterNpcsNames[ColorsNpcsMax + 1] = {
		"Dead",
		"Angry",
		"Friendly",
		"Party",
		"HostileHuman",
		"HostileMonster",
		"Trader",
#if ENGINE >= Engine_G2
		"Pickpocket",
#endif
		"All"
	};

	static constexpr std::string_view DefaultColorsNpcs[ColorsNpcsMax] = {
		"#000000",
		"#FF8000",
		"#00FF00",
		"#AFFFAF",
		"#C800C8",
		"#FF0000",
		"#FFFF80"
#if ENGINE >= Engine_G2
		,"#80AFFF"
#endif
	};

	static constexpr size_t HelpMax = 12;
	static constexpr std::string_view Help[HelpMax] = {
		"MAP KEY, ESC - Close map",
		"F1 - On/Off Search bar",
		"F2 - On/Off Markers on map",
		"F3 - On/Off List of unique items/npcs with total amount",
		"F4 - On/Off This help screen :)",
		"F5 - Switch category to Items",
		"F6 - Switch category to NPCs",
		"Shift + Backspace - Clear search bar (if active)",
		"Arrow Left/Right - Change filter",
		"Arrow Up/Down - Change page on list (if visible)",
		"CTRL + Arrow Up/Down - Change marker size (if visible)",
		"CTRL + Arrow Left/Right - Change list width (if visible)"
	};

	struct PrintItem
	{
		zPOS pos;
		zCOLOR color;
		zSTRING name;
		zSTRING instancename;
		std::variant<ItemMapFilterItems, int> flags;

		PrintItem(zPOS pos, zCOLOR color, const zSTRING& name, const zSTRING& instancename, std::variant<ItemMapFilterItems, int> flags)
			: pos(pos), color(color), name(name), instancename(instancename), flags(flags)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		zSTRING instancename;
		int num;
		std::variant<ItemMapFilterItems, int> flags;

		PrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int num, std::variant<ItemMapFilterItems, int> flags)
			: instanz(instanz), name(name), instancename(instancename), num(num), flags(flags)
		{}
	};

	class ItemMap
	{
	public:
		ItemMap();
		~ItemMap();
		void Print();
		void ClearPrintItems();
		void AddPrintItem(oCItem* item, zPOS pos);
		void AddPrintNpc(oCNpc* npc, zPOS pos);
		void SortUniques();
		void RefreshLists();
		void UpdateSettings();
		void ResizeMarkers(int size);
		void ResizeList(int size);
		void HandleInput();
		void Close();
		void InitMap(HookType hook, int rotate = 0);
		zCOLOR GetColor(std::variant<ItemMapFilterItems, int> flags);
		ItemMapFilterItems GetFilterFlagItems(oCItem* item);
		int GetFilterFlagNpcs(oCNpc* npc);
		HookType Hook;
		bool OnScreen;
		int listWidth;
		int imgSize;
		int markX;
		int markY;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		ItemMapMode mode;
		ItemMapFilterItems filterItems;
		ItemMapFilterNpcs filterNpcs;
		zVEC4 mapCoords;
		zVEC4 worldCoords;

#if ENGINE >= Engine_G2
		void GetPickPockets();
#endif
		void GetTraders();

		//For CoM ikarus maps
		void CoMHack();

	private:
		void PrintMarkers();
		void PrintList();
		void PrintSearchBar();
		void PrintHelp();
		std::vector<PrintItem*>& GetCurrentVectorAll();
		std::vector<PrintItemUnique*>& GetCurrentVectorUniques();
		zSTRING GetFilterName() const;
		zCOLOR HexToColor(std::string_view hexstring);

		zCView* printViewMarker;
		zCView* printViewList;
		zCView* printViewSearchBar;
		zCView* printViewHelp;

		std::vector<PrintItem*> vecItemsAll;
		std::vector<PrintItemUnique*> vecItemsUniqueAll;
		std::vector<PrintItem*> vecNpcsAll;
		std::vector<PrintItemUnique*> vecNpcsUniqueAll;

		std::vector<PrintItem*> vecPrintItemsCurrent;
		std::vector<PrintItemUnique*> vecPrintItemsUniqueCurrent;

		zCOLOR colorsItems[ColorsItemsMax];
		zCOLOR colorsNpcs[ColorsNpcsMax];
		bool ShowMarkers;
		bool ShowList;
		bool ShowSearchBar;
		bool ShowHelp;
		bool SearchBarActive;

		bool ShowFilteredStaticColor;
		zCOLOR colorStaticFilter;

		int NPC_TYPE_FRIEND;

		void SetNpcFlag(int& npcFlags, ItemMapFilterNpcs filterFlag);
		bool HasNpcFlag(int npcFlags, ItemMapFilterNpcs filterFlag);
#if ENGINE >= Engine_G2
		std::vector<oCInfo*> pickpocketInfos;
		int indexCanStealNpcAST;
		bool CanBePickPocketed(oCNpc* npc);
#endif
		std::vector<oCInfo*> traderInfos;
		bool CanTrade(oCNpc* npc);
		bool ShowTradersNoCond;

		//For CoM ikarus maps
		int indexSpriteMapHandle = 0;
		int indexSpriteCursorHandle = 0;
		void Rotate90Degree(int& x, int& y, zVEC2& mapCenter);
	};

	std::unique_ptr<ItemMap> itemMap;
}