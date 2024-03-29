#include "precompiled.h"
#include "icecrown_citadel.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<EntryTypeMap>
    (NPC_MARROWGAR,         TYPE_MARROWGAR)
    (NPC_DEATHWHISPER,      TYPE_DEATHWHISPER)
    (NPC_SAURFANG,          TYPE_SAURFANG)
    (NPC_FESTERGUT,         TYPE_FESTERGUT)
    (NPC_ROTFACE,           TYPE_ROTFACE)
    (NPC_PUTRICIDE,         TYPE_PUTRICIDE)
    (NPC_VALANAR,           TYPE_VALANAR)
    (NPC_KELESETH,          TYPE_KELESETH)
    (NPC_TALDARAM,          TYPE_TALDARAM)
    (NPC_LANATHEL,          TYPE_LANATHEL)
    (NPC_SVALNA,            TYPE_SVALNA)
    (NPC_VALITHRIA,         TYPE_VALITHRIA)
    (NPC_SINDRAGOSA,        TYPE_SINDRAGOSA)
    (NPC_LICH_KING,         TYPE_LICH_KING);

static EntryTypeMap const GameObjectEntryToType = map_initializer<EntryTypeMap>
    (GO_MARROWGAR_ENTRANCE,         DATA_MARROWGAR_DOOR_ENTRANCE)
    (GO_MARROWGAR_DOOR_1,           DATA_MARROWGAR_DOOR_1)
    (GO_MARROWGAR_DOOR_2,           DATA_MARROWGAR_DOOR_2)
    (GO_DEATHWHISPER_ELEV,          DATA_DEATHWHISPER_ELEV)
    (GO_SAURFANG_CHEST_N10,         DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_N25,         DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_H10,         DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_H25,         DATA_SAURFANG_CHEST)
    (GO_SAURFANG_DOOR,              DATA_SAURFANG_DOOR)
    (GO_FROSTWING_DOOR_ENTRANCE,    DATA_FROSTWING_DOOR_ENTRANCE)
    (GO_VALITHRIA_CHEST_N10,        DATA_VALITHRIA_CHEST)
    (GO_VALITHRIA_CHEST_N25,        DATA_VALITHRIA_CHEST)
    (GO_VALITHRIA_CHEST_H10,        DATA_VALITHRIA_CHEST)
    (GO_VALITHRIA_CHEST_H25,        DATA_VALITHRIA_CHEST)
    (GO_VALITHRIA_DOOR_ENTRANCE,    DATA_VALITHRIA_DOOR_ENTRANCE)
    (GO_VALITHRIA_DOOR_EXIT,        DATA_VALITHRIA_DOOR_EXIT)
    (GO_VALITHRIA_DOOR_L1,          DATA_VALITHRIA_DOOR_LEFT_1)
    (GO_VALITHRIA_DOOR_R1,          DATA_VALITHRIA_DOOR_RIGHT_1)
    (GO_VALITHRIA_DOOR_L2,          DATA_VALITHRIA_DOOR_LEFT_2)
    (GO_VALITHRIA_DOOR_R2,          DATA_VALITHRIA_DOOR_RIGHT_2)
    (GO_GREEN_TUBES,                DATA_GREEN_TUBES)
    (GO_ORANGE_TUBES,               DATA_ORANGE_TUBES)
    (GO_ROTFACE_DOOR,               DATA_ROTFACE_DOOR)
    (GO_FESTERGUT_DOOR,             DATA_FESTERGUT_DOOR)
    (GO_PUTRICIDE_UPPER_DOOR,       DATA_PUTRICIDE_UPPER_DOOR)
    (GO_PUTRICIDE_LOWER_DOOR,       DATA_PUTRICIDE_LOWER_DOOR)
    (GO_PUTRICIDE_LOWER_DOOR_ORANGE,DATA_PUTRICIDE_LOWER_DOOR_ORANGE)
    (GO_PUTRICIDE_LOWER_DOOR_GREEN, DATA_PUTRICIDE_LOWER_DOOR_GREEN)
    (GO_BLOODWING_DOOR,             DATA_BLOODWING_DOOR)
    (GO_BLOOD_PRINCE_DOOR,          DATA_BLOOD_PRINCE_DOOR)
    (GO_LANATHEL_DOOR_LEFT,         DATA_LANATHEL_DOOR_LEFT)
    (GO_LANATHEL_DOOR_RIGHT,        DATA_LANATHEL_DOOR_RIGHT)
    (GO_SINDRAGOSA_DOOR,            DATA_SINDRAGOSA_DOOR)
    (GO_SINDRAGOSA_EXIT_DOOR,       DATA_SINDRAGOSA_EXIT_DOOR)
    (GO_SINDRAGOSA_EXIT_DOOR2,      DATA_SINDRAGOSA_EXIT_DOOR2)
    (GO_LICHKING_PLATFORM,          DATA_LICHKING_PLATFORM)
    (GO_LICHKING_PRECIPICE,         DATA_LICHKING_PRECIPICE)
    (GO_LICHKING_SPIKE1,            DATA_LICHKING_SPIKE1)
    (GO_LICHKING_SPIKE2,            DATA_LICHKING_SPIKE2)
    (GO_LICHKING_SPIKE3,            DATA_LICHKING_SPIKE3)
    (GO_LICHKING_SPIKE4,            DATA_LICHKING_SPIKE4)
    (GO_LICHKING_EDGE_WARNING,      DATA_LICHKING_EDGE_WARNING)
    (GO_LICHKING_FROSTY_WIND,       DATA_LICHKING_FROSTY_WIND)
    (GO_LICHKING_FROSTY_EDGE,       DATA_LICHKING_FROSTY_EDGE);

