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
SDName: boss_argent_challenge
SD%Complete: 92%
SDComment: missing yells. radiance is "wrong"
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"
#include "TemporarySummon.h"

enum Spells
{
    //eadric
    SPELL_VENGEANCE             = 66889,
    SPELL_RADIANCE              = 66862,
    SPELL_RADIANCE_H            = 67681,
    SPELL_HAMMER_OF_JUSTICE     = 66940,
    SPELL_HAMMER                = 67680,
    //paletress
    SPELL_SMITE                 = 66536,
    SPELL_SMITE_H               = 67674,
    SPELL_HOLY_FIRE             = 66538,
    SPELL_HOLY_FIRE_H           = 67676,
    SPELL_RENEW                 = 66537,
    SPELL_RENEW_H               = 67675,
    SPELL_HOLY_NOVA             = 66546,
    SPELL_SHIELD                = 66515,
    SPELL_CONFESS               = 66547,
    //memory
    SPELL_FEAR                  = 66552,
    SPELL_FEAR_H                = 67677,
    SPELL_SHADOWS               = 66619,
    SPELL_SHADOWS_H             = 67678,
    SPELL_OLD_WOUNDS            = 66620,
    SPELL_OLD_WOUNDS_H          = 67679,
};

static const uint32 MemorySummons[MAX_MEMORY] = {
    MEMORY_ALGALON,
    MEMORY_ARCHIMONDE,
    MEMORY_CHROMAGGUS,
    MEMORY_CYANIGOSA,
    MEMORY_DELRISSA,
    MEMORY_ECK,
    MEMORY_ENTROPIUS,
    MEMORY_GRUUL,
    MEMORY_HAKKAR,
    MEMORY_HEIGAN,
    MEMORY_HEROD,
    MEMORY_HOGGER,
    MEMORY_IGNIS,
    MEMORY_ILLIDAN,
    MEMORY_INGVAR,
    MEMORY_KALITHRESH,
    MEMORY_LUCIFRON,
    MEMORY_MALCHEZAAR,
    MEMORY_MUTANUS,
    MEMORY_ONYXIA,
    MEMORY_THUNDERAAN,
    MEMORY_VANCLEEF,
    MEMORY_VASHJ,
    MEMORY_VEKNILASH,
    MEMORY_VEZAX
};

// Eadric The Pure
struct MANGOS_DLL_DECL boss_eadricAI: public boss_trial_of_the_championAI
{
    uint32 Vengeance_Timer;
    uint32 Radiance_Timer;
    uint32 Hammer_Timer;
    uint32 Hammer_Dmg_Timer;

    uint64 HammerTarget;

