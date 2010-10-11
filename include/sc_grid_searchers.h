/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_GRIDSEARCH_H
#define SC_GRIDSEARCH_H

#include "Unit.h"
#include "GameObject.h"

#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"

struct ObjectDistanceOrder : public std::binary_function<const WorldObject, const WorldObject, bool>
{
    const Unit* m_pSource;

    ObjectDistanceOrder(const Unit* pSource) : m_pSource(pSource) {};

    bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
    {
        return m_pSource->GetDistanceOrder(pLeft, pRight);
    }
};

struct ObjectDistanceOrderReversed : public std::binary_function<const WorldObject, const WorldObject, bool>
{
    const Unit* m_pSource;

    ObjectDistanceOrderReversed(const Unit* pSource) : m_pSource(pSource) {};

    bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
    {
        return !m_pSource->GetDistanceOrder(pLeft, pRight);
    }
};

GameObject* GetClosestGameObjectWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange);
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange);

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList , WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange);
void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange);

class AllGameObjectsWithEntryInRangeCheck
{
    public:
        AllGameObjectsWithEntryInRangeCheck(const WorldObject* pObject, uint32 uiEntry, float fMaxRange) : m_pObject(pObject), m_uiEntry(uiEntry), m_fRange(fMaxRange) {}
        WorldObject const& GetFocusObject() const { return *m_pObject; }
        bool operator() (GameObject* pGo)
        {
            if (pGo->GetEntry() == m_uiEntry && m_pObject->IsWithinDist(pGo,m_fRange,false))
                return true;

            return false;
        }

    private:
        const WorldObject* m_pObject;
        uint32 m_uiEntry;
        float m_fRange;
};

class AllCreaturesOfEntryInRangeCheck
{
    public:
        AllCreaturesOfEntryInRangeCheck(const WorldObject* pObject, uint32 uiEntry, float fMaxRange) : m_pObject(pObject), m_uiEntry(uiEntry), m_fRange(fMaxRange) {}
        WorldObject const& GetFocusObject() const { return *m_pObject; }
        bool operator() (Unit* pUnit)
        {
            if (pUnit->GetEntry() == m_uiEntry && m_pObject->IsWithinDist(pUnit,m_fRange,false))
                return true;

            return false;
        }

    private:
        const WorldObject* m_pObject;
        uint32 m_uiEntry;
        float m_fRange;
};

//Used in: hyjalAI.cpp
/*
class AllFriendlyCreaturesInGrid
{
    public:
        AllFriendlyCreaturesInGrid(Unit const* obj) : pUnit(obj) {}
        bool operator() (Unit* u)
        {
            if (u->isAlive() && u->GetVisibility() == VISIBILITY_ON && u->IsFriendlyTo(pUnit))
                return true;

            return false;
        }

    private:
        Unit const* pUnit;
};
*/

// DynamicObject grid searchers
template <class Check>
struct DynamicObjectLastSearcher
{
    uint32 i_phaseMask;
    DynamicObject*& i_object;
    Check& i_check;

    DynamicObjectLastSearcher(DynamicObject*& result, Check& check):
        i_phaseMask(check.GetFocusObject().GetPhaseMask()), i_object(result), i_check(check)
    {
    }

    void Visit(DynamicObjectMapType& m)
    {
        for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
            if (itr->getSource()->InSamePhase(i_phaseMask))
                if (i_check(itr->getSource()))
                    i_object = itr->getSource();
    }

    template <class SKIP> void Visit(GridRefManager<SKIP>&) {}
};

template <class Check>
struct DynamicObjectListSearcher
{
    uint32 i_phaseMask;
    std::list<DynamicObject*> &i_objects;
    Check &i_check;

    DynamicObjectListSearcher(std::list<DynamicObject*>& objects, Check& check):
        i_phaseMask(check.GetFocusObject().GetPhaseMask()), i_objects(objects), i_check(check)
    {
    }

    void Visit(DynamicObjectMapType& m)
    {
        for (DynamicObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
            if (itr->getSource()->InSamePhase(i_phaseMask))
                if (i_check(itr->getSource()))
                    i_objects.push_back(itr->getSource());
    }

    template <class SKIP> void Visit(GridRefManager<SKIP>&) {}
};

// DynamicObject checks
class NearestDynamicObjectEntryInRangeCheck
{
public:
    NearestDynamicObjectEntryInRangeCheck(WorldObject const& obj, uint32 spellId, float range):
        i_obj(obj), i_spellId(spellId), i_range(range)
    {
    }

    WorldObject const& GetFocusObject() const { return i_obj; }

    bool operator() (DynamicObject *dynobj)
    {
        if (dynobj->GetSpellId() == i_spellId && i_obj.IsWithinDistInMap(dynobj, i_range))
        {
            i_range = i_obj.GetDistance(dynobj);
            return true;
        }
        return false;
    }

    float GetLastRange() const { return i_range; }

private:
    WorldObject const& i_obj;
    uint32 i_spellId;
    float i_range;

    NearestDynamicObjectEntryInRangeCheck(NearestDynamicObjectEntryInRangeCheck const&);
};

class AllDynamicObjectsWithEntryInRangeCheck
{
public:
    AllDynamicObjectsWithEntryInRangeCheck(WorldObject const& obj, uint32 spellId, float range):
        i_obj(obj), i_spellId(spellId), i_range(range)
    {
    }

    WorldObject const& GetFocusObject() const { return i_obj; }

    bool operator() (DynamicObject* dynobj)
    {
        if (dynobj->GetSpellId() == i_spellId && i_obj.IsWithinDistInMap(dynobj, i_range))
            return true;

        return false;
    }

private:
    WorldObject const& i_obj;
    uint32 i_spellId;
    float i_range;

    AllDynamicObjectsWithEntryInRangeCheck(AllDynamicObjectsWithEntryInRangeCheck const&);
};

DynamicObject* GetClosestDynamicObjectWithEntry(WorldObject* pSource, uint32 spellId, float fMaxSearchRange);
void GetDynamicObjectListWithEntryInGrid(std::list<DynamicObject*>& lList, WorldObject* pSource, uint32 spellId, float fMaxSearchRange);

#endif
