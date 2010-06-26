#include "precompiled.h"
#include "TemporarySummon.h"

//--- EventMap ------------------------------------------------------

void EventMap::Reset()
{
    m_events.clear();
    m_time = m_phase = 0;
}

void EventMap::SetPhase(uint32 phase)
{
    if (phase && phase < 9)
        m_phase = (1 << (phase + 24));
}

void EventMap::ScheduleEvent(uint32 eventId, uint32 time, uint32 gcd, uint32 phase)
{
    time += m_time;
    if (gcd && gcd < 9)
        eventId |= (1 << (gcd + 16));
    if (phase && phase < 9)
        eventId |= (1 << (phase + 24));
    EventContainer::iterator itr = m_events.find(time);
    while (itr != m_events.end())
    {
        ++time;
        itr = m_events.find(time);
    }
    m_events.insert(std::make_pair(time, eventId));
}

void EventMap::RescheduleEvent(uint32 eventId, uint32 time, uint32 gcd, uint32 phase)
{
    CancelEvent(eventId);
    ScheduleEvent(eventId, time, gcd, phase);
}

void EventMap::RepeatEvent(uint32 time)
{
    if (m_events.empty())
        return;
    uint32 eventId = m_events.begin()->second;
    m_events.erase(m_events.begin());
    time += m_time;
    EventContainer::iterator itr = m_events.find(time);
    while (itr != m_events.end())
    {
        ++time;
        itr = m_events.find(time);
    }
    m_events.insert(std::make_pair(time, eventId));
}

void EventMap::PopEvent()
{
    m_events.erase(m_events.begin());
}

uint32 EventMap::ExecuteEvent()
{
    while (!m_events.empty())
    {
        if (m_events.begin()->first > m_time)
            return 0;
        else if (m_phase && (m_events.begin()->second & 0xFF000000) && !(m_events.begin()->second & m_phase))
            m_events.erase(m_events.begin());
        else
        {
            uint32 eventId = (m_events.begin()->second & 0x0000FFFF);
            m_events.erase(m_events.begin());
            return eventId;
        }
    }
    return 0;
}

uint32 EventMap::GetEvent()
{
    while (!m_events.empty())
    {
        if (m_events.begin()->first > m_time)
            return 0;
        else if (m_phase && (m_events.begin()->second & 0xFF000000) && !(m_events.begin()->second & m_phase))
            m_events.erase(m_events.begin());
        else
            return (m_events.begin()->second & 0x0000FFFF);
    }
    return 0;
}

void EventMap::DelayEvents(uint32 time, uint32 gcd)
{
    time += m_time;
    gcd = (1 << (gcd + 16));
    for (EventContainer::iterator itr = m_events.begin(); itr != m_events.end(); )
    {
        if (itr->first >= time)
            break;
        if (itr->second & gcd)
        {
            ScheduleEvent(time, itr->second);
            m_events.erase(itr++);
        }
        else
            ++itr;
    }
}

void EventMap::CancelEvent(uint32 eventId)
{
    for (EventContainer::iterator itr = m_events.begin(); itr != m_events.end(); )
    {
        if (eventId == (itr->second & 0x0000FFFF))
            m_events.erase(itr++);
        else
            ++itr;
    }
}

void EventMap::CancelEventsByGCD(uint32 gcd)
{
    for (EventContainer::iterator itr = m_events.begin(); itr != m_events.end(); )
    {
        if (itr->second & gcd)
            m_events.erase(itr++);
        else
            ++itr;
    }
}

//--- SummonManager -------------------------------------------------

void SummonManager::AddSummonToList(ObjectGuid const& guid)
{
    m_Summons.push_back(guid);
}

void SummonManager::RemoveSummonFromList(ObjectGuid const& guid)
{
    // could have used remove(guid) but we know they're unique so it wastes time searching after unique entry removed
    for (SummonContainer::iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (*i == guid)
        {
            m_Summons.erase(i);
            break;
        }
}

Creature* SummonManager::SummonCreature(uint32 Id, float x, float y, float z, float ang, TempSummonType type, uint32 SummonTimer)
{
    Creature *pSummon = m_creature->SummonCreature(Id, x, y, z, ang, type, SummonTimer);
    if (pSummon)
        AddSummonToList(pSummon->GetObjectGuid());
    return pSummon;
}

void SummonManager::SummonCreatures(uint32 Id, float x, float y, float z, uint32 number, float ang, TempSummonType type, uint32 SummonTimer)
{
    for (; number; --number)
        SummonCreature(Id, x, y, z, ang, type, SummonTimer);
}

Creature* SummonManager::SummonCreatureAt(WorldObject* target, uint32 Id, TempSummonType type, uint32 SummonTimer, float dx, float dy, float dz, float dang)
{
    return SummonCreature(Id, target->GetPositionX()+dx, target->GetPositionY()+dy, target->GetPositionZ()+dz, target->GetOrientation()+dang, type, SummonTimer);
}

void SummonManager::SummonCreaturesAt(WorldObject* target, uint32 Id, uint32 number, TempSummonType type, uint32 SummonTimer, float dx, float dy, float dz, float dang)
{
    for (; number; --number)
        SummonCreatureAt(target, Id, type, SummonTimer, dx, dy, dz, dang);
}

uint32 SummonManager::GetSummonCount(uint32 Id) const
{
    uint32 count = 0;
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (i->GetEntry() == Id)
            count++;
    return count;
}

void SummonManager::UnsummonCreature(Creature *pSummon)
{
    if (!pSummon)
        return;
    if (pSummon->isTemporarySummon())
    {
        RemoveSummonFromList(pSummon->GetObjectGuid());
        static_cast<TemporarySummon*>(pSummon)->UnSummon();
    }
    else    // called for non-summoned unit
        pSummon->ForcedDespawn();
}

void SummonManager::UnsummonByGuid(ObjectGuid const& guid)
{
    if (Creature *pSummon = m_creature->GetMap()->GetCreature(guid))
        UnsummonCreature(pSummon);
}

void SummonManager::UnsummonAllWithId(uint32 Id)
{
    for (SummonContainer::iterator i = m_Summons.begin(); i != m_Summons.end(); )
    {
        if (i->GetEntry() == Id)
        {
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                static_cast<TemporarySummon*>(pSummon)->UnSummon();
            i = m_Summons.erase(i);
        }
        else
            ++i;
    }
}

void SummonManager::UnsummonAll()
{
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
            static_cast<TemporarySummon*>(pSummon)->UnSummon();

    m_Summons.clear();
}

//----------------------------------------------------------------

void GetRandomPointInCircle(float& x, float& y, float max_rad, float cx, float cy)
{
    float ang = 2*M_PI * rand_norm();
    float rad = max_rad * sqrt(rand_norm());
    x = cx + rad * cos(ang);
    y = cy + rad * sin(ang);
}
