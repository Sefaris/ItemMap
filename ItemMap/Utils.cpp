// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	bool zCVob::IsValidItem_Union()
	{
		if (this->GetVobType() != zVOB_TYPE_ITEM)
		{
			return false;
		}

		auto item = static_cast<oCItem*>(this);

		if (item->instanz < 0)
		{
			return false;
		}

		if (item->flags & ITM_FLAG_NFOCUS)
		{
			return false;
		}

		if (!item->GetHomeWorld())
		{
			return false;
		}

		return true;
	}

	bool zCVob::IsValidNpc_Union()
	{
		if (this->GetVobType() != zVOB_TYPE_NSC)
		{
			return false;
		}

		auto npc = static_cast<oCNpc*>(this);

		if (npc->instanz < 0)
		{
			return false;
		}

		if (npc == player)
		{
			return false;
		}

		if (npc->name->IsEmpty())
		{
			return false;
		}

		if (npc->attribute[NPC_ATR_HITPOINTS] <= 0 && !npc->CanBeLooted_Union())
		{
			return false;
		}

		return true;
	}

	zVEC3 zCVob::GetRealPosition_Union()
	{
		zVEC3 vobPos = { 0.0f, 0.0f, 0.0f };

		if (this->GetVobType() == zVOB_TYPE_ITEM)
		{
			vobPos = this->GetPositionWorld();
		}
		else if (this->GetVobType() == zVOB_TYPE_NSC)
		{
			auto npc = static_cast<oCNpc*>(this);

			if (npc->GetHomeWorld())
			{
				vobPos = npc->GetPositionWorld();
			}
			else
			{
				if (npc->state.rtnNow && !npc->state.rtnNow->wpname.IsEmpty() && !npc->state.rtnNow->wpname.CompareI("TOT"))
				{
					if (auto wp = ogame->GetGameWorld()->wayNet->GetWaypoint(npc->state.rtnNow->wpname))
					{
						vobPos = wp->GetPositionWorld();
					}
				}
				else if (!npc->state.rtnNow)
				{
					vobPos = npc->state.aiStatePosition;
				}
			}
		}

		return vobPos;
	}

	bool oCNpc::CanBeLooted_Union()
	{
#if ENGINE <= Engine_G1A
		for (int i = INV_NONE; i < INV_MAX; i++)
		{
			if (!this->inventory2.packString[i].IsEmpty())
			{
				return true;
			}
		}
#else
		if (!this->inventory2.packString.IsEmpty())
		{
			return true;
		}
#endif

#if ENGINE <= Engine_G1A
		for (int i = INV_NONE; i < INV_MAX; i++)
		{
			for (int x = 0; x < this->inventory2.GetNumItemsInCategory(i); x++)
			{
				auto item = this->GetItem(i, x);

				if (item->instanz < 0)
				{
					continue;
				}

				if (item->HasFlag(ITM_CAT_ARMOR))
				{
					continue;
				}

				return true;
			}
		}
#else
		auto npcInv = this->inventory2.GetContents()->next;
		while (npcInv)
		{
			auto item = npcInv->GetData();
			npcInv = npcInv->next;

			if (item->instanz < 0)
			{
				continue;
			}

			if (item->HasFlag(ITM_FLAG_ACTIVE))
			{
				continue;
			}

			return true;
		}
#endif

		return false;
	}

	int oCNpc::GetAivar(const zSTRING& aivar) {
		auto sym = parser->GetSymbol(aivar);
		if (!sym)
		{
			return 0;
		}

		auto aiv = sym->single_intdata;
		if (aiv < 0 || 99 < aiv)
		{
			return 0;
		}

		return this->aiscriptvars[aiv];
	}
}