namespace icc {

uint32 GetType(Creature *pCreature)
{
    return map_find(CreatureEntryToType, pCreature->GetEntry(), DATA_GUID_MAX);
}

uint32 GetType(GameObject *pGO)
{
    return map_find(GameObjectEntryToType, pGO->GetEntry(), DATA_GUID_MAX);
}

#define BOSS_DONE(id)   (instance->GetData(id) == DONE)

bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id)
{
    if (!instance)
        return true;

    switch (boss_id)
    {
        case TYPE_MARROWGAR:
            //TODO: add heroic check here
            return true;
        case TYPE_DEATHWHISPER:
            return BOSS_DONE(TYPE_MARROWGAR) && MeetsRequirementsForBoss(instance, TYPE_MARROWGAR);
        case TYPE_GUNSHIP_BATTLE:
            return BOSS_DONE(TYPE_DEATHWHISPER) && MeetsRequirementsForBoss(instance, TYPE_DEATHWHISPER);
        case TYPE_SAURFANG:
            return /*BOSS_DONE(TYPE_GUNSHIP_BATTLE) && */MeetsRequirementsForBoss(instance, TYPE_GUNSHIP_BATTLE);
        case TYPE_FESTERGUT:
        case TYPE_ROTFACE:
        case TYPE_VALANAR:
        case TYPE_KELESETH:
        case TYPE_TALDARAM:
        case TYPE_SVALNA:
            return BOSS_DONE(TYPE_SAURFANG) && MeetsRequirementsForBoss(instance, TYPE_SAURFANG);
        case TYPE_PUTRICIDE:
            return BOSS_DONE(TYPE_FESTERGUT) && BOSS_DONE(TYPE_ROTFACE) &&
                MeetsRequirementsForBoss(instance, TYPE_FESTERGUT); // same check for rotface
        case TYPE_LANATHEL:
            return BOSS_DONE(TYPE_VALANAR) && BOSS_DONE(TYPE_KELESETH) && BOSS_DONE(TYPE_TALDARAM) &&
                MeetsRequirementsForBoss(instance, TYPE_VALANAR);   // same check for keleseth, taldaram
        case TYPE_VALITHRIA:
            return BOSS_DONE(TYPE_SVALNA) && MeetsRequirementsForBoss(instance, TYPE_SVALNA);
        case TYPE_SINDRAGOSA:
            return BOSS_DONE(TYPE_VALITHRIA) && MeetsRequirementsForBoss(instance, TYPE_VALITHRIA);
        case TYPE_LICH_KING:
            return BOSS_DONE(TYPE_PUTRICIDE) && BOSS_DONE(TYPE_LANATHEL) && BOSS_DONE(TYPE_SINDRAGOSA) &&
                MeetsRequirementsForBoss(instance, TYPE_PUTRICIDE) &&
                MeetsRequirementsForBoss(instance, TYPE_LANATHEL) &&
                MeetsRequirementsForBoss(instance, TYPE_SINDRAGOSA);
        default:    // unknown boss
            return true;
    }
}

