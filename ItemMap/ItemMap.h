// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
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

	string FilterNames[ColorsItemsMax + 1] = {
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

	string DefaultColorsItems[ColorsItemsMax] = {
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

	string FilterNpcsNames[ColorsNpcsMax + 1] = {
		"Dead",
		"HostileHuman",
		"HostileMonster",
		"Angry",
		"Friendly",
		"Party",
		"All"
	};

	string DefaultColorsNpcs[ColorsNpcsMax] = {
		"#000000",
		"#C800C8",
		"#FF0000",
		"#FF8000",
		"#00FF00",
		"#AFFFAF"
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
		std::vector<PrintItem*>& GetCurrentVectorAll();
		std::vector<PrintItemUnique*>& GetCurrentVectorUniques();
		zSTRING GetFilterName();
		zCOLOR HexToColor(const zSTRING& hexstring);

		zCView* printViewMarker;
		zCView* printViewList;
		zCView* printViewSearchBar;

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
		bool SearchBarActive;

		int NPC_TYPE_FRIEND;

		//For CoM ikarus maps
		int indexSpriteMapHandle = 0;
		int indexSpriteCursorHandle = 0;
	};

	std::unique_ptr<ItemMap> itemMap;
}