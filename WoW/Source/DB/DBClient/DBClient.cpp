#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/WowClientDB2.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/KeyChainRec.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Locale.h"
#include "Engine/Source/Services/AsyncFile.h"
#include <ctime>

// STATIC: 0x00C9A990
const char* g_localeNames[15] = { "deDE", "enGB", "enUS", "esES", "frFR", "koKR", "zhCN", "zhTW", "enCN", "enTW", "esMX", "ruRU", "enUS", "ptBR", "ptPT" };

// STATIC: #0x00AD2F90
static int32_t s_physicalDamageClassID = -1;

// STATIC: #0x00C5D660
static const ItemSubClassRec* s_unarmedWeapon;

// STATIC: #0x00C5D664
static const ItemClassRec* s_weaponItemClass;

// STATIC: #0x00C5D668
static TSFixedArray<const ResistancesRec*> s_resistances;

// STATIC: #0x00C5D674
static TSFixedArray<uint32_t> s_terrainTypeSoundIDs;

struct SPELLACTIVATIONOVERLAY : public TSHashObject<SPELLACTIVATIONOVERLAY, HASHKEY_NONE> {
    /* 0018 */ const SpellActivationOverlayRec* m_rec;
};

// STATIC: 0x00D97EBC
static TSGrowableArray<TSGrowableArray<const SpellEffectRec*> > s_spellEffects;

// STATIC: 0x00D97ECC
static TSHashTable<SPELLACTIVATIONOVERLAY, HASHKEY_NONE> s_spellActivationOverlays;

// STATIC: 0x00D97EB0
static TSFixedArray<const ItemSubClassRec*> s_weaponSubclasses;

// STATIC: 0x00D73300
WowClientDB<AnimationDataRec> g_animationDataDB;

// STATIC: 0x00D7331C
WowClientDB<AreaTableRec> g_areaTableDB;

// STATIC: 0x00D73338
WowClientDB<FootprintTexturesRec> g_footprintTexturesDB;

// STATIC: 0x00D73354
WowClientDB<GroundEffectDoodadRec> g_groundEffectDoodadDB;

// STATIC: 0x00D73370
WowClientDB<GroundEffectTextureRec> g_groundEffectTextureDB;

// STATIC: 0x00D7338C
WowClientDB<LightRec> g_lightDB;

// STATIC: 0x00D733A8
WowClientDB<LightIntBandRec> g_lightIntBandDB;

// STATIC: 0x00D733C4
WowClientDB<LightFloatBandRec> g_lightFloatBandDB;

// STATIC: 0x00D733E0
WowClientDB<LightParamsRec> g_lightParamsDB;

// STATIC: 0x00D733FC
WowClientDB<LightSkyboxRec> g_lightSkyboxDB;

// STATIC: 0x00D73418
WowClientDB<LiquidMaterialRec> g_liquidMaterialDB;

// STATIC: 0x00D73434
WowClientDB<LiquidObjectRec> g_liquidObjectDB;

// STATIC: 0x00D73450
WowClientDB<LiquidTypeRec> g_liquidTypeDB;

// STATIC: 0x00D7346C
WowClientDB<MapRec> g_mapDB;

// STATIC: 0x00D73488
WowClientDB<SoundEmitterPillPointsRec> g_soundEmitterPillPointsDB;

// STATIC: 0x00D734A4
WowClientDB<SoundEmittersRec> g_soundEmittersDB;

// STATIC: 0x00D734C0
WowClientDB<SoundEntriesAdvancedRec> g_soundEntriesAdvancedDB;

// STATIC: 0x00D734DC
WowClientDB<SoundEntriesRec> g_soundEntriesDB;

// STATIC: 0x00D734F8
WowClientDB<SoundEntriesFallbacksRec> g_soundEntriesFallbacksDB;

// STATIC: 0x00D73514
WowClientDB<TerrainMaterialRec> g_terrainMaterialDB;

// STATIC: 0x00D73530
WowClientDB<TerrainTypeRec> g_terrainTypeDB;

// STATIC: 0x00D73568
WowClientDB<WeatherRec> g_weatherDB;

// STATIC: 0x00D73584
WowClientDB<WMOAreaTableRec> g_wmoAreaTableDB;

// STATIC: 0x00D735A0
WowClientDB<ZoneLightRec> g_zoneLightDB;

// STATIC: 0x00D735BC
WowClientDB<ZoneLightPointRec> g_zoneLightPointDB;

// STATIC: 0x00D98968
WowClientDB<AchievementRec> g_achievementDB;

// STATIC: 0x00D98984
WowClientDB<Achievement_CriteriaRec> g_achievement_CriteriaDB;

// STATIC: 0x00D989A0
WowClientDB<Achievement_CategoryRec> g_achievement_CategoryDB;

// STATIC: 0x00D989BC
WowClientDB<AnimKitRec> g_animKitDB;

// STATIC: 0x00D989D8
WowClientDB<AnimKitBoneSetRec> g_animKitBoneSetDB;

// STATIC: 0x00D989F4
WowClientDB<AnimKitBoneSetAliasRec> g_animKitBoneSetAliasDB;

// STATIC: 0x00D98A10
WowClientDB<AnimKitConfigRec> g_animKitConfigDB;

// STATIC: 0x00D98A2C
WowClientDB<AnimKitConfigBoneSetRec> g_animKitConfigBoneSetDB;

// STATIC: 0x00D98A48
WowClientDB<AnimKitPriorityRec> g_animKitPriorityDB;

// STATIC: 0x00D98A64
WowClientDB<AnimKitSegmentRec> g_animKitSegmentDB;

// STATIC: 0x00D98A80
WowClientDB<AnimReplacementRec> g_animReplacementDB;

// STATIC: 0x00D98A9C
WowClientDB<AnimReplacementSetRec> g_animReplacementSetDB;

// STATIC: 0x00D98AB8
WowClientDB<AreaGroupRec> g_areaGroupDB;

// STATIC: 0x00D98AD4
WowClientDB<AreaPOIRec> g_areaPOIDB;

// STATIC: 0x00D98AF0
WowClientDB<AreaPOISortedWorldStateRec> g_areaPOISortedWorldStateDB;

// STATIC: 0x00D98B0C
WowClientDB<AreaAssignmentRec> g_areaAssignmentDB;

// STATIC: 0x00D98B28
WowClientDB<AreaTriggerRec> g_areaTriggerDB;

// STATIC: 0x00D98B44
WowClientDB<ArmorLocationRec> g_armorLocationDB;

// STATIC: 0x00D98B60
WowClientDB<AuctionHouseRec> g_auctionHouseDB;

// STATIC: 0x00D98B7C
WowClientDB<BankBagSlotPricesRec> g_bankBagSlotPricesDB;

// STATIC: 0x00D98B98
WowClientDB<BannedAddOnsRec> g_bannedAddOnsDB;

// STATIC: 0x00D98BB4
WowClientDB<BarberShopStyleRec> g_barberShopStyleDB;

// STATIC: 0x00D98BD0
WowClientDB<BattlemasterListRec> g_battlemasterListDB;

// STATIC: 0x00D98BEC
WowClientDB<CameraModeRec> g_cameraModeDB;

// STATIC: 0x00D98C08
WowClientDB<CameraShakesRec> g_cameraShakesDB;

// STATIC: 0x00D98C24
WowClientDB<CastableRaidBuffsRec> g_castableRaidBuffsDB;

// STATIC: 0x00D98C40
WowClientDB<Cfg_CategoriesRec> g_cfg_CategoriesDB;

// STATIC: 0x00D98C5C
WowClientDB<Cfg_ConfigsRec> g_cfg_ConfigsDB;

// STATIC: 0x00D98C78
WowClientDB<CharBaseInfoRec> g_charBaseInfoDB;

// STATIC: 0x00D98C94
WowClientDB<CharHairGeosetsRec> g_charHairGeosetsDB;

// STATIC: 0x00D98CB0
WowClientDB<CharSectionsRec> g_charSectionsDB;

// STATIC: 0x00D98CCC
WowClientDB<CharStartOutfitRec> g_charStartOutfitDB;

// STATIC: 0x00D98CE8
WowClientDB<CharTitlesRec> g_charTitlesDB;

// STATIC: 0x00D98D04
WowClientDB<CharacterFacialHairStylesRec> g_characterFacialHairStylesDB;

// STATIC: 0x00D98D20
WowClientDB<ChatChannelsRec> g_chatChannelsDB;

// STATIC: 0x00D98D3C
WowClientDB<ChatProfanityRec> g_chatProfanityDB;

// STATIC: 0x00D98D58
WowClientDB<ChrClassesRec> g_chrClassesDB;

// STATIC: 0x00D98D74
WowClientDB<ChrClassesXPowerTypesRec> g_chrClassesXPowerTypesDB;

// STATIC: 0x00D98D90
WowClientDB<ChrRacesRec> g_chrRacesDB;

// STATIC: 0x00D98DAC
WowClientDB<CinematicCameraRec> g_cinematicCameraDB;

// STATIC: 0x00D98DC8
WowClientDB<CinematicSequencesRec> g_cinematicSequencesDB;

// STATIC: 0x00D98DE4
WowClientDB<CreatureDisplayInfoExtraRec> g_creatureDisplayInfoExtraDB;

// STATIC: 0x00D98E00
WowClientDB<CreatureDisplayInfoRec> g_creatureDisplayInfoDB;

// STATIC: 0x00D98E1C
WowClientDB<CreatureFamilyRec> g_creatureFamilyDB;

// STATIC: 0x00D98E38
WowClientDB<CreatureImmunitiesRec> g_creatureImmunitiesDB;

// STATIC: 0x00D98E54
WowClientDB<CreatureModelDataRec> g_creatureModelDataDB;

// STATIC: 0x00D98E70
WowClientDB<CreatureMovementInfoRec> g_creatureMovementInfoDB;

// STATIC: 0x00D98E8C
WowClientDB<CreatureSoundDataRec> g_creatureSoundDataDB;

// STATIC: 0x00D98EA8
WowClientDB<CreatureSpellDataRec> g_creatureSpellDataDB;

// STATIC: 0x00D98EC4
WowClientDB<CreatureTypeRec> g_creatureTypeDB;

// STATIC: 0x00D98EE0
WowClientDB<CurrencyTypesRec> g_currencyTypesDB;

// STATIC: 0x00D98EFC
WowClientDB<CurrencyCategoryRec> g_currencyCategoryDB;

// STATIC: 0x00D98F18
WowClientDB<DanceMovesRec> g_danceMovesDB;

