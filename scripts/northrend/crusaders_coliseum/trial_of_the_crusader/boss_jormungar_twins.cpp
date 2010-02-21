#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    // Acidmaw
    SPELL_ACIDIC_SPEW           = 66818,    //triggers 66819, there is 67609, 67610, 67611 related and currently unused, maybe core should auto-select them?
    SPELL_PARALYTIC_BITE_N10    = 66824,
    SPELL_PARALYTIC_BITE_N25    = 67612,
    SPELL_PARALYTIC_BITE_H10    = 67613,
    SPELL_PARALYTIC_BITE_H25    = 67614,
    SPELL_ACID_SPIT_N10         = 66880,
    SPELL_ACID_SPIT_N25         = 67606,
    SPELL_ACID_SPIT_H10         = 67607,
    SPELL_ACID_SPIT_H25         = 67608,
    SPELL_PARALYTIC_SPRAY_N10   = 66901,
    SPELL_PARALYTIC_SPRAY_N25   = 67615,
    SPELL_PARALYTIC_SPRAY_H10   = 67616,
    SPELL_PARALYTIC_SPRAY_H25   = 67617,
    // Dreadscale
    SPELL_MOLTEN_SPEW           = 66821,    //triggers 66820, related and unused 67635, 67636, 67637
    SPELL_BURNING_BITE_N10      = 66879,
    SPELL_BURNING_BITE_N25      = 67624,
    SPELL_BURNING_BITE_H10      = 67625,
    SPELL_BURNING_BITE_H25      = 67626,
    SPELL_FIRE_SPIT_N10         = 66796,
    SPELL_FIRE_SPIT_N25         = 67632,
    SPELL_FIRE_SPIT_H10         = 67633,
    SPELL_FIRE_SPIT_H25         = 67634,
    SPELL_BURNING_SPRAY_N10     = 66902,
    SPELL_BURNING_SPRAY_N25     = 67627,
    SPELL_BURNING_SPRAY_H10     = 67628,
    SPELL_BURNING_SPRAY_H25     = 67629,
    // all bosses
    SPELL_SLIME_POOL_N10        = 66883,
    SPELL_SLIME_POOL_N25        = 67641,
    SPELL_SLIME_POOL_H10        = 67642,
    SPELL_SLIME_POOL_H25        = 67643,
    SPELL_SWEEP_N10             = 66794,
    SPELL_SWEEP_N25             = 67644,
    SPELL_SWEEP_H10             = 67645,
    SPELL_SWEEP_H25             = 67646,
    SPELL_ENRAGE                = 68335,
};

struct MANGOS_DLL_DECL boss_toc_acidmawAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_acidmawAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_dreadscaleAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_dreadscaleAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_jormungar_twins()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_acidmaw);
    REGISTER_SCRIPT(boss_toc_dreadscale);
}
