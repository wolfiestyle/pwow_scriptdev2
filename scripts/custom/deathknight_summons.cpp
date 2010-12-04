#include "precompiled.h"

enum Spells
{
    SPELL_BLOODWORM_HEALTH_LEECH = 50454,
};

struct MANGOS_DLL_DECL npc_bloodwornAI: public ScriptedAI
{
    ObjectGuid mOwner;

    npc_bloodwornAI(Creature* pCreature):
        ScriptedAI(pCreature),
        mOwner(m_creature->GetOwnerGUID())
    {
    }

    void Reset() {}

    void DamageDeal(Unit* pDoneTo, uint32& uiDamage)
    {
        if (Unit *pOwner = m_creature->GetMap()->GetUnit(mOwner))
        {
            int32 bp0 = uiDamage * 2; // Not sure of exact healing multiplier
            pOwner->CastCustomSpell(pOwner, SPELL_BLOODWORM_HEALTH_LEECH, &bp0, NULL, NULL, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (Unit *pOwner = m_creature->GetMap()->GetUnit(mOwner))
            if (Unit *pTarget = pOwner->getVictim())
            {
                m_creature->Attack(pTarget, true);
                DoMeleeAttackIfReady();
            }
            else 
                m_creature->AttackStop();
    }
};

CreatureAI* GetAI_npc_bloodworn(Creature* pCreature)
{
    return new npc_bloodwornAI(pCreature);
}

void AddSC_deathknight_summons()
{
    Script *newscript = new Script;
    newscript->Name = "npc_bloodworm";
    newscript->GetAI = &GetAI_npc_bloodworn;
    newscript->RegisterSelf();
}