// STATIC: 0x00D98F34
WowClientDB<DeathThudLookupsRec> g_deathThudLookupsDB;

// STATIC: 0x00D98F88
WowClientDB<DestructibleModelDataRec> g_destructibleModelDataDB;

// STATIC: 0x00D98FA4
WowClientDB<DungeonEncounterRec> g_dungeonEncounterDB;

// STATIC: 0x00D98FC0
WowClientDB<DungeonMapRec> g_dungeonMapDB;

// STATIC: 0x00D98FDC
WowClientDB<DungeonMapChunkRec> g_dungeonMapChunkDB;

// STATIC: 0x00D98FF8
WowClientDB<DurabilityCostsRec> g_durabilityCostsDB;

// STATIC: 0x00D99014
WowClientDB<DurabilityQualityRec> g_durabilityQualityDB;

// STATIC: 0x00D99030
WowClientDB<EmotesRec> g_emotesDB;

// STATIC: 0x00D9904C
WowClientDB<EmotesTextDataRec> g_emotesTextDataDB;

// STATIC: 0x00D99068
WowClientDB<EmotesTextSoundRec> g_emotesTextSoundDB;

// STATIC: 0x00D99084
WowClientDB<EmotesTextRec> g_emotesTextDB;

// STATIC: 0x00D990A0
WowClientDB<EnvironmentalDamageRec> g_environmentalDamageDB;

// STATIC: 0x00D990BC
WowClientDB<ExhaustionRec> g_exhaustionDB;

// STATIC: 0x00D990D8
WowClientDB<FactionGroupRec> g_factionGroupDB;

// STATIC: 0x00D990F4
WowClientDB<FactionRec> g_factionDB;

// STATIC: 0x00D99110
WowClientDB<FactionTemplateRec> g_factionTemplateDB;

// STATIC: 0x00D9912C
WowClientDB<FileDataRec> g_fileDataDB;

// STATIC: 0x00D99148
WowClientDB<FootstepTerrainLookupRec> g_footstepTerrainLookupDB;

// STATIC: 0x00D99164
WowClientDB<GameObjectArtKitRec> g_gameObjectArtKitDB;

// STATIC: 0x00D99180
WowClientDB<GameObjectDisplayInfoRec> g_gameObjectDisplayInfoDB;

// STATIC: 0x00D9919C
WowClientDB<GameTablesRec> g_gameTablesDB;

// STATIC: 0x00D991B8
WowClientDB<GameTipsRec> g_gameTipsDB;

// STATIC: 0x00D991D4
WowClientDB<GemPropertiesRec> g_gemPropertiesDB;

// STATIC: 0x00D991F0
WowClientDB<GlueScreenEmoteRec> g_glueScreenEmoteDB;

// STATIC: 0x00D9920C
WowClientDB<GlyphPropertiesRec> g_glyphPropertiesDB;

// STATIC: 0x00D99228
WowClientDB<GlyphSlotRec> g_glyphSlotDB;

// STATIC: 0x00D99244
WowClientDB<GMSurveyAnswersRec> g_gmSurveyAnswersDB;

// STATIC: 0x00D99260
WowClientDB<GMSurveyCurrentSurveyRec> g_gmSurveyCurrentSurveyDB;

// STATIC: 0x00D9927C
WowClientDB<GMSurveyQuestionsRec> g_gmSurveyQuestionsDB;

// STATIC: 0x00D99298
WowClientDB<GMSurveySurveysRec> g_gmSurveySurveysDB;

// STATIC: 0x00D992B4
WowClientDB<GMTicketCategoryRec> g_gmTicketCategoryDB;

// STATIC: 0x00D992D0
WowClientDB<gtBarberShopCostBaseRec> g_gtBarberShopCostBaseDB;

// STATIC: 0x00D992EC
WowClientDB<gtCombatRatingsRec> g_gtCombatRatingsDB;

// STATIC: 0x00D99308
WowClientDB<gtChanceToMeleeCritRec> g_gtChanceToMeleeCritDB;

// STATIC: 0x00D99324
WowClientDB<gtChanceToMeleeCritBaseRec> g_gtChanceToMeleeCritBaseDB;

// STATIC: 0x00D99340
WowClientDB<gtChanceToSpellCritRec> g_gtChanceToSpellCritDB;

// STATIC: 0x00D9935C
WowClientDB<gtChanceToSpellCritBaseRec> g_gtChanceToSpellCritBaseDB;

// STATIC: 0x00D99378
WowClientDB<gtNPCManaCostScalerRec> g_gtNPCManaCostScalerDB;

// STATIC: 0x00D99394
WowClientDB<gtOCTBaseHPByClassRec> g_gtOCTBaseHPByClassDB;

// STATIC: 0x00D993B0
WowClientDB<gtOCTBaseMPByClassRec> g_gtOCTBaseMPByClassDB;

// STATIC: 0x00D993CC
WowClientDB<gtOCTClassCombatRatingScalarRec> g_gtOCTClassCombatRatingScalarDB;

// STATIC: 0x00D993E8
WowClientDB<gtOCTHpPerStaminaRec> g_gtOCTHpPerStaminaDB;

// STATIC: 0x00D99404
WowClientDB<gtOCTRegenMPRec> g_gtOCTRegenMPDB;

// STATIC: 0x00D99420
WowClientDB<gtRegenMPPerSptRec> g_gtRegenMPPerSptDB;

// STATIC: 0x00D9943C
WowClientDB<gtSpellScalingRec> g_gtSpellScalingDB;

// STATIC: 0x00D99458
WowClientDB<GuildColorBackgroundRec> g_guildColorBackgroundDB;

// STATIC: 0x00D99474
WowClientDB<GuildColorBorderRec> g_guildColorBorderDB;

// STATIC: 0x00D99490
WowClientDB<GuildColorEmblemRec> g_guildColorEmblemDB;

// STATIC: 0x00D994AC
WowClientDB<GuildPerkSpellsRec> g_guildPerkSpellsDB;

// STATIC: 0x00D994C8
WowClientDB<HelmetGeosetVisDataRec> g_helmetGeosetVisDataDB;

// STATIC: 0x00D994E4
WowClientDB<HolidayDescriptionsRec> g_holidayDescriptionsDB;

// STATIC: 0x00D99500
WowClientDB<HolidayNamesRec> g_holidayNamesDB;

// STATIC: 0x00D9951C
WowClientDB<HolidaysRec> g_holidaysDB;

// STATIC: 0x00D99538
WowClientDB<ImportPriceArmorRec> g_importPriceArmorDB;

// STATIC: 0x00D99554
WowClientDB<ImportPriceQualityRec> g_importPriceQualityDB;

// STATIC: 0x00D99570
WowClientDB<ImportPriceShieldRec> g_importPriceShieldDB;

// STATIC: 0x00D9958C
WowClientDB<ImportPriceWeaponRec> g_importPriceWeaponDB;

// STATIC: 0x00D995A8
WowClientDB<ItemArmorTotalRec> g_itemArmorTotalDB;

// STATIC: 0x00D995C4
WowClientDB<ItemArmorQualityRec> g_itemArmorQualityDB;

// STATIC: 0x00D995E0
WowClientDB<ItemArmorShieldRec> g_itemArmorShieldDB;

// STATIC: 0x00D995FC
WowClientDB<ItemBagFamilyRec> g_itemBagFamilyDB;

// STATIC: 0x00D99618
WowClientDB<ItemClassRec> g_itemClassDB;

// STATIC: 0x00D99634
WowClientDB<ItemDamageAmmoRec> g_itemDamageAmmoDB;

// STATIC: 0x00D99650
WowClientDB<ItemDamageOneHandRec> g_itemDamageOneHandDB;

// STATIC: 0x00D9966C
WowClientDB<ItemDamageOneHandCasterRec> g_itemDamageOneHandCasterDB;

// STATIC: 0x00D99688
WowClientDB<ItemDamageRangedRec> g_itemDamageRangedDB;

// STATIC: 0x00D996A4
WowClientDB<ItemDamageThrownRec> g_itemDamageThrownDB;

// STATIC: 0x00D996C0
WowClientDB<ItemDamageTwoHandRec> g_itemDamageTwoHandDB;

// STATIC: 0x00D996DC
WowClientDB<ItemDamageTwoHandCasterRec> g_itemDamageTwoHandCasterDB;

// STATIC: 0x00D996F8
WowClientDB<ItemDamageWandRec> g_itemDamageWandDB;

// STATIC: 0x00D99714
WowClientDB<ItemDisenchantLootRec> g_itemDisenchantLootDB;

// STATIC: 0x00D99730
WowClientDB_RLE<ItemDisplayInfoRec> g_itemDisplayInfoDB;

// STATIC: 0x00D99754
WowClientDB<ItemGroupSoundsRec> g_itemGroupSoundsDB;

// STATIC: 0x00D99770
WowClientDB<ItemLimitCategoryRec> g_itemLimitCategoryDB;

// STATIC: 0x00D9978C
WowClientDB<ItemNameDescriptionRec> g_itemNameDescriptionDB;

// STATIC: 0x00D997A8
WowClientDB<ItemPetFoodRec> g_itemPetFoodDB;

// STATIC: 0x00D997C4
WowClientDB<ItemPriceBaseRec> g_itemPriceBaseDB;

// STATIC: 0x00D997E0
WowClientDB<ItemPurchaseGroupRec> g_itemPurchaseGroupDB;

// STATIC: 0x00D997FC
WowClientDB<ItemRandomPropertiesRec> g_itemRandomPropertiesDB;

// STATIC: 0x00D99818
WowClientDB<ItemRandomSuffixRec> g_itemRandomSuffixDB;

// STATIC: 0x00D99834
WowClientDB<ItemReforgeRec> g_itemReforgeDB;

// STATIC: 0x00D99850
WowClientDB<ItemSetRec> g_itemSetDB;

// STATIC: 0x00D9986C
WowClientDB<ItemSubClassMaskRec> g_itemSubClassMaskDB;

// STATIC: 0x00D99888
WowClientDB<ItemSubClassRec> g_itemSubClassDB;

// STATIC: 0x00D998A4
WowClientDB<ItemVisualEffectsRec> g_itemVisualEffectsDB;

// STATIC: 0x00D998C0
WowClientDB<ItemVisualsRec> g_itemVisualsDB;

// STATIC: 0x00D998DC
WowClientDB<JournalEncounterCreatureRec> g_journalEncounterCreatureDB;

