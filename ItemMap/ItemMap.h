// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
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
		const zSTRING name;

		PrintItem(zPOS pos, zCOLOR color, int flag, const zSTRING& name)
			: pos(pos), color(color), flag(flag), name(name)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		const zSTRING name;
		int amount;

		PrintItemUnique(int instanz, const zSTRING& name, int amount)
			: instanz(instanz), name(name), amount(amount)
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

	class ItemMap
	{
	public:
		ItemMap();
		~ItemMap();
		void Print();
		bool OnScreen = false;
		bool ShowMarkers = true;
		bool ShowList = true;
		void ClearPrintItems();
		void AddPrintItem(PrintItem* printItem);
		void AddPrintItemUnique(oCItem* item);
		void SortUniques();
		void RefreshLists();
		zVEC4 margins;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		bool refresh = false;

	private:
		zCView* printViewMarker;
		zCView* printViewList;
		zCView* printViewSearchBar;
		std::vector<PrintItem*> printItemsAll;
		std::vector<PrintItemUnique*> printItemsUniqueAll;
		std::vector<PrintItem*> printItems;
		std::vector<PrintItemUnique*> printItemsUnique;
		int imgSize = 10;
		int markX;
		int markY;
		int listWidth;
		void PrintMarkers();
		void PrintList();
		void PrintSearchBar();
	};

	std::unique_ptr<ItemMap> itemMap;
}