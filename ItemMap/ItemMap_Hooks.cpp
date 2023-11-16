// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_oCDocumentManager_HandleEvent PATCH(&oCDocumentManager::HandleEvent, &oCDocumentManager::HandleEvent_Union);
	int oCDocumentManager::HandleEvent_Union(int key)
	{
		if (!itemMap->OnScreen)
		{
			return THISCALL(Ivk_oCDocumentManager_HandleEvent)(key);
		}

		//bool shift = (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT));
		bool shift = (GetKeyState(VK_SHIFT) & 0x8000);

		ItemMapMode mode = static_cast<ItemMapMode>(itemMap->mode);

		if (key == KEY_ESCAPE)
		{
			player->movlock = FALSE;
			itemMap->OnScreen = false;
			itemMap->ClearPrintItems();

			return THISCALL(Ivk_oCDocumentManager_HandleEvent)(key);
		}
		else if (key == KEY_PGUP && itemMap->mode != ItemMapMode::NPCS)
		{
			itemMap->listPage = 0;
			itemMap->mode = ItemMapMode::NPCS;
			itemMap->search = L"";
		}
		else if (key == KEY_PGDN && itemMap->mode != ItemMapMode::ITEMS)
		{
			itemMap->listPage = 0;
			itemMap->mode = ItemMapMode::ITEMS;
			itemMap->search = L"";
		}
		else if (key == KEY_UP && itemMap->listPage > 0 && itemMap->ShowList)
		{
			itemMap->listPage--;
		}
		else if (key == KEY_DOWN && itemMap->listPage < itemMap->listPageMax && itemMap->ShowList)
		{
			itemMap->listPage++;
		}
		else if (key == KEY_RIGHT && mode == ItemMapMode::ITEMS && itemMap->filter < ItemMapFilter::ALL)
		{
			int filter = static_cast<int>(itemMap->filter);
			itemMap->listPage = 0;
			itemMap->filter = static_cast<ItemMapFilter>(++filter);
		}
		else if (key == KEY_LEFT && mode == ItemMapMode::ITEMS && itemMap->filter > ItemMapFilter::PLANT)
		{
			int filter = static_cast<int>(itemMap->filter);
			itemMap->listPage = 0;
			itemMap->filter = static_cast<ItemMapFilter>(--filter);
		}
		else if (key == KEY_END)
		{
			itemMap->ShowMarkers = !itemMap->ShowMarkers;
		}
		else if (key == KEY_HOME)
		{
			itemMap->ShowList = !itemMap->ShowList;
		}
		else if (key == KEY_BACKSPACE && (itemMap->ShowMarkers || itemMap->ShowList))
		{
			if (shift)
			{
				itemMap->search = L"";
			}
			else if (itemMap->search.Length() > 0)
			{
				itemMap->search = itemMap->search.Copy(0, itemMap->search.Length() - 1);
			}
		}
		else if (key == KEY_SPACE && (itemMap->ShowMarkers || itemMap->ShowList))
		{
			itemMap->search = itemMap->search + L" ";
		}
		else if (itemMap->ShowMarkers || itemMap->ShowList)
		{
			BYTE keys[256];
			memset(keys, 0, sizeof(BYTE) * 256);

			auto keyboardLayout = GetKeyboardLayout(0);

			auto scan = MapVirtualKeyExW(key, MAPVK_VSC_TO_VK, keyboardLayout);

			if (GetKeyboardState(keys) != FALSE && scan != 0 && (GetKeyState(scan) & 0x8000))
			{
				keys[VK_CAPITAL] = (BYTE)GetKeyState(VK_CAPITAL);
				keys[VK_SHIFT] = (BYTE)GetKeyState(VK_SHIFT);

				wchar_t buff[] = { 0,0 };

				auto numChars = ToUnicodeEx(scan, scan, keys, buff, 2, 0, keyboardLayout);
				if (numChars > 0 && !iswspace(buff[0]))
				{
					itemMap->search = itemMap->search + buff;
				}
			}
		}

		zinput->ClearKeyBuffer();
		itemMap->RefreshLists();
		return THISCALL(Ivk_oCDocumentManager_HandleEvent)(0);
	}

	HOOK Ivk_zCViewDraw_Draw PATCH(&zCViewDraw::Draw, &zCViewDraw::Draw_Union);
	void __fastcall zCViewDraw::Draw_Union()
	{
		THISCALL(Ivk_zCViewDraw_Draw)();

		itemMap->Print();
	}

	HOOK Ivk_oCDocumentManager_Show PATCH(&oCDocumentManager::Show, &oCDocumentManager::Show_Union);
	void __fastcall oCDocumentManager::Show_Union(int id)
	{
		THISCALL(Ivk_oCDocumentManager_Show)(id);

		itemMap->OnScreen = false;

		oCViewDocumentMap* docMap = zDYNAMIC_CAST<oCViewDocumentMap>(this->GetDocumentView(id));
		if (!docMap)
		{
			return;
		}

		auto world = ogame->GetGameWorld();

		if (!world->GetWorldFilename().CompareI(docMap->Level))
		{
			return;
		}

		itemMap->OnScreen = true;
		player->movlock = TRUE;

		auto worldBox = world->bspTree.bspRoot->bbox3D;
		zVEC4 worldPos = { 0.0f, 0.0f, 0.0f, 0.0f };

		auto mapView = docMap->ViewPageMap;
		auto mapPos = mapView->PixelPosition;
		auto mapSize = mapView->PixelSize;
		zVEC2 mapCenter = zVEC2(((mapPos.X * 2) + mapSize.X) / 2, ((mapPos.Y * 2) + mapSize.Y) / 2);

#if ENGINE >= Engine_G2
		worldPos[0] = docMap->LevelCoords[0];
		worldPos[1] = docMap->LevelCoords[3];
		worldPos[2] = docMap->LevelCoords[2];
		worldPos[3] = docMap->LevelCoords[1];
#endif

		if (worldPos.Length() == 0.0f)
		{
			worldPos[0] = worldBox.mins[0];
			worldPos[1] = worldBox.mins[2];
			worldPos[2] = worldBox.maxs[0];
			worldPos[3] = worldBox.maxs[2];
		}

		zVEC2 worldDim = zVEC2(worldPos[2] - worldPos[0], worldPos[3] - worldPos[1]);
		zVEC2 world2map = zVEC2(mapSize.X / worldDim[0], mapSize.Y / worldDim[1]);

#if ENGINE <= Engine_G1A
		world2map[1] = world2map[1] * -1.0f;
#endif

		itemMap->listPage = 0;
		itemMap->listPageMax = 0;
		itemMap->search = L"";
		itemMap->margins = zVEC4(mapPos.X, mapPos.Y, mapPos.X + mapSize.X, mapPos.Y + mapSize.Y);

		itemMap->ClearPrintItems();

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

			int x, y;

#if ENGINE <= Engine_G1A
			x = static_cast<int>((world2map[0] * vobPos[VX]) + mapCenter[0]);
			y = static_cast<int>((world2map[1] * vobPos[VZ]) + mapCenter[1]);
#else
			x = static_cast<int>((world2map[0] * (vobPos[VX] - worldPos[0])) + mapPos.X);
			y = static_cast<int>(mapSize.Y - (world2map[1] * (vobPos[VZ] - worldPos[1])) + mapPos.Y);
#endif

			zPOS pos;
			pos.X = screen->anx(x) + (itemMap->markX / 2);
			pos.Y = screen->any(y) + (itemMap->markY / 2);

			if (pos.X < 0 || pos.X > 8192
				|| pos.Y < 0 || pos.Y > 8192
				)
			{
				continue;
			}

			ItemMapFilter filterFlag = itemMap->GetFilterFlag(vob);
			zCOLOR color = itemMap->GetColor(vob);

			auto vobType = vob->GetVobType();

			if (vobType == zVOB_TYPE_ITEM)
			{
				auto item = zDYNAMIC_CAST<oCItem>(vob);

				itemMap->AddPrintItem(new PrintItem(pos, color, item->name, item->GetInstanceName(), filterFlag));
				itemMap->AddPrintItemUnique(item->instanz, item->name, item->GetInstanceName(), item->amount, filterFlag);
			}
			else if (vobType == zVOB_TYPE_NSC)
			{
				auto npc = zDYNAMIC_CAST<oCNpc>(vob);

				itemMap->AddPrintNpc(new PrintItem(pos, color, npc->name, npc->GetInstanceName()));
				itemMap->AddPrintNpcUnique(npc);
			}
		}

		itemMap->SortUniques();

		itemMap->RefreshLists();
	}
}