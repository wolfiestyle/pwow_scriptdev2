/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss Freya
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_BERSERK           = 47008,
    SPELL_SUNBEAM           = 62623,
    SPELL_SUNBEAM_H         = 62872,
    SPELL_ATTUNED_TO_NATURE = 62519,
    SPELL_TOUCH_OF_EONAR    = 62528,
    SPELL_TOUCH_OF_EONAR_H  = 62892
};

enum Events
{
    EVENT_BERSERK,
    EVENT_SUNBEAM
};

enum Says
{
    // Auriaya
    SAY_AGGRO                       = -1300216,
    SAY_AGGRO_WITH_ELDERS           = -1300105,
    SAY_SUMMON_CONSERVATOR          = -1300106,
    SAY_SUMMON_TRIO                 = -1300107,
    SAY_SUMMON_LASHERS              = -1300108,
    SAY_KILLED_PLAYER1              = -1300109,
    SAY_KILLED_PLAYER2              = -1300110,
    SAY_BERSERK                     = -1300111,
    SAY_DEFEATED                    = -1300112,
    SAY_YOGG_HELP                   = -1300113,

    // Brightleaf
    SAY_BRIGHTLEAF_AGGRO            = -1300114,
    SAY_BRIGHTLEAF_KILLED_PLAYER1   = -1300115,
    SAY_BRIGHTLEAF_KILLED_PLAYER2   = -1300116,
    SAY_BRIGHTLEAF_DEATH            = -1300117,
    
    // Stonebark
    SAY_STONE_AGGRO                 = -1300118,
    SAY_STONE_KILLED_PLAYER1        = -1300119,
    SAY_STONE_KILLED_PLAYER2        = -1300120,
    SAY_STONE_DEATH                 = -1300121,
    
    // Ironbranch
    SAY_IRON_AGGRO                  = -1300122,
    SAY_IRON_KILLED_PLAYER1         = -1300123,
    SAY_IRON_KILLED_PLAYER2         = -1300124,
    SAY_IRON_DEATH                  = -1300125
};

#define BERSERK_TIMER   10*MINUTE*IN_MILLISECONDS
#define SUNBEAM_TIMER   urand(10, 15)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_freyaAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;

    boss_freyaAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_FREYA)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEFEATED, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_SUNBEAM, SUNBEAM_TIMER);
        DoCast(m_creature, SPELL_ATTUNED_TO_NATURE, true);
        DoCast(m_creature, HEROIC(SPELL_TOUCH_OF_EONAR, SPELL_TOUCH_OF_EONAR_H), true);
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_SUNBEAM:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, HEROIC(SPELL_SUNBEAM, SPELL_SUNBEAM_H));
                    events.ScheduleEvent(EVENT_SUNBEAM, SUNBEAM_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

void AddSC_boss_freya()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_freya";
    NewScript->GetAI = &GetAI_boss_freya;
    NewScript->RegisterSelf();
}
