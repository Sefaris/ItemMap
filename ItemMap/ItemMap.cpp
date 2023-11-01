// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	ItemMap::ItemMap()
	{
		this->printViewMarker = new zCView(0, 0, screen->anx(this->imgSize), screen->any(this->imgSize));
		zSTRING textureMarker = "ITEMMAP_MARKER.TGA";
		this->printViewMarker->InsertBack(textureMarker);

		zSTRING textureBackground = "DLG_CONVERSATION.TGA";
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

		printItemsAll.clear();
		printItemsUniqueAll.clear();
		printItems.clear();
		printItemsUnique.clear();
	}

	void ItemMap::SortUniques()
	{
		std::sort(this->printItemsUniqueAll.begin(), this->printItemsUniqueAll.end(), SortPrintItemUnique());
	}

	void ItemMap::PrintMarkers()
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

	void ItemMap::PrintList()
	{
		if (!this->ShowList)
		{
			return;
		}

		this->printViewList->ClrPrintwin();
		screen->InsertItem(this->printViewList);
		this->printViewList->SetPos(0, 0);
		int width = this->listWidth < 2000 ? 2000 : this->listWidth;
		this->printViewList->SetSize(width, 8192);
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
		this->printViewSearchBar->SetSize(screen->anx((this->margins[2] - this->margins[0]) / 3), screen->FontY() * 2);
		this->printViewSearchBar->SetTransparency(127);

		int fontHeight = this->printViewSearchBar->FontY();

		this->printViewSearchBar->SetFontColor(GFX_WHITE);
		zSTRING txtSearchTitle = "Search";
		this->printViewSearchBar->Print(0, 0, txtSearchTitle);

		if (this->search.IsEmpty())
		{
			zSTRING txtSearchPlaceholder = "Type name or instance";
			this->printViewSearchBar->SetFontColor(GFX_GREY);
			this->printViewSearchBar->Print(0, 8192 - fontHeight, txtSearchPlaceholder);
		}
		else
		{
			this->printViewSearchBar->SetFontColor(GFX_PINK);
			this->printViewSearchBar->Print(0, 8192 - fontHeight, this->search.WToA());
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

		this->PrintMarkers();
		this->PrintList();
		this->PrintSearchBar();
	}

	void ItemMap::AddPrintItem(PrintItem* printItem)
	{
		this->printItemsAll.push_back(printItem);
	}

	void ItemMap::AddPrintItemUnique(oCItem* item)
	{
		for (auto it : printItemsUniqueAll)
		{
			if (it->instanz == item->instanz) {
				it->amount = it->amount + item->amount;
				return;
			}
		}
		this->printItemsUniqueAll.push_back(new PrintItemUnique(item->instanz, item->name, item->amount));
	}

	void ItemMap::RefreshLists()
	{
		printItems.clear();
		printItemsUnique.clear();

		for (auto printItem : printItemsAll)
		{
			if (!this->search.IsEmpty() && !printItem->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			printItems.push_back(printItem);
		}

		this->listWidth = 0;
		for (auto printItemUnique : printItemsUniqueAll)
		{
			if (!this->search.IsEmpty() && !printItemUnique->name.HasWordI(this->search.WToA()))
			{
				continue;
			}

			zSTRING txt = printItemUnique->name + zSTRING(" x") + zSTRING(printItemUnique->amount);
			int txtWidth = screen->FontSize(txt);
			if (txtWidth > this->listWidth)
			{
				this->listWidth = txtWidth;
			}

			printItemsUnique.push_back(printItemUnique);
		}
	}
}