#ifndef SC_UTILITY_H
#define SC_UTILITY_H

class EventMap
{
protected:
    typedef std::map<uint32 /*time*/, uint32 /*eventId*/> EventMapType;
    EventMapType m_events;

    uint32 m_time, m_phase;

public:
    explicit EventMap(): m_time(0), m_phase(0) {}

    uint32 GetTimer() const { return m_time; }
    void Update(uint32 time) { m_time += time; }

    void Reset();
    void SetPhase(uint32 phase);
    void ScheduleEvent(uint32 eventId, uint32 time, uint32 gcd = 0, uint32 phase = 0);
    void RescheduleEvent(uint32 eventId, uint32 time, uint32 gcd = 0, uint32 phase = 0);
    void RepeatEvent(uint32 time);
    void PopEvent();
    uint32 ExecuteEvent();
    uint32 GetEvent();
    void DelayEvents(uint32 time, uint32 gcd);
    void CancelEvent(uint32 eventId);
    void CancelEventsByGCD(uint32 gcd);

private:
    EventMap(EventMap const&);
    EventMap& operator= (EventMap const&);
};

#endif
