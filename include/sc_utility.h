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

// helper for static initialization of containers
template <typename ContainerType>
class map_initializer
{
protected:
    typedef typename ContainerType::key_type    KeyType;
    typedef typename ContainerType::mapped_type ValueType;

    ContainerType m_map;

public:
    map_initializer(KeyType const& key, ValueType const& val)
    {
        m_map[key] = val;
    }

    map_initializer& operator() (KeyType const& key, ValueType const& val)
    {
        m_map[key] = val;
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

template <typename ContainerType>
void SummonManager::GetAllSummonsWithId(ContainerType& list, uint32 Id) const
{
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (i->GetEntry() == Id)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                list.push_back(pSummon);
}

// get random point inside area delimited by circle
void GetRandomPointInCircle(float& x, float& y, float max_rad, float cx = 0.0f, float cy = 0.0f);

// get correct spell_id for the given difficulty
uint32 GetSpellIdWithDifficulty(uint32, Difficulty);

#endif
