#ifndef SC_UTILITY_H
#define SC_UTILITY_H

// class for handling timed events
class EventManager
{
protected:
    struct Event
    {
        uint32 id;          // event identifier
        uint32 period_min;  // repeating period (0 = doesn't repeat)
        uint32 period_max;  // random value in interval 'period_min' to 'period_max'
        uint32 phase_mask;  // phases where this event can happen (bit mask)
        uint32 cooldown;    // cooldown triggered after event executes
        uint32 category;    // cooldown category (will trigger cd only for this category)

        Event(uint32 _id, uint32 _pmin, uint32 _pmax, uint32 _phase, uint32 _cd, uint32 _cat):
            id(_id), period_min(_pmin), period_max(_pmax), phase_mask(_phase), cooldown(_cd), category(_cat)
        {
        }
    };

    struct Cooldown
    {
        uint32 duration;    // total length of the cooldown
        uint32 remaining;   // remaining time

        explicit Cooldown(uint32 _val = 0):
            duration(_val), remaining(_val)
        {
        }

        void Update(uint32 time_diff);
    };

    typedef std::multimap<uint32 /*time*/, Event> EventContainer;
    typedef UNORDERED_MAP<uint32 /*category*/, Cooldown> CooldownMap;

    uint32 m_time;              // current elapsed time (in milliseconds)
    uint32 m_phase;             // current phase (value, not mask)
    EventContainer m_events;    // event queue
    CooldownMap m_cooldowns;    // cooldowns by category

    void InsertEvent(uint32 time_diff, Event const& ev);

public:
    enum { ALL_PHASES = 0xFFFFFFFF };   // default value for phase mask

    EventManager();

    uint32 GetTimer() const { return m_time; }
    uint32 GetPhase() const { return m_phase; }
    uint32 GetPhaseMask() const { return 1 << m_phase; }
    uint32 GetCooldownDuration(uint32 categ) const;
    uint32 GetCooldownTime(uint32 categ) const;

    void Reset();
    void ResetCooldowns();

    void Update(uint32 time_diff);
    void SetPhase(uint32 phase);
    void SetCooldown(uint32 time, uint32 categ = 0);

    // inserts an event in the queue that will execute in the next 'time' milliseconds and
    // will repeat every 'period' milliseconds (if not 0). Each triggering of this event
    // will cause a cooldown of 'cd' milliseconds on the 'cd_cat' category
    void ScheduleEvent(uint32 event_id, uint32 time, uint32 period = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);
    // cancels pending events with the specified id and inserts new one into the queue
    void RescheduleEvent(uint32 event_id, uint32 time, uint32 period = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);

    // schedule event with random time interval
    // if the _max value is 0, then its assumed no range (value equal to _min)
    void ScheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max = 0, uint32 period_min = 0, uint32 period_max = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);
    void RescheduleEventInRange(uint32 event_id, uint32 time_min, uint32 time_max = 0, uint32 period_min = 0, uint32 period_max = 0, uint32 cd = 0, uint32 cd_cat = 0, uint32 phase_mask = ALL_PHASES);

    bool HasEvent(uint32 time = 0) const;   // check if there is a pending event in the next 'time' milliseconds
    uint32 GetNextEvent() const;            // returns next queued event

    void CancelAllEvents();                                             // cancels all events
    void CancelEvent(uint32 event_id, uint32 phase_mask = ALL_PHASES);  // ^ with specified id
    void CancelNextEvent();                                             // cancels next queued event
    void CancelNextEventWithId(uint32 event_id, uint32 phase_mask = ALL_PHASES); // ^ with specified id
    void CancelEventsWithCategory(uint32 categ, uint32 phase_mask = ALL_PHASES); // cancels all events with specified cd category

    void DelayEvents(uint32 time, uint32 phase_mask = ALL_PHASES);      // delays all events by the specified amount
    void DelayEventsWithId(uint32 event_id, uint32 time, uint32 phase_mask = ALL_PHASES); // ^ with specified id

    uint32 ExecuteEvent();                  // process and return next pending event

