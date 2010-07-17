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
SDName: Boss Thorim
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_CHAIN_LIGHTNING       = 62131,
    SPELL_CHAIN_LIGHTNING_H     = 64390,
    SPELL_UNBALANCING_STRIKE    = 62130,
    SPELL_LIGHTNING_CHARGE_BUFF = 62279,
    SPELL_LIGHTNING_CHARGE_DMG  = 62466     //incorrect one, its a more complex sequence
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_CHAIN_LIGHTNING,
    EVENT_UNBALANCING_STRIKE,
    EVENT_LIGHTNING_CHARGE
};

enum Says
{
    SAY_AGGRO1          = -1300126,
    SAY_AGGRO2          = -1300127,
    SAY_SPECIAL1        = -1300128,
    SAY_SPECIAL2        = -1300129,
    SAY_SPECIAL3        = -1300130,
    SAY_JUMP_DOWN       = -1300131,
    SAY_KILLED_PLAYER1  = -1300132,
    SAY_KILLED_PLAYER2  = -1300133,
    SAY_WIPE_ARENA      = -1300134,
    SAY_BERSERK         = -1300135,
    SAY_DEFEATED        = -1300136,
    SAY_OUTRO_N1        = -1300137,
    SAY_OUTRO_N2        = -1300138,
    SAY_OUTRO_N3        = -1300139,
    SAY_OUTRO_H1        = -1300141,
    SAY_OUTRO_H2        = -1300142,
    SAY_OUTRO_H3        = -1300143,
    SAY_YOGG_HELP       = -1300144
};

#define BERSERK_TIMER               10*MINUTE*IN_MILLISECONDS
#define CHAIN_LIGHTNING_TIMER       urand(15, 20)*IN_MILLISECONDS
#define UNBALANCING_STRIKE_TIMER    urand(20, 30)*IN_MILLISECONDS
#define LIGHTNING_CHARGE_TIMER      urand(45, 60)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_thorimAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    boss_thorimAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_THORIM)
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
        events.RescheduleEvent(EVENT_CHAIN_LIGHTNING, CHAIN_LIGHTNING_TIMER);
        events.RescheduleEvent(EVENT_UNBALANCING_STRIKE, UNBALANCING_STRIKE_TIMER);
        events.RescheduleEvent(EVENT_LIGHTNING_CHARGE, LIGHTNING_CHARGE_TIMER);
        DoScriptText(urand(0,1) ? SAY_AGGRO1 : SAY_AGGRO2, m_creature);
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
                case EVENT_CHAIN_LIGHTNING:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_CHAIN_LIGHTNING, SPELL_CHAIN_LIGHTNING_H));
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, CHAIN_LIGHTNING_TIMER);
                    break;
                case EVENT_UNBALANCING_STRIKE:
                    DoCast(m_creature->getVictim(), SPELL_UNBALANCING_STRIKE);
                    events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, UNBALANCING_STRIKE_TIMER);
                    break;
                case EVENT_LIGHTNING_CHARGE:
                    //DoCast(m_creature, SPELL_LIGHTNING_CHARGE_BUFF, true);
                    //DoCast(m_creature->getVictim(), SPELL_LIGHTNING_CHARGE_DMG);
                    events.ScheduleEvent(EVENT_LIGHTNING_CHARGE, LIGHTNING_CHARGE_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thorim(Creature* pCreature)
{
    return new boss_thorimAI(pCreature);
}

void AddSC_boss_thorim()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_thorim";
    NewScript->GetAI = &GetAI_boss_thorim;
    NewScript->RegisterSelf();
}
