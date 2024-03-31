// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	bool IsValidItemInstance(const zSTRING& itemInstance)
	{
		int indexItem = parser->GetIndex(itemInstance);
		if (indexItem == Invalid)
		{
			return false;
		}

		return true;
	}

	bool oCMobContainer::IsEmpty_Union()
	{
		auto contInv = this->containList.next;

		while (contInv)
		{
			auto item = contInv->GetData();
			contInv = contInv->next;

			if (item->instanz < 0)
			{
				continue;
			}

			return false;
		}

		return true;
	}

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

		if (item->name.IsEmpty() && item->description.IsEmpty())
		{
			return false;
		}

		if (itemMap->NewBalanceWispRule && itemMap->NewBalanceWispRuleBitflag != Invalid && (item->hitp & itemMap->NewBalanceWispRuleBitflag) != 0)
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

	bool zCVob::IsValidInteractiveOrContainer_Union()
	{
		if (this->GetVobType() != zVOB_TYPE_MOB)
		{
			return false;
		}

		if (auto container = zDYNAMIC_CAST<oCMobContainer>(this))
		{
			if (container->locked && container->pickLockStr.IsEmpty() && container->keyInstance.IsEmpty())
			{
				return false;
			}

			if (container->locked && container->pickLockStr.IsEmpty() && !container->keyInstance.IsEmpty() && !IsValidItemInstance(container->keyInstance))
			{
				return false;
			}

			if (container->IsEmpty_Union())
			{
				return false;
			}

			return true;
		}
		else if (auto inter = zDYNAMIC_CAST<oCMobInter>(this))
		{
			if (inter->onStateFuncName.IsEmpty())
			{
				return false;
			}

			return true;
		}

		return false;
	}

	zVEC3 zCVob::GetRealPosition_Union()
	{
		zVEC3 vobPos = { 0.0f, 0.0f, 0.0f };

		auto vobType = this->GetVobType();

		if (vobType == zVOB_TYPE_ITEM || vobType == zVOB_TYPE_MOB)
		{
			vobPos = this->GetPositionWorld();
		}
		else if (vobType == zVOB_TYPE_NSC)
		{
			auto npc = static_cast<oCNpc*>(this);

			if (npc->GetHomeWorld())
			{
				vobPos = npc->GetPositionWorld();
			}
			else
			{
				if (npc->state.rtnNow && !npc->state.rtnNow->wpname.IsEmpty() && 
					!npc->state.rtnNow->wpname.StartWith("TOT_") && 
					!npc->state.rtnNow->wpname.HasWordI("_HWN_"))
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
		if (!this->homeWorld && !this->HasMissionItem())
		{
			return false;
		}

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
			auto npcInv = this->inventory2.inventory[i].next;
			while (npcInv)
			{
				auto item = npcInv->GetData();
				npcInv = npcInv->next;

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
		auto npcInv = this->inventory2.inventory.next;
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

	int oCNpc::GetAivar_Union(const zSTRING& aivar)
	{
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

	bool oCNpc::HaveItem_Union(const zSTRING& itemInstance)
	{
		int indexItem = parser->GetIndex(itemInstance);
		if (indexItem == Invalid)
		{
			return false;
		}

		auto item = this->IsInInv(indexItem, 1);
		if (!item)
		{
			return false;
		}

		if (item->amount < 1)
		{
			return false;
		}

		return true;
	}

	bool HasOneOf(const zSTRING& str, const string& cmp)
	{
			auto cmplist = cmp.Split("|");
			for (auto& cmpit : cmplist)
			{
					cmpit.Shrink();
					if (str.HasWordI(cmpit))
					{
							return true;
					}
			}

			return false;
	}
}