#undef BOSS_DONE

} // namespace icc

boss_icecrown_citadelAI::boss_icecrown_citadelAI(Creature* pCreature):
    ScriptedAI(pCreature),
    ScriptEventInterface(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_Difficulty(pCreature->GetMap()->GetDifficulty()),
    m_bIsHeroic(m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_Difficulty == RAID_DIFFICULTY_25MAN_HEROIC),
    m_bIs10Man(m_Difficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC),
    m_BossEncounter(icc::GetType(pCreature), m_pInstance)
{
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

void boss_icecrown_citadelAI::Reset()
{
    Events.Reset();
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

bool boss_icecrown_citadelAI::IsOutOfCombatArea() const
{
    if (m_creature->GetMapId() != 631)  // invalid check outside ICC
        return false;

    float z = m_creature->GetPositionZ();
    uint32 areaId = m_creature->GetAreaId();

    switch (m_creature->GetEntry())
    {
        case NPC_MARROWGAR:
            return !m_creature->IsWithinDist2d(-401.369f, 2211.1399f, 100.0f);
        case NPC_DEATHWHISPER:
            return std::abs(z - 49.56f) > 10.0f || !m_creature->IsWithinDist2d(-583.15f, 2211.5f, 150.0f);
        case NPC_SAURFANG:
            return std::abs(z - 539.29f) > 5.0f;
        case NPC_FESTERGUT:
            return !m_creature->IsWithinDist2d(4267.939f, 3137.32f, 47.5f);
        case NPC_ROTFACE:
            return !m_creature->IsWithinDist2d(4445.60f, 3137.21f, 47.5f);
        case NPC_PUTRICIDE:
            return std::abs(z - 389.398f) > 4.0f;
        case NPC_VALANAR:
        case NPC_KELESETH:
        case NPC_TALDARAM:
            return std::abs(z - 364.085f) > 6.0f;
        case NPC_LANATHEL:
            return std::abs(z - 401.747f) > 30.0f;
        case NPC_SINDRAGOSA:
            return !m_creature->IsWithinDist2d(4407.94f, 2483.47f, 150.0f);
        case NPC_LICH_KING:
            return areaId != 4859;
        default:
            return false;
    }
}

bool boss_icecrown_citadelAI::OutOfCombatAreaCheck()
{
    if (m_creature->IsInEvadeMode())
        return false;

    if (IsOutOfCombatArea())
    {
        EnterEvadeMode();
        return true;
    }

    return false;
}

bool boss_icecrown_citadelAI::InstanceProgressionCheck()
{
    if (!icc::MeetsRequirementsForBoss(m_pInstance, m_BossEncounter.getDataId()))
    {
        DoStartNoMovement(m_creature);
        EnterEvadeMode();
        return true;
    }

    return false;
}

void boss_icecrown_citadelAI::RemoveEncounterAuras(int32 spellId_1, int32 spellId_2, int32 spellId_3, int32 spellId_4)
{
    Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
    {
        Unit *pPlayer = itr->getSource();
        if (!pPlayer)
            continue;

        if (spellId_1 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_1);
        else if (spellId_1 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_1);

        if (spellId_2 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_2);
        else if (spellId_2 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_2);

        if (spellId_3 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_3);
        else if (spellId_3 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_3);

        if (spellId_4 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_4);
        else if (spellId_4 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_4);
    }
}

void boss_icecrown_citadelAI::SendEncounterUnit(EncounterFrameType type, uint8 param1, uint8 param2)
{
    WorldPacket data(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, 4+8+1);
    data << uint32(type);

    switch (type)
    {
        case ENCOUNTER_FRAME_ADD:
        case ENCOUNTER_FRAME_REMOVE:
        case ENCOUNTER_FRAME_UNK2:
            data << m_creature->GetPackGUID();
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UNK3:
        case ENCOUNTER_FRAME_UNK4:
        case ENCOUNTER_FRAME_UNK6:
            data << uint8(param1);
            data << uint8(param2);
            break;
        case ENCOUNTER_FRAME_UNK5:
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UNK7:
        default:
            break;
    }

    m_creature->GetMap()->SendToPlayers(&data);
}
