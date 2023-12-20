// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_oCDocumentManager_Show PATCH(&oCDocumentManager::Show, &oCDocumentManager::Show_Union);
	void __fastcall oCDocumentManager::Show_Union(int id)
	{
		THISCALL(Ivk_oCDocumentManager_Show)(id);

		oCViewDocumentMap* docMap = dynamic_cast<oCViewDocumentMap*>(this->GetDocumentView(id));
		if (!docMap)
		{
			return;
		}

		if (!docMap->ViewPageMap)
		{
			return;
		}

		zSTRING mapLevelName = docMap->Level;
		mapLevelName.Replace("/", "\\");
		zSTRING worldLevelName = ogame->GetGameWorld()->GetWorldFilename();
		worldLevelName.Replace("/", "\\");

		if (!worldLevelName.CompareI(mapLevelName))
		{
			return;
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
	}
}