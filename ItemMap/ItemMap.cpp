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
		if (this->filter == ITEMMAP_FILTER_PLANT)
		{
			return "Plants";
		}
		else if (this->filter == ITEMMAP_FILTER_MELEE)
		{
			return "Melee";
		}
		else if (this->filter == ITEMMAP_FILTER_RANGED)
		{
			return "Ranged";
		}
		else if (this->filter == ITEMMAP_FILTER_ARMOR)
		{
			return "Armors";
		}
		else if (this->filter == ITEMMAP_FILTER_DOC)
		{
			return "Docs";
		}
		else if (this->filter == ITEMMAP_FILTER_SPELL)
		{
			return "Spells";
		}
		else if (this->filter == ITEMMAP_FILTER_MAGICITEM)
		{
			return "Magic";
		}
		else if (this->filter == ITEMMAP_FILTER_POTION)
		{
			return "Potions";
		}
		else if (this->filter == ITEMMAP_FILTER_FOOD)
		{
			return "Food";
		}
		else if (this->filter == ITEMMAP_FILTER_NONE)
		{
			return "None";
		}
		else if (this->filter == ITEMMAP_FILTER_ALL)
		{
			return "All";
		}

		return "Unknown";
	}

	void ItemMap::ClearPrintItems()
	{
		for (auto printItem : printItemsAll)
		{
			delete printItem;
		}

		for (auto printItemUnique : printItemsUniqueAll)
		{
			delete printItemUnique;
		}

		for (auto printNpc : printNpcsAll)
		{
			delete printNpc;
		}

		for (auto printNpcUnique : printNpcsUniqueAll)
		{
			delete printNpcUnique;
		}

		printItemsAll.clear();
		printItemsUniqueAll.clear();
		printItems.clear();
		printItemsUnique.clear();

		printNpcsAll.clear();
		printNpcsUniqueAll.clear();
		printNpcs.clear();
		printNpcsUnique.clear();
	}

	void ItemMap::SortUniques()
	{
		std::sort(this->printItemsUniqueAll.begin(), this->printItemsUniqueAll.end(), SortPrintItemUnique());
		std::sort(this->printNpcsUniqueAll.begin(), this->printNpcsUniqueAll.end(), SortPrintNpcUnique());
	}

	void ItemMap::PrintNpcsMarkers()
	{
		if (!this->ShowMarkers)
		{
			return;
		}

		this->printViewMarker->ClrPrintwin();

		screen->InsertItem(this->printViewMarker);
		for (auto printNpc : printNpcs)
		{
			this->printViewMarker->SetPos(screen->anx(printNpc->pos.X) + this->markX, screen->any(printNpc->pos.Y) + this->markY);
			this->printViewMarker->SetColor(printNpc->color);
			this->printViewMarker->Blit();
		}
		screen->RemoveItem(this->printViewMarker);
	}

	void ItemMap::PrintNpcsList()
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
		this->listPageMax = this->printNpcsUnique.size() / maxItems;

		if (this->listPage > this->listPageMax)
		{
			itemMap->listPage = this->listPageMax;
		}

		int x = 200, y = 0, temp = 0;
		size_t min = this->listPage * maxItems;
		size_t max = (min + maxItems) > printNpcsUnique.size() ? printNpcsUnique.size() : (min + maxItems);
		for (size_t i = min; i < max; i++)
		{
			auto printNpcUnique = printNpcsUnique[i];

			y = temp * fontHeight;

			zSTRING txt = printNpcUnique->name;

			if (printNpcUnique->count > 1)
			{
				txt = txt + zSTRING(" x") + zSTRING(printNpcUnique->count);
			}

			this->printViewList->Print(x, y, txt);
			temp++;
		}
		this->printViewList->Blit();
		screen->RemoveItem(this->printViewList);
	}

	void ItemMap::PrintItemsMarkers()
	{
		if (!this->ShowMarkers)
		{
			return;
		}

		this->printViewMarker->ClrPrintwin();

		screen->InsertItem(this->printViewMarker);
		for (auto printItem : printItems)
		{
			this->printViewMarker->SetPos(screen->anx(printItem->pos.X) + this->markX, screen->any(printItem->pos.Y) + this->markY);
			this->printViewMarker->SetColor(printItem->color);
			this->printViewMarker->Blit();
		}
		screen->RemoveItem(this->printViewMarker);
	}

	void ItemMap::PrintItemsList()
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
		this->listPageMax = this->printItemsUnique.size() / maxItems;

		if (this->listPage > this->listPageMax)
		{
			itemMap->listPage = this->listPageMax;
		}

		int x = 200, y = 0, temp = 0;
		size_t min = this->listPage * maxItems;
		size_t max = (min + maxItems) > printItemsUnique.size() ? printItemsUnique.size() : (min + maxItems);
		for (size_t i = min; i < max; i++)
		{
			auto printItemUnique = printItemsUnique[i];

			y = temp * fontHeight;

			zSTRING txt = printItemUnique->name + zSTRING(" x") + zSTRING(printItemUnique->amount);

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

		if (this->mode == ITEMMAP_MODE_ITEMS)
		{
			zSTRING less = "<<";
			zSTRING more = ">>";
			if (this->filter > 0)
			{
				this->printViewSearchBar->Print(0, y, less);
			}
			if (this->filter < ITEMMAP_FILTER_ALL)
			{
				this->printViewSearchBar->Print(8192 - this->printViewSearchBar->FontSize(more), y, more);
			}

			zSTRING txtSearchFilter = "Items - " + this->GetFilterName();
			int txtSearchFilterWidth = this->printViewSearchBar->FontSize(txtSearchFilter);
			this->printViewSearchBar->Print(0 + (8192 / 2) - (txtSearchFilterWidth / 2), fontHeight * y++, txtSearchFilter);
		}
		else if (this->mode == ITEMMAP_MODE_NPCS)
		{
			zSTRING txtSearchFilter = "NPCs";
			int txtSearchFilterWidth = this->printViewSearchBar->FontSize(txtSearchFilter);
			this->printViewSearchBar->Print(0 + (8192 / 2) - (txtSearchFilterWidth / 2), fontHeight * y++, txtSearchFilter);
		}
		else
		{
			zSTRING txtSearchFilter = "Unknown";
			int txtSearchFilterWidth = this->printViewSearchBar->FontSize(txtSearchFilter);
			this->printViewSearchBar->Print(0 + (8192 / 2) - (txtSearchFilterWidth / 2), fontHeight * y++, txtSearchFilter);
		}

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

		if (this->mode == ITEMMAP_MODE_ITEMS)
		{
			this->PrintItemsMarkers();
			this->PrintItemsList();
		}
		else if (this->mode == ITEMMAP_MODE_NPCS)
		{
			this->PrintNpcsMarkers();
			this->PrintNpcsList();
		}

		this->PrintSearchBar();
	}

	void ItemMap::AddPrintItem(PrintItem* printItem)
	{
		this->printItemsAll.push_back(printItem);
	}

	void ItemMap::AddPrintItemUnique(int instanz, const zSTRING& name, int amount, int flag)
	{
		for (auto it : printItemsUniqueAll)
		{
			if (it->instanz == instanz) {
				it->amount = it->amount + amount;
				return;
			}
		}
		this->printItemsUniqueAll.push_back(new PrintItemUnique(instanz, name, amount, flag));
	}

	void ItemMap::AddPrintNpc(PrintNpc* printNpc)
	{
		this->printNpcsAll.push_back(printNpc);
	}

	void ItemMap::AddPrintNpcUnique(oCNpc* npc)
	{
		for (auto it : printNpcsUniqueAll)
		{
			if (it->instanz == npc->instanz) {
				it->count = it->count + 1;
				return;
			}
		}
		this->printNpcsUniqueAll.push_back(new PrintNpcUnique(npc->instanz, npc->name, 1));
	}

	void ItemMap::RefreshLists()
	{
		printItems.clear();
		printItemsUnique.clear();
		printNpcs.clear();
		printNpcsUnique.clear();

		for (auto printItem : printItemsAll)
		{
			if (!this->search.IsEmpty() && !printItem->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			if (itemMap->filter != ITEMMAP_FILTER_ALL && printItem->flag != itemMap->filter)
			{
				continue;
			}

			printItems.push_back(printItem);
		}

		for (auto printItemUnique : printItemsUniqueAll)
		{
			if (!this->search.IsEmpty() && !printItemUnique->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			if (itemMap->filter != ITEMMAP_FILTER_ALL && printItemUnique->flag != itemMap->filter)
			{
				continue;
			}

			printItemsUnique.push_back(printItemUnique);
		}

		for (auto printNpc : printNpcsAll)
		{
			if (!this->search.IsEmpty() && !printNpc->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			printNpcs.push_back(printNpc);
		}

		for (auto printNpcUnique : printNpcsUniqueAll)
		{
			if (!this->search.IsEmpty() && !printNpcUnique->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			printNpcsUnique.push_back(printNpcUnique);
		}
	}
}