// STATIC: 0x00D998F8
WowClientDB<JournalEncounterItemRec> g_journalEncounterItemDB;

// STATIC: 0x00D99914
WowClientDB<JournalEncounterRec> g_journalEncounterDB;

// STATIC: 0x00D99930
WowClientDB<JournalEncounterSectionRec> g_journalEncounterSectionDB;

// STATIC: 0x00D9994C
WowClientDB<JournalInstanceRec> g_journalInstanceDB;

// STATIC: 0x00D99968
WowClientDB<LanguageWordsRec> g_languageWordsDB;

// STATIC: 0x00D99984
WowClientDB<LanguagesRec> g_languagesDB;

// STATIC: 0x00D999A0
WowClientDB<LfgDungeonExpansionRec> g_lfgDungeonExpansionDB;

// STATIC: 0x00D999BC
WowClientDB<LfgDungeonGroupRec> g_lfgDungeonGroupDB;

// STATIC: 0x00D999D8
WowClientDB<LfgDungeonsGroupingMapRec> g_lfgDungeonsGroupingMapDB;

// STATIC: 0x00D999F4
WowClientDB<LfgDungeonsRec> g_lfgDungeonsDB;

// STATIC: 0x00D99A10
WowClientDB<LoadingScreensRec> g_loadingScreensDB;

// STATIC: 0x00D99A2C
WowClientDB<LoadingScreenTaxiSplinesRec> g_loadingScreenTaxiSplinesDB;

// STATIC: 0x00D99A48
WowClientDB<LockRec> g_lockDB;

// STATIC: 0x00D99A64
WowClientDB<LockTypeRec> g_lockTypeDB;

// STATIC: 0x00D99A80
WowClientDB<MailTemplateRec> g_mailTemplateDB;

// STATIC: 0x00D99A9C
WowClientDB<MapDifficultyRec> g_mapDifficultyDB;

// STATIC: 0x00D99AB8
WowClientDB<MaterialRec> g_materialDB;

// STATIC: 0x00D99AD4
WowClientDB<MountCapabilityRec> g_mountCapabilityDB;

// STATIC: 0x00D99AF0
WowClientDB<MountTypeRec> g_mountTypeDB;

// STATIC: 0x00D99B0C
WowClientDB<MovieRec> g_movieDB;

// STATIC: 0x00D99B28
WowClientDB<MovieFileDataRec> g_movieFileDataDB;

// STATIC: 0x00D99B44
WowClientDB<MovieVariationRec> g_movieVariationDB;

// STATIC: 0x00D99B60
WowClientDB<NameGenRec> g_nameGenDB;

// STATIC: 0x00D99B7C
WowClientDB<NPCSoundsRec> g_npcSoundsDB;

// STATIC: 0x00D99B98
WowClientDB<NumTalentsAtLevelRec> g_numTalentsAtLevelDB;

// STATIC: 0x00D99BB4
WowClientDB<NamesProfanityRec> g_namesProfanityDB;

// STATIC: 0x00D99BD0
WowClientDB<NamesReservedRec> g_namesReservedDB;

// STATIC: 0x00D99BEC
WowClientDB<OverrideSpellDataRec> g_overrideSpellDataDB;

// STATIC: 0x00D99C08
WowClientDB<PackageRec> g_packageDB;

// STATIC: 0x00D99C24
WowClientDB<PageTextMaterialRec> g_pageTextMaterialDB;

// STATIC: 0x00D99C40
WowClientDB<PaperDollItemFrameRec> g_paperDollItemFrameDB;

// STATIC: 0x00D99C5C
WowClientDB<ParticleColorRec> g_particleColorDB;

// STATIC: 0x00D99C78
WowClientDB<PhaseRec> g_phaseDB;

// STATIC: 0x00D99C94
WowClientDB<PhaseShiftZoneSoundsRec> g_phaseShiftZoneSoundsDB;

// STATIC: 0x00D99CB0
WowClientDB<PhaseXPhaseGroupRec> g_phaseXPhaseGroupDB;

// STATIC: 0x00D99CCC
WowClientDB<PlayerConditionRec> g_playerConditionDB;

// STATIC: 0x00D99CE8
WowClientDB<PowerDisplayRec> g_powerDisplayDB;

// STATIC: 0x00D99D04
WowClientDB<PvpDifficultyRec> g_pvpDifficultyDB;

// STATIC: 0x00D99D20
WowClientDB<QuestFactionRewardRec> g_questFactionRewardDB;

// STATIC: 0x00D99D3C
WowClientDB<QuestInfoRec> g_questInfoDB;

// STATIC: 0x00D99D58
WowClientDB<QuestPOIBlobRec> g_questPOIBlobDB;

// STATIC: 0x00D99D74
WowClientDB<QuestPOIPointRec> g_questPOIPointDB;

// STATIC: 0x00D99D90
WowClientDB<QuestSortRec> g_questSortDB;

// STATIC: 0x00D99DAC
WowClientDB<QuestXPRec> g_questXPDB;

// STATIC: 0x00D99DC8
WowClientDB<ResistancesRec> g_resistancesDB;

// STATIC: 0x00D99DE4
WowClientDB<RandPropPointsRec> g_randPropPointsDB;

// STATIC: 0x00D99E00
WowClientDB<ResearchFieldRec> g_researchFieldDB;

// STATIC: 0x00D99E1C
WowClientDB<ResearchBranchRec> g_researchBranchDB;

// STATIC: 0x00D99E38
WowClientDB<ResearchProjectRec> g_researchProjectDB;

// STATIC: 0x00D99E54
WowClientDB<ResearchSiteRec> g_researchSiteDB;

// STATIC: 0x00D99E70
WowClientDB<ScalingStatDistributionRec> g_scalingStatDistributionDB;

// STATIC: 0x00D99E8C
WowClientDB<ScalingStatValuesRec> g_scalingStatValuesDB;

// STATIC: 0x00D99EA8
WowClientDB<ScreenEffectRec> g_screenEffectDB;

// STATIC: 0x00D99EC4
WowClientDB<ScreenLocationRec> g_screenLocationDB;

// STATIC: 0x00D99EE0
WowClientDB<ServerMessagesRec> g_serverMessagesDB;

// STATIC: 0x00D99EFC
WowClientDB<SkillLineAbilityRec> g_skillLineAbilityDB;

// STATIC: 0x00D99F18
WowClientDB<SkillLineAbilitySortedSpellRec> g_skillLineAbilitySortedSpellDB;

// STATIC: 0x00D99F34
WowClientDB<SkillLineCategoryRec> g_skillLineCategoryDB;

// STATIC: 0x00D99F50
WowClientDB<SkillLineRec> g_skillLineDB;

// STATIC: 0x00D99F6C
WowClientDB<SkillRaceClassInfoRec> g_skillRaceClassInfoDB;

// STATIC: 0x00D99F88
WowClientDB<SkillTiersRec> g_skillTiersDB;

// STATIC: 0x00D99FA4
WowClientDB<SoundAmbienceRec> g_soundAmbienceDB;

// STATIC: 0x00D99FC0
WowClientDB<SoundAmbienceFlavorRec> g_soundAmbienceFlavorDB;

// STATIC: 0x00D99FDC
WowClientDB<SoundProviderPreferencesRec> g_soundProviderPreferencesDB;

// STATIC: 0x00D99FF8
WowClientDB<SpamMessagesRec> g_spamMessagesDB;

// STATIC: 0x00D9A014
WowClientDB<SpellActivationOverlayRec> g_spellActivationOverlayDB;

// STATIC: 0x00D9A030
WowClientDB<SpellAuraOptionsRec> g_spellAuraOptionsDB;

// STATIC: 0x00D9A04C
WowClientDB<SpellAuraRestrictionsRec> g_spellAuraRestrictionsDB;

// STATIC: 0x00D9A068
WowClientDB<SpellAuraVisibilityRec> g_spellAuraVisibilityDB;

// STATIC: 0x00D9A084
WowClientDB<SpellAuraVisXTalentTabRec> g_spellAuraVisXTalentTabDB;

// STATIC: 0x00D9A0A0
WowClientDB<SpellCastingRequirementsRec> g_spellCastingRequirementsDB;

// STATIC: 0x00D9A0BC
WowClientDB<SpellCastTimesRec> g_spellCastTimesDB;

// STATIC: 0x00D9A0D8
WowClientDB<SpellCategoriesRec> g_spellCategoriesDB;

// STATIC: 0x00D9A0F4
WowClientDB<SpellCategoryRec> g_spellCategoryDB;

// STATIC: 0x00D9A110
WowClientDB<SpellChainEffectsRec> g_spellChainEffectsDB;

// STATIC: 0x00D9A12C
WowClientDB<SpellClassOptionsRec> g_spellClassOptionsDB;

// STATIC: 0x00D9A148
WowClientDB<SpellCooldownsRec> g_spellCooldownsDB;

// STATIC: 0x00D9A164
WowClientDB<SpellDescriptionVariablesRec> g_spellDescriptionVariablesDB;

// STATIC: 0x00D9A180
WowClientDB<SpellDifficultyRec> g_spellDifficultyDB;

// STATIC: 0x00D9A19C
WowClientDB<SpellDispelTypeRec> g_spellDispelTypeDB;

// STATIC: 0x00D9A1B8
WowClientDB<SpellDurationRec> g_spellDurationDB;

// STATIC: 0x00D9A1D4
WowClientDB<SpellEffectRec> g_spellEffectDB;

// STATIC: 0x00D9A1F0
WowClientDB<SpellEffectCameraShakesRec> g_spellEffectCameraShakesDB;

// STATIC: 0x00D9A20C
WowClientDB<SpellEquippedItemsRec> g_spellEquippedItemsDB;

// STATIC: 0x00D9A228
WowClientDB<SpellFlyoutRec> g_spellFlyoutDB;

// STATIC: 0x00D9A244
WowClientDB<SpellFlyoutItemRec> g_spellFlyoutItemDB;

// STATIC: 0x00D9A260
WowClientDB<SpellFocusObjectRec> g_spellFocusObjectDB;

// STATIC: 0x00D9A27C
WowClientDB<SpellIconRec> g_spellIconDB;

// STATIC: 0x00D9A298
WowClientDB<SpellInterruptsRec> g_spellInterruptsDB;

// STATIC: 0x00D9A2B4
WowClientDB<SpellItemEnchantmentRec> g_spellItemEnchantmentDB;

// STATIC: 0x00D9A2D0
WowClientDB<SpellItemEnchantmentConditionRec> g_spellItemEnchantmentConditionDB;

