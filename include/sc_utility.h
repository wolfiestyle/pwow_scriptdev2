#ifndef SC_UTILITY_H
#define SC_UTILITY_H

// class for handling timed events
class EventMap
{
protected:
    typedef std::map<uint32 /*time*/, uint32 /*eventId*/> EventContainer;
    EventContainer m_events;

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

// base class that implements event broadcasting using EventMap
class ScriptEventInterface: public ScriptMessageInterface
{
public:
    EventMap Events;

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
