// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
#define PI 3.1415926535897932384626 

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
		bool OnScreen;
		bool ShowMarkers;
		bool ShowList;
		HookType Hook;
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
		int HandleInput(int key = -1);
		void Close();
		void InitMap(HookType hook, int rotate = 0);
		int listWidth;
		int imgSize;
		int markX;
		int markY;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		ItemMapMode mode;
		ItemMapFilter filter;
		zCOLOR GetColor(zCVob* vob);
		ItemMapFilter GetFilterFlag(zCVob* vob);
		zVEC4 mapCoords;
		zVEC4 worldCoords;

		//For CoM ikarus maps
		void CoMHack();

	private:
		zCView* printViewMarker;
		zCView* printViewList;
		zCView* printViewSearchBar;

		std::vector<PrintItem*> vecItemsAll;
		std::vector<PrintItemUnique*> vecItemsUniqueAll;
		std::vector<PrintItem*> vecNpcsAll;
		std::vector<PrintItemUnique*> vecNpcsUniqueAll;

		std::vector<PrintItem*> vecPrintItemsCurrent;
		std::vector<PrintItemUnique*> vecPrintItemsUniqueCurrent;

		void PrintMarkers();
		void PrintList();
		void PrintSearchBar();
		std::map<ItemMapFilter, zCOLOR> colorsItems;
		std::map<std::string, zCOLOR> colorsNpcs;
		std::vector<PrintItem*>& GetCurrentVectorAll();
		std::vector<PrintItemUnique*>& GetCurrentVectorUniques();
		zSTRING GetFilterName();
		zCOLOR HexToColor(const zSTRING& hexstring);

		//For CoM ikarus maps
		int indexSpriteMapHandle = 0;
		int indexSpriteCursorHandle = 0;
		std::map<unsigned int, std::chrono::high_resolution_clock::time_point> keyPressedTS;
	};

	std::unique_ptr<ItemMap> itemMap;
}