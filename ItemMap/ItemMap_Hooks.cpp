// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_oCDocumentManager_Show PATCH(&oCDocumentManager::Show, &oCDocumentManager::Show_Union);
	void __fastcall oCDocumentManager::Show_Union(int id)
	{
		THISCALL(Ivk_oCDocumentManager_Show)(id);
		
		if (auto docView = this->GetDocumentView(id))
		{
			itemMap->TryInitMap(docView);
		}
	}
}