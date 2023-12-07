// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_oCDocumentManager_Show PATCH(&oCDocumentManager::Show, &oCDocumentManager::Show_Union);
	void __fastcall oCDocumentManager::Show_Union(int id)
	{
		THISCALL(Ivk_oCDocumentManager_Show)(id);

		oCViewDocumentMap* docMap = zDYNAMIC_CAST<oCViewDocumentMap>(this->GetDocumentView(id));
		if (!docMap)
		{
			return;
		}

		if (!ogame->GetGameWorld()->GetWorldFilename().CompareI(docMap->Level))
		{
			return;
		}

		auto mapView = docMap->ViewPageMap;
		auto mapPos = mapView->PixelPosition;
		auto mapSize = mapView->PixelSize;

		int TopX = mapPos.X;
		int TopY = mapPos.Y;
		int BottomX = TopX + mapSize.X;
		int BottomY = TopY + mapSize.Y;

		itemMap->mapCoords = zVEC4(TopX, TopY, BottomX, BottomY);

		auto world = ogame->GetGameWorld();
		auto worldBox = world->bspTree.bspRoot->bbox3D;
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
	}
}