private:
    EventManager(EventManager const&);
    EventManager& operator= (EventManager const&);
};

// helper for calculating bit masks (compile time constant that can be used in enums)
// usage: bit_mask<bit_num, ...>::value
template <int N1, int N2 = -1, int N3 = -1, int N4 = -1, int N5 = -1, int N6 = -1>
struct bit_mask { enum { value = 1 << N1 | bit_mask<N2, N3, N4, N5, N6, -1>::value }; };

template <>
struct bit_mask<-1, -1, -1, -1, -1, -1> { enum { value = 0 }; };

// helper for static initialization of map containers
// (map, multimap, unordered_map, unordered_multimap)
template <typename ContainerType>
class map_initializer
{
protected:
    typedef typename ContainerType::key_type    KeyType;
    typedef typename ContainerType::mapped_type MappedType;
    typedef typename ContainerType::value_type  ValueType;

    ContainerType m_map;

public:
    map_initializer(KeyType const& key, MappedType const& val)
    {
        m_map.insert(ValueType(key, val));
    }

    map_initializer& operator() (KeyType const& key, MappedType const& val)
    {
        m_map.insert(ValueType(key, val));
        return *this;
    }

    operator ContainerType() const
    {
        return m_map;
    }
};

// helper for searching value inside a map container
template <typename MapType>
typename MapType::mapped_type map_find(MapType const& map,
        typename MapType::key_type const& search_value,
        typename MapType::mapped_type const& fail_value = typename MapType::mapped_type())
{
    typename MapType::const_iterator it = map.find(search_value);
    return it != map.end() ? it->second : fail_value;
}

// defines stream operators to serialize a std::map container
namespace serialize {

// outputs a std::pair to a stream
template <typename T1, typename T2>
std::ostream& operator<< (std::ostream& out, std::pair<T1, T2> const& p)
{
    out << p.first << ' ' << p.second;
    return out;
}

// reads a std::pair from a stream
template <typename T1, typename T2>
std::istream& operator>> (std::istream& in, std::pair<T1, T2>& p)
{
    in >> p.first >> p.second;
    return in;
}

// outputs content of std::map into a stream
template <typename K, typename V>
std::ostream& operator<< (std::ostream& out, std::map<K, V> const& m)
{
    for (typename std::map<K, V>::const_iterator i = m.begin(); i != m.end(); ++i)
        out << *i << ' ';
    return out;
}

// reads content of a std::map from a stream
template <typename K, typename V>
std::istream& operator>> (std::istream& in, std::map<K, V>& m)
{
    std::pair<K, V> value;
    while (in >> value)
        m[value.first] = value.second;
    return in;
}

} // namespace serialize

// wrapper for accessing data variables in instance scripts
template <typename T>
class InstanceVarBase
{
public:
    virtual ~InstanceVarBase() { }

    uint32 getDataId() const { return m_dataId; }
    InstanceData* getInstanceData() const { return m_instance; }
    void setInstanceData(InstanceData* _instance) { m_instance = _instance; }

    virtual T get() const = 0;
    virtual void set(T val) = 0;

    operator T() const
    {
        return get();
    }

    T operator++ ()
    {
        T val = get();
        set(++val);
        return val;
    }

    T operator-- ()
    {
        T val = get();
        if (val > 0)    // prevent wrap-around to unsigned negative value
            set(--val);
        return val;
    }

    T operator++ (int)
    {
        T val = get();
        T temp = val++;
        set(val);
        return temp;
    }

    T operator-- (int)
    {
        T val = get();
        if (val > 0)    // prevent wrap-around to unsigned negative value
        {
            T temp = val--;
            set(val);
            return temp;
        }
        else
            return val;
    }

