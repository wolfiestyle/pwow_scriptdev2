#include "precompiled.h"
#include "TemporarySummon.h"

//--- EventManager --------------------------------------------------

void EventManager::Cooldown::Update(uint32 time_diff)
{
    if (!remaining)
        return;
    if (remaining <= time_diff)
        duration = remaining = 0;
    else
        remaining -= time_diff;
}

EventManager::EventManager():
    m_time(0), m_phase(0)
{
}

void EventManager::InsertEvent(uint32 time_diff, Event const& ev)
{
    m_events.insert(EventContainer::value_type(m_time + time_diff, ev));
}

uint32 EventManager::GetCooldownDuration(uint32 categ) const
{
    CooldownMap::const_iterator i_cd = m_cooldowns.find(categ);
    if (i_cd == m_cooldowns.end())
        return 0;
    return i_cd->second.duration;
}

uint32 EventManager::GetCooldownTime(uint32 categ) const
{
    CooldownMap::const_iterator i_cd = m_cooldowns.find(categ);
    if (i_cd == m_cooldowns.end())
        return 0;
    return i_cd->second.remaining;
}

void EventManager::Reset()
{
    m_time = m_phase = 0;
    m_events.clear();
    m_cooldowns.clear();
}

void EventManager::ResetCooldowns()
{
    m_cooldowns.clear();
}

void EventManager::Update(uint32 time_diff)
{
    m_time += time_diff;
    for (CooldownMap::iterator i = m_cooldowns.begin(); i != m_cooldowns.end(); ++i)
        i->second.Update(time_diff);
}

void EventManager::SetPhase(uint32 phase)
{
    if (phase >= 32)
        throw std::domain_error("EventManager::SetPhase(): phase value must be < 32");
    m_phase = phase;
}

void EventManager::SetCooldown(uint32 time, uint32 categ)
{
    m_cooldowns[categ] = Cooldown(time);
}

void EventManager::ScheduleEvent(uint32 event_id, uint32 time, uint32 period, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    if (!event_id)
        throw std::invalid_argument("EventManager::ScheduleEvent(): event_id must be != 0");
    if (!phase_mask)
        throw std::invalid_argument("EventManager::ScheduleEvent(): phase_mask must be != 0");
    InsertEvent(time, Event(event_id, period, period, phase_mask, cd, cd_cat));
}

void EventManager::RescheduleEvent(uint32 event_id, uint32 time, uint32 period, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    CancelEvent(event_id);
    ScheduleEvent(event_id, time, period, cd, cd_cat, phase_mask);
}

void EventManager::ScheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max, uint32 period_min, uint32 period_max, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    if (!event_id)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): event_id must be != 0");
    if (!phase_mask)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): phase_mask must be != 0");
    if (!time_max)
        time_max = time_min;
    if (!period_max)
        period_max = period_min;
    if (time_max < time_min)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): time_max must be >= time_min");
    if (period_max < period_min)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): period_max must be >= period_min");
    if (!period_min && period_max)
        throw std::invalid_argument("EventManager::ScheduleEventInRange(): period range must not include 0");
    uint32 time = time_min != time_max ? urand(time_min, time_max) : time_min;
    InsertEvent(time, Event(event_id, period_min, period_max, phase_mask, cd, cd_cat));
}

void EventManager::RescheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max, uint32 period_min, uint32 period_max, uint32 cd, uint32 cd_cat, uint32 phase_mask)
{
    CancelEvent(event_id);
    ScheduleEventInRange(event_id, time_min, time_max, period_min, period_max, cd, cd_cat, phase_mask);
}

bool EventManager::HasEvent(uint32 time) const
{
    EventContainer::const_iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return false;
    return i_ev->first <= m_time + time;
}

uint32 EventManager::GetNextEvent() const
{
    EventContainer::const_iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return 0;
    return i_ev->second.id;
}

void EventManager::CancelAllEvents()
{
    m_events.clear();
}

void EventManager::CancelEvent(uint32 event_id, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
}

void EventManager::CancelNextEvent()
{
    EventContainer::iterator i_ev = m_events.begin();
    if (i_ev == m_events.end())
        return;
    m_events.erase(i_ev);
}

void EventManager::CancelNextEventWithId(uint32 event_id, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); ++i)
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            break;
        }
}

void EventManager::CancelEventsWithCategory(uint32 categ, uint32 phase_mask)
{
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.category == categ && i->second.phase_mask & phase_mask)
        {
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
}

void EventManager::DelayEvents(uint32 time, uint32 phase_mask)
{
    EventContainer delayed;
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.phase_mask & phase_mask)
        {
            delayed.insert(std::make_pair(i->first + time, i->second));
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
    m_events.insert(delayed.begin(), delayed.end());
}

void EventManager::DelayEventsWithId(uint32 event_id, uint32 time, uint32 phase_mask)
{
    EventContainer delayed;
    for (EventContainer::iterator i = m_events.begin(); i != m_events.end(); )
        if (i->second.id == event_id && i->second.phase_mask & phase_mask)
        {
            delayed.insert(std::make_pair(i->first + time, i->second));
            m_events.erase(i);
            i = m_events.begin();
        }
        else
            ++i;
    m_events.insert(delayed.begin(), delayed.end());
}

