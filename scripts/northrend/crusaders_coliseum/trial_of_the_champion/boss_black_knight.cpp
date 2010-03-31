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
SDName: boss_black_knight
SD%Complete: 92%
SDComment: missing yells. not sure about timers.
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

enum
{
    //undead
    SPELL_PLAGUE_STRIKE         = 67724,
    SPELL_PLAGUE_STRIKE_H       = 67884,
    SPELL_ICY_TOUCH             = 67718,
    SPELL_ICY_TOUCH_H           = 67881,
    SPELL_OBLITERATE            = 67725,
    SPELL_OBLITERATE_H          = 67883,
    SPELL_CHOKE                 = 68306,
    //skeleton
    SPELL_ARMY                  = 42650,            //replacing original one, since that one spawns millions of ghouls!!
    //ghost
    SPELL_DEATH                 = 67808,
    SPELL_DEATH_H               = 67875,
    SPELL_MARK                  = 67823,

    //risen ghoul
    SPELL_CLAW                  = 67879,
    SPELL_EXPLODE               = 67729,
    SPELL_EXPLODE_H             = 67886,
    SPELL_LEAP                  = 67749,
    SPELL_LEAP_H                = 67880,

    //sword ID
    EQUIP_SWORD                 = 40343
};

// Risen Ghoul
struct MANGOS_DLL_DECL mob_toc5_risen_ghoulAI: public boss_trial_of_the_championAI
{
    uint32 Attack_Timer;

    mob_toc5_risen_ghoulAI(Creature* pCreature):
        boss_trial_of_the_championAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Attack_Timer = 2500;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Attack_Timer < diff)
        {
            if (Creature *pTemp = GET_CREATURE(DATA_BLACK_KNIGHT))
                if (pTemp->isAlive() && pTemp->GetHealthPercent() < 25.0f)
                {
                    DoCast(m_creature, DIFFICULTY(SPELL_EXPLODE));
                    return;
                }

            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 4.0f))
            {
                DoCast(m_creature->getVictim(), SPELL_CLAW);
                if (Player *target = SelectRandomPlayer())
                    AttackStart(target);
                Attack_Timer = 2500;
            }
            else if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30.0f))
            {
                DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_LEAP));
                Attack_Timer = 2500;
            }
        }
        else
            Attack_Timer -= diff;

        if (m_creature->GetHealthPercent() < 25.0f)
            DoCast(m_creature, DIFFICULTY(SPELL_EXPLODE));

        DoMeleeAttackIfReady();
    }
};

// The Black Knight
struct MANGOS_DLL_DECL boss_black_knightAI: public boss_trial_of_the_championAI
{
    uint32 Plague_Strike_Timer;
    uint32 Icy_Touch_Timer;
    uint32 Obliterate_Timer;
    uint32 Choke_Timer;
    uint32 Death_Timer;
    uint32 Mark_Timer;
    uint32 Phase_Delay;
    uint32 Summon_Ghoul;

    bool phase1;
    bool phase2;
    bool phase3;
    bool ghoul;

    boss_black_knightAI(Creature* pCreature):
        boss_trial_of_the_championAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Plague_Strike_Timer = 5000;
        Icy_Touch_Timer = 10000;
        Obliterate_Timer = 16000;
        Choke_Timer = 15000;
        Summon_Ghoul = 4000;
        phase1 = true;      //FIXME: use one numeric value instead
        phase2 = false;
        phase3 = false;
        ghoul = false;
        m_creature->SetDisplayId(29837);
        SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_BLACK_KNIGHT) == DONE)
            m_creature->ForcedDespawn();
        else
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, IN_PROGRESS);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && !phase3)
        {
            uiDamage = 0;
            if (phase2)
                StartPhase3();
            if (phase1)
                StartPhase2();
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
        if (phase3)
        {
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, DONE);
        }
        if (phase2 && !m_creature->isAlive())
        {
            m_creature->Respawn();
            StartPhase3();
        }
        if (phase1 && !m_creature->isAlive())
        {
            m_creature->Respawn();
            StartPhase2();
        }
    }

    void StartPhase2()
    {
        phase1 = false;
        phase2 = true;
        phase3 = false;
        Plague_Strike_Timer = 14000;
        Icy_Touch_Timer = 12000;
        Obliterate_Timer = 18000;
        m_creature->SetHealthPercent(100.0f);
        m_creature->SetDisplayId(27550);
        DoCast(m_creature, SPELL_ARMY);
    }

    void StartPhase3()
    {
        phase1 = false;
        phase2 = false;
        phase3 = true;
        Death_Timer = 5000;
        Mark_Timer = 9000;
        m_creature->SetHealthPercent(100.0f);
        m_creature->SetDisplayId(14560);
        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Plague_Strike_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_PLAGUE_STRIKE));
            Plague_Strike_Timer = 10500;
        }
        else
            Plague_Strike_Timer -= diff;  

        if (Icy_Touch_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_ICY_TOUCH));
            Icy_Touch_Timer = 10000;
        }
        else
            Icy_Touch_Timer -= diff;

        if (Obliterate_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_OBLITERATE));
            Obliterate_Timer = 11000;
        }
        else
            Obliterate_Timer -= diff;

        if (Choke_Timer < diff && phase1)
        {
            if (Player *target = SelectRandomPlayer())
                DoCast(m_creature->getVictim(), SPELL_CHOKE);
            Choke_Timer = 15000;
        }
        else
            Choke_Timer -= diff;

        if (Summon_Ghoul < diff && phase1 && !ghoul)
        {
            if (uint32 minion_id = m_pInstance ? m_pInstance->GetData(DATA_BLACK_KNIGHT_MINION) : 0)
                DoSpawnCreature(minion_id, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
            ghoul = true;
        }
        else
            Summon_Ghoul -= diff;

        if (Mark_Timer < diff && phase3)
        {
            if (Player *target = SelectRandomPlayer())
                DoCast(target, SPELL_MARK);
            Mark_Timer = 15000;
        }
        else
            Mark_Timer -= diff;

        if (Death_Timer < diff && phase3)
        {
            DoCast(m_creature, DIFFICULTY(SPELL_DEATH));
            Death_Timer = 3500;
        }
        else
            Death_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_black_knight()
{
    Script *NewScript;

    REGISTER_SCRIPT(mob_toc5_risen_ghoul);
    REGISTER_SCRIPT(boss_black_knight);
}
