// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
	enum
	{
		ITEMMAP_MODE_ITEMS = 0,
		ITEMMAP_MODE_NPCS,
		ITEMMAP_MODE_MAX
	};

	enum
	{
		ITEMMAP_FILTER_PLANT = 0,
		ITEMMAP_FILTER_MELEE,
		ITEMMAP_FILTER_RANGED,
		ITEMMAP_FILTER_ARMOR,
		ITEMMAP_FILTER_DOC,
		ITEMMAP_FILTER_SPELL,
		ITEMMAP_FILTER_MAGICITEM,
		ITEMMAP_FILTER_POTION,
		ITEMMAP_FILTER_FOOD,
		ITEMMAP_FILTER_NONE,
		ITEMMAP_FILTER_ALL
	};

	inline auto operator < (const zSTRING& left, const zSTRING& right)
	{
		return strcmp(left.ToChar(), right.ToChar()) < 0;
	}

	inline auto operator > (const zSTRING& left, const zSTRING& right)
	{
		return strcmp(left.ToChar(), right.ToChar()) > 0;
	}

	struct PrintItem
	{
		zPOS pos;
		zCOLOR color;
		int flag;
		zSTRING name;

		PrintItem(zPOS pos, zCOLOR color, int flag, const zSTRING& name)
			: pos(pos), color(color), flag(flag), name(name)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		int amount;
		int flag;

		PrintItemUnique(int instanz, const zSTRING& name, int amount, int flag)
			: instanz(instanz), name(name), amount(amount), flag(flag)
		{}

		bool operator < (const PrintItemUnique& printItemUnique) const
		{
			return name < printItemUnique.name;
		}
	};

	struct SortPrintItemUnique
	{
		bool operator () (const PrintItemUnique* left, const PrintItemUnique* right) const
		{
			return *left < *right;
		}
	};

	struct PrintNpc
	{
		zPOS pos;
		zSTRING name;
		zCOLOR color;

		PrintNpc(zPOS pos, const zSTRING& name, zCOLOR color)
			: pos(pos), name(name), color(color)
		{}
	};

	struct PrintNpcUnique
	{
		int instanz;
		zSTRING name;
		int count;

		PrintNpcUnique(int instanz, const zSTRING& name, int count)
			: instanz(instanz), name(name), count(count)
		{}

		bool operator < (const PrintNpcUnique& printNpcUnique) const
		{
			return name < printNpcUnique.name;
		}
	};

	struct SortPrintNpcUnique
	{
		bool operator () (const PrintNpcUnique* left, const PrintNpcUnique* right) const
		{
			return *left < *right;
		}
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
		void AddPrintItemUnique(int instanz, const zSTRING& name, int amount, int flag);
		void AddPrintNpc(PrintNpc* printItem);
		void AddPrintNpcUnique(oCNpc* npc);
		void SortUniques();
		void RefreshLists();
		zVEC4 margins;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		int mode = ITEMMAP_MODE_ITEMS;
		int filter = ITEMMAP_FILTER_PLANT;

	private:
		zCView* printViewMarker;
		zCView* printViewList;
		zCView* printViewSearchBar;

		std::vector<PrintItem*> printItemsAll;
		std::vector<PrintItemUnique*> printItemsUniqueAll;
		std::vector<PrintItem*> printItems;
		std::vector<PrintItemUnique*> printItemsUnique;

		std::vector<PrintNpc*> printNpcsAll;
		std::vector<PrintNpcUnique*> printNpcsUniqueAll;
		std::vector<PrintNpc*> printNpcs;
		std::vector<PrintNpcUnique*> printNpcsUnique;

		int imgSize = 10;
		int markX;
		int markY;
		int listWidth = 2500;
		void PrintItemsMarkers();
		void PrintItemsList();
		void PrintNpcsMarkers();
		void PrintNpcsList();
		void PrintSearchBar();
		zSTRING GetFilterName();
	};

	std::unique_ptr<ItemMap> itemMap;
}