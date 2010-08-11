#include "precompiled.h"

#define OUT_OF_COMBAT_TIME 5000

struct MANGOS_DLL_DECL npc_training_dummyAI: public Scripted_NoMovementAI
{
    uint32 m_CombatTimer;

    npc_training_dummyAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_CombatTimer(0)
    {
    }

    void Reset()
    {
        m_CombatTimer = 0;
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        m_CombatTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isAlive() || !m_creature->isInCombat() || m_creature->IsInEvadeMode())
            return;

        m_creature->SetTargetGUID(0);

        if (m_creature->GetHealthPercent() < 100.0f)
            m_creature->SetHealthPercent(100.0f);

        m_CombatTimer += diff;
        if (m_CombatTimer > OUT_OF_COMBAT_TIME)
            EnterEvadeMode();
    }
};

CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
    return new npc_training_dummyAI(pCreature);
}

void AddSC_npc_training_dummy()
{
    Script *newscript = new Script;
    newscript->Name = "npc_training_dummy";
    newscript->GetAI = &GetAI_npc_training_dummy;
    newscript->RegisterSelf();
}