// STATIC: 0x00D9A2EC
WowClientDB<SpellLevelsRec> g_spellLevelsDB;

// STATIC: 0x00D9A308
WowClientDB<SpellMechanicRec> g_spellMechanicDB;

// STATIC: 0x00D9A324
WowClientDB<SpellMissileRec> g_spellMissileDB;

// STATIC: 0x00D9A340
WowClientDB<SpellMissileMotionRec> g_spellMissileMotionDB;

// STATIC: 0x00D9A35C
WowClientDB<SpellPowerRec> g_spellPowerDB;

// STATIC: 0x00D9A378
WowClientDB<SpellRadiusRec> g_spellRadiusDB;

// STATIC: 0x00D9A394
WowClientDB<SpellRangeRec> g_spellRangeDB;

// STATIC: 0x00D9A3B0
WowClientDB<SpellRuneCostRec> g_spellRuneCostDB;

// STATIC: 0x00D9A3CC
WowClientDB<SpellReagentsRec> g_spellReagentsDB;

// STATIC: 0x00D9A3E8
WowClientDB<SpellRec> g_spellDB;

// STATIC: 0x00D9A404
WowClientDB<SpellScalingRec> g_spellScalingDB;

// STATIC: 0x00D9A420
WowClientDB<SpellShapeshiftRec> g_spellShapeshiftDB;

// STATIC: 0x00D9A43C
WowClientDB<SpellShapeshiftFormRec> g_spellShapeshiftFormDB;

// STATIC: 0x00D9A458
WowClientDB<SpellSpecialUnitEffectRec> g_spellSpecialUnitEffectDB;

// STATIC: 0x00D9A474
WowClientDB<SpellTargetRestrictionsRec> g_spellTargetRestrictionsDB;

// STATIC: 0x00D9A490
WowClientDB<SpellTotemsRec> g_spellTotemsDB;

// STATIC: 0x00D9A4AC
WowClientDB<SpellVisualEffectNameRec> g_spellVisualEffectNameDB;

// STATIC: 0x00D9A4C8
WowClientDB<SpellVisualKitRec> g_spellVisualKitDB;

// STATIC: 0x00D9A4E4
WowClientDB<SpellVisualKitAreaModelRec> g_spellVisualKitAreaModelDB;

// STATIC: 0x00D9A500
WowClientDB<SpellVisualKitModelAttachRec> g_spellVisualKitModelAttachDB;

// STATIC: 0x00D9A51C
WowClientDB<SpellVisualRec> g_spellVisualDB;

// STATIC: 0x00D9A538
WowClientDB<StationeryRec> g_stationeryDB;

// STATIC: 0x00D9A554
WowClientDB<StringLookupsRec> g_stringLookupsDB;

// STATIC: 0x00D9A570
WowClientDB<SummonPropertiesRec> g_summonPropertiesDB;

// STATIC: 0x00D9A58C
WowClientDB<TalentRec> g_talentDB;

// STATIC: 0x00D9A5A8
WowClientDB<TalentTabRec> g_talentTabDB;

// STATIC: 0x00D9A5C4
WowClientDB<TalentTreePrimarySpellsRec> g_talentTreePrimarySpellsDB;

// STATIC: 0x00D9A5E0
WowClientDB<TaxiNodesRec> g_taxiNodesDB;

// STATIC: 0x00D9A5FC
WowClientDB<TaxiPathNodeRec> g_taxiPathNodeDB;

// STATIC: 0x00D9A618
WowClientDB<TaxiPathRec> g_taxiPathDB;

// STATIC: 0x00D9A634
WowClientDB<TerrainTypeSoundsRec> g_terrainTypeSoundsDB;

// STATIC: 0x00D9A650
WowClientDB<TotemCategoryRec> g_totemCategoryDB;

// STATIC: 0x00D9A66C
WowClientDB<TransportAnimationRec> g_transportAnimationDB;

// STATIC: 0x00D9A688
WowClientDB<TransportPhysicsRec> g_transportPhysicsDB;

// STATIC: 0x00D9A6A4
WowClientDB<TransportRotationRec> g_transportRotationDB;

// STATIC: 0x00D9A6C0
WowClientDB<UnitBloodLevelsRec> g_unitBloodLevelsDB;

// STATIC: 0x00D9A6DC
WowClientDB<UnitBloodRec> g_unitBloodDB;

// STATIC: 0x00D9A6F8
WowClientDB<UnitPowerBarRec> g_unitPowerBarDB;

// STATIC: 0x00D9A714
WowClientDB<VehicleRec> g_vehicleDB;

// STATIC: 0x00D9A730
WowClientDB<VehicleSeatRec> g_vehicleSeatDB;

// STATIC: 0x00D9A74C
WowClientDB<VehicleUIIndicatorRec> g_vehicleUIIndicatorDB;

// STATIC: 0x00D9A768
WowClientDB<VehicleUIIndSeatRec> g_vehicleUIIndSeatDB;

// STATIC: 0x00D9A784
WowClientDB<VocalUISoundsRec> g_vocalUISoundsDB;

// STATIC: 0x00D9A7A0
WowClientDB<World_PVP_AreaRec> g_world_PVP_AreaDB;

// STATIC: 0x00D9A7BC
WowClientDB<WeaponImpactSoundsRec> g_weaponImpactSoundsDB;

// STATIC: 0x00D9A7D8
WowClientDB<WeaponSwingSounds2Rec> g_weaponSwingSounds2DB;

// STATIC: 0x00D9A7F4
WowClientDB<WorldMapAreaRec> g_worldMapAreaDB;

// STATIC: 0x00D9A810
WowClientDB<WorldMapContinentRec> g_worldMapContinentDB;

// STATIC: 0x00D9A82C
WowClientDB<WorldMapOverlayRec> g_worldMapOverlayDB;

// STATIC: 0x00D9A848
WowClientDB<WorldMapTransformsRec> g_worldMapTransformsDB;

// STATIC: 0x00D9A864
WowClientDB<WorldSafeLocsRec> g_worldSafeLocsDB;

// STATIC: 0x00D9A880
WowClientDB<WorldStateUIRec> g_worldStateUIDB;

// STATIC: 0x00D9A89C
WowClientDB<ZoneIntroMusicTableRec> g_zoneIntroMusicTableDB;

// STATIC: 0x00D9A8B8
WowClientDB<ZoneMusicRec> g_zoneMusicDB;

// STATIC: 0x00D9A8D4
WowClientDB<WorldStateZoneSoundsRec> g_worldStateZoneSoundsDB;

// STATIC: 0x00D9A8F0
WowClientDB<WorldChunkSoundsRec> g_worldChunkSoundsDB;

// STATIC: 0x00D9A90C
WowClientDB<ObjectEffectRec> g_objectEffectDB;

// STATIC: 0x00D9A928
WowClientDB<ObjectEffectGroupRec> g_objectEffectGroupDB;

// STATIC: 0x00D9A944
WowClientDB<ObjectEffectModifierRec> g_objectEffectModifierDB;

// STATIC: 0x00D9A960
WowClientDB<ObjectEffectPackageRec> g_objectEffectPackageDB;

// STATIC: 0x00D9A97C
WowClientDB<ObjectEffectPackageElemRec> g_objectEffectPackageElemDB;

// STATIC: 0x00D9A998
WowClientDB<SoundFilterRec> g_soundFilterDB;

// STATIC: 0x00D9A9B4
WowClientDB<SoundFilterElemRec> g_soundFilterElemDB;

// STATIC: %0x00C99138
WowClientDB<DeclinedWordRec> g_declinedWordDB;

// STATIC: %0x00C99154
WowClientDB<DeclinedWordCasesRec> g_declinedWordCasesDB;

// STATIC: 0x00D98648
WowClientDB2<ItemRec> g_itemDB;

// STATIC: 0x00D986E8
WowClientDB2<ItemRecSparse> g_itemSparseDB;

// STATIC: 0x00D98788
WowClientDB2<ItemCurrencyCostRec> g_itemCurrencyCostDB;

// STATIC: 0x00D98828
WowClientDB2<ItemExtendedCostRec> g_itemExtendedCostDB;

// STATIC: 0x00D988C8
WowClientDB2<KeyChainRec> g_keyChainDB;

// STATIC: 0x00C9AD64
WowClientDB2_Base* g_clientDB2Tables[] = { &g_keyChainDB, NULL };

// STATIC: 0x00C9AD6C
WowClientDB2_Base* g_clientDB2GameTables[] = { &g_itemDB, &g_itemSparseDB, &g_itemCurrencyCostDB, &g_itemExtendedCostDB, NULL };

WowClientDB<PetPersonalityRec> g_petPersonalityDB;

