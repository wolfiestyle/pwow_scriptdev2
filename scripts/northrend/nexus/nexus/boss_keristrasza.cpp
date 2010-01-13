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
SDName: Boss_Keristrasza
SD%Complete: 10%
SDComment:
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                   = -1576016,
    SAY_CRYSTAL_NOVA            = -1576017,
    SAY_ENRAGE                  = -1576018,
    SAY_KILL                    = -1576019,
    SAY_DEATH                   = -1576020,

    SPELL_CRYSTALFIRE_BREATH    = 48096,
    SPELL_CRYSTALFIRE_BREATH_H  = 57091,

    SPELL_CRYSTALLIZE           = 48179,

    SPELL_CRYSTAL_CHAINS        = 50997,
    SPELL_CRYSTAL_CHAINS_H      = 57050,

    SPELL_TAIL_SWEEP            = 50155,
    SPELL_INTENSE_COLD          = 48094,

    SPELL_ENRAGE                = 8599
};

/*######
## boss_keristrasza
######*/

struct MANGOS_DLL_DECL boss_keristraszaAI : public ScriptedAI
{
    boss_keristraszaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Unlocked = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool Unlocked;

    uint32 IntenseCold_Timer;
    uint32 CrystalChains_Timer;
    uint32 CrystalfireBreath_Timer;
    uint32 TailSweep_Timer;
    uint32 Crystallize_Timer;
    bool Enraged;

    uint64 TelestraGUID;
    uint64 AnomalusGUID;
    uint64 OrmorokGUID;

    void Reset()
    {
        IntenseCold_Timer = 1000;           //1 second
        CrystalChains_Timer = 8000;         //8 seconds
        CrystalfireBreath_Timer = 15000;    //15 seconds
        TailSweep_Timer = 30000;            //30 seconds
        Crystallize_Timer = 40000;          //40 seconds
        Enraged = false;

        TelestraGUID = 0;
        AnomalusGUID = 0;
        OrmorokGUID = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance)
        {
            if (!TelestraGUID)
                TelestraGUID = m_pInstance->GetData64(NPC_TELESTRA);
            if (!AnomalusGUID)
                AnomalusGUID = m_pInstance->GetData64(NPC_ANOMALUS);
            if (!OrmorokGUID)
                OrmorokGUID = m_pInstance->GetData64(NPC_ORMOROK);
        }

        //Unlock if all 3 bosses are dead
        if (!Unlocked && TelestraGUID && AnomalusGUID && OrmorokGUID)
        {
            Creature* Telestra = ((Creature*)Unit::GetUnit((*m_creature), TelestraGUID));
            Creature* Anomalus = ((Creature*)Unit::GetUnit((*m_creature), AnomalusGUID));
            Creature* Ormorok = ((Creature*)Unit::GetUnit((*m_creature), OrmorokGUID));
            if (Telestra && Anomalus && Ormorok && Telestra->isDead() && Anomalus->isDead() && Ormorok->isDead())
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_9);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6);
                Unlocked = true;
            }
        }

        //Return since boss is locked
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE))
            return;

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Enrage at 25% HP
        if ((int) (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() +0.5) <= 25 && !Enraged)
        {
            DoScriptText(SAY_ENRAGE, m_creature);
            DoCast(m_creature,SPELL_ENRAGE);
            Enraged = true;
        }

        //Crystallize timer
        if (!m_bIsRegularMode)
        {
            if (Crystallize_Timer < uiDiff)
            {
                DoScriptText(SAY_CRYSTAL_NOVA, m_creature);
                DoCast(m_creature,SPELL_CRYSTALLIZE);

                Crystallize_Timer = 40000;
            }else Crystallize_Timer -= uiDiff;
        }

        //Tail Sweep timer
        if (TailSweep_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_TAIL_SWEEP);

            TailSweep_Timer = 30000;
        }else TailSweep_Timer -= uiDiff;

        //Crystalfire Breath timer
        if (CrystalfireBreath_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CRYSTALFIRE_BREATH : SPELL_CRYSTALFIRE_BREATH_H);

            CrystalfireBreath_Timer = 15000;
        }else CrystalfireBreath_Timer -= uiDiff;

        //Crystal Chains timer
        if (CrystalChains_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_CRYSTAL_CHAINS : SPELL_CRYSTAL_CHAINS_H);

            CrystalChains_Timer = 8000;
        }else CrystalChains_Timer -= uiDiff;
/*
        //Intense Cold timer
        if (IntenseCold_Timer < uiDiff)
        {
            DoCast(m_creature,SPELL_INTENSE_COLD);

            IntenseCold_Timer = 1000;
        }else IntenseCold_Timer -= uiDiff;
*/
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_keristrasza(Creature* pCreature)
{
    return new boss_keristraszaAI(pCreature);
}

void AddSC_boss_keristrasza()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_keristrasza";
    newscript->GetAI = &GetAI_boss_keristrasza;
    newscript->RegisterSelf();
}