    T operator+= (T rhs)
    {
        T val = get() + rhs;
        set(val);
        return val;
    }

    T operator-= (T rhs)
    {
        T val = get();
        if (val > rhs)  // prevent wrap-around to unsigned negative value
            val -= rhs;
        else
            val = 0;
        set(val);
        return val;
    }

protected:
    InstanceData *m_instance;
    uint32 const m_dataId;

    InstanceVarBase(uint32 _id, InstanceData* _instance):
        m_instance(_instance), m_dataId(_id)
    {
    }
};

// empty base template (only uint32 and uint64 cases meaningful)
template <typename T>
class InstanceVar;

// specialization for GetData / SetData
template <>
class InstanceVar<uint32>: public InstanceVarBase<uint32>
{
public:
    explicit InstanceVar(uint32 _id, InstanceData* _instance = NULL):
        InstanceVarBase<uint32>(_id, _instance)
    {
    }

    uint32 get() const
    {
        return m_instance ? m_instance->GetData(m_dataId) : 0;
    }

    void set(uint32 val)
    {
        if (m_instance)
            m_instance->SetData(m_dataId, val);
    }

    // copy value instead of whole object
    InstanceVar& operator= (InstanceVar const& var)
    {
        set(var.get());
        return *this;
    }

    uint32 operator= (uint32 val)
    {
        set(val);
        return val;
    }

private:
    InstanceVar(InstanceVar const&);    // prohibit object copy
};

// specialization for GetData64 / SetData64
template <>
class InstanceVar<uint64>: public InstanceVarBase<uint64>
{
public:
    explicit InstanceVar(uint32 _id, InstanceData* _instance = NULL):
        InstanceVarBase<uint64>(_id, _instance)
    {
    }

    uint64 get() const
    {
        return m_instance ? m_instance->GetData64(m_dataId) : 0;
    }

    void set(uint64 val)
    {
        if (m_instance)
            m_instance->SetData64(m_dataId, val);
    }

    // copy value instead of whole object
    InstanceVar& operator= (InstanceVar const& var)
    {
        set(var.get());
        return *this;
    }

    uint64 operator= (uint64 val)
    {
        set(val);
        return val;
    }

    // find object using stored value as guid
    template <typename SearchType>
    SearchType* getObject() const
    {
        return m_instance ? m_instance->instance->GetObjectsStore().find<SearchType>(get(), (SearchType*)NULL) : NULL;
    }

private:
    InstanceVar(InstanceVar const&);    // prohibit object copy
};

// SummonManager - keeps track of summons.
class SummonManager
{
public:
    typedef std::list<ObjectGuid> SummonContainer;

    explicit SummonManager(Creature* pCreature): m_creature(pCreature) { }

    void AddSummonToList(ObjectGuid const&);
    void RemoveSummonFromList(ObjectGuid const&);

    Creature* SummonCreature(uint32 Id, float x, float y, float z, float ang = 0.0f, TempSummonType type = TEMPSUMMON_MANUAL_DESPAWN, uint32 SummonTimer = 0);
    void SummonCreatures(uint32 Id, float x, float y, float z, uint32 number, float ang = 0.0f, TempSummonType type = TEMPSUMMON_MANUAL_DESPAWN, uint32 SummonTimer = 0);
    Creature* SummonCreatureAt(WorldObject* target, uint32 Id, TempSummonType type = TEMPSUMMON_MANUAL_DESPAWN, uint32 SummonTimer = 0, float dx = 0.0f, float dy = 0.0f, float dz = 0.0f, float dang = 0.0f);
    void SummonCreaturesAt(WorldObject* target, uint32 Id, uint32 number, TempSummonType type = TEMPSUMMON_MANUAL_DESPAWN, uint32 SummonTimer = 0, float dx = 0.0f, float dy = 0.0f, float dz = 0.0f, float dang = 0.0f);

