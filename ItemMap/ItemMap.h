// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE
{
	static constexpr std::string_view PluginName = "ItemMap";
	static constexpr std::string_view PluginVersion = "1.0.5a";

	static constexpr float sin90 = 1.0f; // 90 degree sinus
	static constexpr float cos90 = 0.0f; // 90 degree cosinus

	static constexpr std::string_view textureBackground = "ITEMMAP_BACKGROUND.TGA";
	static constexpr std::string_view textureMarker = "ITEMMAP_MARKER.TGA";
	static constexpr std::string_view textureMarkerUp = "ITEMMAP_MARKER_UP.TGA";
	static constexpr std::string_view textureMarkerDown = "ITEMMAP_MARKER_DOWN.TGA";

	enum class HookType : size_t
	{
		Normal,
		CoM,
		NoHook
	};

	enum class FilterOperation : size_t
	{
		INCREMENT,
		DECREMENT
	};

	enum class ItemMapGroundLevel : size_t
	{
		SAME,
		HIGHER,
		LOWER
	};

	enum class ItemMapMode : size_t
	{
		ITEMS,
		NPCS,
		CONTAINERS,
		INTERACTIVES
	};

	enum class ItemMapFilterItems : size_t
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

	template <typename EnumT>
	class Flags
	{
		static_assert(std::is_enum_v<EnumT>, "Flags can only be specialized for enum types");

	public:
		Flags& set(EnumT e, bool value = true) noexcept
		{
			this->bits_.set(std::to_underlying(e), value);
			return *this;
		}

		[[nodiscard]] bool none() const noexcept
		{
			return this->bits_.none();
		}

		[[nodiscard]] bool compare(Flags& cmp) const noexcept
		{
			return this->bits_ == cmp.bits_;
		}

		[[nodiscard]] bool isset(EnumT e) const noexcept
		{
			return this->bits_.test(std::to_underlying(e));
		}

	private:
		std::bitset<std::to_underlying(EnumT::ALL)> bits_;
	};

	enum class ItemMapFilterNpcs : size_t
	{
		DEAD,
		ANGRY,
		FRIENDLY,
		PARTY,
		HOSTILEHUMAN,
		HOSTILEMONSTER,
		TRADER,
		PICKPOCKET,
		DEFEATEXP,
		NEUTRAL,
		ALL
	};
	using ItemMapFilterNpcsFlags = Flags<ItemMapFilterNpcs>;
	static constexpr auto ColorsNpcsMax = static_cast<size_t>(ItemMapFilterNpcs::ALL);

	static constexpr std::string_view FilterNpcsNames[ColorsNpcsMax + 1] = {
		"Dead",
		"Angry",
		"Friendly",
		"Party",
		"HostileHuman",
		"HostileMonster",
		"Trader",
		"Pickpocket",
		"DefeatExp",
		"Neutral",
		"All"
	};

	static constexpr std::string_view DefaultColorsNpcs[ColorsNpcsMax] = {
		"#000000",
		"#FF8000",
		"#00FF00",
		"#AFFFAF",
		"#C800C8",
		"#FF0000",
		"#FFFF80",
		"#80AFFF",
		"#123456",
		"#696969"
	};

	enum class ItemMapFilterContainers : size_t
	{
		ITEMS,
		LOCKPICK,
		KEY_OWNED,
		KEY_NOT_OWNED,
		OPEN,
		ALL
	};
	static constexpr auto ColorsContainersMax = static_cast<size_t>(ItemMapFilterContainers::ALL);

	static constexpr std::string_view FilterContainersNames[ColorsContainersMax + 1] = {
		"ItemsInContainers",
		"Lockpick",
		"OwnedKey",
		"NotOwnedKey",
		"Open",
		"All"
	};

	static constexpr std::string_view DefaultColorsContainers[ColorsContainersMax] = {
		"#FFFFFF",
		"#FF8000",
		"#C800C8",
		"#FF0000",
		"#00FF00"
	};

	template<FilterOperation op>
	void CheckFilterRange(auto& enumFilter)
	{
		using EnumType = std::decay_t<decltype(enumFilter)>;
		int filter = static_cast<int>(enumFilter);

		filter = op == FilterOperation::INCREMENT ? std::min(filter + 1, static_cast<int>(EnumType::ALL)) : std::max(filter - 1, 0);

		enumFilter = static_cast<EnumType>(filter);
	}

	static constexpr size_t HelpMax = 15;
	static constexpr std::string_view Help[HelpMax] = {
		"MAP KEY, ESC - Close map",
		"TAB - On/Off Transparency of panels (search and list)",
		"F1 - On/Off Search bar",
		"F2 - On/Off Markers on map",
		"F3 - On/Off List of unique items/npcs with total amount",
		"F4 - On/Off This help screen :)",
		"F5 - Switch category to Items",
		"F6 - Switch category to NPCs",
		"F7 - Switch category to Containers",
		"F8 - Switch category to Interactives",
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
		std::variant<ItemMapFilterItems, ItemMapFilterNpcsFlags, ItemMapFilterContainers, int> flags;
		ItemMapGroundLevel groundlevel;

		PrintItem(zPOS pos, zCOLOR color, const zSTRING& name, const zSTRING& instancename, std::variant<ItemMapFilterItems, ItemMapFilterNpcsFlags, ItemMapFilterContainers, int> flags, ItemMapGroundLevel groundlevel)
			: pos(pos), color(color), name(name), instancename(instancename), flags(flags), groundlevel(groundlevel)
		{}
	};

	struct PrintItemUnique
	{
		int instanz;
		zSTRING name;
		zSTRING instancename;
		int count;
		int totalamount;
		std::variant<ItemMapFilterItems, ItemMapFilterNpcsFlags, ItemMapFilterContainers, int> flags;

		PrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int count, int totalamount, std::variant<ItemMapFilterItems, ItemMapFilterNpcsFlags, ItemMapFilterContainers, int> flags)
			: instanz(instanz), name(name), instancename(instancename), count(count), totalamount(totalamount), flags(flags)
		{}
	};

	class ItemMap
	{
	public:
		ItemMap();
		~ItemMap();
		void Print();
		void ClearPrintItems();
		void AddPrintItem(oCItem* item, zPOS pos, ItemMapGroundLevel groundlevel);
		void AddPrintNpc(oCNpc* npc, zPOS pos, ItemMapGroundLevel groundlevel);
		void AddPrintItemInContainer(oCItem* item, zPOS pos, ItemMapGroundLevel groundlevel, zCOLOR color);
		void AddPrintContainer(oCMobContainer* container, zPOS pos, ItemMapGroundLevel groundlevel);
		void AddPrintInteractive(oCMobInter* inter, zPOS pos, ItemMapGroundLevel groundlevel);
		void SortUniques();
		void RefreshLists();
		void UpdateSettings();
		void ResizeMarkers(int size);
		void ResizeList(int size);
		void HandleInput();
		void Close();
		bool TryInitMap(oCViewDocument* document);
		void InitMap(HookType hook, int rotate = 0);
		zCOLOR GetColor(std::variant<ItemMapFilterItems, ItemMapFilterNpcsFlags, ItemMapFilterContainers> flags);
		ItemMapFilterItems GetFilterFlagItems(oCItem* item);
		ItemMapFilterNpcsFlags GetFilterFlagNpcs(oCNpc* npc);
		ItemMapFilterContainers GetFilterFlagContainers(oCMobContainer* container);
		HookType Hook;
		bool OnScreen;
		int listWidth;
		int imgSize;
		int markX;
		int markY;
		size_t listPage;
		size_t listPageMax;
		wstring search;
		string searchStringSeparator;
		ItemMapMode mode;
		ItemMapFilterItems filterItems;
		ItemMapFilterNpcs filterNpcs;
		ItemMapFilterContainers filterContainers;
		zVEC4 mapCoords;
		zVEC4 worldCoords;

		bool NewBalanceWispRule;
		int NewBalanceWispRuleBitflag;

		void GetPickPockets();
		void GetTraders();

		void InitMapHack();
		std::chrono::high_resolution_clock::time_point LastInit;

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
		std::vector<PrintItem*> vecContainersAll;
		std::vector<PrintItemUnique*> vecContainersUniqueAll;
		std::vector<PrintItem*> vecItemsInContainersAll;
		std::vector<PrintItemUnique*> vecItemsInContainersUniqueAll;
		std::vector<PrintItem*> vecInteractivesAll;
		std::vector<PrintItemUnique*> vecInteractivesUniqueAll;

		std::vector<PrintItem*> vecPrintItemsCurrent;
		std::vector<PrintItemUnique*> vecPrintItemsUniqueCurrent;
		int CurrentUniquesTotalCount;

		zCOLOR colorsItems[ColorsItemsMax];
		zCOLOR colorsNpcs[ColorsNpcsMax];
		zCOLOR colorsContainers[ColorsContainersMax];
		bool ShowMarkers;
		bool ShowList;
		bool ShowSearchBar;
		bool ShowHelp;
		bool SearchBarActive;

		bool ShowFilteredStaticColor;
		zCOLOR colorStaticFilter;
		bool RememberSearchInput;

		bool TransparentPanels;

		int NPC_TYPE_FRIEND;

		void SetNpcFlag(ItemMapFilterNpcsFlags& npcFlags, ItemMapFilterNpcs filterFlag);
		bool HasNpcFlag(ItemMapFilterNpcsFlags& npcFlags, ItemMapFilterNpcs filterFlag);

		std::vector<oCInfo*> pickpocketInfos;
		int indexCanStealNpcAST;
		bool CanBePickPocketed(oCNpc* npc);
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