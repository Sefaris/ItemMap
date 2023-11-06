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

		if (key == KEY_ESCAPE)
		{
			player->movlock = FALSE;
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
			itemMap->listPage--;
		}
		else if (key == KEY_PGDN && itemMap->listPage < itemMap->listPageMax && itemMap->ShowList)
		{
			itemMap->listPage++;
		}
		else if (key == KEY_UP && itemMap->mode < ITEMMAP_MODE_MAX - 1)
		{
			itemMap->listPage = 0;
			itemMap->mode++;
			itemMap->search = L"";
		}
		else if (key == KEY_DOWN && itemMap->mode > 0)
		{
			itemMap->listPage = 0;
			itemMap->mode--;
			itemMap->search = L"";
		}
		else if (key == KEY_RIGHT && itemMap->mode == ITEMMAP_MODE_ITEMS && itemMap->filter < ITEMMAP_FILTER_ALL)
		{
			itemMap->listPage = 0;
			itemMap->filter++;
		}
		else if (key == KEY_LEFT && itemMap->mode == ITEMMAP_MODE_ITEMS && itemMap->filter > 0)
		{
			itemMap->listPage = 0;
			itemMap->filter--;
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
		zVEC4 worldPos;

		auto mapView = docMap->ViewPageMap;
		auto mapPos = mapView->PixelPosition;
		auto mapSize = mapView->PixelSize;
		zVEC2 mapCenter = zVEC2(((mapPos.X * 2) + mapSize.X) / 2, ((mapPos.Y * 2) + mapSize.Y) / 2);

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

		itemMap->listPage = 0;
		itemMap->listPageMax = 0;
		itemMap->search = L"";
		itemMap->margins = zVEC4(mapPos.X, mapPos.Y, mapPos.X + mapSize.X, mapPos.Y + mapSize.Y);

		itemMap->ClearPrintItems();

		auto listItems = world->voblist_items->next;
		while (listItems)
		{
			auto item = listItems->data;
			listItems = listItems->next;

			if (item->instanz < 0)
			{
				continue;
			}

			if (item->flags & ITM_FLAG_NFOCUS)
			{
				continue;
			}

			if (!item->GetHomeWorld())
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

			if (x < itemMap->margins[0] || x > itemMap->margins[2] || y < itemMap->margins[1] || y > itemMap->margins[3])
			{
				continue;
			}
			
			int flag = item->mainflag;
			int flagFilter = ITEMMAP_FILTER_NONE;
			zCOLOR color = GFX_COLDGREY;

			if (flag == ITM_CAT_NF)
			{
				color = GFX_RED;
				flagFilter = ITEMMAP_FILTER_MELEE;
			}
			else if (flag == ITM_CAT_FF)
			{
				color = GFX_RED;
				flagFilter = ITEMMAP_FILTER_RANGED;
			}
			else if (flag == ITM_CAT_MUN)
			{
				color = GFX_RED;
				flagFilter = ITEMMAP_FILTER_RANGED;
			}
			else if (flag == ITM_CAT_ARMOR)
			{
				color = GFX_BROWN;
				flagFilter = ITEMMAP_FILTER_ARMOR;
			}
			else if (flag == ITM_CAT_FOOD)
			{
				if (item->GetInstanceName().StartWith("ITPL_"))
				{
					color = GFX_GREEN;
					flagFilter = ITEMMAP_FILTER_PLANT;
				}
				else
				{
					color = GFX_ORANGE;
					flagFilter = ITEMMAP_FILTER_FOOD;
				}
			}
			else if (flag == ITM_CAT_DOCS)
			{
				color = GFX_YELLOW;
				flagFilter = ITEMMAP_FILTER_DOC;
			}
			else if (flag == ITM_CAT_POTION)
			{
				color = GFX_PINK;
				flagFilter = ITEMMAP_FILTER_POTION;
			}
			else if (flag == ITM_CAT_RUNE)
			{
				color = GFX_LBLUE;
				flagFilter = ITEMMAP_FILTER_SPELL;
			}
			else if (flag == ITM_CAT_MAGIC)
			{
				color = GFX_PURPLE;
				flagFilter = ITEMMAP_FILTER_MAGICITEM;
			}

			itemMap->AddPrintItem(new PrintItem(pos, color, flagFilter, item->name));
			itemMap->AddPrintItemUnique(item->instanz, item->name, item->amount, flagFilter);
		}

		auto listNpcs = world->voblist_npcs->next;
		while (listNpcs)
		{
			auto npc = listNpcs->data;
			listNpcs = listNpcs->next;

			//if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
			//{
			//	continue;
			//}

			zVEC3 npcPos;

			if (npc->state.rtnNow)
			{
				if (npc->state.rtnNow->wpname.IsEmpty() || npc->state.rtnNow->wpname.CompareI("TOT"))
				{
					continue;
				}

				auto wp = world->wayNet->GetWaypoint(npc->state.rtnNow->wpname);
				npcPos = wp->GetPositionWorld();
			}
			else
			{
				npcPos = npc->state.aiStatePosition;
			}

			if (npcPos[VX] == 0.0f && npcPos[VY] == 0.0f && npcPos[VZ] == 0.0f)
			{
				continue;
			}

			int x, y;

#if ENGINE <= Engine_G1A
			x = (world2map[0] * npcPos[VX]) + mapCenter[0];
			y = (world2map[1] * npcPos[VZ]) + mapCenter[1];
#else
			x = (world2map[0] * (npcPos[VX] - worldPos[0])) + mapPos.X;
			y = mapSize.Y - (world2map[1] * (npcPos[VZ] - worldPos[1])) + mapPos.Y;
#endif
			zPOS pos;
			pos.X = x;
			pos.Y = y;

			if (x < itemMap->margins[0] || x > itemMap->margins[2] || y < itemMap->margins[1] || y > itemMap->margins[3])
			{
				continue;
			}

			zCOLOR color = GFX_COLDGREY;

			int attitude = npc->GetPermAttitude(player);
			if (npc->IsHostile(player) || attitude == NPC_ATT_HOSTILE)
			{
				if (npc->guild < NPC_GIL_HUMANS)
				{
					color = GFX_PURPLE;
				}
				else
				{
					color = GFX_RED;
				}
			}

			if (npc->IsAngry(player) || attitude == NPC_ATT_ANGRY)
			{
				color = GFX_ORANGE;
			}

			if (npc->IsFriendly(player) || npc->npcType == NPCTYPE_FRIEND || attitude == NPC_ATT_FRIENDLY)
			{
				color = GFX_GREEN;
			}

			if (ogame->GetGuilds()->GetAttitude(npc->guild, player->guild) == NPC_ATT_FRIENDLY)
			{
				color = zCOLOR(175, 255, 175);
			}

			itemMap->AddPrintNpc(new PrintNpc(pos, npc->name, color));
			itemMap->AddPrintNpcUnique(npc);
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