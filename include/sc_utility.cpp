#include "precompiled.h"
#include "sc_utility.h"

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
