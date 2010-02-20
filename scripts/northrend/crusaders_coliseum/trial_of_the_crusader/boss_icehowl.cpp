#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    SPELL_BERKSERK              = 26662,
    SPELL_FEROCIOUS_BUTT_N10    = 66770,
    SPELL_FEROCIOUS_BUTT_N25    = 67654,
    SPELL_FEROCIOUS_BUTT_H10    = 67655,
    SPELL_FEROCIOUS_BUTT_H25    = 67656,
    SPELL_ARCTIC_BREATH_N10     = 66689,
    SPELL_ARCTIC_BREATH_N25     = 67650,
    SPELL_ARCTIC_BREATH_H10     = 67651,
    SPELL_ARCTIC_BREATH_H25     = 67652,
    SPELL_WHIRL_N10             = 67345,
    SPELL_WHIRL_N25             = 67663,
    SPELL_WHIRL_H10             = 67664,
    SPELL_WHIRL_H25             = 67665,
    SPELL_MASSIVE_CRASH_N10     = 66683,
    SPELL_MASSIVE_CRASH_N25     = 67660,
    SPELL_MASSIVE_CRASH_H10     = 67661,
    SPELL_MASSIVE_CRASH_H25     = 67662,
    SPELL_FROTHING_RAGE_N10     = 66759,
    SPELL_FROTHING_RAGE_N25     = 67657,
    SPELL_FROTHING_RAGE_H10     = 67658,
    SPELL_FROTHING_RAGE_H25     = 67659,
    SPELL_TRAMPLE               = 66734,    // just the dmg part
    SPELL_STAGGERED_DAZE        = 66758,    // part of the Trample effect
};

struct MANGOS_DLL_DECL boss_toc_icehowlAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_icehowlAI(Creature* pCreature):
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

void AddSC_boss_icehowl()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_icehowl);
}