// STATIC: 0x00C9AD80
static ClientDBTable s_clientDBGameTables[] = {
    { &g_achievementDB, &AchievementRec::s_meta },
    { &g_achievement_CriteriaDB, &Achievement_CriteriaRec::s_meta },
    { &g_achievement_CategoryDB, &Achievement_CategoryRec::s_meta },
    { &g_areaGroupDB, &AreaGroupRec::s_meta },
    { &g_areaPOIDB, &AreaPOIRec::s_meta },
    { &g_areaPOISortedWorldStateDB, &AreaPOISortedWorldStateRec::s_meta },
    { &g_areaAssignmentDB, &AreaAssignmentRec::s_meta },
    { &g_areaTriggerDB, &AreaTriggerRec::s_meta },
    { &g_armorLocationDB, &ArmorLocationRec::s_meta },
    { &g_auctionHouseDB, &AuctionHouseRec::s_meta },
    { &g_bankBagSlotPricesDB, &BankBagSlotPricesRec::s_meta },
    { &g_barberShopStyleDB, &BarberShopStyleRec::s_meta },
    { &g_battlemasterListDB, &BattlemasterListRec::s_meta },
    { &g_cameraModeDB, &CameraModeRec::s_meta },
    { &g_cameraShakesDB, &CameraShakesRec::s_meta },
    { &g_castableRaidBuffsDB, &CastableRaidBuffsRec::s_meta },
    { &g_charTitlesDB, &CharTitlesRec::s_meta },
    { &g_chatChannelsDB, &ChatChannelsRec::s_meta },
    { &g_chrClassesXPowerTypesDB, &ChrClassesXPowerTypesRec::s_meta },
    { &g_cinematicCameraDB, &CinematicCameraRec::s_meta },
    { &g_cinematicSequencesDB, &CinematicSequencesRec::s_meta },
    { &g_creatureImmunitiesDB, &CreatureImmunitiesRec::s_meta },
    { &g_creatureMovementInfoDB, &CreatureMovementInfoRec::s_meta },
    { &g_creatureSoundDataDB, &CreatureSoundDataRec::s_meta },
    { &g_creatureSpellDataDB, &CreatureSpellDataRec::s_meta },
    { &g_creatureTypeDB, &CreatureTypeRec::s_meta },
    { &g_currencyTypesDB, &CurrencyTypesRec::s_meta },
    { &g_currencyCategoryDB, &CurrencyCategoryRec::s_meta },
    { &g_danceMovesDB, &DanceMovesRec::s_meta },
    { &g_deathThudLookupsDB, &DeathThudLookupsRec::s_meta },
    { &g_destructibleModelDataDB, &DestructibleModelDataRec::s_meta },
    { &g_dungeonEncounterDB, &DungeonEncounterRec::s_meta },
    { &g_dungeonMapDB, &DungeonMapRec::s_meta },
    { &g_dungeonMapChunkDB, &DungeonMapChunkRec::s_meta },
    { &g_durabilityCostsDB, &DurabilityCostsRec::s_meta },
    { &g_durabilityQualityDB, &DurabilityQualityRec::s_meta },
    { &g_emotesDB, &EmotesRec::s_meta },
    { &g_emotesTextDataDB, &EmotesTextDataRec::s_meta },
    { &g_emotesTextSoundDB, &EmotesTextSoundRec::s_meta },
    { &g_emotesTextDB, &EmotesTextRec::s_meta },
    { &g_environmentalDamageDB, &EnvironmentalDamageRec::s_meta },
    { &g_exhaustionDB, &ExhaustionRec::s_meta },
    { &g_factionDB, &FactionRec::s_meta },
    { &g_footstepTerrainLookupDB, &FootstepTerrainLookupRec::s_meta },
    { &g_gameObjectArtKitDB, &GameObjectArtKitRec::s_meta },
    { &g_gameObjectDisplayInfoDB, &GameObjectDisplayInfoRec::s_meta },
    { &g_gameTablesDB, &GameTablesRec::s_meta },
    { &g_gemPropertiesDB, &GemPropertiesRec::s_meta },
    { &g_glyphPropertiesDB, &GlyphPropertiesRec::s_meta },
    { &g_glyphSlotDB, &GlyphSlotRec::s_meta },
    { &g_gmSurveyAnswersDB, &GMSurveyAnswersRec::s_meta },
    { &g_gmSurveyCurrentSurveyDB, &GMSurveyCurrentSurveyRec::s_meta },
    { &g_gmSurveyQuestionsDB, &GMSurveyQuestionsRec::s_meta },
    { &g_gmSurveySurveysDB, &GMSurveySurveysRec::s_meta },
    { &g_gmTicketCategoryDB, &GMTicketCategoryRec::s_meta },
    { &g_gtBarberShopCostBaseDB, &gtBarberShopCostBaseRec::s_meta },
    { &g_gtCombatRatingsDB, &gtCombatRatingsRec::s_meta },
    { &g_gtChanceToMeleeCritDB, &gtChanceToMeleeCritRec::s_meta },
    { &g_gtChanceToMeleeCritBaseDB, &gtChanceToMeleeCritBaseRec::s_meta },
    { &g_gtChanceToSpellCritDB, &gtChanceToSpellCritRec::s_meta },
    { &g_gtChanceToSpellCritBaseDB, &gtChanceToSpellCritBaseRec::s_meta },
    { &g_gtNPCManaCostScalerDB, &gtNPCManaCostScalerRec::s_meta },
    { &g_gtOCTBaseHPByClassDB, &gtOCTBaseHPByClassRec::s_meta },
    { &g_gtOCTBaseMPByClassDB, &gtOCTBaseMPByClassRec::s_meta },
    { &g_gtOCTClassCombatRatingScalarDB, &gtOCTClassCombatRatingScalarRec::s_meta },
    { &g_gtOCTHpPerStaminaDB, &gtOCTHpPerStaminaRec::s_meta },
    { &g_gtOCTRegenMPDB, &gtOCTRegenMPRec::s_meta },
    { &g_gtRegenMPPerSptDB, &gtRegenMPPerSptRec::s_meta },
    { &g_gtSpellScalingDB, &gtSpellScalingRec::s_meta },
    { &g_guildPerkSpellsDB, &GuildPerkSpellsRec::s_meta },
    { &g_holidayDescriptionsDB, &HolidayDescriptionsRec::s_meta },
    { &g_holidayNamesDB, &HolidayNamesRec::s_meta },
    { &g_holidaysDB, &HolidaysRec::s_meta },
    { &g_importPriceArmorDB, &ImportPriceArmorRec::s_meta },
    { &g_importPriceQualityDB, &ImportPriceQualityRec::s_meta },
    { &g_importPriceShieldDB, &ImportPriceShieldRec::s_meta },
    { &g_importPriceWeaponDB, &ImportPriceWeaponRec::s_meta },
    { &g_itemArmorQualityDB, &ItemArmorQualityRec::s_meta },
    { &g_itemArmorTotalDB, &ItemArmorTotalRec::s_meta },
    { &g_itemArmorShieldDB, &ItemArmorShieldRec::s_meta },
    { &g_itemBagFamilyDB, &ItemBagFamilyRec::s_meta },
    { &g_itemDamageAmmoDB, &ItemDamageAmmoRec::s_meta },
    { &g_itemDamageOneHandDB, &ItemDamageOneHandRec::s_meta },
    { &g_itemDamageOneHandCasterDB, &ItemDamageOneHandCasterRec::s_meta },
    { &g_itemDamageRangedDB, &ItemDamageRangedRec::s_meta },
    { &g_itemDamageThrownDB, &ItemDamageThrownRec::s_meta },
    { &g_itemDamageTwoHandDB, &ItemDamageTwoHandRec::s_meta },
    { &g_itemDamageTwoHandCasterDB, &ItemDamageTwoHandCasterRec::s_meta },
    { &g_itemDamageWandDB, &ItemDamageWandRec::s_meta },
    { &g_itemDisenchantLootDB, &ItemDisenchantLootRec::s_meta },
    { &g_itemGroupSoundsDB, &ItemGroupSoundsRec::s_meta },
    { &g_itemLimitCategoryDB, &ItemLimitCategoryRec::s_meta },
    { &g_itemNameDescriptionDB, &ItemNameDescriptionRec::s_meta },
    { &g_itemPetFoodDB, &ItemPetFoodRec::s_meta },
    { &g_itemPriceBaseDB, &ItemPriceBaseRec::s_meta },
    { &g_itemPurchaseGroupDB, &ItemPurchaseGroupRec::s_meta },
    { &g_itemRandomPropertiesDB, &ItemRandomPropertiesRec::s_meta },
    { &g_itemRandomSuffixDB, &ItemRandomSuffixRec::s_meta },
    { &g_itemReforgeDB, &ItemReforgeRec::s_meta },
    { &g_itemSetDB, &ItemSetRec::s_meta },
    { &g_itemSubClassMaskDB, &ItemSubClassMaskRec::s_meta },
    { &g_journalEncounterCreatureDB, &JournalEncounterCreatureRec::s_meta },
    { &g_journalEncounterItemDB, &JournalEncounterItemRec::s_meta },
    { &g_journalEncounterDB, &JournalEncounterRec::s_meta },
    { &g_journalEncounterSectionDB, &JournalEncounterSectionRec::s_meta },
    { &g_journalInstanceDB, &JournalInstanceRec::s_meta },
    { &g_languageWordsDB, &LanguageWordsRec::s_meta },
    { &g_languagesDB, &LanguagesRec::s_meta },
    { &g_lfgDungeonExpansionDB, &LfgDungeonExpansionRec::s_meta },
    { &g_lfgDungeonGroupDB, &LfgDungeonGroupRec::s_meta },
    { &g_lfgDungeonsGroupingMapDB, &LfgDungeonsGroupingMapRec::s_meta },
    { &g_lfgDungeonsDB, &LfgDungeonsRec::s_meta },
    { &g_loadingScreenTaxiSplinesDB, &LoadingScreenTaxiSplinesRec::s_meta },
    { &g_lockDB, &LockRec::s_meta },
    { &g_lockTypeDB, &LockTypeRec::s_meta },
    { &g_mailTemplateDB, &MailTemplateRec::s_meta },
    { &g_mapDifficultyDB, &MapDifficultyRec::s_meta },
    { &g_materialDB, &MaterialRec::s_meta },
    { &g_mountCapabilityDB, &MountCapabilityRec::s_meta },
    { &g_mountTypeDB, &MountTypeRec::s_meta },
    { &g_npcSoundsDB, &NPCSoundsRec::s_meta },
    { &g_numTalentsAtLevelDB, &NumTalentsAtLevelRec::s_meta },
    { &g_overrideSpellDataDB, &OverrideSpellDataRec::s_meta },
    { &g_packageDB, &PackageRec::s_meta },
    { &g_pageTextMaterialDB, &PageTextMaterialRec::s_meta },
    { &g_paperDollItemFrameDB, &PaperDollItemFrameRec::s_meta },
    { &g_phaseDB, &PhaseRec::s_meta },
    { &g_phaseXPhaseGroupDB, &PhaseXPhaseGroupRec::s_meta },
    { &g_playerConditionDB, &PlayerConditionRec::s_meta },
    { &g_powerDisplayDB, &PowerDisplayRec::s_meta },
    { &g_pvpDifficultyDB, &PvpDifficultyRec::s_meta },
    { &g_questFactionRewardDB, &QuestFactionRewardRec::s_meta },
    { &g_questInfoDB, &QuestInfoRec::s_meta },
    { &g_questPOIBlobDB, &QuestPOIBlobRec::s_meta },
    { &g_questPOIPointDB, &QuestPOIPointRec::s_meta },
    { &g_questSortDB, &QuestSortRec::s_meta },
    { &g_questXPDB, &QuestXPRec::s_meta },
    { &g_researchBranchDB, &ResearchBranchRec::s_meta },
    { &g_researchFieldDB, &ResearchFieldRec::s_meta },
    { &g_researchProjectDB, &ResearchProjectRec::s_meta },
    { &g_researchSiteDB, &ResearchSiteRec::s_meta },
    { &g_randPropPointsDB, &RandPropPointsRec::s_meta },
    { &g_scalingStatDistributionDB, &ScalingStatDistributionRec::s_meta },
    { &g_scalingStatValuesDB, &ScalingStatValuesRec::s_meta },
    { &g_screenEffectDB, &ScreenEffectRec::s_meta },
    { &g_screenLocationDB, &ScreenLocationRec::s_meta },
    { &g_serverMessagesDB, &ServerMessagesRec::s_meta },
    { &g_skillLineAbilityDB, &SkillLineAbilityRec::s_meta },
    { &g_skillLineAbilitySortedSpellDB, &SkillLineAbilitySortedSpellRec::s_meta },
    { &g_skillLineCategoryDB, &SkillLineCategoryRec::s_meta },
    { &g_skillLineDB, &SkillLineRec::s_meta },
    { &g_skillRaceClassInfoDB, &SkillRaceClassInfoRec::s_meta },
    { &g_skillTiersDB, &SkillTiersRec::s_meta },
    { &g_soundAmbienceDB, &SoundAmbienceRec::s_meta },
    { &g_soundAmbienceFlavorDB, &SoundAmbienceFlavorRec::s_meta },
    { &g_spellActivationOverlayDB, &SpellActivationOverlayRec::s_meta },
    { &g_spellAuraOptionsDB, &SpellAuraOptionsRec::s_meta },
    { &g_spellAuraRestrictionsDB, &SpellAuraRestrictionsRec::s_meta },
    { &g_spellAuraVisibilityDB, &SpellAuraVisibilityRec::s_meta },
    { &g_spellAuraVisXTalentTabDB, &SpellAuraVisXTalentTabRec::s_meta },
    { &g_spellCastingRequirementsDB, &SpellCastingRequirementsRec::s_meta },
    { &g_spellCastTimesDB, &SpellCastTimesRec::s_meta },
    { &g_spellCategoriesDB, &SpellCategoriesRec::s_meta },
    { &g_spellCategoryDB, &SpellCategoryRec::s_meta },
    { &g_spellChainEffectsDB, &SpellChainEffectsRec::s_meta },
    { &g_spellClassOptionsDB, &SpellClassOptionsRec::s_meta },
    { &g_spellCooldownsDB, &SpellCooldownsRec::s_meta },
    { &g_spellDB, &SpellRec::s_meta },
    { &g_spellDescriptionVariablesDB, &SpellDescriptionVariablesRec::s_meta },
    { &g_spellDifficultyDB, &SpellDifficultyRec::s_meta },
    { &g_spellDispelTypeDB, &SpellDispelTypeRec::s_meta },
    { &g_spellDurationDB, &SpellDurationRec::s_meta },
    { &g_spellEffectCameraShakesDB, &SpellEffectCameraShakesRec::s_meta },
    { &g_spellEquippedItemsDB, &SpellEquippedItemsRec::s_meta },
    { &g_spellFlyoutDB, &SpellFlyoutRec::s_meta },
    { &g_spellFlyoutItemDB, &SpellFlyoutItemRec::s_meta },
    { &g_spellFocusObjectDB, &SpellFocusObjectRec::s_meta },
    { &g_spellIconDB, &SpellIconRec::s_meta },
    { &g_spellInterruptsDB, &SpellInterruptsRec::s_meta },
    { &g_spellItemEnchantmentDB, &SpellItemEnchantmentRec::s_meta },
    { &g_spellItemEnchantmentConditionDB, &SpellItemEnchantmentConditionRec::s_meta },
    { &g_spellLevelsDB, &SpellLevelsRec::s_meta },
    { &g_spellMechanicDB, &SpellMechanicRec::s_meta },
    { &g_spellMissileDB, &SpellMissileRec::s_meta },
    { &g_spellMissileMotionDB, &SpellMissileMotionRec::s_meta },
    { &g_spellRadiusDB, &SpellRadiusRec::s_meta },
    { &g_spellRangeDB, &SpellRangeRec::s_meta },
    { &g_spellPowerDB, &SpellPowerRec::s_meta },
    { &g_spellReagentsDB, &SpellReagentsRec::s_meta },
    { &g_spellRuneCostDB, &SpellRuneCostRec::s_meta },
    { &g_spellScalingDB, &SpellScalingRec::s_meta },
    { &g_spellShapeshiftDB, &SpellShapeshiftRec::s_meta },
    { &g_spellShapeshiftFormDB, &SpellShapeshiftFormRec::s_meta },
    { &g_spellSpecialUnitEffectDB, &SpellSpecialUnitEffectRec::s_meta },
    { &g_spellTargetRestrictionsDB, &SpellTargetRestrictionsRec::s_meta },
    { &g_spellTotemsDB, &SpellTotemsRec::s_meta },
    { &g_spellVisualDB, &SpellVisualRec::s_meta },
    { &g_spellVisualKitAreaModelDB, &SpellVisualKitAreaModelRec::s_meta },
    { &g_spellVisualKitModelAttachDB, &SpellVisualKitModelAttachRec::s_meta },
    { &g_stationeryDB, &StationeryRec::s_meta },
    { &g_stringLookupsDB, &StringLookupsRec::s_meta },
    { &g_summonPropertiesDB, &SummonPropertiesRec::s_meta },
    { &g_talentDB, &TalentRec::s_meta },
    { &g_talentTabDB, &TalentTabRec::s_meta },
    { &g_talentTreePrimarySpellsDB, &TalentTreePrimarySpellsRec::s_meta },
    { &g_taxiNodesDB, &TaxiNodesRec::s_meta },
    { &g_taxiPathNodeDB, &TaxiPathNodeRec::s_meta },
    { &g_taxiPathDB, &TaxiPathRec::s_meta },
    { &g_terrainTypeSoundsDB, &TerrainTypeSoundsRec::s_meta },
    { &g_totemCategoryDB, &TotemCategoryRec::s_meta },
    { &g_transportAnimationDB, &TransportAnimationRec::s_meta },
    { &g_transportPhysicsDB, &TransportPhysicsRec::s_meta },
    { &g_transportRotationDB, &TransportRotationRec::s_meta },
    { &g_unitBloodLevelsDB, &UnitBloodLevelsRec::s_meta },
    { &g_unitBloodDB, &UnitBloodRec::s_meta },
    { &g_unitPowerBarDB, &UnitPowerBarRec::s_meta },
    { &g_vehicleDB, &VehicleRec::s_meta },
    { &g_vehicleSeatDB, &VehicleSeatRec::s_meta },
    { &g_vehicleUIIndicatorDB, &VehicleUIIndicatorRec::s_meta },
    { &g_vehicleUIIndSeatDB, &VehicleUIIndSeatRec::s_meta },
    { &g_vocalUISoundsDB, &VocalUISoundsRec::s_meta },
    { &g_world_PVP_AreaDB, &World_PVP_AreaRec::s_meta },
    { &g_weaponImpactSoundsDB, &WeaponImpactSoundsRec::s_meta },
    { &g_weaponSwingSounds2DB, &WeaponSwingSounds2Rec::s_meta },
    { &g_worldMapAreaDB, &WorldMapAreaRec::s_meta },
    { &g_worldMapContinentDB, &WorldMapContinentRec::s_meta },
    { &g_worldMapOverlayDB, &WorldMapOverlayRec::s_meta },
    { &g_worldMapTransformsDB, &WorldMapTransformsRec::s_meta },
    { &g_worldSafeLocsDB, &WorldSafeLocsRec::s_meta },
    { &g_worldStateUIDB, &WorldStateUIRec::s_meta },
    { &g_zoneIntroMusicTableDB, &ZoneIntroMusicTableRec::s_meta },
    { &g_zoneMusicDB, &ZoneMusicRec::s_meta },
    { &g_worldStateZoneSoundsDB, &WorldStateZoneSoundsRec::s_meta },
    { &g_worldChunkSoundsDB, &WorldChunkSoundsRec::s_meta },
    { &g_phaseShiftZoneSoundsDB, &PhaseShiftZoneSoundsRec::s_meta },
    { NULL, NULL },
};

