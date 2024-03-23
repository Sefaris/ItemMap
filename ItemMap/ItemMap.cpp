// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	ItemMap::ItemMap()
	{
		this->printViewMarker = new zCView(0, 0, 0, 0);

		this->printViewList = new zCView(0, 0, 0, 0);
		this->printViewList->InsertBack(textureBackground.data());
		this->printViewList->SetAlphaBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND);

		this->printViewSearchBar = new zCView(0, 0, 0, 0);
		this->printViewSearchBar->InsertBack(textureBackground.data());
		this->printViewSearchBar->SetAlphaBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND);

		this->printViewHelp = new zCView(0, 0, 0, 0);
		this->printViewHelp->InsertBack(textureBackground.data());

		zSTRING texturePlayerMark = "L.TGA";
		zCTexture* temp = temp->Load(texturePlayerMark, True);
		if (temp)
		{
			temp->GetPixelSize(this->markX, this->markY);
			temp->Release();
		}

		this->OnScreen = false;
		this->Hook = HookType::NoHook;
		this->ShowMarkers = true;
		this->ShowList = false;
		this->ShowSearchBar = true;
		this->ShowHelp = true;
		this->SearchBarActive = false;

		this->ShowTradersNoCond = false;
		this->RememberSearchInput = true;
		this->ShowFilteredStaticColor = false;
		this->TransparentPanels = false;

		this->search = L"";

		this->indexSpriteMapHandle = parser->GetIndex("SPRITEMAP_SPRITEHNDL");
		this->indexSpriteCursorHandle = parser->GetIndex("SPRITEMAP_SPRITECURSORHNDL");

#if ENGINE >= Engine_G2
		this->indexCanStealNpcAST = parser->GetIndex("RX_CanStealNpcAST");
#endif

		this->UpdateSettings();

		zCPar_Symbol* sym = parser->GetSymbol("NPCTYPE_FRIEND");
		this->NPC_TYPE_FRIEND = (sym) ? sym->single_intdata : Invalid;
	}

	ItemMap::~ItemMap()
	{
		if (this->printViewMarker)
		{
			delete this->printViewMarker;
		}

		if (this->printViewList)
		{
			delete this->printViewList;
		}

		if (this->printViewSearchBar)
		{
			delete this->printViewSearchBar;
		}

		if (this->printViewHelp)
		{
			delete this->printViewHelp;
		}

		this->ClearPrintItems();
	}

	void ItemMap::ResizeMarkers(int size)
	{
		this->imgSize = std::clamp(size, 5, 50);
		this->printViewMarker->SetSize(screen->anx(this->imgSize), screen->any(this->imgSize));
	}

	void ItemMap::ResizeList(int size)
	{
		this->listWidth = std::clamp((this->listWidth + size), 15, 100);
	}

	void ItemMap::UpdateSettings()
	{
		this->ShowSearchBar = zoptions->ReadBool(PluginName.data(), "ShowSearchBar", True);
		this->ShowMarkers = zoptions->ReadBool(PluginName.data(), "ShowMarkers", True);
		this->ShowList = zoptions->ReadBool(PluginName.data(), "ShowList", False);
		this->ShowHelp = zoptions->ReadBool(PluginName.data(), "ShowHelp", True);

		this->ResizeMarkers(zoptions->ReadInt(PluginName.data(), "IconSize", 10));
		this->listWidth = std::clamp(zoptions->ReadInt(PluginName.data(), "ListWidth", 25), 15, 100);
		this->TransparentPanels = zoptions->ReadBool(PluginName.data(), "TransparentPanels", False);
		this->ShowTradersNoCond = zoptions->ReadBool(PluginName.data(), "ShowTradersNoCond", False);
		this->RememberSearchInput = zoptions->ReadBool(PluginName.data(), "RememberSearchInput", True);

		this->mode = static_cast<ItemMapMode>(zoptions->ReadInt(PluginName.data(), "PrevMode", 0));
		this->filterItems = static_cast<ItemMapFilterItems>(std::clamp(zoptions->ReadInt(PluginName.data(), "PrevFilterItems", 0), 0, static_cast<int>(ItemMapFilterItems::ALL)));
		this->filterNpcs = static_cast<ItemMapFilterNpcs>(std::clamp(zoptions->ReadInt(PluginName.data(), "PrevFilterNpcs", static_cast<int>(ItemMapFilterNpcs::ALL)), 0, static_cast<int>(ItemMapFilterNpcs::ALL)));
		this->filterContainers = static_cast<ItemMapFilterContainers>(std::clamp(zoptions->ReadInt(PluginName.data(), "PrevFilterContainers", static_cast<int>(ItemMapFilterContainers::ALL)), 0, static_cast<int>(ItemMapFilterContainers::ALL)));

		for (size_t i = 0; i < ColorsItemsMax; i++)
		{
			this->colorsItems[i] = this->HexToColor(zoptions->ReadString(PluginName.data(),
				zSTRING{ std::format("ColorItem{}", FilterItemsNames[i].data()).c_str() },
				DefaultColorsItems[i].data()).ToChar());
		}

		for (size_t i = 0; i < ColorsNpcsMax; i++)
		{
			this->colorsNpcs[i] = this->HexToColor(zoptions->ReadString(PluginName.data(),
				zSTRING{ std::format("ColorNpc{}", FilterNpcsNames[i].data()).c_str() },
				DefaultColorsNpcs[i].data()).ToChar());
		}

		for (size_t i = 0; i < ColorsContainersMax; i++)
		{
			this->colorsContainers[i] = this->HexToColor(zoptions->ReadString(PluginName.data(),
				zSTRING{ std::format("ColorContainer{}", FilterContainersNames[i].data()).c_str() },
				DefaultColorsContainers[i].data()).ToChar());
		}

		this->ShowFilteredStaticColor = zoptions->ReadBool(PluginName.data(), "ShowFilteredStaticColor", False);
		this->colorStaticFilter = this->HexToColor(zoptions->ReadString(PluginName.data(), "ColorStaticFilter", "#FFFFFF").ToChar());

		int NewBalanceWispRuleDefault = parser->GetIndex("rx_updateversionvalue") == Invalid ? False : True;
		this->NewBalanceWispRule = zoptions->ReadBool(PluginName.data(), "NewBalanceWispRule", NewBalanceWispRuleDefault);
		auto sym = parser->GetSymbol("bit_item_nowisp");
		this->NewBalanceWispRuleBitflag = sym ? sym->single_intdata : Invalid;
	}

	zCOLOR ItemMap::HexToColor(std::string_view hexstring)
	{
		int r, g, b;
		if (sscanf_s(hexstring.data(), "#%02x%02x%02x", &r, &g, &b) == 3)
		{
			return zCOLOR(r, g, b);
		}

		return zCOLOR(255, 255, 255);
	}

	zSTRING ItemMap::GetFilterName() const
	{
		switch (this->mode)
		{
		case ItemMapMode::ITEMS:
			return std::format("Items - {}", FilterItemsNames[static_cast<size_t>(this->filterItems)]).c_str();
		case ItemMapMode::NPCS:
			return std::format("NPCs - {}", FilterNpcsNames[static_cast<size_t>(this->filterNpcs)]).c_str();
		case ItemMapMode::CONTAINERS:
			return std::format("Containers - {}", FilterContainersNames[static_cast<size_t>(this->filterContainers)]).c_str();
		case ItemMapMode::INTERACTIVES:
			return "Interactives";
		}
	}

	zCOLOR ItemMap::GetColor(std::variant<ItemMapFilterItems, int, ItemMapFilterContainers> flags)
	{
		if (std::holds_alternative<ItemMapFilterItems>(flags))
		{
			return this->colorsItems[static_cast<size_t>(std::get<ItemMapFilterItems>(flags))];
		}
		else if (std::holds_alternative<int>(flags))
		{
			for (size_t i = 0; i < static_cast<size_t>(ItemMapFilterNpcs::ALL); i++)
			{
				if (this->HasNpcFlag(std::get<int>(flags), static_cast<ItemMapFilterNpcs>(i)))
				{
					return this->colorsNpcs[i];
				}
			}
		}
		else if (std::holds_alternative<ItemMapFilterContainers>(flags))
		{
			return this->colorsContainers[static_cast<size_t>(std::get<ItemMapFilterContainers>(flags))];
		}

		return zCOLOR(128, 128, 128);
	}

	ItemMapFilterItems ItemMap::GetFilterFlagItems(oCItem* item)
	{
		switch (item->mainflag)
		{
		case ITM_CAT_NF:
			return ItemMapFilterItems::MELEE;
		case ITM_CAT_FF:
		case ITM_CAT_MUN:
			return ItemMapFilterItems::RANGED;
		case ITM_CAT_ARMOR:
			return ItemMapFilterItems::ARMOR;
		case ITM_CAT_FOOD:
			if (item->GetInstanceName().StartWith("ITPL_") || item->GetInstanceName().HasWordI("PLANT"))
			{
				return ItemMapFilterItems::PLANT;
			}
			return ItemMapFilterItems::FOOD;
		case ITM_CAT_DOCS:
			return ItemMapFilterItems::DOC;
		case ITM_CAT_POTION:
			return ItemMapFilterItems::POTION;
		case ITM_CAT_RUNE:
			return ItemMapFilterItems::SPELL;
		case ITM_CAT_MAGIC:
			return ItemMapFilterItems::MAGICITEM;
		}

		return ItemMapFilterItems::NONE;
	}

	void ItemMap::SetNpcFlag(int& npcFlags, ItemMapFilterNpcs filterFlag)
	{
		npcFlags |= (1 << static_cast<int>(filterFlag));
	}

	bool ItemMap::HasNpcFlag(int npcFlags, ItemMapFilterNpcs filterFlag)
	{
		int flag = 1 << static_cast<int>(filterFlag);
		return (npcFlags & flag) == flag;
	}

	ItemMapFilterContainers ItemMap::GetFilterFlagContainers(oCMobContainer* container)
	{
		auto lock = zDYNAMIC_CAST<oCMobLockable>(container);
		if (lock)
		{
			bool playerNeedKey = !lock->keyInstance.IsEmpty();
			bool playerHaveKey = playerNeedKey ? player->HaveItem_Union(lock->keyInstance) : false;
			if (playerNeedKey && playerHaveKey)
			{
				return ItemMapFilterContainers::KEY_OWNED;
			}
			else if (!lock->pickLockStr.IsEmpty())
			{
				return ItemMapFilterContainers::LOCKPICK;
			}
			else if (playerNeedKey && !playerHaveKey)
			{
				return ItemMapFilterContainers::KEY_NOT_OWNED;
			}
		}

		return ItemMapFilterContainers::OPEN;
	}

	int ItemMap::GetFilterFlagNpcs(oCNpc* npc)
	{
		int flags = 0;

		if (npc->attribute[NPC_ATR_HITPOINTS] <= 0 && npc->CanBeLooted_Union())
		{
			this->SetNpcFlag(flags, ItemMapFilterNpcs::DEAD);
		}
		else if (npc->attribute[NPC_ATR_HITPOINTS] > 0)
		{
#if ENGINE >= Engine_G2
			if (this->CanBePickPocketed(npc))
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::PICKPOCKET);
			}
