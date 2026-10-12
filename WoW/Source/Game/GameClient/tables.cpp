#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameClient/tables.h"

// STATIC: 0x00CA283C/0x140B3F2C8
static ClientGameTable<gtBarberShopCostBaseRec> s_gtBarberShopCostBase("BarberShopCostBase", &g_gtBarberShopCostBaseDB);

// STATIC: 0x00CA284C/0x140B3F2E8
static ClientGameTable<gtCombatRatingsRec> s_gtCombatRatings("CombatRatings", &g_gtCombatRatingsDB);

// STATIC: 0x00CA285C/0x140B3F308
static ClientGameTable<gtChanceToMeleeCritRec> s_gtChanceToMeleeCrit("ChanceToMeleeCrit", &g_gtChanceToMeleeCritDB);

// STATIC: 0x00CA286C/0x140B3F328
static ClientGameTable<gtChanceToMeleeCritBaseRec> s_gtChanceToMeleeCritBase("ChanceToMeleeCritBase", &g_gtChanceToMeleeCritBaseDB);

// STATIC: 0x00CA287C/0x140B3F348
static ClientGameTable<gtChanceToSpellCritRec> s_gtChanceToSpellCrit("ChanceToSpellCrit", &g_gtChanceToSpellCritDB);

// STATIC: 0x00CA288C/0x140B3F368
static ClientGameTable<gtChanceToSpellCritBaseRec> s_gtChanceToSpellCritBase("ChanceToSpellCritBase", &g_gtChanceToSpellCritBaseDB);

// STATIC: 0x00CA289C/0x140B3F388
static ClientGameTable<gtNPCManaCostScalerRec> s_gtNPCManaCostScaler("NPCManaCostScaler", &g_gtNPCManaCostScalerDB);

// STATIC: 0x00CA28AC/0x140B3F3A8
static ClientGameTable<gtOCTBaseHPByClassRec> s_gtOCTBaseHPByClass("OCTBaseHPByClass", &g_gtOCTBaseHPByClassDB);

// STATIC: 0x00CA28BC/0x140B3F3C8
static ClientGameTable<gtOCTBaseMPByClassRec> s_gtOCTBaseMPByClass("OCTBaseMPByClass", &g_gtOCTBaseMPByClassDB);

// STATIC: 0x00CA28CC/0x140B3F3E8
static ClientGameTable<gtOCTHpPerStaminaRec> s_gtOCTHpPerStamina("OCTHpPerStamina", &g_gtOCTHpPerStaminaDB);

// STATIC: 0x00CA28DC/0x140B3F408
static ClientGameTable<gtRegenMPPerSptRec> s_gtRegenMPPerSpt("RegenMPPerSpt", &g_gtRegenMPPerSptDB);

// STATIC: 0x00CA28EC/0x140B3F428
static ClientGameTable<gtOCTClassCombatRatingScalarRec> s_gtOCTClassCombatRatingScalar("OCTClassCombatRatingScalar", &g_gtOCTClassCombatRatingScalarDB);

// STATIC: 0x00CA28FC/0x140B3F448
static ClientGameTable<gtSpellScalingRec> s_gtSpellScaling("SpellScaling", &g_gtSpellScalingDB);

// STATIC: 0x00CA2808/0x140B3F260
static ClientGameTableBase* s_gameTables[] = { &s_gtBarberShopCostBase, &s_gtCombatRatings, &s_gtChanceToMeleeCrit, &s_gtChanceToMeleeCritBase, &s_gtChanceToSpellCrit, &s_gtChanceToSpellCritBase, &s_gtNPCManaCostScaler, &s_gtOCTBaseHPByClass, &s_gtOCTBaseMPByClass, &s_gtOCTHpPerStamina, &s_gtRegenMPPerSpt, &s_gtOCTClassCombatRatingScalar, &s_gtSpellScaling };

// INLINED
ClientGameTableBase::ClientGameTableBase(const char* name) {
    this->m_name = name;
    this->m_gameTable = NULL;
}

// PROVEN: b09df120 IDENTICAL/DIFFERENT
// OFFSET: 0x004EAE30/0x14012DE30
void ClientGameTableBase::Initialize() {
    for (int32_t i = 0; i <= g_gameTablesDB.m_numRecords; i++) {
        GameTablesRec* rec = g_gameTablesDB.GetRecordByIndex(i);

        if (!_stricmp(this->m_name, rec->m_name)) {
            this->m_gameTable = rec;

            return;
        }
    }
}

// PROVEN: 04f413c2 IDENTICAL/IDENTICAL
// OFFSET: 0x004EAEA0/0x14012DEC0
void GameTablesInitialize() {
    for (uint32_t i = 0; i < sizeof(s_gameTables) / sizeof(s_gameTables[0]); i++) {
        s_gameTables[i]->Initialize();
    }
}

// PROVEN: 8c5ba323 IDENTICAL/IDENTICAL
// OFFSET: 0x004EAE10/0x14012DE10
float GameFormulaEvaluateTable(int32_t table, int32_t index, int32_t a3) {
    return s_gameTables[table]->EvaluateTable(index, a3);
}
