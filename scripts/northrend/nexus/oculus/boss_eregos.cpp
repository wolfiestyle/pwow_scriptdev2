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
SDName: Boss Ley-Guardian Eregos
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"

#define SPELL_ARCANE_BARRAGE              50804
#define SPELL_ARCANE_BARRAGE_H            59381
#define SPELL_ARCANE_VOLLEY               51153
#define SPELL_ARCANE_VOLLEY_H             59382
#define SPELL_PLANAR_SHIFT                51162
#define SPELL_ENRAGED_ASSAULT             51170
#define SPELL_PLANAR_ANOMALIES            57959

struct MANGOS_DLL_DECL boss_eregosAI : public ScriptedAI
{
    boss_eregosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 ArcaneBarrage_Timer;
    uint32 ArcaneVolley_Timer;
    uint32 PlanarShift_Timer;
    uint32 EnragedAssault_Timer;
    uint32 PlanarAnomalies_Timer;

    uint64 CacheOfEregosGUID;

    void Reset() 
    {
        ArcaneBarrage_Timer = 8000;
        ArcaneVolley_Timer = 15000;
        PlanarShift_Timer = 30000;
        EnragedAssault_Timer = 40000;
        PlanarAnomalies_Timer = 90000;

        CacheOfEregosGUID = 0;
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
        {
            CacheOfEregosGUID = m_pInstance->GetData64(DATA_CACHE_OF_EREGOS);
            if (CacheOfEregosGUID)
                m_pInstance->DoRespawnGameObject(CacheOfEregosGUID);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Planar Anomalies timer
        if (PlanarAnomalies_Timer < diff)
        {
            DoCast(m_creature,SPELL_PLANAR_ANOMALIES);

            PlanarAnomalies_Timer = 90000;
        }else PlanarAnomalies_Timer -= diff;

        //Enraged Assault timer
        if (EnragedAssault_Timer < diff)
        {
            DoCast(m_creature,SPELL_ENRAGED_ASSAULT);

            EnragedAssault_Timer = 40000;
        }else EnragedAssault_Timer -= diff;

        //Planar Shift timer
        if (PlanarShift_Timer < diff)
        {
            DoCast(m_creature,SPELL_PLANAR_SHIFT);

            PlanarShift_Timer = 30000;
        }else PlanarShift_Timer -= diff;

        //Arcane Volley timer
        if (ArcaneVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_VOLLEY : SPELL_ARCANE_VOLLEY_H);

            ArcaneVolley_Timer = 15000;
        }else ArcaneVolley_Timer -= diff;

        //Arcane Barrage timer
        if (ArcaneBarrage_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_BARRAGE : SPELL_ARCANE_BARRAGE_H);

            ArcaneBarrage_Timer = 8000;
        }else ArcaneBarrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
    return new boss_eregosAI(pCreature);
}

void AddSC_boss_eregos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_eregos";
    newscript->GetAI = &GetAI_boss_eregos;
    newscript->RegisterSelf();
}
