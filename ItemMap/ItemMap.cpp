// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	ItemMap::ItemMap()
	{
		this->printViewMarker = new zCView(0, 0, screen->anx(this->imgSize), screen->any(this->imgSize));
		zSTRING textureMarker = "ITEMMAP_MARKER.TGA";
		this->printViewMarker->InsertBack(textureMarker);

		zSTRING textureBackground = "Dlg_Noise.TGA";
		this->printViewList = new zCView(0, 0, 0, 0);
		this->printViewList->InsertBack(textureBackground);

		this->printViewSearchBar = new zCView(0, 0, 0, 0);
		this->printViewSearchBar->InsertBack(textureBackground);

		zSTRING texturePlayerMark = "L.TGA";
		zCTexture* temp = temp->Load(texturePlayerMark, True);
		if (temp)
		{
			temp->GetPixelSize(this->markX, this->markY);
			temp->Release();
		}

		this->search = L"";
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

		this->ClearPrintItems();
	}

	zSTRING ItemMap::GetFilterName()
	{
		switch (static_cast<ItemMapFilter>(this->filter))
		{
		case ItemMapFilter::ARMOR:
			return "Armor";
		case ItemMapFilter::PLANT:
			return "Plants";
		case ItemMapFilter::MELEE:
			return "Melee";
		case ItemMapFilter::RANGED:
			return "Ranged";
		case ItemMapFilter::DOC:
			return "Docs";
		case ItemMapFilter::SPELL:
			return "Spells";
		case ItemMapFilter::MAGICITEM:
			return "Magic";
		case ItemMapFilter::POTION:
			return "Potions";
		case ItemMapFilter::FOOD:
			return "Food";
		case ItemMapFilter::NONE:
			return "None";
		case ItemMapFilter::ALL:
			return "All";
		}

		return "Unknown";
	}

	zCOLOR ItemMap::GetColor(oCItem* item)
	{
		switch (item->mainflag)
		{
		case ITM_CAT_NF:
		case ITM_CAT_FF:
		case ITM_CAT_MUN:
			return GFX_RED;
		case ITM_CAT_ARMOR:
			return GFX_BROWN;
		case ITM_CAT_FOOD:
			if (item->GetInstanceName().StartWith("ITPL_") || item->GetInstanceName().HasWordI("PLANT"))
			{
				return GFX_GREEN;
			}
			return GFX_ORANGE;
		case ITM_CAT_DOCS:
			return GFX_YELLOW;
		case ITM_CAT_POTION:
			return GFX_PINK;
		case ITM_CAT_RUNE:
			return GFX_LBLUE;
		case ITM_CAT_MAGIC:
			return GFX_PURPLE;
		}

		return GFX_COLDGREY;
	}

	ItemMapFilter ItemMap::GetFilterFlag(oCItem* item)
	{
		switch (item->mainflag)
		{
		case ITM_CAT_NF:
			return ItemMapFilter::MELEE;
		case ITM_CAT_FF:
		case ITM_CAT_MUN:
			return ItemMapFilter::RANGED;
		case ITM_CAT_ARMOR:
			return ItemMapFilter::ARMOR;
		case ITM_CAT_FOOD:
			if (item->GetInstanceName().StartWith("ITPL_") || item->GetInstanceName().HasWordI("PLANT"))
			{
				return ItemMapFilter::PLANT;
			}
			return ItemMapFilter::FOOD;
		case ITM_CAT_DOCS:
			return ItemMapFilter::DOC;
		case ITM_CAT_POTION:
			return ItemMapFilter::POTION;
		case ITM_CAT_RUNE:
			return ItemMapFilter::SPELL;
		case ITM_CAT_MAGIC:
			return ItemMapFilter::MAGICITEM;
		}

		return ItemMapFilter::NONE;
	}

	void ItemMap::ClearPrintItems()
	{
		for (auto printItem : vecItemsAll)
		{
			delete printItem;
		}

		for (auto printItemUnique : vecItemsUniqueAll)
		{
			delete printItemUnique;
		}

		for (auto printNpc : vecNpcsAll)
		{
			delete printNpc;
		}

		for (auto printNpcUnique : vecNpcsUniqueAll)
		{
			delete printNpcUnique;
		}

		vecItemsAll.clear();
		vecItemsUniqueAll.clear();

		vecNpcsAll.clear();
		vecNpcsUniqueAll.clear();

		vecPrintItemsCurrent.clear();
		vecPrintItemsUniqueCurrent.clear();
	}

	void ItemMap::SortUniques()
	{
		std::sort(this->vecItemsUniqueAll.begin(), this->vecItemsUniqueAll.end(), [](const auto& left, const auto& right) {
			return strcmp(left->name.ToChar(), right->name.ToChar()) < 0;
		});

		std::sort(this->vecNpcsUniqueAll.begin(), this->vecNpcsUniqueAll.end(), [](const auto& left, const auto& right) {
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
		for (auto printItem : vecPrintItemsCurrent)
		{
			this->printViewMarker->SetPos(printItem->pos.X, printItem->pos.Y);
			this->printViewMarker->SetColor(printItem->color);
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
		this->printViewList->SetSize(this->listWidth, 8192);
		this->printViewList->SetTransparency(127);

		int fontHeight = this->printViewList->FontY();
		size_t maxItems = (8192 / fontHeight);
		this->listPageMax = this->vecPrintItemsUniqueCurrent.size() / maxItems;

		if (this->listPage > this->listPageMax)
		{
			itemMap->listPage = this->listPageMax;
		}

		int x = 200, y = 0, temp = 0;
		size_t min = this->listPage * maxItems;
		size_t max = (min + maxItems) > vecPrintItemsUniqueCurrent.size() ? vecPrintItemsUniqueCurrent.size() : (min + maxItems);
		for (size_t i = min; i < max; i++)
		{
			auto printItemUnique = vecPrintItemsUniqueCurrent[i];

			y = temp * fontHeight;

			zSTRING txt = printItemUnique->name + zSTRING(" x") + zSTRING(printItemUnique->num);

			this->printViewList->Print(x, y, txt);
			temp++;
		}
		this->printViewList->Blit();
		screen->RemoveItem(this->printViewList);
	}

	void ItemMap::PrintSearchBar()
	{
		if (!this->ShowMarkers && !this->ShowList)
		{
			return;
		}

		this->printViewSearchBar->ClrPrintwin();

		screen->InsertItem(this->printViewSearchBar);
		this->printViewSearchBar->SetPos(screen->anx(this->margins[0] + (((this->margins[2] - this->margins[0]) / 3) * 2)) - 200, screen->any(this->margins[1]) + 300);
		this->printViewSearchBar->SetSize(screen->anx((this->margins[2] - this->margins[0]) / 3), screen->FontY() * 3);
		this->printViewSearchBar->SetTransparency(127);

		int fontHeight = this->printViewSearchBar->FontY();
		int y = 0;

		this->printViewSearchBar->SetFontColor(GFX_COLDGREY);

		ItemMapMode mode = static_cast<ItemMapMode>(this->mode);
		zSTRING txtSearchFilter = "Unknown";

		switch (mode)
		{
		case ItemMapMode::ITEMS:
			if (this->filter > 0)
			{
				zSTRING less = "<<";
				this->printViewSearchBar->Print(0, y, less);
			}
			if (static_cast<ItemMapFilter>(this->filter) < ItemMapFilter::ALL)
			{
				zSTRING more = ">>";
				this->printViewSearchBar->Print(8192 - this->printViewSearchBar->FontSize(more), y, more);
			}

			txtSearchFilter = "Items - " + this->GetFilterName();

			break;
		case ItemMapMode::NPCS:
			txtSearchFilter = "NPCs";
			break;
		}

		int txtSearchFilterWidth = this->printViewSearchBar->FontSize(txtSearchFilter);
		this->printViewSearchBar->Print(0 + (8192 / 2) - (txtSearchFilterWidth / 2), fontHeight * y++, txtSearchFilter);

		this->printViewSearchBar->SetFontColor(GFX_WHITE);
		zSTRING txtSearchTitle = "Search";
		this->printViewSearchBar->Print(0, fontHeight * y++, txtSearchTitle);

		if (this->search.IsEmpty())
		{
			zSTRING txtSearchPlaceholder = "Type name or instance";
			this->printViewSearchBar->SetFontColor(GFX_PINK);
			this->printViewSearchBar->Print(0, fontHeight * y++, txtSearchPlaceholder);
		}
		else
		{
			this->printViewSearchBar->SetFontColor(GFX_GREEN);
			this->printViewSearchBar->Print(0, fontHeight * y++, this->search.WToA());
		}

		this->printViewSearchBar->Blit();
		screen->RemoveItem(this->printViewSearchBar);
	}

	void ItemMap::Print()
	{
		if (!this->OnScreen)
		{
			return;
		}

		this->PrintList();
		this->PrintMarkers();
		this->PrintSearchBar();

		int ScreenX, ScreenY, ScreenSX, ScreenSY;
		screen->GetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
		zrenderer->SetViewport(ScreenX, ScreenY, ScreenSX, ScreenSY);
	}

	void ItemMap::AddPrintItem(PrintItem* printItem)
	{
		this->vecItemsAll.push_back(printItem);
	}

	void ItemMap::AddPrintItemUnique(int instanz, const zSTRING& name, int amount, ItemMapFilter flag)
	{
		for (auto it : vecItemsUniqueAll)
		{
			if (it->instanz == instanz) {
				it->num = it->num + amount;
				return;
			}
		}
		this->vecItemsUniqueAll.push_back(new PrintItemUnique(instanz, name, amount, flag));
	}

	void ItemMap::AddPrintNpc(PrintItem* printNpc)
	{
		this->vecNpcsAll.push_back(printNpc);
	}

	void ItemMap::AddPrintNpcUnique(oCNpc* npc)
	{
		for (auto it : vecNpcsUniqueAll)
		{
			if (it->instanz == npc->instanz) {
				it->num = it->num + 1;
				return;
			}
		}
		this->vecNpcsUniqueAll.push_back(new PrintItemUnique(npc->instanz, npc->name, 1));
	}

	void ItemMap::RefreshLists()
	{
		vecPrintItemsCurrent.clear();
		vecPrintItemsUniqueCurrent.clear();

		ItemMapFilter filter = static_cast<ItemMapFilter>(itemMap->filter);
		ItemMapMode mode = static_cast<ItemMapMode>(itemMap->mode);

		std::vector<PrintItem*>* vecAll = nullptr;
		std::vector<PrintItemUnique*>* vecUniqueAll = nullptr;

		if (mode == ItemMapMode::ITEMS)
		{
			vecAll = &vecItemsAll;
			vecUniqueAll = &vecItemsUniqueAll;
		}
		else if (mode == ItemMapMode::NPCS)
		{
			vecAll = &vecNpcsAll;
			vecUniqueAll = &vecNpcsUniqueAll;
		}

		if (!vecAll && !vecUniqueAll)
		{
			return;
		}

		for (auto printItem : *vecAll)
		{
			if (!this->search.IsEmpty() && !printItem->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			if (mode == ItemMapMode::ITEMS && filter != ItemMapFilter::ALL && printItem->flag != static_cast<ItemMapFilter>(itemMap->filter))
			{
				continue;
			}

			vecPrintItemsCurrent.push_back(printItem);
		}

		for (auto printItemUnique : *vecUniqueAll)
		{
			if (!this->search.IsEmpty() && !printItemUnique->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			if (mode == ItemMapMode::ITEMS && filter != ItemMapFilter::ALL && printItemUnique->flag != static_cast<ItemMapFilter>(itemMap->filter))
			{
				continue;
			}

			vecPrintItemsUniqueCurrent.push_back(printItemUnique);
		}
	}
}