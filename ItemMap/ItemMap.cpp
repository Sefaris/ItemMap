// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	ItemMap::ItemMap()
	{
		this->printViewMarker = new zCView(0, 0, screen->anx(this->imgSize), screen->any(this->imgSize));
		this->printViewMarker->InsertBack(Z"NINJA_ITEMMAP_MARKER.TGA");

		this->printViewList = new zCView(0, 0, 0, 0);
		this->printViewList->InsertBack(Z"DLG_CONVERSATION.TGA");

		this->printViewSearchBar = new zCView(0, 0, 0, 0);
		this->printViewSearchBar->InsertBack(Z"DLG_CONVERSATION.TGA");

		zCTexture* temp = temp->Load(Z"L.TGA", True);
		temp->GetPixelSize(this->markX, this->markY);
		temp->Release();
		if (temp)
		{
			delete temp;
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

		this->listPageMax = std::ceil(this->printItemsUnique.size() / 40.0f) - 1;

		this->printViewList->ClrPrintwin();
		screen->InsertItem(this->printViewList);
		this->printViewList->SetPos(0, 0);
		int width = this->listWidth < 2000 ? 2000 : this->listWidth;
		this->printViewList->SetSize(width, 8192);
		this->printViewList->SetTransparency(127);

		int x = 200, y = 0, temp = 0;
		int min = this->listPage * 40;
		int max = min + 40;
		int i = -1;
		for (auto printItemUnique : printItemsUnique)
		{
			i++;
			if (i < min || i > max)
			{
				continue;
			}

			y = temp * 200;

			const zSTRING& txt = printItemUnique->name + Z" x" + Z printItemUnique->amount;

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
		this->printViewSearchBar->SetSize(screen->anx((this->margins[2] - this->margins[0]) / 3), 200);
		this->printViewSearchBar->SetTransparency(127);

		if (this->search.IsEmpty())
		{
			this->printViewSearchBar->SetFontColor(GFX_ORANGE);
			this->printViewSearchBar->Print(0, 0, Z"Type something to search");
		}
		else
		{
			this->printViewSearchBar->SetFontColor(GFX_AQUA);
			this->printViewSearchBar->Print(0, 0, this->search.WToA());
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

	void ItemMap::AddPrintItemUnique(PrintItemUnique* printItemUnique)
	{
		for (auto it : printItemsUniqueAll)
		{
			if (it->instanz == printItemUnique->instanz) {
				it->amount = it->amount + printItemUnique->amount;
				delete printItemUnique;
				return;
			}
		}
		this->printItemsUniqueAll.push_back(printItemUnique);
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

			zSTRING& txt = printItemUnique->name + Z" x" + Z printItemUnique->amount;
			int x = screen->FontSize(txt);
			if (x > this->listWidth)
			{
				this->listWidth = x;
			}

			printItemsUnique.push_back(printItemUnique);
		}
	}
}