#endif

#if ENGINE >= Engine_G2
			zSTRING AIV_EXP = "AIV_VictoryXPGiven";
#else
			zSTRING AIV_EXP = "AIV_WASDEFEATEDBYSC";
#endif
			if (!npc->HasFlag(NPC_FLAG_IMMORTAL) && !npc->GetAivar_Union(AIV_EXP) && npc->guild <= NPC_GIL_HUMANS)
			{
					this->SetNpcFlag(flags, ItemMapFilterNpcs::DEFEATEXP);
			}

			if (this->CanTrade(npc))
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::TRADER);
			}

			zSTRING AIV_PARTYMEMBER = "AIV_PARTYMEMBER";
			if (npc->GetAivar_Union(AIV_PARTYMEMBER))
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::PARTY);
			}

			int attitude = npc->GetPermAttitude(player);

			if (npc->IsHostile(player) || attitude == NPC_ATT_HOSTILE)
			{
				if (npc->guild <= NPC_GIL_HUMANS)
				{
					this->SetNpcFlag(flags, ItemMapFilterNpcs::HOSTILEHUMAN);
				}
				else
				{
					this->SetNpcFlag(flags, ItemMapFilterNpcs::HOSTILEMONSTER);
				}
			}
			else if (npc->IsAngry(player) || attitude == NPC_ATT_ANGRY)
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::ANGRY);
			}
			else if (npc->IsFriendly(player) || npc->npcType == this->NPC_TYPE_FRIEND || attitude == NPC_ATT_FRIENDLY)
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::FRIENDLY);
			}
			else
			{
				this->SetNpcFlag(flags, ItemMapFilterNpcs::NEUTRAL);
			}
		}

		return flags;
	}

	void ItemMap::ClearPrintItems()
	{
		for (auto it : this->vecItemsAll)
		{
			delete it;
		}
		for (auto it : this->vecItemsUniqueAll)
		{
			delete it;
		}

		for (auto it : this->vecNpcsAll)
		{
			delete it;
		}
		for (auto it : this->vecNpcsUniqueAll)
		{
			delete it;
		}

		for (auto it : this->vecContainersAll)
		{
			delete it;
		}
		for (auto it : this->vecContainersUniqueAll)
		{
			delete it;
		}

		for (auto it : this->vecItemsInContainersAll)
		{
			delete it;
		}
		for (auto it : this->vecItemsInContainersUniqueAll)
		{
			delete it;
		}

		for (auto it : this->vecInteractivesAll)
		{
			delete it;
		}
		for (auto it : this->vecInteractivesUniqueAll)
		{
			delete it;
		}

		this->vecItemsAll.clear();
		this->vecItemsUniqueAll.clear();

		this->vecNpcsAll.clear();
		this->vecNpcsUniqueAll.clear();

		this->vecContainersAll.clear();
		this->vecContainersUniqueAll.clear();

		this->vecItemsInContainersAll.clear();
		this->vecItemsInContainersUniqueAll.clear();

		this->vecInteractivesAll.clear();
		this->vecInteractivesUniqueAll.clear();

		this->vecPrintItemsCurrent.clear();
		this->vecPrintItemsUniqueCurrent.clear();
	}

	void ItemMap::SortUniques()
	{
		std::sort(this->vecItemsUniqueAll.begin(), this->vecItemsUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
			});

		std::sort(this->vecNpcsUniqueAll.begin(), this->vecNpcsUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
			});

		std::sort(this->vecContainersUniqueAll.begin(), this->vecContainersUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
			});

		std::sort(this->vecItemsInContainersUniqueAll.begin(), this->vecItemsInContainersUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
			});

		std::sort(this->vecInteractivesUniqueAll.begin(), this->vecInteractivesUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
			});
	}

	void ItemMap::PrintMarkers()
	{
		if (!this->ShowMarkers)
		{
			return;
		}

		this->printViewMarker->ClrPrintwin();

		screen->InsertItem(this->printViewMarker);
		for (auto printItem : this->vecPrintItemsCurrent)
		{

			if (printItem->groundlevel == ItemMapGroundLevel::HIGHER)
			{
				this->printViewMarker->InsertBack(textureMarkerUp.data());
			}
			else if (printItem->groundlevel == ItemMapGroundLevel::LOWER)
			{
				this->printViewMarker->InsertBack(textureMarkerDown.data());
			}
			else
			{
				this->printViewMarker->InsertBack(textureMarker.data());
			}

			this->printViewMarker->SetPos(printItem->pos.X - (screen->anx(this->imgSize) / 2), printItem->pos.Y - (screen->any(this->imgSize) / 2));

			if (this->ShowFilteredStaticColor &&
				!(this->mode == ItemMapMode::ITEMS && this->filterItems == ItemMapFilterItems::ALL) &&
				!(this->mode == ItemMapMode::NPCS && this->filterNpcs == ItemMapFilterNpcs::ALL) &&
				this->mode != ItemMapMode::INTERACTIVES &&
				this->mode != ItemMapMode::CONTAINERS
				)
			{
				this->printViewMarker->SetColor(this->colorStaticFilter);
			}
			else
			{
				this->printViewMarker->SetColor(printItem->color);
			}

			this->printViewMarker->Blit();
		}
		screen->RemoveItem(this->printViewMarker);
	}

	void ItemMap::PrintList()
	{
		if (!this->ShowList)
		{
			return;
		}

		this->printViewList->ClrPrintwin();
		screen->InsertItem(this->printViewList);
		this->printViewList->SetPos(0, 0);
		this->printViewList->SetSize((8182 * this->listWidth) / 100, 8192);

		if (this->TransparentPanels)
		{
			this->printViewList->SetTransparency(128);
		}
		else
		{
			this->printViewList->SetTransparency(255);
		}

		int fontHeight = this->printViewList->FontY();
		size_t maxItems = (8192 / fontHeight) - 3;
		this->listPageMax = this->vecPrintItemsUniqueCurrent.size() / (maxItems);

		if (this->listPage > this->listPageMax)
		{
			this->listPage = this->listPageMax;
		}

		int marginY = (8192 - (fontHeight * (maxItems + 2))) / 2;
		int x = 200, y = 0, temp = 1;

		zSTRING total = zSTRING{ std::format("Uniques: {} (Total count: {})", this->vecPrintItemsUniqueCurrent.size(), this->CurrentUniquesTotalCount).c_str() };
		this->printViewList->Print((8192 / 2) - (this->printViewList->FontSize(total) / 2), marginY, total);

		size_t min = this->listPage * maxItems;
		size_t max = (min + maxItems) > this->vecPrintItemsUniqueCurrent.size() ? this->vecPrintItemsUniqueCurrent.size() : (min + maxItems);
		for (size_t i = min; i < max; i++)
		{
			auto printItemUnique = this->vecPrintItemsUniqueCurrent[i];

			y = temp * fontHeight + marginY;

			zSTRING txt = [&printItemUnique]()->zSTRING
				{
					if (printItemUnique->totalamount > printItemUnique->count)
					{
						return zSTRING{ std::format("{0} x{1} (Total amount: {2})", printItemUnique->name.ToChar(), printItemUnique->count, printItemUnique->totalamount).c_str() };
					}

					return zSTRING{ std::format("{0} x{1}", printItemUnique->name.ToChar(), printItemUnique->count).c_str() };
				}();

				this->printViewList->Print(x, y, txt);

				temp++;
		}

		y = (maxItems + 1) * fontHeight + marginY;
		zSTRING page = zSTRING{ std::format("Page: {0}/{1}", this->listPage + 1, this->listPageMax + 1).c_str() };
		this->printViewList->Print((8192 / 2) - (this->printViewList->FontSize(page) / 2), y, page);

		this->printViewList->Blit();
		screen->RemoveItem(this->printViewList);
	}

	void ItemMap::PrintSearchBar()
	{
		if (!this->ShowSearchBar)
		{
			return;
		}

		this->printViewSearchBar->ClrPrintwin();

		int TopX, TopY, SizeX, SizeY;
		TopX = std::clamp(screen->anx(static_cast<int>(this->mapCoords[0]) + ((static_cast<int>(this->mapCoords[2]) - static_cast<int>(this->mapCoords[0])) / 2)) - 200, 0, 8192);
		TopY = std::clamp(screen->any(static_cast<int>(this->mapCoords[1])) + 300, 0, 8192);
		SizeX = screen->anx(static_cast<int>(this->mapCoords[2]) - static_cast<int>(this->mapCoords[0])) / 2;
		SizeY = screen->FontY() * 3;

		screen->InsertItem(this->printViewSearchBar);
		this->printViewSearchBar->SetPos(TopX, TopY);
		this->printViewSearchBar->SetSize(SizeX, SizeY);

		if (this->TransparentPanels)
		{
			this->printViewSearchBar->SetTransparency(128);
		}
		else
		{
			this->printViewSearchBar->SetTransparency(255);
		}

		int fontHeight = this->printViewSearchBar->FontY();
		int y = 0;

		this->printViewSearchBar->SetFontColor(GFX_COLDGREY);

		zSTRING txtSearchFilter = this->GetFilterName();

		if ((this->mode == ItemMapMode::ITEMS && this->filterItems > static_cast<ItemMapFilterItems>(0))
			|| (this->mode == ItemMapMode::NPCS && this->filterNpcs > static_cast<ItemMapFilterNpcs>(0))
			|| (this->mode == ItemMapMode::CONTAINERS && this->filterContainers > static_cast<ItemMapFilterContainers>(0))
			)
		{
			zSTRING less = "<<";
			this->printViewSearchBar->Print(0, y, less);
		}

		if ((this->mode == ItemMapMode::ITEMS && this->filterItems < ItemMapFilterItems::ALL)
			|| (this->mode == ItemMapMode::NPCS && this->filterNpcs < ItemMapFilterNpcs::ALL)
			|| (this->mode == ItemMapMode::CONTAINERS && this->filterContainers < ItemMapFilterContainers::ALL)
			)
		{
			zSTRING more = ">>";
			this->printViewSearchBar->Print(8192 - this->printViewSearchBar->FontSize(more), y, more);
		}

		int txtSearchFilterWidth = this->printViewSearchBar->FontSize(txtSearchFilter);
		this->printViewSearchBar->Print(0 + (8192 / 2) - (txtSearchFilterWidth / 2), fontHeight * y++, txtSearchFilter);

		this->printViewSearchBar->SetFontColor(GFX_WHITE);
		zSTRING txtSearchTitle = "Search";
		this->printViewSearchBar->Print(0, fontHeight * y++, txtSearchTitle);

		if (this->search.IsEmpty() && !this->SearchBarActive)
		{
			zSTRING txtSearchPlaceholder = "Press ENTER to search";
			this->printViewSearchBar->SetFontColor(GFX_PINK);
			this->printViewSearchBar->Print(0, fontHeight * y++, txtSearchPlaceholder);
		}
		else
		{
			auto search = this->search.WToA() + (this->SearchBarActive ? "<" : "");
			this->printViewSearchBar->SetFontColor(GFX_GREEN);
			this->printViewSearchBar->Print(0, fontHeight * y++, search);
		}

		this->printViewSearchBar->Blit();
		screen->RemoveItem(this->printViewSearchBar);
	}

	void ItemMap::PrintHelp()
	{
		if (!this->ShowHelp)
		{
			return;
		}

		this->printViewHelp->ClrPrintwin();

		int TopX, TopY, SizeX, SizeY;
		TopX = std::clamp(screen->anx(static_cast<int>(this->mapCoords[0])) + 250, 0, 8192);
		SizeX = screen->anx(static_cast<int>(this->mapCoords[2]) - static_cast<int>(this->mapCoords[0])) - 500;
		SizeY = std::clamp(static_cast<int>(screen->FontY() * (HelpMax * 2)), 0, 8192);
		TopY = (8192 - SizeY) / 2;

		screen->InsertItem(this->printViewHelp);
		this->printViewHelp->SetPos(TopX, TopY);
		this->printViewHelp->SetSize(SizeX, SizeY);

		int margin = 200;

		zSTRING title = zSTRING{ PluginName.data() };
		zSTRING version = zSTRING{ PluginVersion.data() };

		this->printViewHelp->SetFont("FONT_OLD_20_WHITE_HI.TGA");
		auto fontHeightBig = this->printViewHelp->FontY();

		this->printViewHelp->SetFontColor(GFX_DORANGE);
		auto titleX = 8192 / 2 - this->printViewHelp->FontSize(title) / 2;
		auto titleY = fontHeightBig;
		this->printViewHelp->Print(titleX, titleY, title);

		this->printViewHelp->SetFont("FONT_OLD_10_WHITE_HI.TGA");
		auto fontHeightSmall = this->printViewHelp->FontY();

		this->printViewHelp->SetFontColor(GFX_LGREY);
		auto startY = titleY + (fontHeightBig * 2);

		for (size_t i = 0; i < HelpMax; i++)
		{
			this->printViewHelp->Print(margin, startY + (i * fontHeightSmall), Help[i].data());
		}

		this->printViewHelp->SetFontColor(GFX_WARMGREY);
		auto versionX = 8192 - this->printViewHelp->FontSize(version) - margin;
		auto versionY = 8192 - fontHeightSmall - margin;
		this->printViewHelp->Print(versionX, versionY, version);

		this->printViewHelp->Blit();
		screen->RemoveItem(this->printViewHelp);
	}

	void ItemMap::Print()
	{
		if (ogame->singleStep || ogame->pause_screen)
		{
			return;
		}

		if (!this->OnScreen)
		{
			return;
		}

		this->PrintMarkers();
		this->PrintList();
		this->PrintSearchBar();
		this->PrintHelp();

		int ScreenX, ScreenY, ScreenSX, ScreenSY;
		screen->GetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
		zrenderer->SetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
	}

	void ItemMap::AddPrintItem(oCItem* item, zPOS pos, ItemMapGroundLevel groundlevel)
	{
		auto flags = this->GetFilterFlagItems(item);
		auto color = this->GetColor(flags);

		this->vecItemsAll.push_back(new PrintItem(pos, color, item->name, item->GetInstanceName(), flags, groundlevel));

		for (auto it : this->vecItemsUniqueAll)
		{
			if (it->instanz == item->instanz) {
				it->count = it->count + 1;
				it->totalamount = it->totalamount + item->amount;
				return;
			}
		}
		this->vecItemsUniqueAll.push_back(new PrintItemUnique(item->instanz, item->name, item->GetInstanceName(), 1, item->amount, flags));
	}

	void ItemMap::AddPrintNpc(oCNpc* npc, zPOS pos, ItemMapGroundLevel groundlevel)
	{
		auto flags = this->GetFilterFlagNpcs(npc);

		if (flags == 0)
		{
			return;
		}

		auto color = this->GetColor(flags);
		this->vecNpcsAll.push_back(new PrintItem(pos, color, npc->name, npc->GetInstanceName(), flags, groundlevel));

		for (auto it : this->vecNpcsUniqueAll)
		{
			if (it->instanz == npc->instanz && std::get<int>(it->flags) == flags) {
				it->count = it->count + 1;
				return;
			}
		}
		this->vecNpcsUniqueAll.push_back(new PrintItemUnique(npc->instanz, npc->name, npc->GetInstanceName(), 1, 1, flags));
	}

	void ItemMap::AddPrintInteractive(oCMobInter* inter, zPOS pos, ItemMapGroundLevel groundlevel)
	{
		auto color = zCOLOR(128, 128, 128);
		zSTRING name = "";
		auto funcName = inter->onStateFuncName;
		funcName.Lower();
		if (!inter->name.IsEmpty())
		{
			auto symbol = parser->GetSymbol(inter->name);
			if (symbol && symbol->type == zPAR_TYPE_STRING)
			{
				if (symbol->stringdata && !symbol->stringdata->IsEmpty())
				{
					name = symbol->stringdata;
				}
			}

			if (name.IsEmpty())
			{
				name = inter->name;
			}
		}
		else if (funcName.StartWith("hiddenfind") || funcName.Compare("b_scgettreasure"))
		{
			name = "HIDDEN TREASURE";
		}

		if (name.IsEmpty())
		{
			return;
		}

		if (!inter->objectName.IsEmpty() && inter->objectName.ToInt() > 0)
		{
			name += zSTRING{ " (" } + inter->objectName + zSTRING{ ")" };
		}

		this->vecInteractivesAll.push_back(new PrintItem(pos, color, name, funcName, 0, groundlevel));

		for (auto it : this->vecInteractivesUniqueAll)
		{
			if (it->name.CompareI(name)/* && it->instancename.CompareI(inter->onStateFuncName)*/) {
				it->count = it->count + 1;
				return;
			}
		}
		this->vecInteractivesUniqueAll.push_back(new PrintItemUnique(0, name, funcName, 1, 1, 0));
	}

	void ItemMap::AddPrintItemInContainer(oCItem* item, zPOS pos, ItemMapGroundLevel groundlevel, zCOLOR color)
	{
		this->vecItemsInContainersAll.push_back(new PrintItem(pos, color, item->name, item->GetInstanceName(), 0, groundlevel));

		for (auto it : this->vecItemsInContainersUniqueAll)
		{
			if (it->instanz == item->instanz) {
				it->count = it->count + 1;
				it->totalamount = it->totalamount + item->amount;
				return;
			}
		}
		this->vecItemsInContainersUniqueAll.push_back(new PrintItemUnique(item->instanz, item->name, item->GetInstanceName(), 1, item->amount, 0));
	}

	void ItemMap::AddPrintContainer(oCMobContainer* container, zPOS pos, ItemMapGroundLevel groundlevel)
	{
		zSTRING name = "";
		if (!container->name.IsEmpty()) {
			if (auto symbol = parser->GetSymbol(container->name))
			{
				if (!symbol->stringdata->IsEmpty())
				{
					name = symbol->stringdata;
				}
			}

			if (name.IsEmpty())
			{
				name = container->name;
			}
		}

		if (name.IsEmpty())
		{
			name = "<unknown>";
		}

		auto flags = this->GetFilterFlagContainers(container);
		auto color = GetColor(flags);

		auto containerInv = container->containList.next;
		while (containerInv)
		{
			auto item = containerInv->GetData();
			containerInv = containerInv->next;

			if (item->instanz < 0)
			{
				continue;
			}

			this->AddPrintItemInContainer(item, pos, groundlevel, color);
		}

		this->vecContainersAll.push_back(new PrintItem(pos, color, name, container->objectName, flags, groundlevel));

		for (auto it : this->vecContainersUniqueAll)
		{
			if (it->name.CompareI(name) && std::get<ItemMapFilterContainers>(it->flags) == flags) {
				it->count = it->count + 1;
				return;
			}
		}

		this->vecContainersUniqueAll.push_back(new PrintItemUnique(0, name, container->objectName, 1, 1, flags));
	}

	std::vector<PrintItem*>& ItemMap::GetCurrentVectorAll()
	{
		switch (this->mode)
		{
		case ItemMapMode::ITEMS:
			return this->vecItemsAll;
		case ItemMapMode::NPCS:
			return this->vecNpcsAll;
		case ItemMapMode::CONTAINERS:
			if (this->filterContainers == ItemMapFilterContainers::ITEMS)
			{
				return this->vecItemsInContainersAll;
			}
			return this->vecContainersAll;
		case ItemMapMode::INTERACTIVES:
			return this->vecInteractivesAll;
		}

		assert(false);
	}

	std::vector<PrintItemUnique*>& ItemMap::GetCurrentVectorUniques()
	{
		switch (this->mode)
		{
		case ItemMapMode::ITEMS:
			return this->vecItemsUniqueAll;
		case ItemMapMode::NPCS:
			return this->vecNpcsUniqueAll;
		case ItemMapMode::CONTAINERS:
			if (this->filterContainers == ItemMapFilterContainers::ITEMS)
			{
				return this->vecItemsInContainersUniqueAll;
			}
			return this->vecContainersUniqueAll;
		case ItemMapMode::INTERACTIVES:
			return this->vecInteractivesUniqueAll;
		}

		assert(false);
	}

	void ItemMap::RefreshLists()
	{
		this->vecPrintItemsCurrent.clear();
		this->vecPrintItemsUniqueCurrent.clear();

		auto search = this->search.WToA();

		for (auto printItem : this->GetCurrentVectorAll())
		{
			if (!search.IsEmpty() && !printItem->name.HasWordI(search) && !printItem->instancename.HasWordI(search))
			{
				continue;
			}

			if (this->mode == ItemMapMode::ITEMS && this->filterItems != ItemMapFilterItems::ALL && std::get<ItemMapFilterItems>(printItem->flags) != this->filterItems)
			{
				continue;
			}
			else if (this->mode == ItemMapMode::NPCS && this->filterNpcs != ItemMapFilterNpcs::ALL && !this->HasNpcFlag(std::get<int>(printItem->flags), this->filterNpcs))
			{
				continue;
			}
			else if (this->mode == ItemMapMode::CONTAINERS && this->filterContainers > ItemMapFilterContainers::ITEMS && this->filterContainers < ItemMapFilterContainers::ALL && std::get<ItemMapFilterContainers>(printItem->flags) != this->filterContainers)
			{
				continue;
			}

			this->vecPrintItemsCurrent.push_back(printItem);
		}

		this->CurrentUniquesTotalCount = 0;
		for (auto printItemUnique : this->GetCurrentVectorUniques())
		{
			if (!search.IsEmpty() && !printItemUnique->name.HasWordI(search) && !printItemUnique->instancename.HasWordI(search))
			{
				continue;
			}

			if (this->mode == ItemMapMode::ITEMS && this->filterItems != ItemMapFilterItems::ALL && std::get<ItemMapFilterItems>(printItemUnique->flags) != this->filterItems)
			{
				continue;
			}
			else if (this->mode == ItemMapMode::NPCS && this->filterNpcs != ItemMapFilterNpcs::ALL && !this->HasNpcFlag(std::get<int>(printItemUnique->flags), this->filterNpcs))
			{
				continue;
			}
			else if (this->mode == ItemMapMode::CONTAINERS && this->filterContainers > ItemMapFilterContainers::ITEMS && this->filterContainers < ItemMapFilterContainers::ALL && std::get<ItemMapFilterContainers>(printItemUnique->flags) != this->filterContainers)
			{
				continue;
			}

			this->CurrentUniquesTotalCount = this->CurrentUniquesTotalCount + printItemUnique->count;
			this->vecPrintItemsUniqueCurrent.push_back(printItemUnique);
		}
	}

	void ItemMap::Close()
	{
		//player->SetMovLock(FALSE);
		this->LastInit = std::chrono::high_resolution_clock::now();
		this->OnScreen = false;
		this->ClearPrintItems();
		this->Hook = HookType::NoHook;

		this->worldCoords = { 0, 0, 0, 0 };
		this->mapCoords = { 0, 0, 0, 0 };

		this->SearchBarActive = false;

		zoptions->WriteBool(PluginName.data(), "ShowSearchBar", this->ShowSearchBar, 0);
		zoptions->WriteBool(PluginName.data(), "ShowMarkers", this->ShowMarkers, 0);
		zoptions->WriteBool(PluginName.data(), "ShowList", this->ShowList, 0);
		zoptions->WriteBool(PluginName.data(), "ShowHelp", this->ShowHelp, 0);

		zoptions->WriteInt(PluginName.data(), "IconSize", this->imgSize, 0);
		zoptions->WriteInt(PluginName.data(), "ListWidth", this->listWidth, 0);
		zoptions->WriteBool(PluginName.data(), "TransparentPanels", this->TransparentPanels, 0);

		zoptions->WriteInt(PluginName.data(), "PrevMode", static_cast<int>(this->mode), 0);
		zoptions->WriteInt(PluginName.data(), "PrevFilterItems", static_cast<int>(this->filterItems), 0);
		zoptions->WriteInt(PluginName.data(), "PrevFilterNpcs", static_cast<int>(this->filterNpcs), 0);
		zoptions->WriteInt(PluginName.data(), "PrevFilterContainers", static_cast<int>(this->filterContainers), 0);
	}

	void ItemMap::HandleInput()
	{
		if (!this->OnScreen)
		{
			return;
		}

		bool shift = (GetKeyState(VK_SHIFT) & 0x8000);
		bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000);

		if (zKeyToggled(KEY_ESCAPE) || (!this->SearchBarActive && zBindPressed(GAME_SCREEN_MAP)))
		{
			this->Close();
			return;
		}

		if ((zKeyToggled(KEY_RETURN) || zKeyToggled(KEY_NUMPADENTER)) && this->ShowSearchBar)
		{
			this->SearchBarActive = !this->SearchBarActive;
		}

		if (zKeyToggled(KEY_UP))
		{
			if (ctrl && this->ShowMarkers)
			{
				this->ResizeMarkers(++this->imgSize);
			}
			else if (!ctrl && this->listPage > 0 && this->ShowList)
			{
				this->listPage--;
			}
		}

		if (zKeyToggled(KEY_DOWN))
		{
			if (ctrl && this->ShowMarkers)
			{
				this->ResizeMarkers(--this->imgSize);
			}
			else if (!ctrl && this->listPage < this->listPageMax && this->ShowList)
			{
				this->listPage++;
			}
		}

		if (zKeyToggled(KEY_RIGHT))
		{
			if (ctrl && this->ShowList)
			{
				this->ResizeList(1);
			}
			else if (!ctrl)
			{
				if (this->mode == ItemMapMode::ITEMS)
				{
					CheckFilterRange<FilterOperation::INCREMENT>(this->filterItems);
				}
				else if (this->mode == ItemMapMode::NPCS)
				{
					CheckFilterRange<FilterOperation::INCREMENT>(this->filterNpcs);
				}
				else if (this->mode == ItemMapMode::CONTAINERS)
				{
					CheckFilterRange<FilterOperation::INCREMENT>(this->filterContainers);
				}
			}
		}

		if (zKeyToggled(KEY_LEFT))
		{
			if (ctrl && this->ShowList)
			{
				this->ResizeList(-1);
			}
			else if (!ctrl)
			{
				if (this->mode == ItemMapMode::ITEMS)
				{
					CheckFilterRange<FilterOperation::DECREMENT>(this->filterItems);
				}
				else if (this->mode == ItemMapMode::NPCS)
				{
					CheckFilterRange<FilterOperation::DECREMENT>(this->filterNpcs);
				}
				else if (this->mode == ItemMapMode::CONTAINERS)
				{
					CheckFilterRange<FilterOperation::DECREMENT>(this->filterContainers);
				}
			}
		}

		if (zKeyToggled(KEY_TAB))
		{
			this->TransparentPanels = !this->TransparentPanels;
		}

		if (zKeyToggled(KEY_F1))
		{
			this->ShowSearchBar = !this->ShowSearchBar;

			if (!this->ShowSearchBar && this->SearchBarActive)
			{
				this->SearchBarActive = false;
			}
		}

		if (zKeyToggled(KEY_F2))
		{
			this->ShowMarkers = !this->ShowMarkers;
		}

		if (zKeyToggled(KEY_F3))
		{
			this->ShowList = !this->ShowList;
		}

		if (zKeyToggled(KEY_F4))
		{
			this->ShowHelp = !this->ShowHelp;
		}

		if (zKeyToggled(KEY_F5) && this->mode != ItemMapMode::ITEMS)
		{
			this->listPage = 0;
			if (!this->RememberSearchInput)
			{
				this->search = L"";
			}
			this->mode = ItemMapMode::ITEMS;
		}

		if (zKeyToggled(KEY_F6) && this->mode != ItemMapMode::NPCS)
		{
			this->listPage = 0;
			if (!this->RememberSearchInput)
			{
				this->search = L"";
			}
			this->mode = ItemMapMode::NPCS;
		}

		if (zKeyToggled(KEY_F7) && this->mode != ItemMapMode::CONTAINERS)
		{
			this->listPage = 0;
			if (!this->RememberSearchInput)
			{
				this->search = L"";
			}
			this->mode = ItemMapMode::CONTAINERS;
		}

		if (zKeyToggled(KEY_F8) && this->mode != ItemMapMode::INTERACTIVES)
		{
			this->listPage = 0;
			if (!this->RememberSearchInput)
			{
				this->search = L"";
			}
			this->mode = ItemMapMode::INTERACTIVES;
		}

		if (zKeyToggled(KEY_BACKSPACE) && this->SearchBarActive && this->ShowSearchBar)
		{
			if (shift)
			{
				this->search = L"";
			}
			else if (this->search.Length() > 0)
			{
				this->search = this->search.Copy(0, this->search.Length() - 1);
			}
		}

		if (this->SearchBarActive && this->ShowSearchBar && zinput->AnyKeyPressed())
		{
			BYTE keys[256] = {};
			auto keyboardLayout = GetKeyboardLayout(0);

			if (GetKeyboardState(keys) != FALSE)
			{
				keys[VK_CAPITAL] = (BYTE)GetKeyState(VK_CAPITAL);
				keys[VK_SHIFT] = (BYTE)GetKeyState(VK_SHIFT);

				wchar_t buff[] = { 0,0 };

				for (int i = 0; i < MAX_KEYS; i++)
				{
					auto scan = MapVirtualKeyExW(i, MAPVK_VSC_TO_VK_EX, keyboardLayout);
					if (
						scan != 0
						&& (zKeyToggled(i))
						)
					{
						auto numChars = ToUnicodeEx(scan, scan, keys, buff, 2, 0, keyboardLayout);
						if (numChars == 1 && iswprint(buff[0]))
						{
							this->search = this->search + buff;
						}
					}
				}
			}
		}

		zinput->ClearKeyBuffer();
		this->RefreshLists();

		return;
	}

	bool ItemMap::TryInitMap(oCViewDocument* document)
	{
		oCViewDocumentMap* docMap = dynamic_cast<oCViewDocumentMap*>(document);

		if (!docMap)
		{
			return false;
		}

		if (!docMap->ViewPageMap)
		{
			return false;
		}

		zSTRING mapLevelName = docMap->Level;
		mapLevelName.Replace("/", "\\");
		zSTRING worldLevelName = ogame->GetGameWorld()->GetWorldFilename();
		worldLevelName.Replace("/", "\\");

		if (!worldLevelName.CompareI(mapLevelName))
		{
			return false;
		}
		
		auto mapView = docMap->ViewPageMap;
		auto mapPos = mapView->PixelPosition;
		auto mapSize = mapView->PixelSize;

		float TopX = static_cast<float>(mapPos.X);
		float TopY = static_cast<float>(mapPos.Y);
		float BottomX = static_cast<float>(TopX + mapSize.X);
		float BottomY = static_cast<float>(TopY + mapSize.Y);

		itemMap->mapCoords = zVEC4(TopX, TopY, BottomX, BottomY);

		auto& worldBox = ogame->GetGameWorld()->bspTree.bspRoot->bbox3D;
		itemMap->worldCoords = { 0, 0, 0, 0 };

#if ENGINE >= Engine_G2
		itemMap->worldCoords[0] = docMap->LevelCoords[0];
		itemMap->worldCoords[1] = docMap->LevelCoords[3];
		itemMap->worldCoords[2] = docMap->LevelCoords[2];
		itemMap->worldCoords[3] = docMap->LevelCoords[1];
#endif

		if (itemMap->worldCoords.Length() == 0.0f)
		{
			itemMap->worldCoords[0] = worldBox.mins[0];
			itemMap->worldCoords[1] = worldBox.mins[2];
			itemMap->worldCoords[2] = worldBox.maxs[0];
			itemMap->worldCoords[3] = worldBox.maxs[2];
		}

		itemMap->InitMap(HookType::Normal);

		return true;
	}

	void ItemMap::InitMapHack()
	{
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - this->LastInit) < std::chrono::milliseconds(500))
		{
			return;
		}

		if (!this->OnScreen && this->Hook == HookType::NoHook)
		{
			auto& docMan = oCDocumentManager::GetDocumentManager();
			auto docList = docMan.ListDocuments->next;

			while (docList)
			{
				auto doc = docList->GetData();
				docList = docList->next;

				if (!doc)
				{
					continue;
				}

				if (!doc->HasOpened)
				{
					continue;
				}

				if (this->TryInitMap(doc))
				{
					return;
				}
			}
		}
	}

	void ItemMap::CoMHack()
	{
		if (!this->OnScreen && this->Hook == HookType::NoHook)
		{
			if (this->indexSpriteMapHandle == Invalid || this->indexSpriteCursorHandle == Invalid)
			{
				return;
			}

			int SpriteMapHandle = parser->GetSymbol(this->indexSpriteMapHandle)->single_intdata;
			int SpriteCursorHandle = parser->GetSymbol(this->indexSpriteCursorHandle)->single_intdata;

			if (!SpriteMapHandle || !SpriteCursorHandle)
			{
				return;
			}

			zCPar_Symbol* sym = nullptr;

			sym = parser->GetSymbol("SPRITEMAP_ROTATE90");
			int SpriteRotate = (sym) ? sym->single_intdata : Invalid;

			sym = parser->GetSymbol("SPRITEMAP_POSX");
			int SpriteMapPosX = (sym) ? sym->single_intdata : Invalid;

			sym = parser->GetSymbol("SPRITEMAP_POSY");
			int SpriteMapPosY = (sym) ? sym->single_intdata : Invalid;

			sym = parser->GetSymbol("SPRITEMAP_SIZE");
			int SpriteMapSize = (sym) ? (sym->single_intdata / 2) : Invalid;

			if (SpriteRotate == Invalid || SpriteMapPosX == Invalid || SpriteMapPosY == Invalid || SpriteMapSize == Invalid)
			{
				return;
			}

			auto SpriteMapTopX = static_cast<float>(SpriteMapPosX - SpriteMapSize);
			auto SpriteMapTopY = static_cast<float>(SpriteMapPosY - SpriteMapSize);
			auto SpriteMapBottomX = static_cast<float>(SpriteMapPosX + SpriteMapSize);
			auto SpriteMapBottomY = static_cast<float>(SpriteMapPosY + SpriteMapSize);

			this->mapCoords = zVEC4((SpriteMapTopX), (SpriteMapTopY), (SpriteMapBottomX), (SpriteMapBottomY));

			sym = parser->GetSymbol("SPRITEMAP_MINXF");
			this->worldCoords[0] = (sym) ? sym->single_floatdata : 0.0f;

			sym = parser->GetSymbol("SPRITEMAP_MINYF");
			this->worldCoords[1] = (sym) ? sym->single_floatdata : 0.0f;

			sym = parser->GetSymbol("SPRITEMAP_DISTXF");
			this->worldCoords[2] = (sym) ? (this->worldCoords[0] + sym->single_floatdata) : 0.0f;

			sym = parser->GetSymbol("SPRITEMAP_DISTYF");
			this->worldCoords[3] = (sym) ? (this->worldCoords[1] + sym->single_floatdata) : 0.0f;

			player->CloseInventory();
			player->CloseDeadNpc();
			player->CloseSteal();
			player->CloseTradeContainer();

			this->InitMap(HookType::CoM, SpriteRotate);
		}
	}

	void ItemMap::Rotate90Degree(int& x, int& y, zVEC2& mapCenter)
	{
		auto newx = static_cast<int>(cos90 * (x - mapCenter[0]) - sin90 * (y - mapCenter[1]) + mapCenter[0]);
		auto newy = static_cast<int>(sin90 * (x - mapCenter[0]) + cos90 * (y - mapCenter[1]) + mapCenter[1]);

		x = newx;
		y = newy;
	}

	void ItemMap::InitMap(HookType hook, int rotate)
	{
		if (ogame->singleStep || ogame->pause_screen)
		{
			return;
		}

		this->OnScreen = true;
		this->Hook = hook;
		this->listPage = 0;
		this->listPageMax = 0;
		//player->SetMovLock(TRUE);

		auto world = ogame->GetGameWorld();

		zVEC2 mapCenter = zVEC2((this->mapCoords[0] + this->mapCoords[2]) / 2, (this->mapCoords[1] + this->mapCoords[3]) / 2);
		zVEC2 worldDim = zVEC2(this->worldCoords[2] - this->worldCoords[0], this->worldCoords[3] - this->worldCoords[1]);

		zVEC2 world2map = zVEC2((this->mapCoords[2] - this->mapCoords[0]) / worldDim[0], (this->mapCoords[3] - this->mapCoords[1]) / worldDim[1]);

#if ENGINE <= Engine_G1A
		world2map[1] = world2map[1] * -1.0f;
#endif

		float playerPosY = player->GetPositionWorld()[VY];

		this->ClearPrintItems();
		auto listVobs = world->voblist->next;
		while (listVobs)
		{
			auto vob = listVobs->data;
			listVobs = listVobs->next;

			if (!vob->IsValidItem_Union() && !vob->IsValidNpc_Union() && !vob->IsValidInteractiveOrContainer_Union())
			{
				continue;
			}

			auto vobPos = vob->GetRealPosition_Union();

			if (vobPos.Length() == 0.0f)
			{
				continue;
			}

			int x = 0, y = 0;

#if ENGINE <= Engine_G1A
			x = static_cast<int>((world2map[0] * vobPos[VX]) + mapCenter[0]);
			y = static_cast<int>((world2map[1] * vobPos[VZ]) + mapCenter[1]);
#else
			x = static_cast<int>(this->mapCoords[0] + (world2map[0] * (vobPos[VX] - this->worldCoords[0])));
			y = static_cast<int>(this->mapCoords[3] - (world2map[1] * (vobPos[VZ] - this->worldCoords[1])));
#endif

			ItemMapGroundLevel groundlevel = ItemMapGroundLevel::SAME;
			if (playerPosY - vobPos[VY] > 500.0f)
			{
				groundlevel = ItemMapGroundLevel::LOWER;
			}
			else if (playerPosY - vobPos[VY] < -500.0f)
			{
				groundlevel = ItemMapGroundLevel::HIGHER;
			}

			zPOS pos;

			if (rotate)
			{
				this->Rotate90Degree(x, y, mapCenter);
			}

			pos.X = screen->anx(x);
			pos.Y = screen->any(y);

			if (this->Hook != HookType::CoM)
			{
				pos.X += screen->anx((this->markX / 2));
				pos.Y += screen->any((this->markY / 2));
			}


			if (pos.X < 0 || pos.X > 8192
				|| pos.Y < 0 || pos.Y > 8192
				)
			{
				continue;
			}

			auto vobType = vob->GetVobType();

			if (vobType == zVOB_TYPE_ITEM)
			{
				auto item = static_cast<oCItem*>(vob);

				this->AddPrintItem(item, pos, groundlevel);
			}
			else if (vobType == zVOB_TYPE_NSC)
			{
				auto npc = static_cast<oCNpc*>(vob);

				this->AddPrintNpc(npc, pos, groundlevel);
			}
			else if (vob->GetVobType() == zVOB_TYPE_MOB)
			{
				if (auto container = zDYNAMIC_CAST<oCMobContainer>(vob))
				{
					this->AddPrintContainer(container, pos, groundlevel);
				}
				else if (auto inter = zDYNAMIC_CAST<oCMobInter>(vob))
				{
					this->AddPrintInteractive(inter, pos, groundlevel);
				}
			}
		}

		this->SortUniques();
		this->RefreshLists();
	}

