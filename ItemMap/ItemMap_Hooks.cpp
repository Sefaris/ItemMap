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

		bool shift = (zKeyPressed(KEY_LSHIFT) || zKeyPressed(KEY_RSHIFT));
		bool uppercase = shift;

		if (key == KEY_ESCAPE)
		{
			itemMap->OnScreen = false;
			itemMap->ClearPrintItems();

			return THISCALL(Ivk_oCDocumentManager_HandleEvent)(key);
		}
		else if (key == KEY_BACKSPACE)
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
		else if (key == KEY_PGUP && itemMap->listPage > 0 && itemMap->ShowList)
		{
			itemMap->listPage -= 1;
		}
		else if (key == KEY_PGDN && itemMap->listPage < itemMap->listPageMax && itemMap->ShowList)
		{
			itemMap->listPage += 1;
		}
		else if (key == KEY_END)
		{
			itemMap->ShowMarkers = !itemMap->ShowMarkers;
		}
		else if (key == KEY_HOME)
		{
			itemMap->ShowList = !itemMap->ShowList;
		}
		else if (key == KEY_SPACE)
		{
			itemMap->search = itemMap->search + L" ";
		}
		else
		{
			BYTE keys[256];
			memset(keys, 0, sizeof(BYTE) * 256);

			auto keyboardLayout = GetKeyboardLayout(0);

			auto scan = MapVirtualKeyExW(key, MAPVK_VSC_TO_VK, keyboardLayout);

			if (GetKeyboardState(keys) != FALSE && scan != 0 && (GetKeyState(scan) & 0x8000))
			{
				keys[VK_CAPITAL] = (BYTE)GetKeyState(VK_CAPITAL);
				keys[VK_SHIFT] = (BYTE)GetKeyState(VK_SHIFT);

				wchar_t buff[] = {0,0};

				auto numChars = ToUnicodeEx(scan, scan, keys, buff, 2, 0, keyboardLayout);
				if (numChars > 0)
				{
					itemMap->search = itemMap->search + buff;
				}
			}
		}

		itemMap->RefreshLists();
		zinput->ClearKeyBuffer();
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

		oCViewDocumentMap* docMap = this->GetDocumentView(id)->CastTo<oCViewDocumentMap>();
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

		auto worldBox = world->bspTree.bspRoot->bbox3D;
		zVEC4 worldPos;

		auto mapView = docMap->ViewPageMap;
		auto mapPos = mapView->PixelPosition;
		auto mapSize = mapView->PixelSize;
		zVEC2 mapCenter = zVEC2((mapPos.X + mapSize.X) / 2, (mapPos.Y + mapSize.Y) / 2);

#if ENGINE >= Engine_G2
		worldPos[0] = docMap->LevelCoords[0];
		worldPos[1] = docMap->LevelCoords[3];
		worldPos[2] = docMap->LevelCoords[2];
		worldPos[3] = docMap->LevelCoords[1];
#else
		worldPos[0] = worldBox.mins[0];
		worldPos[1] = worldBox.mins[2];
		worldPos[2] = worldBox.maxs[0];
		worldPos[3] = worldBox.maxs[2];
#endif

		zVEC2 worldDim = zVEC2(worldPos[2] - worldPos[0], worldPos[3] - worldPos[1]);
		zVEC2 world2map = zVEC2(mapSize.X / worldDim[0], mapSize.Y / worldDim[1]);

#if ENGINE <= Engine_G1A
		world2map[1] = world2map[1] * -1.0f;
#endif

		itemMap->ClearPrintItems();

		for (int i = 0; i < world->voblist_items->GetNumInList(); i++)
		{
			auto item = world->voblist_items->Get(i);
			if (!item)
			{
				continue;
			}

			if (item->instanz < 0)
			{
				continue;
			}

			if (item->flags & ITM_FLAG_NFOCUS)
			{
				continue;
			}

			if (!item->homeWorld)
			{
				continue;
			}

			auto itemPos = item->GetPositionWorld();

			int x, y;

#if ENGINE <= Engine_G1A
			x = (world2map[0] * itemPos[VX]) + mapCenter[0];
			y = (world2map[1] * itemPos[VZ]) + mapCenter[1];
#else
			x = (world2map[0] * (itemPos[VX] - worldPos[0])) + mapPos.X;
			y = mapSize.Y - (world2map[1] * (itemPos[VZ] - worldPos[1])) + mapPos.Y;
#endif
			zPOS pos;
			pos.X = x;
			pos.Y = y;

			if (x < mapPos.X || x > mapSize.X || y < mapPos.Y || y > mapSize.Y)
			{
				continue;
			}


			itemMap->AddPrintItem(new PrintItem(pos, GFX_RED, item->mainflag, item->name));
			itemMap->AddPrintItemUnique(new PrintItemUnique(item->instanz, item->name, item->amount));
			itemMap->listPage = 0;
			itemMap->listPageMax = 0;
			itemMap->search = L"";
			itemMap->margins = zVEC4(mapPos.X, mapPos.Y, mapPos.X + mapSize.X, mapPos.Y + mapSize.Y);
		}

		itemMap->SortUniques();

		itemMap->RefreshLists();
	}

	/*	void PrintX(oCViewDocument* map, zPOS pos, zPOS size)
		{
			oCViewDocument* doc = new oCViewDocument();
			doc->Init(map, 1, 1, 1, 300.0f, 300.0f, Z"NINJA_ITEMMAP_MARKER.TGA");
			doc->SetTexture(Z"NINJA_ITEMMAP_MARKER.TGA", False);
			doc->SetTextureColor(GFX_RED);
			doc->SetPixelPosition(pos);
			doc->SetPixelSize(size);

			if ((doc->refCtr -= 1) <= 0)
				delete doc;
		}
	*/
}