// STATIC: 0x00C9B4E0
static ClientDBTable s_clientDBTables[] = {
    { &g_animKitBoneSetAliasDB, &AnimKitBoneSetAliasRec::s_meta },
    { &g_animKitBoneSetDB, &AnimKitBoneSetRec::s_meta },
    { &g_animKitDB, &AnimKitRec::s_meta },
    { &g_animKitConfigDB, &AnimKitConfigRec::s_meta },
    { &g_animKitConfigBoneSetDB, &AnimKitConfigBoneSetRec::s_meta },
    { &g_bannedAddOnsDB, &BannedAddOnsRec::s_meta },
    { &g_cfg_CategoriesDB, &Cfg_CategoriesRec::s_meta },
    { &g_cfg_ConfigsDB, &Cfg_ConfigsRec::s_meta },
    { &g_charBaseInfoDB, &CharBaseInfoRec::s_meta },
    { &g_charHairGeosetsDB, &CharHairGeosetsRec::s_meta },
    { &g_charSectionsDB, &CharSectionsRec::s_meta },
    { &g_characterFacialHairStylesDB, &CharacterFacialHairStylesRec::s_meta },
    { &g_chatProfanityDB, &ChatProfanityRec::s_meta },
    { &g_chrClassesDB, &ChrClassesRec::s_meta },
    { &g_chrRacesDB, &ChrRacesRec::s_meta },
    { &g_factionGroupDB, &FactionGroupRec::s_meta },
    { &g_factionTemplateDB, &FactionTemplateRec::s_meta },
    { &g_fileDataDB, &FileDataRec::s_meta },
    { &g_itemClassDB, &ItemClassRec::s_meta },
    { &g_itemSubClassDB, &ItemSubClassRec::s_meta },
    { &g_movieDB, &MovieRec::s_meta },
    { &g_movieFileDataDB, &MovieFileDataRec::s_meta },
    { &g_movieVariationDB, &MovieVariationRec::s_meta },
    { &g_namesProfanityDB, &NamesProfanityRec::s_meta },
    { &g_namesReservedDB, &NamesReservedRec::s_meta },
    { &g_resistancesDB, &ResistancesRec::s_meta },
    { &g_soundFilterDB, &SoundFilterRec::s_meta },
    { &g_soundFilterElemDB, &SoundFilterElemRec::s_meta },
    { &g_spamMessagesDB, &SpamMessagesRec::s_meta },
    { &g_soundProviderPreferencesDB, &SoundProviderPreferencesRec::s_meta },
    { NULL, NULL },
};