#if ENGINE >= Engine_G2
	void ItemMap::GetPickPockets()
	{
		this->pickpocketInfos.clear();

		auto list = ogame->GetInfoManager()->infoList.next;
		while (list) {
			auto info = list->data;
			list = list->next;

			if (!info->name.HasWordI("pickpocket") && !info->name.HasWordI("_steal") && !info->name.HasWordI("pickme"))
			{
				continue;
			}

			if (info->name.HasWordI("_DOIT") || info->name.HasWordI("_TRY"))
			{
				continue;
			}

			if (parser->GetIndex(info->name + "_DOIT") == Invalid && parser->GetIndex(info->name + "_TRY") == Invalid)
			{
				continue;
			}

			pickpocketInfos.push_back(info);
		}
	}

	bool ItemMap::CanBePickPocketed(oCNpc* npc)
	{
		if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
		{
			return false;
		}

		for (auto info : pickpocketInfos) {

			if (info->GetNpcID() != npc->GetInstance())
			{
				continue;
			}

			parser->SetInstance("SELF", npc);
			parser->SetInstance("OTHER", player);

			if (info->InfoConditions())
			{
				return true;
			}
		}

		//For Gothic 2: New Balance
		if (this->indexCanStealNpcAST != Invalid)
		{
			//NPC_GIL_HUMANS == GIL_TPL in New Balance
			if (npc->guild > NPC_GIL_HUMANS)
			{
				return false;
			}

			static auto argumentSymbol = []() -> zCPar_Symbol*
			{
				if (parser->GetSymbol(itemMap->indexCanStealNpcAST)->ele != 1)
				{
					return nullptr;
				}

				auto symbol = parser->GetSymbol(itemMap->indexCanStealNpcAST + 1);
				if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
				{
					return nullptr;
				}

				return symbol;
			}();

			if (argumentSymbol)
			{
				const auto pos = parser->GetSymbol(this->indexCanStealNpcAST)->single_intdata;
				parser->datastack.Clear();

				argumentSymbol->offset = reinterpret_cast<int>(npc);
				parser->datastack.Push(this->indexCanStealNpcAST + 1);
				parser->DoStack(pos);

				auto ret = parser->PopDataValue();

				if (ret)
				{
					return true;
				}
			}
		}

		return false;
	}
#endif

	void ItemMap::GetTraders()
	{
		this->traderInfos.clear();

		auto list = ogame->GetInfoManager()->infoList.next;
		while (list) {
			auto info = list->data;
			list = list->next;

			if (!info->pd.trade)
			{
				continue;
			}

			traderInfos.push_back(info);
		}
	}

	bool ItemMap::CanTrade(oCNpc* npc)
	{
		if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
		{
			return false;
		}

		for (auto info : traderInfos)
		{
			if (info->GetNpcID() != npc->GetInstance())
			{
				continue;
			}

			if (this->ShowTradersNoCond)
			{
				return true;
			}

			parser->SetInstance("SELF", npc);
			parser->SetInstance("OTHER", player);

			if (info->InfoConditions())
			{
				return true;
			}
		}

		return false;
	}
}