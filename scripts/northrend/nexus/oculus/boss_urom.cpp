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
SDName: Mage-Lord Urom
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"

#define SPELL_FROST_BOMB              51103
#define SPELL_FROST_BOMB_H            51103
#define SPELL_TIME_BOMB               51121
#define SPELL_TIME_BOMB_H             59376
#define SPELL_ARCANE_EXPLOSION        51110
#define SPELL_ARCANE_EXPLOSION_H      59377
#define SPELL_TELEPORT                51112

struct MANGOS_DLL_DECL boss_uromAI : public ScriptedAI
{
    boss_uromAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 FrostBomb_Timer;
    uint32 TimeBomb_Timer;
    uint32 ArcaneExplosion_Timer;
    uint32 Teleport_Timer;

    void Reset() 
    {
        FrostBomb_Timer = 15000;
        TimeBomb_Timer = 30000;
        Teleport_Timer = 40000;
        ArcaneExplosion_Timer = 41500;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Teleport timer
        if (Teleport_Timer < diff)
        {
            DoCast(m_creature,SPELL_TELEPORT);

            Teleport_Timer = 40000;
            ArcaneExplosion_Timer = 1500;
        }else Teleport_Timer -= diff;

        //Arcane Explosion timer
        if (ArcaneExplosion_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION_H);

            ArcaneExplosion_Timer = 41500;
        }else ArcaneExplosion_Timer -= diff;

        //Time Bomb timer
        if (TimeBomb_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_TIME_BOMB : SPELL_TIME_BOMB_H);

            TimeBomb_Timer = 30000;
        }else TimeBomb_Timer -= diff;

        //Frost Bomb timer
        if (FrostBomb_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_BOMB : SPELL_FROST_BOMB_H);

            FrostBomb_Timer = 15000;
        }else FrostBomb_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_urom(Creature* pCreature)
{
    return new boss_uromAI(pCreature);
}

void AddSC_boss_urom()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_urom";
    newscript->GetAI = &GetAI_boss_urom;
    newscript->RegisterSelf();
}