    uint32 GetSummonCount() const { return m_Summons.size(); }
    uint32 GetSummonCount(uint32 Id) const;

    template <typename ContainerType>
    void GetAllSummonsWithId(ContainerType& list, uint32 Id) const;
    Creature* GetFirstFoundSummonWithId(uint32 Id) const;
    SummonContainer const& GetSummonList() const { return m_Summons; }

    void UnsummonCreature(Creature*);
    void UnsummonByGuid(ObjectGuid const&);
    void UnsummonAllWithId(uint32 Id);
    void UnsummonAll();

protected:
    Creature *m_creature;
    SummonContainer m_Summons;

private:
    SummonManager(SummonManager const&);
    SummonManager& operator= (SummonManager const&);
};

// get random point inside area delimited by circle
void GetRandomPointInCircle(float& x, float& y, float max_rad, float cx = 0.0f, float cy = 0.0f);

// get correct spell_id for the given difficulty
uint32 GetSpellIdWithDifficulty(uint32, Difficulty);

// --- Script Messaging System ---

// defines a callback function used for receiving script messages
struct ScriptMessageInterface
{
    virtual ~ScriptMessageInterface() {}
    virtual void ScriptMessage(Creature* /*sender*/, uint32 /*data1*/, uint32 /*data2*/) = 0;
};

// grid notifier that does the script messaging (internal usage)
template <class Check>
class ScriptMessageDeliverer
{
public:
    ScriptMessageDeliverer(Creature* sender, Check& check, uint32 data1, uint32 data2, bool to_self):
        i_sender(sender), i_check(check), i_data1(data1), i_data2(data2), i_toSelf(to_self)
    {
    }

    void Visit(CreatureMapType& m)
    {
        for (CreatureMapType::iterator i = m.begin(); i != m.end(); ++i)
            if (i_toSelf || i->getSource() != i_sender)
                if (i_check(i->getSource()))
                    if (ScriptMessageInterface *smi = dynamic_cast<ScriptMessageInterface*>(i->getSource()->AI()))
                        smi->ScriptMessage(i_sender, i_data1, i_data2);
    }

    template <class SKIP> void Visit(GridRefManager<SKIP>&) {}

private:
    Creature *i_sender;
    Check &i_check;
    uint32 i_data1, i_data2;
    bool i_toSelf;
};

// check for any creature in range (internal usage)
class AllCreaturesInRangeCheck
{
public:
    AllCreaturesInRangeCheck(WorldObject const* source, float range):
        i_source(source), i_range(range)
    {
    }

    bool operator() (Unit* pUnit)
    {
        return i_source->IsWithinDist(pUnit, i_range);
    }

private:
    WorldObject const* i_source;
    float i_range;

    AllCreaturesInRangeCheck(AllCreaturesInRangeCheck const&);
};

// sends a script message to all creatures in range
void BroadcastScriptMessage(Creature* pSender, float fMaxRange, uint32 data1 = 0, uint32 data2 = 0, bool to_self = false);

// sends a script message to all creatures with entry in range
void BroadcastScriptMessageToEntry(Creature* pSender, uint32 entry, float fMaxRange, uint32 data1 = 0, uint32 data2 = 0, bool to_self = false);

// base class that implements event broadcasting using EventManager
class ScriptEventInterface: public ScriptMessageInterface
{
public:
    EventManager Events;

    // callback function that schedules a remote event
    void ScriptMessage(Creature* from, uint32 event_id, uint32 event_timer);

    // sends an event to all creatures in range
    void BroadcastEvent(uint32 event_id, uint32 event_timer, float max_range, bool to_self = false);

    // sends an event to all creatures with entry in range
    void BroadcastEventToEntry(uint32 entry, uint32 event_id, uint32 event_timer, float max_range, bool to_self = false);

protected:
    Creature *m_sender;

    explicit ScriptEventInterface(Creature *pSender);
};

#endif // SC_UTILITY_H
