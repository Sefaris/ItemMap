// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	ItemMap::ItemMap()
	{
		this->printViewMarker = new zCView(0, 0, 0, 0);
		zSTRING textureMarker = "ITEMMAP_MARKER.TGA";
		this->printViewMarker->InsertBack(textureMarker);

		zSTRING textureBackground = "ITEMMAP_BACKGROUND.TGA";
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

		this->OnScreen = false;
		this->Hook = HookType::NoHook;
		this->ShowMarkers = true;
		this->ShowList = false;
		this->ShowSearchBar = true;
		this->SearchBarActive = false;

		this->search = L"";

		this->indexSpriteMapHandle = parser->GetIndex("SPRITEMAP_SPRITEHNDL");
		this->indexSpriteCursorHandle = parser->GetIndex("SPRITEMAP_SPRITECURSORHNDL");

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

		this->ClearPrintItems();
	}

	void ItemMap::ResizeMarkers(int size)
	{
		this->imgSize = std::clamp(size, 5, 50);
		this->printViewMarker->SetSize(screen->anx(this->imgSize), screen->any(this->imgSize));
	}

	void ItemMap::ResizeList(int size)
	{
		this->listWidth = std::clamp((this->listWidth + size), 1, 100);
	}

	void ItemMap::UpdateSettings()
	{
		this->ResizeMarkers(zoptions->ReadInt("ItemMap", "IconSize", 10));
		this->listWidth = std::clamp(zoptions->ReadInt("ItemMap", "ListWidth", 25), 1, 100);

		this->mode = static_cast<ItemMapMode>(zoptions->ReadInt("ItemMap", "PrevMode", 0));
		this->filter = static_cast<ItemMapFilter>(zoptions->ReadInt("ItemMap", "PrevFilter", 0));

		for (size_t i = 0; i < ColorsItemsMax; i++)
		{
			this->colorsItems[i] = this->HexToColor(zoptions->ReadString("ItemMap", zSTRING{ "ColorItem" } + zSTRING{ FilterNames[i] }, DefaultColorsItems[i]));
		}

		for (size_t i = 0; i < ColorsNpcsMax; i++)
		{
			this->colorsNpcs[i] = this->HexToColor(zoptions->ReadString("ItemMap", zSTRING{ "ColorNpc" } + zSTRING{ FilterNpcsNames[i] }, DefaultColorsNpcs[i]));
		}
	}

	zCOLOR ItemMap::HexToColor(const zSTRING& hexstring)
	{
		int r, g, b;
		if (sscanf(hexstring.ToChar(), "#%02x%02x%02x", &r, &g, &b) == 3)
		{
			return zCOLOR(r, g, b);
		}

		return zCOLOR(255, 255, 255);
	}

	zSTRING ItemMap::GetFilterName()
	{
		auto filter = static_cast<int>(this->filter);

		if (filter < 0 || filter > ColorsItemsMax + 1)
		{
			return "Unknown";
		}

		return FilterNames[filter];
	}

	zCOLOR ItemMap::GetColor(zCVob* vob)
	{
		if (vob->GetVobType() == zVOB_TYPE_ITEM)
		{
			auto item = static_cast<oCItem*>(vob);

			switch (item->mainflag)
			{
			case ITM_CAT_NF:
				return colorsItems[static_cast<int>(ItemMapFilter::MELEE)];
			case ITM_CAT_FF:
			case ITM_CAT_MUN:
				return colorsItems[static_cast<int>(ItemMapFilter::RANGED)];
			case ITM_CAT_ARMOR:
				return colorsItems[static_cast<int>(ItemMapFilter::ARMOR)];
			case ITM_CAT_FOOD:
				if (item->GetInstanceName().StartWith("ITPL_") || item->GetInstanceName().HasWordI("PLANT"))
				{
					return colorsItems[static_cast<int>(ItemMapFilter::PLANT)];
				}
				return colorsItems[static_cast<int>(ItemMapFilter::FOOD)];
			case ITM_CAT_DOCS:
				return colorsItems[static_cast<int>(ItemMapFilter::DOC)];
			case ITM_CAT_POTION:
				return colorsItems[static_cast<int>(ItemMapFilter::POTION)];
			case ITM_CAT_RUNE:
				return colorsItems[static_cast<int>(ItemMapFilter::SPELL)];
			case ITM_CAT_MAGIC:
				return colorsItems[static_cast<int>(ItemMapFilter::MAGICITEM)];
			case ITM_CAT_NONE:
				return colorsItems[static_cast<int>(ItemMapFilter::NONE)];
			}
		}
		else if (vob->GetVobType() == zVOB_TYPE_NSC)
		{
			auto npc = static_cast<oCNpc*>(vob);

			if (npc->attribute[NPC_ATR_HITPOINTS] <= 0 && npc->CanBeLooted_Union())
			{
				return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::DEAD)];
			}

			int attitude = npc->GetPermAttitude(player);
			if (npc->IsHostile(player) || attitude == NPC_ATT_HOSTILE)
			{
				if (npc->guild < NPC_GIL_HUMANS)
				{
					return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::HOSTILEHUMAN)];
				}
				else
				{
					return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::HOSTILEMONSTER)];
				}
			}

			if (npc->IsAngry(player) || attitude == NPC_ATT_ANGRY)
			{
				return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::ANGRY)];
			}

			if (npc->GetAivar("AIV_PARTYMEMBER"))
			{
				return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::PARTY)];
			}

			if (npc->IsFriendly(player) || npc->npcType == this->NPC_TYPE_FRIEND || attitude == NPC_ATT_FRIENDLY)
			{
				return colorsNpcs[static_cast<int>(ItemMapFilterNpcs::FRIENDLY)];
			}
		}

		return zCOLOR(128, 128, 128);
	}

	ItemMapFilter ItemMap::GetFilterFlag(zCVob* vob)
	{
		if (vob->GetVobType() == zVOB_TYPE_ITEM)
		{
			auto item = static_cast<oCItem*>(vob);

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
			this->printViewMarker->SetPos(printItem->pos.X - (screen->anx(this->imgSize) / 2), printItem->pos.Y - (screen->any(this->imgSize) / 2));
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
		this->printViewList->SetSize((8182 * this->listWidth) / 100, 8192);
		this->printViewList->SetTransparency(255);

		int fontHeight = this->printViewList->FontY();
		size_t maxItems = (8192 / fontHeight);
		this->listPageMax = this->vecPrintItemsUniqueCurrent.size() / maxItems;

		if (this->listPage > this->listPageMax)
		{
			this->listPage = this->listPageMax;
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
		if (!this->ShowSearchBar)
		{
			return;
		}

		this->printViewSearchBar->ClrPrintwin();

		screen->InsertItem(this->printViewSearchBar);
		this->printViewSearchBar->SetPos(screen->anx(this->mapCoords[0] + (((this->mapCoords[2] - this->mapCoords[0]) / 3) * 2)) - 200, screen->any(this->mapCoords[1]) + 300);
		this->printViewSearchBar->SetSize(screen->anx(this->mapCoords[2] - this->mapCoords[0]) / 3, screen->FontY() * 3);
		this->printViewSearchBar->SetTransparency(255);

		int fontHeight = this->printViewSearchBar->FontY();
		int y = 0;

		this->printViewSearchBar->SetFontColor(GFX_COLDGREY);

		ItemMapMode mode = static_cast<ItemMapMode>(this->mode);
		zSTRING txtSearchFilter = "Unknown";

		switch (mode)
		{
		case ItemMapMode::ITEMS:
			if (this->filter > ItemMapFilter::PLANT)
			{
				zSTRING less = "<<";
				this->printViewSearchBar->Print(0, y, less);
			}
			if (this->filter < ItemMapFilter::ALL)
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

	void ItemMap::AddPrintItemUnique(int instanz, const zSTRING& name, const zSTRING& instancename, int amount, ItemMapFilter flag)
	{
		for (auto it : vecItemsUniqueAll)
		{
			if (it->instanz == instanz) {
				it->num = it->num + amount;
				return;
			}
		}
		this->vecItemsUniqueAll.push_back(new PrintItemUnique(instanz, name, instancename, amount, flag));
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
		this->vecNpcsUniqueAll.push_back(new PrintItemUnique(npc->instanz, npc->name, npc->GetInstanceName(), 1));
	}

	std::vector<PrintItem*>& ItemMap::GetCurrentVectorAll()
	{
		switch (this->mode)
		{
		case ItemMapMode::ITEMS:
			return vecItemsAll;
		case ItemMapMode::NPCS:
			return vecNpcsAll;
		}

		assert(false);
	}

	std::vector<PrintItemUnique*>& ItemMap::GetCurrentVectorUniques()
	{
		switch (this->mode)
		{
		case ItemMapMode::ITEMS:
			return vecItemsUniqueAll;
		case ItemMapMode::NPCS:
			return vecNpcsUniqueAll;
		}

		assert(false);
	}

	void ItemMap::RefreshLists()
	{
		vecPrintItemsCurrent.clear();
		vecPrintItemsUniqueCurrent.clear();

		if (this->mode != ItemMapMode::ITEMS && this->mode != ItemMapMode::NPCS)
		{
			return;
		}

		auto search = this->search.WToA();

		for (auto printItem : this->GetCurrentVectorAll())
		{
			if (!search.IsEmpty() && !printItem->name.HasWordI(search) && !printItem->instancename.HasWordI(search))
			{
				continue;
			}

			if (mode == ItemMapMode::ITEMS && this->filter != ItemMapFilter::ALL && printItem->flag != this->filter)
			{
				continue;
			}

			vecPrintItemsCurrent.push_back(printItem);
		}

		for (auto printItemUnique : this->GetCurrentVectorUniques())
		{
			if (!search.IsEmpty() && !printItemUnique->name.HasWordI(search) && !printItemUnique->instancename.HasWordI(search))
			{
				continue;
			}

			if (mode == ItemMapMode::ITEMS && this->filter != ItemMapFilter::ALL && printItemUnique->flag != this->filter)
			{
				continue;
			}

			vecPrintItemsUniqueCurrent.push_back(printItemUnique);
		}
	}

	void ItemMap::Close()
	{
		//player->SetMovLock(FALSE);
		this->OnScreen = false;
		this->ClearPrintItems();
		this->Hook = HookType::NoHook;

		this->worldCoords = { 0, 0, 0, 0 };
		this->mapCoords = { 0, 0, 0, 0 };

		this->SearchBarActive = false;

		zoptions->WriteInt("ItemMap", "PrevMode", static_cast<int>(this->mode), 0);
		zoptions->WriteInt("ItemMap", "PrevFilter", static_cast<int>(this->filter), 0);
		zoptions->WriteInt("ItemMap", "IconSize", this->imgSize, 0);
		zoptions->WriteInt("ItemMap", "ListWidth", this->listWidth, 0);
	}

	void ItemMap::HandleInput()
	{
		if (!this->OnScreen)
		{
			return;
		}

		bool shift = (GetKeyState(VK_SHIFT) & 0x8000);
		bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000);

		if (zKeyToggled(KEY_ESCAPE))
		{
			this->Close();
			return;
		}

		if ((zKeyToggled(KEY_RETURN) || zKeyToggled(KEY_NUMPADENTER)) && this->ShowSearchBar)
		{
			this->SearchBarActive = !this->SearchBarActive;
		}

		if (zKeyToggled(KEY_PGUP) && this->mode < ItemMapMode::NPCS)
		{
			int mode = static_cast<int>(this->mode);
			this->listPage = 0;
			this->search = L"";
			this->mode = static_cast<ItemMapMode>(++mode);
		}

		if (zKeyToggled(KEY_PGDN) && this->mode > ItemMapMode::ITEMS)
		{
			int mode = static_cast<int>(this->mode);
			this->listPage = 0;
			this->search = L"";
			this->mode = static_cast<ItemMapMode>(--mode);
		}

		if (zKeyToggled(KEY_UP))
		{
			if (ctrl && this->ShowMarkers)
			{
				this->ResizeMarkers(++this->imgSize);
			}

			if (!ctrl && this->listPage > 0 && this->ShowList)
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

			if (!ctrl && this->listPage < this->listPageMax && this->ShowList)
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

			if (!ctrl && this->mode == ItemMapMode::ITEMS && this->filter < ItemMapFilter::ALL)
			{
				int filter = static_cast<int>(this->filter);
				this->listPage = 0;
				this->filter = static_cast<ItemMapFilter>(++filter);
			}
		}

		if (zKeyToggled(KEY_LEFT))
		{
			if (ctrl && this->ShowList)
			{
				this->ResizeList(-1);
			}

			if (!ctrl && this->mode == ItemMapMode::ITEMS && this->filter > ItemMapFilter::PLANT)
			{
				int filter = static_cast<int>(this->filter);
				this->listPage = 0;
				this->filter = static_cast<ItemMapFilter>(--filter);
			}
		}

		if (zKeyToggled(KEY_END))
		{
			this->ShowMarkers = !this->ShowMarkers;
		}

		if (zKeyToggled(KEY_HOME))
		{
			this->ShowList = !this->ShowList;
		}

		if (zKeyToggled(KEY_DELETE))
		{
			this->ShowSearchBar = !this->ShowSearchBar;

			if (!this->ShowSearchBar && this->SearchBarActive)
			{
				this->SearchBarActive = false;
			}
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

		if (this->SearchBarActive && this->ShowSearchBar)
		{
			BYTE keys[256];
			memset(keys, 0, sizeof(BYTE) * 256);

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

			int SpriteMapTopX = SpriteMapPosX - SpriteMapSize;
			int SpriteMapTopY = SpriteMapPosY - SpriteMapSize;
			int SpriteMapBottomX = SpriteMapPosX + SpriteMapSize;
			int SpriteMapBottomY = SpriteMapPosY + SpriteMapSize;

			this->mapCoords = zVEC4(SpriteMapTopX, SpriteMapTopY, SpriteMapBottomX, SpriteMapBottomY);

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

	void ItemMap::InitMap(HookType hook, int rotate)
	{
		this->OnScreen = true;
		this->Hook = hook;
		this->listPage = 0;
		this->listPageMax = 0;
		this->search = L"";
		//player->SetMovLock(TRUE);

		if (this->mode > ItemMapMode::NPCS || this->mode < ItemMapMode::ITEMS)
		{
			this->mode = ItemMapMode::ITEMS;
		}

		if (this->filter < ItemMapFilter::PLANT || this->filter > ItemMapFilter::ALL)
		{
			this->filter = ItemMapFilter::PLANT;
		}

		auto world = ogame->GetGameWorld();

		zVEC2 mapCenter = zVEC2((this->mapCoords[0] + this->mapCoords[2]) / 2, (this->mapCoords[1] + this->mapCoords[3]) / 2);
		zVEC2 worldDim = zVEC2(this->worldCoords[2] - this->worldCoords[0], this->worldCoords[3] - this->worldCoords[1]);

		zVEC2 world2map = zVEC2((this->mapCoords[2] - this->mapCoords[0]) / worldDim[0], (this->mapCoords[3] - this->mapCoords[1]) / worldDim[1]);

#if ENGINE <= Engine_G1A
		world2map[1] = world2map[1] * -1.0f;
#endif

		float s = sin(RAD90);
		float c = cos(RAD90);
		this->ClearPrintItems();
		auto listVobs = world->voblist->next;
		while (listVobs)
		{
			auto vob = listVobs->data;
			listVobs = listVobs->next;

			if (!vob->IsValidItem_Union() && !vob->IsValidNpc_Union())
			{
				continue;
			}

			auto vobPos = vob->GetRealPosition_Union();

			if (vobPos.Length() == 0.0f)
			{
				continue;
			}

			int x;
			int y;

#if ENGINE <= Engine_G1A
			x = static_cast<int>((world2map[0] * vobPos[VX]) + mapCenter[0]);
			y = static_cast<int>((world2map[1] * vobPos[VZ]) + mapCenter[1]);
#else
			x = static_cast<int>(this->mapCoords[0] + (world2map[0] * (vobPos[VX] - this->worldCoords[0])));
			y = static_cast<int>(this->mapCoords[3] - (world2map[1] * (vobPos[VZ] - this->worldCoords[1])));
#endif

			zPOS pos;

			if (this->Hook == HookType::CoM && rotate)
			{
				int newx = c * (x - mapCenter[0]) - s * (y - mapCenter[1]) + mapCenter[0];
				int newy = s * (x - mapCenter[0]) + c * (y - mapCenter[1]) + mapCenter[1];

				x = newx;
				y = newy;

				pos.X = screen->anx(x);
				pos.Y = screen->any(y);
			}
			else
			{
				pos.X = screen->anx(x) + screen->anx((this->markX / 2));
				pos.Y = screen->any(y) + screen->any((this->markY / 2));
			}


			if (pos.X < 0 || pos.X > 8192
				|| pos.Y < 0 || pos.Y > 8192
				)
			{
				continue;
			}

			ItemMapFilter filterFlag = this->GetFilterFlag(vob);
			zCOLOR color = this->GetColor(vob);

			auto vobType = vob->GetVobType();

			if (vobType == zVOB_TYPE_ITEM)
			{
				auto item = static_cast<oCItem*>(vob);

				this->AddPrintItem(new PrintItem(pos, color, item->name, item->GetInstanceName(), filterFlag));
				this->AddPrintItemUnique(item->instanz, item->name, item->GetInstanceName(), item->amount, filterFlag);
			}
			else if (vobType == zVOB_TYPE_NSC)
			{
				auto npc = static_cast<oCNpc*>(vob);

				this->AddPrintNpc(new PrintItem(pos, color, npc->name, npc->GetInstanceName()));
				this->AddPrintNpcUnique(npc);
			}
		}

		this->SortUniques();
		this->RefreshLists();
	}
}