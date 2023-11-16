// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
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
		bool OnScreen = false;
		bool ShowMarkers = true;
		bool ShowList = false;
		void ClearPrintItems();
		void AddPrintItem(PrintItem* printItem);
		void AddPrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int amount, ItemMapFilter flag);
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
		ItemMapMode mode = ItemMapMode::ITEMS;
		ItemMapFilter filter = ItemMapFilter::PLANT;
		zCOLOR GetColor(zCVob* vob);
		ItemMapFilter GetFilterFlag(zCVob* vob);

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
		std::vector<PrintItem*>& GetCurrentVectorAll();
		std::vector<PrintItemUnique*>& GetCurrentVectorUniques();
		zSTRING GetFilterName();
	};

	std::unique_ptr<ItemMap> itemMap;
}