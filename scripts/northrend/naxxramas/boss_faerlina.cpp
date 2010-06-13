/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Faerlina
SD%Complete: 50
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_GREET                 = -1533009,
    SAY_AGGRO1                = -1533010,
    SAY_AGGRO2                = -1533011,
    SAY_AGGRO3                = -1533012,
    SAY_AGGRO4                = -1533013,
    SAY_SLAY1                 = -1533014,
    SAY_SLAY2                 = -1533015,
    SAY_DEATH                 = -1533016,

    //SOUND_RANDOM_AGGRO        = 8955,                              //soundId containing the 4 aggro sounds, we not using this

    SPELL_POSIONBOLT_VOLLEY   = 28796,
    H_SPELL_POSIONBOLT_VOLLEY = 54098,
    SPELL_ENRAGE              = 28798,
    H_SPELL_ENRAGE            = 54100,

    SPELL_RAINOFFIRE          = 28794,                      //Not sure if targeted AoEs work if casted directly upon a pPlayer
    H_SPELL_RAINOFFIRE        = 58936,

    SPELL_WIDOWSEMBRACE       = 28732,

    SPELL_FIREBALL            = 54095,
    SPELL_FIREBALL_H          = 54096
};

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bHasTaunted = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPoisonBoltVolleyTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiEnrageTimer;
    bool   m_bHasTaunted;

    void Reset()
    {
        m_uiPoisonBoltVolleyTimer = 12000+rand()%3000;
        m_uiRainOfFireTimer = 6000+rand()%12000;
        m_uiEnrageTimer = 60000+rand()%20000;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            case 3: DoScriptText(SAY_AGGRO4, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasTaunted && m_creature->IsWithinDistInMap(pWho, 60.0f))
        {
            DoScriptText(SAY_GREET, m_creature);
            m_bHasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1)?SAY_SLAY1:SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FAERLINA, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Sacrifice part
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
            DoCast(m_creature,SPELL_WIDOWSEMBRACE);
        }

        if (m_creature->HasAura(SPELL_WIDOWSEMBRACE) && m_creature->HasAura(SPELL_ENRAGE))
            m_creature->RemoveAurasDueToSpell(SPELL_ENRAGE);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Poison Bolt Volley
        if (!m_creature->HasAura(SPELL_WIDOWSEMBRACE))
        {
            if (m_uiPoisonBoltVolleyTimer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POSIONBOLT_VOLLEY : H_SPELL_POSIONBOLT_VOLLEY);
                m_uiPoisonBoltVolleyTimer = 12000+rand()%3000;
            }
            else
                m_uiPoisonBoltVolleyTimer -= uiDiff;
        }

        // Rain Of Fire
        if (m_uiRainOfFireTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_RAINOFFIRE : H_SPELL_RAINOFFIRE);

            m_uiRainOfFireTimer = 6000+rand()%12000;
        }
        else
            m_uiRainOfFireTimer -= uiDiff;

        //Enrage_Timer
        if (!m_creature->HasAura(SPELL_WIDOWSEMBRACE))
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ENRAGE : H_SPELL_ENRAGE);
                m_uiEnrageTimer = 60000+rand()%20000;
            }
            else 
                m_uiEnrageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

//Naxxramas Worshipper
struct MANGOS_DLL_DECL mob_naxxramasworshipperAI : public ScriptedAI
{
    mob_naxxramasworshipperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFireball_Timer;
    uint64 FaerlinaGUID;

    void Reset()
    {
        FaerlinaGUID = 0;
        m_uiFireball_Timer = 7000 + rand()%4000;
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            if (FaerlinaGUID = m_pInstance->GetData64(NPC_FAERLINA))
                if (Creature* Faerlina = ((Creature*)Unit::GetUnit((*m_creature), FaerlinaGUID)))
                    Faerlina->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiFireball_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H);
            m_uiFireball_Timer = 7000 + rand()%4000;
        }
        else m_uiFireball_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_faerlina(Creature* pCreature)
{
    return new boss_faerlinaAI(pCreature);
}

CreatureAI* GetAI_mob_naxxramasworshipper(Creature* pCreature)
{
    return new mob_naxxramasworshipperAI(pCreature);
}

void AddSC_boss_faerlina()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_faerlina";
    NewScript->GetAI = &GetAI_boss_faerlina;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_naxxramasworshipper";
    NewScript->GetAI = &GetAI_mob_naxxramasworshipper;
    NewScript->RegisterSelf();
}
