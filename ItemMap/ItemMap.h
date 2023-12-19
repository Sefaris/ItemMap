// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
	static constexpr std::string_view PluginName = "ItemMap";
	static constexpr std::string_view PluginVersion = "1.0.0";

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
		TRADER,
#if ENGINE >= Engine_G2
		PICKPOCKET,
#endif
		HOSTILEHUMAN,
		HOSTILEMONSTER,
		ALL
	};
	static constexpr auto ColorsNpcsMax = static_cast<size_t>(ItemMapFilterNpcs::ALL);

	static constexpr std::string_view FilterNpcsNames[ColorsNpcsMax + 1] = {
		"Dead",
		"Angry",
		"Friendly",
		"Party",
		"Trader",
#if ENGINE >= Engine_G2
		"Pickpocket",
#endif
		"HostileHuman",
		"HostileMonster",
		"All"
	};

	static constexpr std::string_view DefaultColorsNpcs[ColorsNpcsMax] = {
		"#000000",
		"#FF8000",
		"#00FF00",
		"#AFFFAF",
		"#ffff80",
#if ENGINE >= Engine_G2
		"#80afff",
#endif
		"#C800C8",
		"#FF0000"
	};

	static constexpr size_t HelpMax = 12;
	static constexpr std::string_view Help[HelpMax] = {
		"MAP KEY, ESC - Close map",
		"F1 - On/Off Search bar",
		"F2 - On/Off Markers on map",
		"F3 - On/Off List of unique items/npcs",
		"F4 - On/Off This help screen :)",
		"F5 - Switch category to Items",
		"F6 - Switch category to NPCs",
		"Shift + Backspace - Clear search bar (if active)",
		"Arrow Left/Right - Change filter",
		"Arrow Up/Down - Scroll list (if visible)",
		"CTRL + Arrow Up/Down - Change marker size (if visible)",
		"CTRL + Arrow Left/Right - Change list size (if visible)"
	};

	struct PrintItem
	{
		zPOS pos;
		zCOLOR color;
		zSTRING name;
		zSTRING instancename;
		ItemMapFilterItems flagItems;
		int flagNpcs;

		PrintItem(zPOS pos, zCOLOR color, const zSTRING& name, const zSTRING& instancename, ItemMapFilterItems flagItems, int flagNpcs)
			: pos(pos), color(color), name(name), instancename(instancename), flagItems(flagItems), flagNpcs(flagNpcs)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		zSTRING instancename;
		int num;
		ItemMapFilterItems flagItems;
		int flagNpcs;

		PrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int num, ItemMapFilterItems flagItems, int flagNpcs)
			: instanz(instanz), name(name), instancename(instancename), num(num), flagItems(flagItems), flagNpcs(flagNpcs)
		{}
	};

	class ItemMap
	{
	public:
		ItemMap();
		~ItemMap();
		void Print();
		void ClearPrintItems();
		void AddPrintItem(PrintItem* printItem);
		void AddPrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int amount, ItemMapFilterItems flagItems, int flagNpcs);
		void AddPrintNpc(PrintItem* printItem);
		void AddPrintNpcUnique(int instanz, const zSTRING& name, const zSTRING& instancename, ItemMapFilterItems flagItems, int flagNpcs);
		void SortUniques();
		void RefreshLists();
		void UpdateSettings();
		void ResizeMarkers(int size);
		void ResizeList(int size);
		void HandleInput();
		void Close();
		void InitMap(HookType hook, int rotate = 0);
		zCOLOR GetColor(ItemMapFilterItems flagItems, int flagNpcs);
		ItemMapFilterItems GetFilterFlagItems(zCVob* vob);
		int GetFilterFlagNpcs(zCVob* vob);
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
		zSTRING GetFilterName();
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

		int NPC_TYPE_FRIEND;

#if ENGINE >= Engine_G2
		zCArray<oCInfo*> pickpocketInfos;
		int indexCanStealNpcAST;
		bool CanBePickPocketed(oCNpc* npc);
#endif
		zCArray<oCInfo*> traderInfos;
		bool CanTrade(oCNpc* npc);

		//For CoM ikarus maps
		int indexSpriteMapHandle = 0;
		int indexSpriteCursorHandle = 0;
	};

	std::unique_ptr<ItemMap> itemMap;
}