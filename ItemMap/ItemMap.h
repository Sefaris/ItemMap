// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
	enum class ItemMapMode : int
	{
		ITEMS,
		NPCS,
		MAX
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
		ItemMapFilter flag;

		PrintItem(zPOS pos, zCOLOR color, const zSTRING& name, ItemMapFilter flag = ItemMapFilter::NONE)
			: pos(pos), color(color), flag(flag), name(name)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		int num;
		ItemMapFilter flag;

		PrintItemUnique(int instanz, const zSTRING& name, int num, ItemMapFilter flag = ItemMapFilter::NONE)
			: instanz(instanz), name(name), num(num), flag(flag)
		{}
	};

	class ItemMap
	{
	public:
		ItemMap();
		~ItemMap();
		void Print();
		bool OnScreen = false;
		bool ShowMarkers = true;
		bool ShowList = false;
		void ClearPrintItems();
		void AddPrintItem(PrintItem* printItem);
		void AddPrintItemUnique(int instanz, const zSTRING& name, int amount, ItemMapFilter flag);
		void AddPrintNpc(PrintItem* printItem);
		void AddPrintNpcUnique(oCNpc* npc);
		void SortUniques();
		void RefreshLists();
		int markX;
		int markY;
		zVEC4 margins;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		int mode = 0;
		int filter = 0;
		zCOLOR GetColor(oCItem* item);
		ItemMapFilter GetFilterFlag(oCItem* item);

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

		int imgSize = 10;
		int listWidth = 2000;
		void PrintMarkers();
		void PrintList();
		void PrintSearchBar();
		zSTRING GetFilterName();
	};

	std::unique_ptr<ItemMap> itemMap;
}