// STATIC: 0x00C9B5D8
static ClientDBTable s_clientDBGlueTables[] = {
    { &g_animKitPriorityDB, &AnimKitPriorityRec::s_meta },
    { &g_animKitSegmentDB, &AnimKitSegmentRec::s_meta },
    { &g_animReplacementDB, &AnimReplacementRec::s_meta },
    { &g_animReplacementSetDB, &AnimReplacementSetRec::s_meta },
    { &g_charStartOutfitDB, &CharStartOutfitRec::s_meta },
    { &g_creatureDisplayInfoDB, &CreatureDisplayInfoRec::s_meta },
    { &g_creatureDisplayInfoExtraDB, &CreatureDisplayInfoExtraRec::s_meta },
    { &g_creatureFamilyDB, &CreatureFamilyRec::s_meta },
    { &g_creatureModelDataDB, &CreatureModelDataRec::s_meta },
    { &g_gameTipsDB, &GameTipsRec::s_meta },
    { &g_glueScreenEmoteDB, &GlueScreenEmoteRec::s_meta },
    { &g_guildColorBackgroundDB, &GuildColorBackgroundRec::s_meta },
    { &g_guildColorBorderDB, &GuildColorBorderRec::s_meta },
    { &g_guildColorEmblemDB, &GuildColorEmblemRec::s_meta },
    { &g_helmetGeosetVisDataDB, &HelmetGeosetVisDataRec::s_meta },
    { &g_itemDisplayInfoDB, &ItemDisplayInfoRec::s_meta },
    { &g_itemVisualsDB, &ItemVisualsRec::s_meta },
    { &g_itemVisualEffectsDB, &ItemVisualEffectsRec::s_meta },
    { &g_loadingScreensDB, &LoadingScreensRec::s_meta },
    { &g_nameGenDB, &NameGenRec::s_meta },
    { &g_objectEffectDB, &ObjectEffectRec::s_meta },
    { &g_objectEffectGroupDB, &ObjectEffectGroupRec::s_meta },
    { &g_objectEffectModifierDB, &ObjectEffectModifierRec::s_meta },
    { &g_objectEffectPackageDB, &ObjectEffectPackageRec::s_meta },
    { &g_objectEffectPackageElemDB, &ObjectEffectPackageElemRec::s_meta },
    { &g_particleColorDB, &ParticleColorRec::s_meta },
    { &g_spellEffectDB, &SpellEffectRec::s_meta },
    { &g_spellVisualEffectNameDB, &SpellVisualEffectNameRec::s_meta },
    { &g_spellVisualKitDB, &SpellVisualKitRec::s_meta },
    { NULL, NULL },
};

// STATIC: 0x00D97E94
static uint32_t s_hotfixTime;

// STATIC: 0x00EBB998
int32_t g_clientDBWorldTablesLoaded;

// STATIC: 0x00EBB99C
int32_t g_clientDBGlueTablesLoaded;

// PROVEN: 7350848c IDENTICAL/NONE
// OFFSET: 0x00494A60/0x1400BA230
int32_t GetPhysicalDamageClassID() {
    return s_physicalDamageClassID;
}

// PROVEN: c401089c IDENTICAL/NONE
// OFFSET: 0x00494A70/0x1400BA240
uint32_t GetPhysicalDamageClassMask() {
    return 1 << s_physicalDamageClassID;
}

// PROVEN: f05eb75b IDENTICAL/NONE
// OFFSET: 0x00494A80/0x1400BA250
int32_t ClientDBGetUnarmedWeapon() {
    return s_unarmedWeapon->m_subClassID;
}

// OFFSET: #0x006348C0
static void LocateWeaponSubclass() {
    s_weaponItemClass = NULL;

    for (int32_t i = g_itemClassDB.m_numRecords - 1; i >= 0; i--) {
        const ItemClassRec* rec = g_itemClassDB.GetRecordByIndex(i);

        if (rec->m_flags & 1) {
            s_weaponItemClass = rec;
        }
    }
}

// OFFSET: #0x00634940
int32_t ClientDBGetNumWeaponSubClasses() {
    return s_weaponSubclasses.Count();
}

// PROVEN: 056beead IDENTICAL/NONE
// OFFSET: 0x00495350
int32_t GetMapRaidDuration(int32_t mapId, uint32_t difficulty) {
    MapDifficultyRec* entry = GetMapDifficultyRec(mapId, difficulty, NULL);

    if (entry) {
        return entry->m_raidDuration;
    }

    return 0;
}

// OFFSET: #0x00634AE0
void InitResistances() {
    uint32_t count = g_resistancesDB.m_numRecords;
    s_physicalDamageClassID = -1;

    if (count != 7) {
        ConsolePrintf("Warning: The Resistances table has the wrong number of entries");

        if (count > 7) {
            count = 7;
        }
    }

    s_resistances.SetCount(count);

    for (int32_t i = 0; static_cast<uint32_t>(i) < count; i++) {
        const ResistancesRec* rec = g_resistancesDB.GetRecordByIndex(i);

        if (rec->m_flags & 1) {
            s_physicalDamageClassID = i;
        }

        s_resistances[i] = rec;
    }
}

// OFFSET: #0x00634BA0
void InitTerrainTypeFlags() {
    uint32_t count = g_terrainTypeDB.m_maxID + 1;
    s_terrainTypeSoundIDs.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        s_terrainTypeSoundIDs[i] = 0;

        const TerrainTypeRec* rec = g_terrainTypeDB.GetRecord(i);

        if (rec) {
            s_terrainTypeSoundIDs[i] = rec->m_soundID;
        }
    }
}

// OFFSET: 0x00494A90/0x1400BA260
static void ClientDBLoadTables() {
    char path[260];

    WowClientDB2_Base::GetCacheDir(path, "Hotfix.tbl", false);
    HOSFILE file = OsCreateFile(path, 0x80000000, 1, 3, 0x80, 0x3F3F3F3F);

    if (file != OS_INVALID_FILE) {
        uint32_t buffer[4];
        uint32_t bytesRead;
        WowClientDB2_Base* table = NULL;
        uint32_t tableHash = 0;

        OsReadFile(file, buffer, 16, &bytesRead);

        if (bytesRead == 16 && buffer[0] == 'HTFX' && buffer[1] == 1 && buffer[3] == 15595) {
            s_hotfixTime = buffer[2];
            OsReadFile(file, buffer, 12, &bytesRead);

            while (bytesRead == 12) {
                if (!table || buffer[0] != tableHash) {
                    table = WowClientDB2_Base::GetTableByHash(buffer[0]);
                    tableHash = buffer[0];
                }

                if (table) {
                    table->RestoreHotfix(buffer[1], buffer[2]);
                }

                OsReadFile(file, buffer, 12, &bytesRead);
            }
        }

        OsCloseFile(file);
    }

    for (WowClientDB2_Base** table = g_clientDB2Tables; *table; table++) {
        (*table)->Load(s_hotfixTime);
    }

    for (ClientDBTable* table = s_clientDBTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }

    for (ClientDBTable* table = s_clientDBGlueTables; table->m_db; table++) {
        AsyncFileReadCreateObject(table->m_meta->m_filename, 5, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
    }

    for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
        AsyncFileReadCreateObject(table->m_meta->m_filename, 5, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
    }
}

// PROVEN: 3474e97d IDENTICAL/NONE
// OFFSET: 0x00494D60
void ClientDBInitializeGlue() {
    for (ClientDBTable* table = s_clientDBGlueTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }
}

// PROVEN: 6369db2d IDENTICAL/NONE
// OFFSET: 0x00495070
void ClientDBUnloadTables() {
    char path[260];
    uint32_t hotfixTime = 0;

    WowClientDB2_Base::GetCacheDir(path, "Hotfix.tbl", false);
    HOSFILE file = OsCreateFile(path, 0x40000000, 1, 2, 0x80, 0x3F3F3F3F);

    if (file != OS_INVALID_FILE) {
        uint32_t now = static_cast<uint32_t>(time(NULL));
        uint32_t header[4] = { 'HTFX', 1, now, 15595 };
        uint32_t bytesWritten;

        OsWriteFile(file, header, 16, &bytesWritten);

        if (bytesWritten == 16) {
            hotfixTime = now;

            for (WowClientDB2_Base** table = g_clientDB2Tables; *table; table++) {
                (*table)->WriteHotfixes(file);
            }

            if (g_clientDBWorldTablesLoaded == 1) {
                for (WowClientDB2_Base** table = g_clientDB2GameTables; *table; table++) {
                    (*table)->WriteHotfixes(file);
                }
            }
        }

        OsCloseFile(file);
    }

    for (ClientDBTable* table = s_clientDBTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }

    for (WowClientDB2_Base** table = g_clientDB2Tables; *table; table++) {
        (*table)->Save(hotfixTime);
        (*table)->Unload();
    }

    if (g_clientDBGlueTablesLoaded == 1) {
        for (ClientDBTable* table = s_clientDBGlueTables; table->m_db; table++) {
            table->m_db->Unload(table->m_meta);
        }

        g_clientDBGlueTablesLoaded = 0;
    }

    if (g_clientDBWorldTablesLoaded == 1) {
        for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
            table->m_db->Unload(table->m_meta);
        }

        for (WowClientDB2_Base** table = g_clientDB2GameTables; *table; table++) {
            (*table)->Save(hotfixTime);
            (*table)->Unload();
        }

        g_clientDBWorldTablesLoaded = 0;
    }
}

// OFFSET: 0x004953B0/0x1400BAE80
const SpellEffectRec* SpellGetEffect(uint32_t spellID, uint32_t index) {
    if (spellID >= s_spellEffects.Count()) {
        return NULL;
    }

    TSGrowableArray<const SpellEffectRec*>& effects = s_spellEffects[spellID];
    uint32_t count = effects.Count();

    if (count >= 3) {
        count = 3;
    }

    if (index >= count) {
        return NULL;
    }

    return effects[index];
}

// OFFSET: 0x004953F0/0x1400BAEC0
const TSGrowableArray<const SpellEffectRec*>* SpellGetEffects(uint32_t spellID, uint32_t& count) {
    if (spellID >= s_spellEffects.Count()) {
        count = 0;
        return NULL;
    }

    count = s_spellEffects[spellID].Count();

    if (!count) {
        return NULL;
    }

    if (count >= 3) {
        count = 3;
    }

    return &s_spellEffects[spellID];
}

// OFFSET: 0x00495440/0x1400BAF10
uint32_t SpellGetEffectCount(uint32_t spellID) {
    if (spellID < s_spellEffects.Count()) {
        return s_spellEffects[spellID].Count();
    }

    return 0;
}

