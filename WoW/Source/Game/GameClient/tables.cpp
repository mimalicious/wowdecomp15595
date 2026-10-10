#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameClient/tables.h"

// STATIC: 0x00CA283C
static ClientGameTable<gtBarberShopCostBaseRec> s_gtBarberShopCostBase("BarberShopCostBase", &g_gtBarberShopCostBaseDB);

// STATIC: 0x00CA284C
static ClientGameTable<gtCombatRatingsRec> s_gtCombatRatings("CombatRatings", &g_gtCombatRatingsDB);

// STATIC: 0x00CA285C
static ClientGameTable<gtChanceToMeleeCritRec> s_gtChanceToMeleeCrit("ChanceToMeleeCrit", &g_gtChanceToMeleeCritDB);

// STATIC: 0x00CA286C
static ClientGameTable<gtChanceToMeleeCritBaseRec> s_gtChanceToMeleeCritBase("ChanceToMeleeCritBase", &g_gtChanceToMeleeCritBaseDB);

// STATIC: 0x00CA287C
static ClientGameTable<gtChanceToSpellCritRec> s_gtChanceToSpellCrit("ChanceToSpellCrit", &g_gtChanceToSpellCritDB);

// STATIC: 0x00CA288C
static ClientGameTable<gtChanceToSpellCritBaseRec> s_gtChanceToSpellCritBase("ChanceToSpellCritBase", &g_gtChanceToSpellCritBaseDB);

// STATIC: 0x00CA289C
static ClientGameTable<gtNPCManaCostScalerRec> s_gtNPCManaCostScaler("NPCManaCostScaler", &g_gtNPCManaCostScalerDB);

// STATIC: 0x00CA28AC
static ClientGameTable<gtOCTBaseHPByClassRec> s_gtOCTBaseHPByClass("OCTBaseHPByClass", &g_gtOCTBaseHPByClassDB);

// STATIC: 0x00CA28BC
static ClientGameTable<gtOCTBaseMPByClassRec> s_gtOCTBaseMPByClass("OCTBaseMPByClass", &g_gtOCTBaseMPByClassDB);

// STATIC: 0x00CA28CC
static ClientGameTable<gtOCTHpPerStaminaRec> s_gtOCTHpPerStamina("OCTHpPerStamina", &g_gtOCTHpPerStaminaDB);

// STATIC: 0x00CA28DC
static ClientGameTable<gtRegenMPPerSptRec> s_gtRegenMPPerSpt("RegenMPPerSpt", &g_gtRegenMPPerSptDB);

// STATIC: 0x00CA28EC
static ClientGameTable<gtOCTClassCombatRatingScalarRec> s_gtOCTClassCombatRatingScalar("OCTClassCombatRatingScalar", &g_gtOCTClassCombatRatingScalarDB);

// STATIC: 0x00CA28FC
static ClientGameTable<gtSpellScalingRec> s_gtSpellScaling("SpellScaling", &g_gtSpellScalingDB);

// STATIC: 0x00CA2808
static ClientGameTableBase* s_gameTables[] = { &s_gtBarberShopCostBase, &s_gtCombatRatings, &s_gtChanceToMeleeCrit, &s_gtChanceToMeleeCritBase, &s_gtChanceToSpellCrit, &s_gtChanceToSpellCritBase, &s_gtNPCManaCostScaler, &s_gtOCTBaseHPByClass, &s_gtOCTBaseMPByClass, &s_gtOCTHpPerStamina, &s_gtRegenMPPerSpt, &s_gtOCTClassCombatRatingScalar, &s_gtSpellScaling };

// INLINED
ClientGameTableBase::ClientGameTableBase(const char* name) {
    this->m_name = name;
    this->m_gameTable = NULL;
}

// OFFSET: 0x004EAE30
void ClientGameTableBase::Initialize() {
    for (int32_t i = 0; i <= g_gameTablesDB.m_numRecords; i++) {
        GameTablesRec* rec = g_gameTablesDB.GetRecordByIndex(i);

        if (!_stricmp(this->m_name, rec->m_name)) {
            this->m_gameTable = rec;

            return;
        }
    }
}

// OFFSET: 0x004EAEA0
void GameTablesInitialize() {
    for (uint32_t i = 0; i < sizeof(s_gameTables) / sizeof(s_gameTables[0]); i++) {
        s_gameTables[i]->Initialize();
    }
}

// PROVEN: efa079a9 IDENTICAL/NONE
// OFFSET: 0x004EAE10/0x14012DE10
float GameFormulaEvaluateTable(int32_t table, int32_t index, int32_t a3) {
    return s_gameTables[table]->EvaluateTable(index, a3);
}
