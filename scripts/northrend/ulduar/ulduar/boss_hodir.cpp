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
SDName: Boss Hodir
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_BERSERK           = 26662,
    SPELL_BITING_COLD       = 62038,  // needs core support
    SPELL_FREEZE            = 62469,
    SPELL_FLASH_FREEZE      = 61968,
    SPELL_ICICLE            = 62460,
    SPELL_FROZEN_BLOWS      = 62478,
    SPELL_FROZEN_BLOWS_H    = 63512
};

enum Events
{
    EVENT_BERSERK,
    EVENT_FREEZE,
    EVENT_FROZEN_BLOWS
};

enum Says
{
    SAY_AGGRO           = -1300145,
    SAY_FROZEN_BLOWS    = -1300146,
    SAY_FLASH_FREEZE    = -1300147,
    SAY_NO_HARD_MODE    = -1300148,
    SAY_KILLED_PLAYER1  = -1300149,
    SAY_KILLED_PLAYER2  = -1300150,
    SAY_BERSERK         = -1300151,
    SAY_DEFEATED        = -1300152,
    SAY_YOGG_HELP       = -1300153
};

#define BERSERK_TIMER       8*MINUTE*IN_MILISECONDS
#define FREEZE_TIMER        urand(10, 15)*IN_MILISECONDS
#define FROZEN_BLOWS_TIMER  urand(45, 60)*IN_MILISECONDS

struct MANGOS_DLL_DECL boss_hodirAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiHardModeTimer;
    EventMap events;

    boss_hodirAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_HODIR)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        m_uiHardModeTimer = 0;
        events.Reset();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEFEATED, m_creature);
        if (m_pInstance)
        {
            /*
            if (m_uiHardModeTimer < 3*MINUTE*IN_MILISECONDS)
                if (uint64 HardLootGuid = m_pInstance->GetData64(DATA_HODIR_CHEST_HARD))                
                    m_pInstance->DoRespawnGameObject(HardLootGuid, 7*DAY);
            */
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
        }
    }

    void Aggro(Unit* pWho)
    {
        m_uiHardModeTimer = 0;
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_FREEZE, FREEZE_TIMER);
        events.RescheduleEvent(EVENT_FROZEN_BLOWS, FROZEN_BLOWS_TIMER);
        DoCast(m_creature, SPELL_BITING_COLD, true);
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

        m_uiHardModeTimer += uiDiff;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_FREEZE:
                    if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_FREEZE);
                    events.ScheduleEvent(EVENT_FREEZE, FREEZE_TIMER);
                    break;
                case EVENT_FROZEN_BLOWS:
                    DoScriptText(SAY_FROZEN_BLOWS, m_creature);
                    DoCast(m_creature, HEROIC(SPELL_FROZEN_BLOWS, SPELL_FROZEN_BLOWS_H));
                    events.ScheduleEvent(EVENT_FROZEN_BLOWS, FROZEN_BLOWS_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hodir(Creature* pCreature)
{
    return new boss_hodirAI(pCreature);
}

void AddSC_boss_hodir()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_hodir";
    NewScript->GetAI = &GetAI_boss_hodir;
    NewScript->RegisterSelf();
}
