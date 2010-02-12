#include "precompiled.h"
#include "ulduar.h"

bool IsOutOfCombatArea(Creature *pCreature)
{
    float x, y, z;
    pCreature->GetPosition(x, y, z);
    uint32 areaId = pCreature->GetMap()->GetAreaId(x, y, z);

    switch (pCreature->GetEntry())
    {
        case NPC_LEVIATHAN:
            return areaId != 4652;  // Formation Grounds
        case NPC_IGNIS:
            return areaId != 4654;  // The Colossal Forge
        case NPC_RAZORSCALE:
            return areaId != 4653;  // Razorscale's Aerie
        case NPC_XT002:
            return areaId != 4655;  // The Scrapyard
        case NPC_STEELBREAKER:
        case NPC_MOLGEIM:
        case NPC_BRUNDIR:
            return !pCreature->IsWithinDist2d(1587.6f, 119.8f, 90.0f);
        case NPC_KOLOGARN:
        case NPC_LEFT_ARM:
        case NPC_RIGHT_ARM:
            return !pCreature->IsWithinDist2d(1799.6f, -24.3f, 70.0f);
        case NPC_AURIAYA:
            return z < 400.0f || !pCreature->IsWithinDist2d(1981.5f, -28.5f, 120.0f);
        case NPC_MIMIRON:
        case NPC_LEVIATHAN_MKII:
        case NPC_VX001:
        case NPC_AERIAL_COMMAND_UNIT:
            return !pCreature->IsWithinDist2d(2744.1f, 2569.1f, 100.0f);
        case NPC_HODIR:
            return !pCreature->IsWithinDist2d(2000.7f, -233.7f, 70.0f);
        case NPC_THORIM:
            return !pCreature->IsWithinDist2d(2135.0f, -303.8f, 90.0f);
        case NPC_FREYA:
            return areaId != 4656;  // The Conservatory of Life
        case NPC_VEZAX:
            return !pCreature->IsWithinDist2d(1841.1f, 133.7f, 90.0f);
        case NPC_YOGGSARON:
            return z > 400.0f || !pCreature->IsWithinDist2d(1981.5f, -28.5f, 100.0f);
        case NPC_ALGALON:
            return !pCreature->IsWithinDist2d(1632.9f, -307.9f, 90.0f);
        default:
            return false;
    }
}

void EventMap::Reset()
{
    clear();
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
    iterator itr = find(time);
    while (itr != end())
    {
        ++time;
        itr = find(time);
    }
    insert(std::make_pair(time, eventId));
}

void EventMap::RescheduleEvent(uint32 eventId, uint32 time, uint32 gcd, uint32 phase)
{
    CancelEvent(eventId);
    ScheduleEvent(eventId, time, gcd, phase);
}

void EventMap::RepeatEvent(uint32 time)
{
    if (empty())
        return;
    uint32 eventId = begin()->second;
    erase(begin());
    time += m_time;
    iterator itr = find(time);
    while (itr != end())
    {
        ++time;
        itr = find(time);
    }
    insert(std::make_pair(time, eventId));
}

void EventMap::PopEvent()
{
    erase(begin());
}

uint32 EventMap::ExecuteEvent()
{
    while (!empty())
    {
        if (begin()->first > m_time)
            return 0;
        else if (m_phase && (begin()->second & 0xFF000000) && !(begin()->second & m_phase))
            erase(begin());
        else
        {
            uint32 eventId = (begin()->second & 0x0000FFFF);
            erase(begin());
            return eventId;
        }
    }
    return 0;
}

uint32 EventMap::GetEvent()
{
    while (!empty())
    {
        if (begin()->first > m_time)
            return 0;
        else if (m_phase && (begin()->second & 0xFF000000) && !(begin()->second & m_phase))
            erase(begin());
        else
            return (begin()->second & 0x0000FFFF);
    }
    return 0;
}

void EventMap::DelayEvents(uint32 time, uint32 gcd)
{
    time += m_time;
    gcd = (1 << (gcd + 16));
    for (iterator itr = begin(); itr != end();)
    {
        if (itr->first >= time)
            break;
        if (itr->second & gcd)
        {
            ScheduleEvent(time, itr->second);
            erase(itr++);
        }
        else
            ++itr;
    }
}

void EventMap::CancelEvent(uint32 eventId)
{
    for (iterator itr = begin(); itr != end();)
    {
        if (eventId == (itr->second & 0x0000FFFF))
            erase(itr++);
        else
            ++itr;
    }
}

void EventMap::CancelEventsByGCD(uint32 gcd)
{
    for (iterator itr = begin(); itr != end();)
    {
        if (itr->second & gcd)
            erase(itr++);
        else
            ++itr;
    }
}
