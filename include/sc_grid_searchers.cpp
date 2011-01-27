/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//return closest GO in grid, with range from pSource
GameObject* GetClosestGameObjectWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    GameObject* pGo = NULL;

    MaNGOS::NearestGameObjectEntryInObjectRangeCheck go_check(*pSource, uiEntry, fMaxSearchRange);
    MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> searcher(pGo, go_check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);

    return pGo;
}

//return closest creature alive in grid, with range from pSource
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    Creature* pCreature = NULL;

    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*pSource, uiEntry, true, fMaxSearchRange);
    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);

    return pCreature;
}

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList , WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    AllGameObjectsWithEntryInRangeCheck check(pSource, uiEntry, fMaxSearchRange);
    MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRangeCheck> searcher(lList, check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);
}

void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    AllCreaturesOfEntryInRangeCheck check(pSource, uiEntry, fMaxSearchRange);
    MaNGOS::CreatureListSearcher<AllCreaturesOfEntryInRangeCheck> searcher(lList, check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);
}

// return closest DynamicObject in grid, with range from pSource
DynamicObject* GetClosestDynamicObjectWithEntry(WorldObject* pSource, uint32 spellId, float fMaxSearchRange)
{
    DynamicObject* pDynObj = NULL;

    NearestDynamicObjectEntryInRangeCheck check(*pSource, spellId, fMaxSearchRange);
    DynamicObjectLastSearcher<NearestDynamicObjectEntryInRangeCheck> searcher(pDynObj, check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);

    return pDynObj;
}

// return list of DynamicObject's in grid within range from pSource
void GetDynamicObjectListWithEntryInGrid(std::list<DynamicObject*>& lList, WorldObject* pSource, uint32 spellId, float fMaxSearchRange)
{
    AllDynamicObjectsWithEntryInRangeCheck check(*pSource, spellId, fMaxSearchRange);
    DynamicObjectListSearcher<AllDynamicObjectsWithEntryInRangeCheck> searcher(lList, check);

    Cell::VisitGridObjects(pSource, searcher, fMaxSearchRange);
}