// OFFSET: 0x00495910/0x1400BB530
const SpellActivationOverlayRec* GetSpellActivationOverlayRec(int32_t spellID) {
    SPELLACTIVATIONOVERLAY* overlay = s_spellActivationOverlays.Ptr(spellID, HASHKEY_NONE());

    if (!overlay) {
        return NULL;
    }

    return overlay->m_rec;
}

// OFFSET: 0x00496010
void InitSpellEffects() {
    s_spellEffects.SetCount(g_spellDB.m_maxID + 1);

    for (uint32_t i = 0; i < s_spellEffects.Count(); i++) {
        s_spellEffects[i].m_count = 0;
    }

    int32_t lastSpellID = 0;

    for (int32_t i = 0; i < g_spellEffectDB.m_numRecords; i++) {
        const SpellEffectRec* rec = g_spellEffectDB.GetRecordByIndex(i);
        int32_t spellID = rec->m_spellID;

        if (!g_spellDB.GetRecord(spellID)) {
            continue;
        }

        if (spellID != lastSpellID) {
            TSGrowableArray<const SpellEffectRec*>& effects = s_spellEffects[spellID];
            effects.SetCount(rec->m_effectIndex + 1);
            lastSpellID = rec->m_spellID;

            for (int32_t j = 0; j < rec->m_effectIndex; j++) {
                s_spellEffects[spellID][j] = NULL;
            }
        }

        s_spellEffects[spellID][rec->m_effectIndex] = rec;
    }
}

// OFFSET: 0x00496660
void InitSpellActivationOverlays() {
    for (int32_t i = 0; i < g_spellActivationOverlayDB.m_numRecords; i++) {
        const SpellActivationOverlayRec* rec = g_spellActivationOverlayDB.GetRecordByIndex(i);
        SPELLACTIVATIONOVERLAY* overlay = s_spellActivationOverlays.New(rec->m_spellID, HASHKEY_NONE(), 0, 0);
        overlay->m_rec = rec;
    }
}

// OFFSET: 0x00495CE0
void ClientDBShutdown() {
    s_resistances.Clear();
    s_terrainTypeSoundIDs.Clear();
    s_spellEffects.Clear();
    s_spellActivationOverlays.Clear();
    ClientDBUnloadTables();
    ClientDBReply::s_handler = NULL;
    ClientHotfixNotify::s_handler = NULL;
    ClientHotfixNotifyBlob::s_handler = NULL;
}

// OFFSET: 0x00496790
void ClientDBInitializeInGame() {
    for (WowClientDB2_Base** table = g_clientDB2GameTables; *table; table++) {
        (*table)->Load(s_hotfixTime);
    }

    for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }

    GameTablesInitialize();
    InitTerrainTypeFlags();
    InitSpellEffects();
    InitSpellActivationOverlays();
    InitLights(1);
}

// OFFSET: #0x00634CC0
void InitWeaponSubclasses() {
    s_weaponSubclasses.SetCount(0);
    s_unarmedWeapon = NULL;
    LocateWeaponSubclass();

    int32_t weaponClass = s_weaponItemClass->m_classID;
    int32_t count = 0;

    for (int32_t i = g_itemSubClassDB.m_numRecords - 1; i >= 0; i--) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID == weaponClass && count <= rec->m_subClassID + 1) {
            count = rec->m_subClassID + 1;
        }
    }

    s_weaponSubclasses.SetCount(count);

    for (int32_t i = g_itemSubClassDB.m_numRecords - 1; i >= 0; i--) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID == weaponClass) {
            s_weaponSubclasses[rec->m_subClassID] = rec;

            if (rec->m_flags & 4) {
                s_unarmedWeapon = rec;
            }
        }
    }
}

// OFFSET: 0x00495810/0x1400BB3B0
void ClientDBInitialize() {
    CVar* cvar = CVar::Lookup("dbCompress");

    if (cvar) {
        g_clientDbPerformCompress = cvar->m_intValue != 0;
    }

    for (ClientDBTable* table = g_staticDBTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }

    for (ClientDBTable* table = g_staticDBGlueTables; table->m_db; table++) {
        AsyncFileReadCreateObject(table->m_meta->m_filename, 5, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
    }

    for (ClientDBTable* table = g_staticDBWorldTables; table->m_db; table++) {
        AsyncFileReadCreateObject(table->m_meta->m_filename, 5, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
    }

    ClientDBLoadTables();
    InitResistances();
    InitWeaponSubclasses();
    SDBItemSubclassInitialize();
    ClientHotfixNotifyBlob::s_handler = WowClientDB2_Base::HotfixNotifyBlob;
    ClientHotfixNotify::s_handler = WowClientDB2_Base::HotfixNotifyMultiplex;
    ClientDBReply::s_handler = WowClientDB2_Base::DBReplyMultiplex;
}

// OFFSET: 0x0048A5D0/0x1400ACCF0
int32_t LocaleIdToIndex(uint32_t locale) {
    switch (locale) {
        case 0x45446564:
            return 0;
        case 0x42476E65:
            return 1;
        case 0x53556E65:
            return 2;
        case 0x53457365:
            return 3;
        case 0x52467266:
            return 4;
        case 0x524B6F6B:
            return 5;
        case 0x4E43687A:
            return 6;
        case 0x5754687A:
            return 7;
        case 0x4E436E65:
            return 8;
        case 0x57546E65:
            return 9;
        case 0x584D7365:
            return 10;
        case 0x55527572:
            return 11;
        case 0x52427470:
            return 13;
        case 0x54507470:
            return 14;
        default:
            return 15;
    }
}

// PROVEN: 7375dc07 IDENTICAL/NONE
// OFFSET: 0x0048A6C0
int32_t LocaleIndexToDB2Locale(int32_t index) {
    switch (index) {
        case 0:
            return 3;
        case 3:
            return 6;
        case 4:
            return 2;
        case 5:
            return 1;
        case 6:
        case 8:
            return 4;
        case 7:
        case 9:
            return 5;
        case 10:
            return 7;
        case 11:
            return 8;
        case 13:
        case 14:
            return 10;
        default:
            return 0;
    }
}

// OFFSET: 0x0048A760
int32_t LocaleNameToDB2Locale(const char* locale) {
    int32_t index = LocaleIdToIndex(*reinterpret_cast<const uint32_t*>(locale));

    if (index == 15) {
        index = 2;
    }

    return LocaleIndexToDB2Locale(index);
}

// OFFSET: 0x0048A790/0x1400ACEA0
int32_t GetCurrentLocaleIndex() {
    uint32_t locale = OsGetCurrentLocale();
    int32_t index = LocaleIdToIndex(locale);

    if (index != 15) {
        return index;
    }

    switch (locale) {
        case 0x45496167:
            return 1;
        case 0x41437266:
            return 2;
        case 0x53457565:
            return 3;
        case 0x554C626C:
            return 4;
        case 0x4E4D6E6D:
            return 6;
    }

    const char* name = reinterpret_cast<const char*>(&locale);

    switch (name[0]) {
        case 'd':
            if (name[1] == 'e') {
                return 0;
            }

            break;

        case 'e':
            if (name[1] == 'n') {
                return 2;
            }

            if (name[1] == 's') {
                return 10;
            }

            break;

        case 'f':
            if (name[1] == 'r') {
                return 4;
            }

            break;

        case 'k':
            if (name[1] == 'o') {
                return 5;
            }

            break;

        case 'p':
            if (name[1] == 't') {
                return 13;
            }

            break;

        case 'r':
            if (name[1] == 'u') {
                return 11;
            }

            break;

        case 'z':
            if (name[1] == 'h') {
                return 6;
            }

            break;
    }

    switch (name[2]) {
        case 'B':
            return name[3] == 'R' ? 13 : 2;
        case 'C':
            return name[3] == 'N' ? 6 : 2;
        case 'D':
            return name[3] == 'E' ? 0 : 2;
        case 'E':
            return name[3] == 'S' ? 3 : 2;
        case 'F':
            return name[3] == 'R' ? 4 : 2;
        case 'G':
            return name[3] == 'B' ? 1 : 2;
        case 'K':
            return name[3] == 'R' ? 5 : 2;
        case 'M':
            return name[3] == 'X' ? 10 : 2;
        case 'P':
            return name[3] == 'T' ? 14 : 2;
        case 'R':
            return name[3] == 'U' ? 11 : 2;
        case 'T':
            return name[3] == 'W' ? 7 : 2;
        default:
            return 2;
    }
}

// OFFSET: 0x00000000/0x1400AD130
int32_t GetCurrentDB2Locale() {
    return LocaleIndexToDB2Locale(GetCurrentLocaleIndex());
}

// OFFSET: 0x0048A6A0
int32_t LocaleNameToIndex(const char* locale) {
    int32_t index = LocaleIdToIndex(*reinterpret_cast<const uint32_t*>(locale));

    if (index == 15) {
        index = 2;
    }

    return index;
}

// PROVEN: 20c6d65c IDENTICAL/NONE
// OFFSET: 0x00495030/0x1400BA9A0
const char* ClientDBStringLookup(uint32_t lookup) {
    StringLookupsRec* rec = g_stringLookupsDB.GetRecord(lookup);
    return rec ? static_cast<const char*>(rec->m_string) : NULL;
}

// OFFSET: #0x00634950
MapDifficultyRec* GetMapDifficultyRec(int32_t mapId, uint32_t difficulty, int32_t* index) {
    for (int32_t i = 0; i < g_mapDifficultyDB.m_numRecords; i++) {
        MapDifficultyRec* rec = g_mapDifficultyDB.GetRecordByIndex(i);

        if (rec->m_mapID != mapId) {
            continue;
        }

        for (int32_t j = i; j < g_mapDifficultyDB.m_numRecords; j++) {
            MapDifficultyRec* entry = g_mapDifficultyDB.GetRecordByIndex(j);

            if (entry->m_mapID != mapId) {
                break;
            }

            if (entry->m_difficulty == static_cast<int32_t>(difficulty)) {
                if (index) {
                    *index = j;
                }

                return entry;
            }
        }
    }

    return NULL;
}

// PROVEN: 97860801 IDENTICAL/NONE
// OFFSET: 0x00495380
int32_t GetMaxPlayersForMap(const MapRec* map, int32_t difficulty) {
    MapDifficultyRec* entry = GetMapDifficultyRec(map->m_id, difficulty, NULL);

    if (!entry || !entry->m_maxPlayers) {
        return map->m_maxPlayers;
    }

    return entry->m_maxPlayers;
}
