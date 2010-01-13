/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ULDUAR_H
#define DEF_ULDUAR_H

#define HEROIC(N, H) (m_bIsRegularMode ? N : H)
#define GET_CREATURE(C) (m_pInstance ? m_creature->GetMap()->GetCreature(m_pInstance->GetData64(C)) : NULL)

enum
{
    MAX_ENCOUNTER               = 14,

    TYPE_LEVIATHAN              = 0,
    TYPE_IGNIS                  = 1,
    TYPE_RAZORSCALE             = 2,
    TYPE_XT002                  = 3,
    TYPE_ASSEMBLY               = 4,
    TYPE_KOLOGARN               = 5,
    TYPE_AURIAYA                = 6,
    TYPE_MIMIRON                = 7,
    TYPE_HODIR                  = 8,
    TYPE_THORIM                 = 9,
    TYPE_FREYA                  = 10,
    TYPE_VEZAX                  = 11,
    TYPE_YOGGSARON              = 12,
    TYPE_ALGALON                = 13,

    DATA_STEELBREAKER           = 20,
    DATA_MOLGEIM                = 21,
    DATA_BRUNDIR                = 22,
    DATA_LEFT_ARM               = 30,
    DATA_RIGHT_ARM              = 31,
    DATA_SANCTUM_SENTRY         = 40,
    DATA_SANCTUM_SENTRY2        = 41,
    DATA_SANCTUM_SENTRY3        = 42,
    DATA_SANCTUM_SENTRY4        = 43,
    DATA_KOLOGARN_CHEST         = 50,
    DATA_FREYA_CHEST            = 51,
    DATA_HODIR_CHEST            = 52,
    DATA_HODIR_CHEST_HARD       = 53,
    DATA_MIMIRON_CHEST          = 54,
    DATA_THORIM_CHEST           = 55,
    DATA_ALGALON_CHEST          = 56,
    DATA_ASSEMBLY_DOOR          = 60,
    DATA_HODIR_DOOR             = 61,
    DATA_VEZAX_DOOR             = 62,
    DATA_BOSS_ENGAGED           = 70,

    NPC_LEVIATHAN               = 33113,
    NPC_IGNIS                   = 33118,
    NPC_RAZORSCALE              = 33186,
    NPC_XT002                   = 33293,
    NPC_STEELBREAKER            = 32867,
    NPC_MOLGEIM                 = 32927,
    NPC_BRUNDIR                 = 32857,
    NPC_KOLOGARN                = 32930,
    NPC_LEFT_ARM                = 32933,
    NPC_RIGHT_ARM               = 32934,
    NPC_AURIAYA                 = 33515,
    NPC_SANCTUM_SENTRY          = 34014,
    NPC_MIMIRON                 = 33350,
    NPC_HODIR                   = 32845,
    NPC_THORIM                  = 32865,
    NPC_FREYA                   = 32906,
    NPC_VEZAX                   = 33271,
    NPC_YOGGSARON               = 33288,
    NPC_ALGALON                 = 32871,

    GO_KOLOGARN_CHEST           = 195046,
    GO_KOLOGARN_CHEST_H         = 195047,
    GO_FREYA_CHEST              = 194324,
    GO_FREYA_CHEST_H            = 194328,
    GO_HODIR_CHEST              = 194307,
    GO_HODIR_CHEST_H            = 194308,
    GO_HODIR_CHEST_HARD         = 194200,
    GO_HODIR_CHEST_HARD_H       = 194201,
    GO_MIMIRON_CHEST            = 194789,
    GO_MIMIRON_CHEST_H          = 194956,
    GO_THORIM_CHEST             = 194312,
    GO_THORIM_CHEST_H           = 194314,
    GO_ALGALON_CHEST            = 194822,
    GO_ALGALON_CHEST_H          = 194821,
    GO_ASSEMBLY_DOOR            = 194556,
    GO_HODIR_DOOR               = 194441,
    GO_VEZAX_DOOR               = 194750
};

//---------------------------------------------------------

class EventMap : private std::map<uint32, uint32>
{
    private:
        uint32 m_time, m_phase;
    public:
        explicit EventMap() : m_phase(0), m_time(0) {}

        uint32 GetTimer() const { return m_time; }

        void Reset() { clear(); m_time = 0; m_phase = 0; }

        void Update(uint32 time) { m_time += time; }

        void SetPhase(uint32 phase)
        {
            if (phase && phase < 9)
                m_phase = (1 << (phase + 24));
        }

        void ScheduleEvent(uint32 eventId, uint32 time, uint32 gcd = 0, uint32 phase = 0)
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

        void RescheduleEvent(uint32 eventId, uint32 time, uint32 gcd = 0, uint32 phase = 0)
        {
            CancelEvent(eventId);
            ScheduleEvent(eventId, time, gcd, phase);
        }

        void RepeatEvent(uint32 time)
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

        void PopEvent()
        {
            erase(begin());
        }

        uint32 ExecuteEvent()
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

        uint32 GetEvent()
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

        void DelayEvents(uint32 time, uint32 gcd)
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

        void CancelEvent(uint32 eventId)
        {
            for (iterator itr = begin(); itr != end();)
            {
                if (eventId == (itr->second & 0x0000FFFF))
                    erase(itr++);
                else
                    ++itr;
            }
        }

        void CancelEventsByGCD(uint32 gcd)
        {
            for (iterator itr = begin(); itr != end();)
            {
                if (itr->second & gcd)
                    erase(itr++);
                else
                    ++itr;
            }
        }
};

#endif
