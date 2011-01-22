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
SDName: Boss Razorscale
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_INTRO_WELCOME                   = -1603036,
    SAY_INTRO_1                         = -1603037,
    SAY_INTRO_2                         = -1603038,
    SAY_INTRO_3                         = -1603039,
    SAY_GROUNDED                        = -1603040,
    SAY_EXTINGUISH_FIRE                 = -1603041,

    EMOTE_BREATH                        = -1603042,
    EMOTE_HARPOON_READY                 = -1603043,
    EMOTE_GROUNDED                      = -1603044,
};
*/

enum Spells
{
    SPELL_BERSERK           = 47008,
    SPELL_FIREBALL          = 62796,
    SPELL_FIREBALL_H        = 63815,
    SPELL_WING_BUFFET       = 62666,
    SPELL_FLAME_BREATH      = 63317,
    SPELL_FLAME_BREATH_H    = 64021,
    SPELL_FLAME_BUFFET      = 64016,
    SPELL_FLAME_BUFFET_H    = 64023,
    SPELL_DEVOURING_FLAME   = 64704,
    SPELL_DEVOURING_FLAME_H = 64733,
    SPELL_FUSE_ARMOR        = 64771
};

enum Says
{
    SAY_INTRO1              = -1300032, // commanders
    SAY_AGGRO1              = -1300033, // engineer
    SAY_AGGRO2              = -1300034, // commander
    SAY_AGGRO3              = -1300035, // engineer
    SAY_GROUND_PHASE        = -1300036, // commander

    EMOTE_FLAME_BREATH      = -1300037,
    SAY_FIRE_EXTINGUISHED   = -1300038
};

#define BERKSERK_TIMER          10*MINUTE*IN_MILLISECONDS
#define FIREBALL_TIMER          15*IN_MILLISECONDS
#define WING_BUFFET_TIMER       40*IN_MILLISECONDS
#define FLAME_BREATH_TIMER      (Phase==1 ? 30 : 40)*IN_MILLISECONDS
#define FLAME_BUFFET_TIMER      8*IN_MILLISECONDS
#define DEVOURING_FAME_TIMER    (Phase==1 ? 8 : 15)*IN_MILLISECONDS
#define FUSE_ARMOR_TIMER        30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_razorscaleAI : public ScriptedAI
{
    boss_razorscaleAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_RAZORSCALE)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 const m_uiBossEncounterId;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Phase;
    uint32 DevouringFlame_Timer;
    uint32 Fireball_Timer;
    uint32 FlameBreath_Timer;
    uint32 WingBuffet_Timer;
    uint32 FlameBuffet_Timer;
    uint32 FuseArmor_Timer;
    uint32 Berserk_Timer;

    void Reset() 
    {
        Phase = 1;
        DevouringFlame_Timer = DEVOURING_FAME_TIMER;
        Fireball_Timer = FIREBALL_TIMER;
        FlameBreath_Timer = FLAME_BREATH_TIMER;
        WingBuffet_Timer = WING_BUFFET_TIMER;
        FlameBuffet_Timer = FLAME_BUFFET_TIMER;
        FuseArmor_Timer = FUSE_ARMOR_TIMER;
        Berserk_Timer = BERKSERK_TIMER;

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        //m_creature->GetMotionMaster()->Clear(false);
        //m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetHover(true);

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
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

        //Berserk timer
        if (Berserk_Timer < diff)
        {
            DoCast(m_creature,SPELL_BERSERK);
            Berserk_Timer = BERKSERK_TIMER;
        }
        else
            Berserk_Timer -= diff;

        //Phase 1
        if (Phase == 1)
        {
            if (m_creature->GetHealthPercent() <= 50.0f)
            {
                m_creature->SetHover(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                Phase = 2;
                return;
            }

            //Wing Buffet timer
            if (WingBuffet_Timer < diff)
            {
                DoCast(m_creature, SPELL_WING_BUFFET);
                WingBuffet_Timer = WING_BUFFET_TIMER;
            }
            else
                WingBuffet_Timer -= diff;

            //Fireball timer
            if (Fireball_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, HEROIC(SPELL_FIREBALL, SPELL_FIREBALL_H));
                Fireball_Timer = FIREBALL_TIMER;
            }
            else
                Fireball_Timer -= diff;
        }

        //Phase 1 and 2
        if (Phase == 1 || Phase == 2)
        {
            //Flame Breath timer
            if (FlameBreath_Timer < diff)
            {
                DoScriptText(EMOTE_FLAME_BREATH, m_creature);
                DoCast(m_creature, HEROIC(SPELL_FLAME_BREATH, SPELL_FLAME_BREATH_H));
                FlameBreath_Timer = FLAME_BREATH_TIMER;
            }
            else
                FlameBreath_Timer -= diff;

            //Devouring Flame timer
            if (DevouringFlame_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, HEROIC(SPELL_DEVOURING_FLAME, SPELL_DEVOURING_FLAME_H));
                DevouringFlame_Timer = DEVOURING_FAME_TIMER;
            }
            else
                DevouringFlame_Timer -= diff;
        }

        //Phase 2
        if (Phase == 2)
        {
            //Fuse Armor timer
            if (FuseArmor_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target,SPELL_FUSE_ARMOR);
                FuseArmor_Timer = FUSE_ARMOR_TIMER;
            }
            else
                FuseArmor_Timer -= diff;

            //Flame Buffet timer
            if (FlameBuffet_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, HEROIC(SPELL_FLAME_BUFFET, SPELL_FLAME_BUFFET_H));

                FlameBuffet_Timer = FLAME_BUFFET_TIMER;
            }
            else
                FlameBuffet_Timer -= diff;
        }

        //if (phase == 2)   //re-enable this when fly phase works properly
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI(pCreature);
}

void AddSC_boss_razorscale()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_razorscale";
    newscript->GetAI = &GetAI_boss_razorscale;
    newscript->RegisterSelf();
}
