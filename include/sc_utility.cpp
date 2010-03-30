#include "precompiled.h"
#include "sc_utility.h"

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
    EventMapType::iterator itr = m_events.find(time);
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
    EventMapType::iterator itr = m_events.find(time);
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
    for (EventMapType::iterator itr = m_events.begin(); itr != m_events.end(); )
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
    for (EventMapType::iterator itr = m_events.begin(); itr != m_events.end(); )
    {
        if (eventId == (itr->second & 0x0000FFFF))
            m_events.erase(itr++);
        else
            ++itr;
    }
}

void EventMap::CancelEventsByGCD(uint32 gcd)
{
    for (EventMapType::iterator itr = m_events.begin(); itr != m_events.end(); )
    {
        if (itr->second & gcd)
            m_events.erase(itr++);
        else
            ++itr;
    }
}
