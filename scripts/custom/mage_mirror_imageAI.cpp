#include "precompiled.h"

enum Spells
{
    SPELL_FIREBLAST     = 59637,
    SPELL_FROSTBOLT     = 59638,
};

struct MANGOS_DLL_DECL npc_mirror_imageAI: public ScriptedAI
{
    uint32 m_FrostboltTimer;
    uint32 m_FireblastTimer;

    npc_mirror_imageAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_FrostboltTimer = 0;
        m_FireblastTimer = urand(3100, 3200);
    }

    void UpdateAI(uint32 const diff)
    {
        if (Unit *owner = m_creature->GetOwner())
            if (Unit *target = owner->getAttackerForHelper())
                if (m_creature->Attack(target, false))
                    Reset();

        if (!m_creature->getVictim())
            return;

        if (m_FireblastTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FIREBLAST);
            m_FireblastTimer = urand(6100, 6200);
        }
        else
            m_FireblastTimer -= diff;

        if (m_FrostboltTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_FrostboltTimer = urand(3100, 3200);
        }
        else
            m_FrostboltTimer -= diff;
    }
};

CreatureAI* GetAI_npc_mirror_image(Creature* pCreature)
{
    return new npc_mirror_imageAI(pCreature);
}

void AddSC_Mirror_Image()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_mirror_image";
    newscript->GetAI = &GetAI_npc_mirror_image;
    newscript->RegisterSelf();
}
