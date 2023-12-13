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

	enum class ItemMapFilter : int
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
	static constexpr auto ColorsItemsMax = static_cast<size_t>(ItemMapFilter::ALL);

	static constexpr std::string_view FilterNames[ColorsItemsMax + 1] = {
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
		HOSTILEHUMAN,
		HOSTILEMONSTER,
		ANGRY,
		FRIENDLY,
		PARTY,
		ALL
	};
	static constexpr auto ColorsNpcsMax = static_cast<size_t>(ItemMapFilterNpcs::ALL);

	static constexpr std::string_view FilterNpcsNames[ColorsNpcsMax + 1] = {
		"Dead",
		"HostileHuman",
		"HostileMonster",
		"Angry",
		"Friendly",
		"Party",
		"All"
	};

	static constexpr std::string_view DefaultColorsNpcs[ColorsNpcsMax] = {
		"#000000",
		"#C800C8",
		"#FF0000",
		"#FF8000",
		"#00FF00",
		"#AFFFAF"
	};

	static constexpr size_t HelpMax = 11;
	static constexpr std::string_view Help[HelpMax] = {
		"F1 - On/Off Search bar",
		"F2 - On/Off Markers on map",
		"F3 - On/Off List of items/npcs",
		"F4 - On/Off This help screen :)",
		"F5 - Switch to Items",
		"F6 - Switch to NPCs",
		"Shift + Backspace - Clear search bar (if active)",
		"Arrow Left/Right - Change filter (only items)",
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
		ItemMapFilter flag;

		PrintItem(zPOS pos, zCOLOR color, const zSTRING& name, const zSTRING& instancename, ItemMapFilter flag = ItemMapFilter::NONE)
			: pos(pos), color(color), name(name), instancename(instancename), flag(flag)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		zSTRING instancename;
		int num;
		ItemMapFilter flag;

		PrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int num, ItemMapFilter flag = ItemMapFilter::NONE)
			: instanz(instanz), name(name), instancename(instancename), num(num), flag(flag)
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
		void AddPrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int amount, ItemMapFilter flag);
		void AddPrintNpc(PrintItem* printItem);
		void AddPrintNpcUnique(oCNpc* npc);
		void SortUniques();
		void RefreshLists();
		void UpdateSettings();
		void ResizeMarkers(int size);
		void ResizeList(int size);
		void HandleInput();
		void Close();
		void InitMap(HookType hook, int rotate = 0);
		zCOLOR GetColor(zCVob* vob);
		ItemMapFilter GetFilterFlag(zCVob* vob);
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
		ItemMapFilter filter;
		zVEC4 mapCoords;
		zVEC4 worldCoords;

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

		//For CoM ikarus maps
		int indexSpriteMapHandle = 0;
		int indexSpriteCursorHandle = 0;
	};

	std::unique_ptr<ItemMap> itemMap;
}