    boss_eadricAI(Creature* pCreature):
        boss_trial_of_the_championAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Vengeance_Timer = 1000;
        Radiance_Timer = 15000;
        Hammer_Timer = 40000;
        Hammer_Dmg_Timer = 45000;
        HammerTarget = 0;
    }

    void Aggro(Unit *pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
            m_creature->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

    void JustDied(Unit *pKiller)
    {
        static_cast<TemporarySummon*>(m_creature)->UnSummon();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Vengeance_Timer < diff)
        {
            DoCast(m_creature, SPELL_VENGEANCE);
            Vengeance_Timer = 12000;
        }
        else
            Vengeance_Timer -= diff;  

        if (Radiance_Timer < diff)
        {
            DoCast(m_creature, DIFFICULTY(SPELL_RADIANCE));
            Radiance_Timer = 20000;
        }
        else
            Radiance_Timer -= diff;

        if (Hammer_Timer < diff)
        {
            if (Player *target = SelectRandomPlayer())
            {
                DoCast(target, SPELL_HAMMER_OF_JUSTICE);
                HammerTarget = target->GetGUID();
            }
            Hammer_Timer = 50000;
        }
        else
            Hammer_Timer -= diff;

        if (Hammer_Dmg_Timer < diff)
        {
            if (Unit *pHammerTarget = Unit::GetUnit(*m_creature, HammerTarget))
                DoCast(pHammerTarget, SPELL_HAMMER);
            Hammer_Dmg_Timer = 50000;
        }
        else Hammer_Dmg_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

// Argent Confessor Paletress
struct MANGOS_DLL_DECL boss_paletressAI: public boss_trial_of_the_championAI
{
    uint32 Smite_Timer;
    uint32 Holy_Fire_Timer;
    uint32 Renew_Timer;
    uint32 Shield_Delay;
    uint32 Shield_Check;

    bool summoned;
    bool shielded;

    boss_paletressAI(Creature* pCreature):
        boss_trial_of_the_championAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Smite_Timer = 5000;
        Holy_Fire_Timer = 10000;
        Renew_Timer = 7000;
        Shield_Delay = 0;
        Shield_Check = 1000;
        summoned = false;
        shielded = false;
        m_creature->RemoveAurasDueToSpell(SPELL_SHIELD);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
            m_creature->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        static_cast<TemporarySummon*>(m_creature)->UnSummon();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Smite_Timer < diff)
        {
            if (Player* target = SelectRandomPlayer())
                DoCast(target, DIFFICULTY(SPELL_SMITE));
            Smite_Timer = 2000;
        }
        else
            Smite_Timer -= diff;  

        if (Holy_Fire_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            if (Player *target = SelectRandomPlayer())
                DoCast(target, DIFFICULTY(SPELL_HOLY_FIRE));
            Holy_Fire_Timer = 10000;
        }
        else
            Holy_Fire_Timer -= diff;

        if (Renew_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(true);
            switch(urand(0, 1))
            {
                case 0:
                    if (Creature *pTemp = GET_CREATURE(DATA_MEMORY))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, DIFFICULTY(SPELL_RENEW));
                        else
                            DoCast(m_creature, DIFFICULTY(SPELL_RENEW));
                    }
                    break;
                case 1:
                    DoCast(m_creature, DIFFICULTY(SPELL_RENEW));
                    break;
            }
            Renew_Timer = 25000;
        }
        else
            Renew_Timer -= diff;

        if (!summoned && m_creature->GetHealthPercent() < 35.0f)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, SPELL_HOLY_NOVA);
            if (Creature *pSummon = DoSpawnCreature(MemorySummons[urand(0, MAX_MEMORY-1)], 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000))
            {
                pSummon->AddThreat(m_creature->getVictim());
                if (pSummon->AI())
                    pSummon->AI()->AttackStart(m_creature->getVictim());
                summoned = true;
            }
            Shield_Delay = 1000;
        }

        if (Shield_Delay < diff && !shielded && summoned)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, SPELL_SHIELD);
            shielded = true;
            Shield_Check = 1000;
        }
        else
            Shield_Delay -= diff;

        if (Shield_Check < diff && shielded)
        {
            if (Creature *pTemp = GET_CREATURE(DATA_MEMORY))
                if (!pTemp->isAlive())
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_SHIELD);
                    shielded = false;
                }
                else
                    Shield_Check = 1000;
        }
        else
            Shield_Check -= diff;

        DoMeleeAttackIfReady();
    }
};

// Summoned Memory
struct MANGOS_DLL_DECL mob_toc5_memoryAI: public boss_trial_of_the_championAI
{
    uint32 Old_Wounds_Timer;
    uint32 Shadows_Timer;
    uint32 Fear_Timer;

    mob_toc5_memoryAI(Creature* pCreature):
        boss_trial_of_the_championAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Old_Wounds_Timer = 5000;
        Shadows_Timer = 8000;
        Fear_Timer = 13000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Old_Wounds_Timer < diff)
        {
            if (Player* target = SelectRandomPlayer())
                DoCast(target, DIFFICULTY(SPELL_OLD_WOUNDS));
            Old_Wounds_Timer = 10000;
        }
        else
            Old_Wounds_Timer -= diff;  

        if (Fear_Timer < diff)
        {
            DoCast(m_creature, DIFFICULTY(SPELL_FEAR));
            Fear_Timer = 40000;
        }
        else
            Fear_Timer -= diff; 

        if (Shadows_Timer < diff)
        {
            if (Player* target = SelectRandomPlayer())
                DoCast(target, DIFFICULTY(SPELL_SHADOWS));
            Shadows_Timer = 10000;
        }
        else
            Shadows_Timer -= diff; 

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_argent_challenge()
{
    Script *NewScript;

    REGISTER_SCRIPT(boss_eadric);
    REGISTER_SCRIPT(boss_paletress);
    REGISTER_SCRIPT(mob_toc5_memory);
}
