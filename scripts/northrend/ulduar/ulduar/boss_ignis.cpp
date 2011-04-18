/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss Ignis the Furnace Master
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1603026,
    SAY_SCORCH_1                        = -1603027,
    SAY_SCORCH_2                        = -1603028,
    SAY_SLAGPOT                         = -1603029,
    SAY_ADDS                            = -1603030,
    SAY_SLAY_1                          = -1603031,
    SAY_SLAY_2                          = -1603032,
    SAY_BERSERK                         = -1603033,
    SAY_DEATH                           = -1603034,

    EMOTE_FLAME_JETS                    = -1603035,
};
*/

enum Spells
{
    SPELL_FLAME_JETS        = 62680,
    SPELL_FLAME_JETS_H      = 63472,
    SPELL_SCORCH            = 62546,
    SPELL_SCORCH_H          = 63473,
    SPELL_SLAG_POT          = 62717,
    SPELL_SLAG_POT_H        = 63477,
};

enum Says
{
    SAY_AGGRO           = -1300023,
    SAY_SCORCH1         = -1300024,
    SAY_SCORCH2         = -1300025,
    SAY_SLAG_POT        = -1300026,
    SAY_SUMMON_ADDS     = -1300027,
    SAY_KILLED_PLAYER1  = -1300028,
    SAY_KILLED_PLAYER2  = -1300029,
    SAY_BERSERK         = -1300030,
    SAY_DEATH           = -1300031
};

struct MANGOS_DLL_DECL boss_ignisAI : public ScriptedAI
{
    boss_ignisAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_IGNIS)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 const m_uiBossEncounterId;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 FlameJets_Timer;
    uint32 Scorch_Timer;
    uint32 SlagPot_Timer;

    void Reset() 
    {
        FlameJets_Timer = 20000;
        Scorch_Timer = 15000;
        SlagPot_Timer = 30000;

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
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

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        //Slag Pot timer
        if (SlagPot_Timer < diff)
        {
            DoScriptText(SAY_SLAG_POT, m_creature);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target, HEROIC(SPELL_SLAG_POT, SPELL_SLAG_POT_H));

            SlagPot_Timer = 30000;
        }else SlagPot_Timer -= diff;

        //Scorch timer
        if (Scorch_Timer < diff)
        {
            DoScriptText(urand(0,1) ? SAY_SCORCH1 : SAY_SCORCH2, m_creature);
            DoCast(m_creature, HEROIC(SPELL_SCORCH, SPELL_SCORCH_H));

            Scorch_Timer = 15000;
        }else Scorch_Timer -= diff;

        //Flames Jet timer
        if (FlameJets_Timer < diff)
        {
            DoCast(m_creature, HEROIC(SPELL_FLAME_JETS, SPELL_FLAME_JETS_H));

            FlameJets_Timer = 20000;
        }else FlameJets_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignisAI(pCreature);
}

void AddSC_boss_ignis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ignis";
    newscript->GetAI = &GetAI_boss_ignis;
    newscript->RegisterSelf();
}
