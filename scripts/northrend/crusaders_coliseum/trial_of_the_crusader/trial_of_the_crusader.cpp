#include "precompiled.h"
#include "trial_of_the_crusader.h"

uint32 toc_GetType(Creature *pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_GORMOK                  : return TYPE_GORMOK;
        case NPC_ACIDMAW                 : return TYPE_ACIDMAW;
        case NPC_DREADSCALE              : return TYPE_DREADSCALE;
        case NPC_ICEHOWL                 : return TYPE_ICEHOWL;
        case NPC_JARAXXUS                : return TYPE_JARAXXUS;
        case NPC_TYRIUS_DUSKBLADE        : return TYPE_TYRIUS_DUSKBLADE;
        case NPC_KAVINA_GROVESONG        : return TYPE_KAVINA_GROVESONG;
        case NPC_MELADOR_VALESTRIDER     : return TYPE_MELADOR_VALESTRIDER;
        case NPC_ALYSSIA_MOONSTALKER     : return TYPE_ALYSSIA_MOONSTALKER;
        case NPC_NOOZLE_WHIZZLESTICK     : return TYPE_NOOZLE_WHIZZLESTICK;
        case NPC_VELNAA                  : return TYPE_VELNAA;
        case NPC_BAELNOR_LIGHTBEARER     : return TYPE_BAELNOR_LIGHTBEARER;
        case NPC_ANTHAR_FORGEMENDER      : return TYPE_ANTHAR_FORGEMENDER;
        case NPC_BRIENNA_NIGHTFELL       : return TYPE_BRIENNA_NIGHTFELL;
        case NPC_IRIETH_SHADOWSTEP       : return TYPE_IRIETH_SHADOWSTEP;
        case NPC_SHAABAD                 : return TYPE_SHAABAD;
        case NPC_SAAMUL                  : return TYPE_SAAMUL;
        case NPC_SERISSA_GRIMDABBLER     : return TYPE_SERISSA_GRIMDABBLER;
        case NPC_SHOCUUL                 : return TYPE_SHOCUUL;
        case NPC_GORGRIM_SHADOWCLEAVE    : return TYPE_GORGRIM_SHADOWCLEAVE;
        case NPC_BIRANA_STORMHOOF        : return TYPE_BIRANA_STORMHOOF;
        case NPC_ERIN_MISTHOOF           : return TYPE_ERIN_MISTHOOF;
        case NPC_RUJKAH                  : return TYPE_RUJKAH;
        case NPC_GINSELLE_BLIGHTSLINGER  : return TYPE_GINSELLE_BLIGHTSLINGER;
        case NPC_LIANDRA_SUNCALLER       : return TYPE_LIANDRA_SUNCALLER;
        case NPC_MALITHAS_BRIGHTBLADE    : return TYPE_MALITHAS_BRIGHTBLADE;
        case NPC_CAIPHUS_THE_STERN       : return TYPE_CAIPHUS_THE_STERN;
        case NPC_VIVIENNE_BLACKWHISPER   : return TYPE_VIVIENNE_BLACKWHISPER;
        case NPC_MAZDINAH                : return TYPE_MAZDINAH;
        case NPC_BROLN_STOUTHORN         : return TYPE_BROLN_STOUTHORN;
        case NPC_THRAKGAR                : return TYPE_THRAKGAR;
        case NPC_HARKZOG                 : return TYPE_HARKZOG;
        case NPC_NARRHOK_STEELBREAKER    : return TYPE_NARRHOK_STEELBREAKER;
        case NPC_FJOLA_LIGHTBANE         : return TYPE_FJOLA_LIGHTBANE;
        case NPC_EYDIS_DARKBANE          : return TYPE_EYDIS_DARKBANE;
        case NPC_ANUBARAK                : return TYPE_ANUBARAK;
        case NPC_BARRETT_RAMSAY          : return TYPE_BARRETT_RAMSAY;

        default:
            return 0;
    }
}

boss_trial_of_the_crusaderAI::boss_trial_of_the_crusaderAI(Creature* pCreature):
    ScriptedAI(pCreature),
    m_uiBossEncounterId(toc_GetType(pCreature))
{
    m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
    Difficulty diff = pCreature->GetMap()->GetDifficulty();
    m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
    m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
    if (m_pInstance)
        m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    Events.Reset();
}

void boss_trial_of_the_crusaderAI::Reset()
{
    if (m_pInstance)
        m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    if(Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
        if(ScriptedAI *barrettAI = dynamic_cast<ScriptedAI*>(barrett->AI()))
            barrettAI->Reset();
}