uint32 EventManager::ExecuteEvent()
{
    EventContainer::iterator i_ev = m_events.begin();
    // container empty, no events to process
    if (i_ev == m_events.end())
        return 0;
    // we got a event to process, executing
    if (i_ev->first <= m_time)
    {
        uint32 delta = m_time - i_ev->first;    // latency time in executing this event
        Event ev = i_ev->second;
        m_events.erase(i_ev);
        // if event is periodic, calculate next execution time
        uint32 period = ev.period_min != ev.period_max ? urand(ev.period_min, ev.period_max) : ev.period_min;
        if (period)
        {
            delta %= period; // in case we took way too long, get difference counting lost ticks
            InsertEvent(period - delta, ev);
        }
        // check if can be executed in current phase
        if (!(ev.phase_mask & GetPhaseMask()))
            return 0;
        // if we're on cooldown then cannot execute event
        CooldownMap::const_iterator i_cd = m_cooldowns.find(ev.category);
        if (i_cd != m_cooldowns.end() && i_cd->second.remaining)
        {
            // try again after cooldown ends, but don't wait longer than remaining period time
            if (!period || (i_cd->second.duration < period && i_cd->second.remaining < period - delta))
            {
                ev.period_min = ev.period_max = 0;
                InsertEvent(i_cd->second.remaining, ev);
            }
            return 0;
        }
        // all ok, now trigger cooldown if any
        SetCooldown(ev.cooldown, ev.category);
        // return succeful event executed
        return ev.id;
    }
    // all events in future, nothing to do for now
    return 0;
}

//--- SummonManager -------------------------------------------------

void SummonManager::AddSummonToList(ObjectGuid const& guid)
{
    if (!guid.IsCreature())
        return;
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

template <typename ContainerType>
void SummonManager::GetAllSummonsWithId(ContainerType& list, uint32 Id) const
{
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (i->GetEntry() == Id)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                list.push_back(pSummon);
}

template void SummonManager::GetAllSummonsWithId(std::vector<Creature*>&, uint32) const;
template void SummonManager::GetAllSummonsWithId(std::deque<Creature*>&,  uint32) const;
template void SummonManager::GetAllSummonsWithId(std::list<Creature*>&,   uint32) const;

Creature* SummonManager::GetFirstFoundSummonWithId(uint32 Id) const
{
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (i->GetEntry() == Id)
            return m_creature->GetMap()->GetCreature(*i);
    return NULL;
}

void SummonManager::UnsummonCreature(Creature *pSummon)
{
    if (!pSummon)
        return;
    RemoveSummonFromList(pSummon->GetObjectGuid());
    if (pSummon->isTemporarySummon())
        static_cast<TemporarySummon*>(pSummon)->UnSummon();
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
            {
                if (pSummon->isTemporarySummon())
                    static_cast<TemporarySummon*>(pSummon)->UnSummon();
                else    // non-summoned creature in list
                    pSummon->ForcedDespawn();
            }
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
        {
            if (pSummon->isTemporarySummon())
                static_cast<TemporarySummon*>(pSummon)->UnSummon();
            else    // non-summoned creature in list
                pSummon->ForcedDespawn();
        }

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

uint32 GetSpellIdWithDifficulty(uint32 spell_id, Difficulty diff)
{
    SpellEntry const* sp_entry = GetSpellStore()->LookupEntry(spell_id);
    if (!sp_entry)
        return spell_id;
    SpellDifficultyEntry const* diff_entry = GetSpellDifficultyStore()->LookupEntry(sp_entry->SpellDifficultyId);
    if (!diff_entry)
        return spell_id;
    return diff_entry->spellId[diff];
}

//--- Script Messaging ----------------------------------------------

void BroadcastScriptMessage(Creature* pSender, float fMaxRange, uint32 data1, uint32 data2, bool to_self)
{
    AllCreaturesInRangeCheck check(pSender, fMaxRange);
    ScriptMessageDeliverer<AllCreaturesInRangeCheck> deliverer(pSender, check, data1, data2, to_self);

    Cell::VisitGridObjects(pSender, deliverer, fMaxRange);
}

void BroadcastScriptMessageToEntry(Creature* pSender, uint32 entry, float fMaxRange, uint32 data1, uint32 data2, bool to_self)
{
    AllCreaturesOfEntryInRange check(pSender, entry, fMaxRange);
    ScriptMessageDeliverer<AllCreaturesOfEntryInRange> deliverer(pSender, check, data1, data2, to_self);

    Cell::VisitGridObjects(pSender, deliverer, fMaxRange);
}

ScriptEventInterface::ScriptEventInterface(Creature *pSender):
    m_sender(pSender)
{
}

void ScriptEventInterface::ScriptMessage(Creature* from, uint32 event_id, uint32 event_timer)
{
    Events.ScheduleEvent(event_id, event_timer);
}

void ScriptEventInterface::BroadcastEvent(uint32 event_id, uint32 event_timer, float max_range, bool to_self)
{
    BroadcastScriptMessage(m_sender, max_range, event_id, event_timer, to_self);
}

void ScriptEventInterface::BroadcastEventToEntry(uint32 entry, uint32 event_id, uint32 event_timer, float max_range, bool to_self)
{
    BroadcastScriptMessageToEntry(m_sender, entry, max_range, event_id, event_timer, to_self);
}
