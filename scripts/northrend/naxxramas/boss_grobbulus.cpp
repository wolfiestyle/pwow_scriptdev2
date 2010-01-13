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
SDName: Boss_Grobbulus
SD%Complete: 0
SDComment: Place holder
SDCategory: Naxxramas
EndScriptData */

/*Poison Cloud 26590
Slime Spray 28157
Fallout slime 28218
Mutating Injection 28169
Enrages 26527*/

#include "precompiled.h"
#include "naxxramas.h"

#define SPELL_POISONCLOUD         28240
#define SPELL_SLIMESPRAY          28157
#define SPELL_SLIMESPRAY_H        54364
#define SPELL_MUTATINGINJECTION   28169
#define SPELL_BERSERK             26662

struct MANGOS_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 PoisonCloud_Timer;                               //Timer for spell Poison Cloud when in combat
    uint32 SlimeSpray_Timer;                                //Timer for spell Slime Spray when in combat
    uint32 MutatingInjection_Timer;                         //Timer for spell Mutating Injection when in combat
    uint32 Berserk_Timer;                                   //Timer until we go into Beserk (enraged) mode

    void Reset()
    {
        PoisonCloud_Timer = 15000;                          //15 seconds
        SlimeSpray_Timer = 40000;                           //40 seconds
        MutatingInjection_Timer = 20000;                    //20 seconds
        Berserk_Timer = 720000;                             //12 minutes

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, DONE);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, IN_PROGRESS);
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
		if(spell->Id == (m_bIsRegularMode ? SPELL_SLIMESPRAY : SPELL_SLIMESPRAY_H))
            (m_creature->SummonCreature(16290, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))->TauntApply(target);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Poison Cloud timer
        if (PoisonCloud_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature,SPELL_POISONCLOUD);

            PoisonCloud_Timer = 15000;
        }else PoisonCloud_Timer -= diff;

        //Spell Slime Spray timer
        if (SlimeSpray_Timer < diff)
        {
            //Cast spell one on our current target.
			DoCast(m_creature, m_bIsRegularMode ? SPELL_SLIMESPRAY : SPELL_SLIMESPRAY_H);

            SlimeSpray_Timer = 40000;
        }else SlimeSpray_Timer -= diff;

        //Spell Mutating Injection timer
        if (MutatingInjection_Timer < diff)
        {
            //Cast spell one on a random target.
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_MUTATINGINJECTION);

            MutatingInjection_Timer = 10000 + (int) (m_creature->GetHealth()*10000 / m_creature->GetMaxHealth());    //10-20 seconds
        }else MutatingInjection_Timer -= diff;

        //Berserk timer
        if (Berserk_Timer < diff)
        {
            //Cast spell on self.
            DoCast(m_creature,SPELL_BERSERK);

            //Cast our beserk spell agian in 12 seconds if we didn't kill everyone
            Berserk_Timer = 12000;
        }else Berserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_grobbulus(Creature* pCreature)
{
    return new boss_grobbulusAI(pCreature);
}

void AddSC_boss_grobbulus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_grobbulus";
    newscript->GetAI = &GetAI_boss_grobbulus;
    newscript->RegisterSelf();
}
