#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/WowClientDB2.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsFile.h"
#include <ctime>

// STATIC: #0x00AD3028
const char* g_localeNames[12] = { "deDE", "enGB", "enUS", "esES", "frFR", "koKR", "zhCN", "zhTW", "enCN", "enTW", "esMX", "ruRU" };

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

// STATIC: #0x00C5D680
static TSFixedArray<const ItemSubClassRec*> s_weaponSubclasses;

// STATIC: %0x00C7672C
WowClientDB<AnimationDataRec> g_animationDataDB;

// STATIC: %0x00C76748
WowClientDB<LiquidMaterialRec> g_liquidMaterialDB;

// STATIC: %0x00C76764
WowClientDB<LiquidObjectRec> g_liquidObjectDB;

// STATIC: %0x00C76780
WowClientDB<LiquidTypeRec> g_liquidTypeDB;

// STATIC: %0x00C7679C
WowClientDB<SoundEntriesAdvancedRec> g_soundEntriesAdvancedDB;

// STATIC: %0x00C767B8
WowClientDB<SoundEntriesRec> g_soundEntriesDB;

// STATIC: %0x00C767D4
WowClientDB<SoundEntriesFallbacksRec> g_soundEntriesFallbacksDB;

// STATIC: %0x00C767F0
WowClientDB<TerrainMaterialRec> g_terrainMaterialDB;

// STATIC: %0x00C98A24
WowClientDB<AchievementRec> g_achievementDB;

// STATIC: %0x00C98AE4
WowClientDB<Achievement_CriteriaRec> g_achievement_CriteriaDB;

// STATIC: %0x00C98BA4
WowClientDB<Achievement_CategoryRec> g_achievement_CategoryDB;

// STATIC: %0x00C98BC0
WowClientDB<AnimKitRec> g_animKitDB;

// STATIC: %0x00C98BDC
WowClientDB<AnimKitBoneSetRec> g_animKitBoneSetDB;

// STATIC: %0x00C98BF8
WowClientDB<AnimKitBoneSetAliasRec> g_animKitBoneSetAliasDB;

// STATIC: %0x00C98C14
WowClientDB<AnimKitConfigRec> g_animKitConfigDB;

// STATIC: %0x00C98C30
WowClientDB<AnimKitConfigBoneSetRec> g_animKitConfigBoneSetDB;

// STATIC: %0x00C98C4C
WowClientDB<AnimKitPriorityRec> g_animKitPriorityDB;

// STATIC: %0x00C98C68
WowClientDB<AnimKitSegmentRec> g_animKitSegmentDB;

// STATIC: %0x00C98C84
WowClientDB<AnimReplacementRec> g_animReplacementDB;

// STATIC: %0x00C98CA0
WowClientDB<AnimReplacementSetRec> g_animReplacementSetDB;

// STATIC: %0x00C98CBC
WowClientDB<AreaGroupRec> g_areaGroupDB;

// STATIC: %0x00C98CD8
WowClientDB<AreaPOIRec> g_areaPOIDB;

// STATIC: %0x00C98CF4
WowClientDB<AreaPOISortedWorldStateRec> g_areaPOISortedWorldStateDB;

// STATIC: %0x00C98D10
WowClientDB<AreaTableRec> g_areaTableDB;

// STATIC: %0x00C98D2C
WowClientDB<AreaAssignmentRec> g_areaAssignmentDB;

// STATIC: %0x00C98D48
WowClientDB<AreaTriggerRec> g_areaTriggerDB;

// STATIC: %0x00C98D64
WowClientDB<ArmorLocationRec> g_armorLocationDB;

// STATIC: %0x00C98D80
WowClientDB<AuctionHouseRec> g_auctionHouseDB;

// STATIC: %0x00C98D9C
WowClientDB<BankBagSlotPricesRec> g_bankBagSlotPricesDB;

// STATIC: %0x00C98DB8
WowClientDB<BannedAddOnsRec> g_bannedAddOnsDB;

// STATIC: %0x00C98DD4
WowClientDB<BarberShopStyleRec> g_barberShopStyleDB;

// STATIC: %0x00C98DF0
WowClientDB<BattlemasterListRec> g_battlemasterListDB;

// STATIC: %0x00C98E0C
WowClientDB<CameraModeRec> g_cameraModeDB;

// STATIC: %0x00C98E28
WowClientDB<CameraShakesRec> g_cameraShakesDB;

// STATIC: %0x00C98E44
WowClientDB<CastableRaidBuffsRec> g_castableRaidBuffsDB;

// STATIC: %0x00C98E60
WowClientDB<Cfg_CategoriesRec> g_cfg_CategoriesDB;

// STATIC: %0x00C98E7C
WowClientDB<Cfg_ConfigsRec> g_cfg_ConfigsDB;

// STATIC: %0x00C98E98
WowClientDB<CharBaseInfoRec> g_charBaseInfoDB;

// STATIC: %0x00C98EB4
WowClientDB<CharHairGeosetsRec> g_charHairGeosetsDB;

// STATIC: %0x00C98ED0
WowClientDB<CharSectionsRec> g_charSectionsDB;

// STATIC: %0x00C98EEC
WowClientDB<CharStartOutfitRec> g_charStartOutfitDB;

// STATIC: %0x00C98F08
WowClientDB<CharTitlesRec> g_charTitlesDB;

// STATIC: %0x00C98F24
WowClientDB<CharacterFacialHairStylesRec> g_characterFacialHairStylesDB;

// STATIC: %0x00C98F40
WowClientDB<ChatChannelsRec> g_chatChannelsDB;

// STATIC: %0x00C98F5C
WowClientDB<ChatProfanityRec> g_chatProfanityDB;

// STATIC: %0x00C98F78
WowClientDB<ChrClassesRec> g_chrClassesDB;

// STATIC: %0x00C98F94
WowClientDB<ChrRacesRec> g_chrRacesDB;

// STATIC: %0x00C98FB0
WowClientDB<CinematicCameraRec> g_cinematicCameraDB;

// STATIC: %0x00C98FCC
WowClientDB<CinematicSequencesRec> g_cinematicSequencesDB;

// STATIC: %0x00C98FE8
WowClientDB<CreatureDisplayInfoExtraRec> g_creatureDisplayInfoExtraDB;

// STATIC: %0x00C99004
WowClientDB<CreatureDisplayInfoRec> g_creatureDisplayInfoDB;

// STATIC: %0x00C99020
WowClientDB<CreatureFamilyRec> g_creatureFamilyDB;

// STATIC: %0x00C9903C
WowClientDB<CreatureModelDataRec> g_creatureModelDataDB;

// STATIC: %0x00C99058
WowClientDB<CreatureMovementInfoRec> g_creatureMovementInfoDB;

// STATIC: %0x00C99074
WowClientDB<CreatureSoundDataRec> g_creatureSoundDataDB;

// STATIC: %0x00C99090
WowClientDB<CreatureSpellDataRec> g_creatureSpellDataDB;

// STATIC: %0x00C990AC
WowClientDB<CreatureTypeRec> g_creatureTypeDB;

// STATIC: %0x00C990C8
WowClientDB<CurrencyTypesRec> g_currencyTypesDB;

// STATIC: %0x00C990E4
WowClientDB<CurrencyCategoryRec> g_currencyCategoryDB;

// STATIC: %0x00C99100
WowClientDB<DanceMovesRec> g_danceMovesDB;

// STATIC: %0x00C9911C
WowClientDB<DeathThudLookupsRec> g_deathThudLookupsDB;

// STATIC: %0x00C99170
WowClientDB<DestructibleModelDataRec> g_destructibleModelDataDB;

// STATIC: %0x00C9918C
WowClientDB<DungeonEncounterRec> g_dungeonEncounterDB;

// STATIC: %0x00C991A8
WowClientDB<DungeonMapRec> g_dungeonMapDB;

// STATIC: %0x00C991C4
WowClientDB<DungeonMapChunkRec> g_dungeonMapChunkDB;

// STATIC: %0x00C991E0
WowClientDB<DurabilityCostsRec> g_durabilityCostsDB;

// STATIC: %0x00C991FC
WowClientDB<DurabilityQualityRec> g_durabilityQualityDB;

// STATIC: %0x00C99218
WowClientDB<EmotesRec> g_emotesDB;

// STATIC: %0x00C99234
WowClientDB<EmotesTextDataRec> g_emotesTextDataDB;

// STATIC: %0x00C99250
WowClientDB<EmotesTextSoundRec> g_emotesTextSoundDB;

// STATIC: %0x00C9926C
WowClientDB<EmotesTextRec> g_emotesTextDB;

// STATIC: %0x00C99288
WowClientDB<EnvironmentalDamageRec> g_environmentalDamageDB;

// STATIC: %0x00C992A4
WowClientDB<ExhaustionRec> g_exhaustionDB;

// STATIC: %0x00C992C0
WowClientDB<FactionGroupRec> g_factionGroupDB;

// STATIC: %0x00C992DC
WowClientDB<FactionRec> g_factionDB;

// STATIC: %0x00C992F8
WowClientDB<FactionTemplateRec> g_factionTemplateDB;

// STATIC: %0x00C99314
WowClientDB<FileDataRec> g_fileDataDB;

// STATIC: %0x00C99330
WowClientDB<FootprintTexturesRec> g_footprintTexturesDB;

// STATIC: %0x00C9934C
WowClientDB<FootstepTerrainLookupRec> g_footstepTerrainLookupDB;

// STATIC: %0x00C99368
WowClientDB<GameObjectArtKitRec> g_gameObjectArtKitDB;

// STATIC: %0x00C99384
WowClientDB<GameObjectDisplayInfoRec> g_gameObjectDisplayInfoDB;

// STATIC: %0x00C993A0
WowClientDB<GameTablesRec> g_gameTablesDB;

// STATIC: %0x00C993BC
WowClientDB<GameTipsRec> g_gameTipsDB;

// STATIC: %0x00C993D8
WowClientDB<GemPropertiesRec> g_gemPropertiesDB;

// STATIC: %0x00C993F4
WowClientDB<GlueScreenEmoteRec> g_glueScreenEmoteDB;

// STATIC: %0x00C99410
WowClientDB<GlyphPropertiesRec> g_glyphPropertiesDB;

// STATIC: %0x00C9942C
WowClientDB<GlyphSlotRec> g_glyphSlotDB;

// STATIC: %0x00C99448
WowClientDB<GMSurveyAnswersRec> g_gmSurveyAnswersDB;

// STATIC: %0x00C99464
WowClientDB<GMSurveyCurrentSurveyRec> g_gmSurveyCurrentSurveyDB;

// STATIC: %0x00C99480
WowClientDB<GMSurveyQuestionsRec> g_gmSurveyQuestionsDB;

// STATIC: %0x00C9949C
WowClientDB<GMSurveySurveysRec> g_gmSurveySurveysDB;

// STATIC: %0x00C994B8
WowClientDB<GMTicketCategoryRec> g_gmTicketCategoryDB;

// STATIC: %0x00C994D4
WowClientDB<GroundEffectDoodadRec> g_groundEffectDoodadDB;

// STATIC: %0x00C994F0
WowClientDB<GroundEffectTextureRec> g_groundEffectTextureDB;

// STATIC: %0x00C9950C
WowClientDB<gtBarberShopCostBaseRec> g_gtBarberShopCostBaseDB;

// STATIC: %0x00C99528
WowClientDB<gtCombatRatingsRec> g_gtCombatRatingsDB;

// STATIC: %0x00C99544
WowClientDB<gtChanceToMeleeCritRec> g_gtChanceToMeleeCritDB;

// STATIC: %0x00C99560
WowClientDB<gtChanceToMeleeCritBaseRec> g_gtChanceToMeleeCritBaseDB;

// STATIC: %0x00C9957C
WowClientDB<gtChanceToSpellCritRec> g_gtChanceToSpellCritDB;

// STATIC: %0x00C99598
WowClientDB<gtChanceToSpellCritBaseRec> g_gtChanceToSpellCritBaseDB;

// STATIC: %0x00C995B4
WowClientDB<gtNPCManaCostScalerRec> g_gtNPCManaCostScalerDB;

// STATIC: %0x00C995D0
WowClientDB<gtOCTClassCombatRatingScalarRec> g_gtOCTClassCombatRatingScalarDB;

// STATIC: %0x00C995EC
WowClientDB<gtOCTHpPerStaminaRec> g_gtOCTHpPerStaminaDB;

// STATIC: %0x00C99608
WowClientDB<gtOCTRegenMPRec> g_gtOCTRegenMPDB;

// STATIC: %0x00C99624
WowClientDB<gtRegenMPPerSptRec> g_gtRegenMPPerSptDB;

// STATIC: %0x00C99640
WowClientDB<gtSpellScalingRec> g_gtSpellScalingDB;

// STATIC: %0x00C9965C
WowClientDB<GuildColorBackgroundRec> g_guildColorBackgroundDB;

// STATIC: %0x00C99678
WowClientDB<GuildColorBorderRec> g_guildColorBorderDB;

// STATIC: %0x00C99694
WowClientDB<GuildColorEmblemRec> g_guildColorEmblemDB;

// STATIC: %0x00C996B0
WowClientDB<GuildPerkSpellsRec> g_guildPerkSpellsDB;

// STATIC: %0x00C996CC
WowClientDB<HelmetGeosetVisDataRec> g_helmetGeosetVisDataDB;

// STATIC: %0x00C996E8
WowClientDB<HolidayDescriptionsRec> g_holidayDescriptionsDB;

// STATIC: %0x00C99704
WowClientDB<HolidayNamesRec> g_holidayNamesDB;

// STATIC: %0x00C99720
WowClientDB<HolidaysRec> g_holidaysDB;

// STATIC: %0x00C9973C
WowClientDB<ItemArmorTotalRec> g_itemArmorTotalDB;

// STATIC: %0x00C99758
WowClientDB<ItemArmorQualityRec> g_itemArmorQualityDB;

// STATIC: %0x00C99774
WowClientDB<ItemArmorShieldRec> g_itemArmorShieldDB;

// STATIC: %0x00C99790
WowClientDB<ItemBagFamilyRec> g_itemBagFamilyDB;

// STATIC: %0x00C997AC
WowClientDB<ItemClassRec> g_itemClassDB;

// STATIC: %0x00C997C8
WowClientDB<ItemCurrencyCostRec> g_itemCurrencyCostDB;

// STATIC: %0x00C997E4
WowClientDB<ItemDamageAmmoRec> g_itemDamageAmmoDB;

// STATIC: %0x00C99800
WowClientDB<ItemDamageOneHandRec> g_itemDamageOneHandDB;

// STATIC: %0x00C9981C
WowClientDB<ItemDamageOneHandCasterRec> g_itemDamageOneHandCasterDB;

// STATIC: %0x00C99838
WowClientDB<ItemDamageRangedRec> g_itemDamageRangedDB;

// STATIC: %0x00C99854
WowClientDB<ItemDamageThrownRec> g_itemDamageThrownDB;

// STATIC: %0x00C99870
WowClientDB<ItemDamageTwoHandRec> g_itemDamageTwoHandDB;

// STATIC: %0x00C9988C
WowClientDB<ItemDamageTwoHandCasterRec> g_itemDamageTwoHandCasterDB;

// STATIC: %0x00C998A8
WowClientDB<ItemDamageWandRec> g_itemDamageWandDB;

// STATIC: %0x00C998C4
WowClientDB<ItemDisenchantLootRec> g_itemDisenchantLootDB;

// STATIC: %0x00C998E0
WowClientDB_RLE<ItemDisplayInfoRec> g_itemDisplayInfoDB;

// STATIC: %0x00C99900
WowClientDB<ItemExtendedCostRec> g_itemExtendedCostDB;

// STATIC: %0x00C9991C
WowClientDB<ItemGroupSoundsRec> g_itemGroupSoundsDB;

// STATIC: %0x00C99938
WowClientDB<ItemLimitCategoryRec> g_itemLimitCategoryDB;

// STATIC: %0x00C99954
WowClientDB<ItemPetFoodRec> g_itemPetFoodDB;

// STATIC: %0x00C99970
WowClientDB<ItemPurchaseGroupRec> g_itemPurchaseGroupDB;

// STATIC: %0x00C9998C
WowClientDB<ItemRandomPropertiesRec> g_itemRandomPropertiesDB;

// STATIC: %0x00C999A8
WowClientDB<ItemRandomSuffixRec> g_itemRandomSuffixDB;

// STATIC: %0x00C999C4
WowClientDB<ItemReforgeRec> g_itemReforgeDB;

// STATIC: %0x00C999E0
WowClientDB<ItemSetRec> g_itemSetDB;

// STATIC: %0x00C999FC
WowClientDB<ItemSubClassMaskRec> g_itemSubClassMaskDB;

// STATIC: %0x00C99A18
WowClientDB<ItemSubClassRec> g_itemSubClassDB;

// STATIC: %0x00C99A34
WowClientDB<ItemVisualEffectsRec> g_itemVisualEffectsDB;

// STATIC: %0x00C99A50
WowClientDB<ItemVisualsRec> g_itemVisualsDB;

// STATIC: %0x00C99A6C
WowClientDB<LanguageWordsRec> g_languageWordsDB;

// STATIC: %0x00C99A88
WowClientDB<LanguagesRec> g_languagesDB;

// STATIC: %0x00C99AA4
WowClientDB<LfgDungeonExpansionRec> g_lfgDungeonExpansionDB;

// STATIC: %0x00C99AC0
WowClientDB<LfgDungeonGroupRec> g_lfgDungeonGroupDB;

// STATIC: %0x00C99ADC
WowClientDB<LfgDungeonsRec> g_lfgDungeonsDB;

// STATIC: %0x00C99AF8
WowClientDB<LoadingScreensRec> g_loadingScreensDB;

// STATIC: %0x00C99B14
WowClientDB<LoadingScreenTaxiSplinesRec> g_loadingScreenTaxiSplinesDB;

// STATIC: %0x00C99B30
WowClientDB<LockRec> g_lockDB;

// STATIC: %0x00C99B4C
WowClientDB<LockTypeRec> g_lockTypeDB;

// STATIC: %0x00C99B68
WowClientDB<MailTemplateRec> g_mailTemplateDB;

// STATIC: %0x00C99B84
WowClientDB<MapRec> g_mapDB;

// STATIC: %0x00C99BA0
WowClientDB<MapDifficultyRec> g_mapDifficultyDB;

// STATIC: %0x00C99BBC
WowClientDB<MaterialRec> g_materialDB;

// STATIC: %0x00C99BD8
WowClientDB<MountCapabilityRec> g_mountCapabilityDB;

// STATIC: %0x00C99BF4
WowClientDB<MountTypeRec> g_mountTypeDB;

// STATIC: %0x00C99C10
WowClientDB<MovieRec> g_movieDB;

// STATIC: %0x00C99C2C
WowClientDB<MovieFileDataRec> g_movieFileDataDB;

// STATIC: %0x00C99C48
WowClientDB<MovieVariationRec> g_movieVariationDB;

// STATIC: %0x00C99C64
WowClientDB<NameGenRec> g_nameGenDB;

// STATIC: %0x00C99C80
WowClientDB<NPCSoundsRec> g_npcSoundsDB;

// STATIC: %0x00C99C9C
WowClientDB<NumTalentsAtLevelRec> g_numTalentsAtLevelDB;

// STATIC: %0x00C99CB8
WowClientDB<NamesProfanityRec> g_namesProfanityDB;

// STATIC: %0x00C99CD4
WowClientDB<NamesReservedRec> g_namesReservedDB;

// STATIC: %0x00C99CF0
WowClientDB<OverrideSpellDataRec> g_overrideSpellDataDB;

// STATIC: %0x00C99D0C
WowClientDB<PackageRec> g_packageDB;

// STATIC: %0x00C99D28
WowClientDB<PageTextMaterialRec> g_pageTextMaterialDB;

// STATIC: %0x00C99D44
WowClientDB<PaperDollItemFrameRec> g_paperDollItemFrameDB;

// STATIC: %0x00C99D60
WowClientDB<ParticleColorRec> g_particleColorDB;

// STATIC: %0x00C99D7C
WowClientDB<PetPersonalityRec> g_petPersonalityDB;

// STATIC: %0x00C99D98
WowClientDB<PhaseRec> g_phaseDB;

// STATIC: %0x00C99DB4
WowClientDB<PhaseShiftZoneSoundsRec> g_phaseShiftZoneSoundsDB;

// STATIC: %0x00C99DD0
WowClientDB<PhaseXPhaseGroupRec> g_phaseXPhaseGroupDB;

// STATIC: %0x00C99DEC
WowClientDB<PowerDisplayRec> g_powerDisplayDB;

// STATIC: %0x00C99E08
WowClientDB<PvpDifficultyRec> g_pvpDifficultyDB;

// STATIC: %0x00C99E24
WowClientDB<QuestFactionRewardRec> g_questFactionRewardDB;

// STATIC: %0x00C99E40
WowClientDB<QuestInfoRec> g_questInfoDB;

// STATIC: %0x00C99E5C
WowClientDB<QuestPOIBlobRec> g_questPOIBlobDB;

// STATIC: %0x00C99E78
WowClientDB<QuestPOIPointRec> g_questPOIPointDB;

// STATIC: %0x00C99E94
WowClientDB<QuestSortRec> g_questSortDB;

// STATIC: %0x00C99EB0
WowClientDB<QuestXPRec> g_questXPDB;

// STATIC: %0x00C99ECC
WowClientDB<ResistancesRec> g_resistancesDB;

// STATIC: %0x00C99EE8
WowClientDB<RandPropPointsRec> g_randPropPointsDB;

// STATIC: %0x00C99F04
WowClientDB<ResearchFieldRec> g_researchFieldDB;

// STATIC: %0x00C99F20
WowClientDB<ResearchBranchRec> g_researchBranchDB;

// STATIC: %0x00C99F3C
WowClientDB<ResearchProjectRec> g_researchProjectDB;

// STATIC: %0x00C99F58
WowClientDB<ResearchSiteRec> g_researchSiteDB;

// STATIC: %0x00C99F74
WowClientDB<ScalingStatDistributionRec> g_scalingStatDistributionDB;

// STATIC: %0x00C99F90
WowClientDB<ScalingStatValuesRec> g_scalingStatValuesDB;

// STATIC: %0x00C99FAC
WowClientDB<ScreenEffectRec> g_screenEffectDB;

// STATIC: %0x00C99FC8
WowClientDB<ScreenLocationRec> g_screenLocationDB;

// STATIC: %0x00C99FE4
WowClientDB<ServerMessagesRec> g_serverMessagesDB;

// STATIC: %0x00C9A000
WowClientDB<SkillLineAbilityRec> g_skillLineAbilityDB;

// STATIC: %0x00C9A01C
WowClientDB<SkillLineAbilitySortedSpellRec> g_skillLineAbilitySortedSpellDB;

// STATIC: %0x00C9A038
WowClientDB<SkillLineCategoryRec> g_skillLineCategoryDB;

// STATIC: %0x00C9A054
WowClientDB<SkillLineRec> g_skillLineDB;

// STATIC: %0x00C9A070
WowClientDB<SkillRaceClassInfoRec> g_skillRaceClassInfoDB;

// STATIC: %0x00C9A08C
WowClientDB<SkillTiersRec> g_skillTiersDB;

// STATIC: %0x00C9A0A8
WowClientDB<SoundAmbienceRec> g_soundAmbienceDB;

// STATIC: %0x00C9A0C4
WowClientDB<SoundEmittersRec> g_soundEmittersDB;

// STATIC: %0x00C9A0E0
WowClientDB<SoundProviderPreferencesRec> g_soundProviderPreferencesDB;

// STATIC: %0x00C9A0FC
WowClientDB<SpamMessagesRec> g_spamMessagesDB;

// STATIC: %0x00C9A118
WowClientDB<SpellActivationOverlayRec> g_spellActivationOverlayDB;

// STATIC: %0x00C9A134
WowClientDB<SpellAuraOptionsRec> g_spellAuraOptionsDB;

// STATIC: %0x00C9A150
WowClientDB<SpellAuraRestrictionsRec> g_spellAuraRestrictionsDB;

// STATIC: %0x00C9A16C
WowClientDB<SpellCastingRequirementsRec> g_spellCastingRequirementsDB;

// STATIC: %0x00C9A188
WowClientDB<SpellCastTimesRec> g_spellCastTimesDB;

// STATIC: %0x00C9A1A4
WowClientDB<SpellCategoriesRec> g_spellCategoriesDB;

// STATIC: %0x00C9A1C0
WowClientDB<SpellCategoryRec> g_spellCategoryDB;

// STATIC: %0x00C9A1DC
WowClientDB<SpellChainEffectsRec> g_spellChainEffectsDB;

// STATIC: %0x00C9A1F8
WowClientDB<SpellClassOptionsRec> g_spellClassOptionsDB;

// STATIC: %0x00C9A214
WowClientDB<SpellCooldownsRec> g_spellCooldownsDB;

// STATIC: %0x00C9A230
WowClientDB<SpellDescriptionVariablesRec> g_spellDescriptionVariablesDB;

// STATIC: %0x00C9A24C
WowClientDB<SpellDifficultyRec> g_spellDifficultyDB;

// STATIC: %0x00C9A268
WowClientDB<SpellDispelTypeRec> g_spellDispelTypeDB;

// STATIC: %0x00C9A284
WowClientDB<SpellDurationRec> g_spellDurationDB;

// STATIC: %0x00C9A2A0
WowClientDB<SpellEffectRec> g_spellEffectDB;

// STATIC: %0x00C9A2BC
WowClientDB<SpellEffectCameraShakesRec> g_spellEffectCameraShakesDB;

// STATIC: %0x00C9A2D8
WowClientDB<SpellEquippedItemsRec> g_spellEquippedItemsDB;

// STATIC: %0x00C9A2F4
WowClientDB<SpellFlyoutRec> g_spellFlyoutDB;

// STATIC: %0x00C9A310
WowClientDB<SpellFlyoutItemRec> g_spellFlyoutItemDB;

// STATIC: %0x00C9A32C
WowClientDB<SpellFocusObjectRec> g_spellFocusObjectDB;

// STATIC: %0x00C9A348
WowClientDB<SpellIconRec> g_spellIconDB;

// STATIC: %0x00C9A364
WowClientDB<SpellInterruptsRec> g_spellInterruptsDB;

// STATIC: %0x00C9A380
WowClientDB<SpellItemEnchantmentRec> g_spellItemEnchantmentDB;

// STATIC: %0x00C9A39C
WowClientDB<SpellItemEnchantmentConditionRec> g_spellItemEnchantmentConditionDB;

// STATIC: %0x00C9A3B8
WowClientDB<SpellLevelsRec> g_spellLevelsDB;

// STATIC: %0x00C9A3D4
WowClientDB<SpellMechanicRec> g_spellMechanicDB;

// STATIC: %0x00C9A3F0
WowClientDB<SpellMissileRec> g_spellMissileDB;

// STATIC: %0x00C9A40C
WowClientDB<SpellMissileMotionRec> g_spellMissileMotionDB;

// STATIC: %0x00C9A428
WowClientDB<SpellPowerRec> g_spellPowerDB;

// STATIC: %0x00C9A444
WowClientDB<SpellRadiusRec> g_spellRadiusDB;

// STATIC: %0x00C9A460
WowClientDB<SpellRangeRec> g_spellRangeDB;

// STATIC: %0x00C9A47C
WowClientDB<SpellRuneCostRec> g_spellRuneCostDB;

// STATIC: %0x00C9A498
WowClientDB<SpellReagentsRec> g_spellReagentsDB;

// STATIC: %0x00C9A4B4
WowClientDB<SpellRec> g_spellDB;

// STATIC: %0x00C9A4D0
WowClientDB<SpellScalingRec> g_spellScalingDB;

// STATIC: %0x00C9A4EC
WowClientDB<SpellShapeshiftRec> g_spellShapeshiftDB;

// STATIC: %0x00C9A508
WowClientDB<SpellShapeshiftFormRec> g_spellShapeshiftFormDB;

// STATIC: %0x00C9A524
WowClientDB<SpellTargetRestrictionsRec> g_spellTargetRestrictionsDB;

// STATIC: %0x00C9A540
WowClientDB<SpellTotemsRec> g_spellTotemsDB;

// STATIC: %0x00C9A55C
WowClientDB<SpellVisualEffectNameRec> g_spellVisualEffectNameDB;

// STATIC: %0x00C9A578
WowClientDB<SpellVisualKitRec> g_spellVisualKitDB;

// STATIC: %0x00C9A594
WowClientDB<SpellVisualKitAreaModelRec> g_spellVisualKitAreaModelDB;

// STATIC: %0x00C9A5B0
WowClientDB<SpellVisualKitModelAttachRec> g_spellVisualKitModelAttachDB;

// STATIC: %0x00C9A5CC
WowClientDB<SpellVisualRec> g_spellVisualDB;

// STATIC: %0x00C9A5E8
WowClientDB<StationeryRec> g_stationeryDB;

// STATIC: %0x00C9A604
WowClientDB<StringLookupsRec> g_stringLookupsDB;

// STATIC: %0x00C9A620
WowClientDB<SummonPropertiesRec> g_summonPropertiesDB;

// STATIC: %0x00C9A63C
WowClientDB<TalentRec> g_talentDB;

// STATIC: %0x00C9A658
WowClientDB<TalentTabRec> g_talentTabDB;

// STATIC: %0x00C9A674
WowClientDB<TalentTreePrimarySpellsRec> g_talentTreePrimarySpellsDB;

// STATIC: %0x00C9A690
WowClientDB<TaxiNodesRec> g_taxiNodesDB;

// STATIC: %0x00C9A6AC
WowClientDB<TaxiPathNodeRec> g_taxiPathNodeDB;

// STATIC: %0x00C9A6C8
WowClientDB<TaxiPathRec> g_taxiPathDB;

// STATIC: %0x00C9A6E4
WowClientDB<TerrainTypeRec> g_terrainTypeDB;

// STATIC: %0x00C9A700
WowClientDB<TerrainTypeSoundsRec> g_terrainTypeSoundsDB;

// STATIC: %0x00C9A71C
WowClientDB<TotemCategoryRec> g_totemCategoryDB;

// STATIC: %0x00C9A738
WowClientDB<TransportAnimationRec> g_transportAnimationDB;

// STATIC: %0x00C9A754
WowClientDB<TransportPhysicsRec> g_transportPhysicsDB;

// STATIC: %0x00C9A770
WowClientDB<TransportRotationRec> g_transportRotationDB;

// STATIC: %0x00C9A78C
WowClientDB<UnitBloodLevelsRec> g_unitBloodLevelsDB;

// STATIC: %0x00C9A7A8
WowClientDB<UnitBloodRec> g_unitBloodDB;

// STATIC: %0x00C9A7C4
WowClientDB<UnitPowerBarRec> g_unitPowerBarDB;

// STATIC: %0x00C9A7E0
WowClientDB<VehicleRec> g_vehicleDB;

// STATIC: %0x00C9A7FC
WowClientDB<VehicleSeatRec> g_vehicleSeatDB;

// STATIC: %0x00C9A818
WowClientDB<VehicleUIIndicatorRec> g_vehicleUIIndicatorDB;

// STATIC: %0x00C9A834
WowClientDB<VehicleUIIndSeatRec> g_vehicleUIIndSeatDB;

// STATIC: %0x00C9A850
WowClientDB<VocalUISoundsRec> g_vocalUISoundsDB;

// STATIC: %0x00C9A86C
WowClientDB<WMOAreaTableRec> g_wmoAreaTableDB;

// STATIC: %0x00C9A888
WowClientDB<World_PVP_AreaRec> g_world_PVP_AreaDB;

// STATIC: %0x00C9A8A4
WowClientDB<WeaponImpactSoundsRec> g_weaponImpactSoundsDB;

// STATIC: %0x00C9A8C0
WowClientDB<WeaponSwingSounds2Rec> g_weaponSwingSounds2DB;

// STATIC: %0x00C9A8DC
WowClientDB<WeatherRec> g_weatherDB;

// STATIC: %0x00C9A8F8
WowClientDB<WorldMapAreaRec> g_worldMapAreaDB;

// STATIC: %0x00C9A914
WowClientDB<WorldMapContinentRec> g_worldMapContinentDB;

// STATIC: %0x00C9A930
WowClientDB<WorldMapOverlayRec> g_worldMapOverlayDB;

// STATIC: %0x00C9A94C
WowClientDB<WorldMapTransformsRec> g_worldMapTransformsDB;

// STATIC: %0x00C9A968
WowClientDB<WorldSafeLocsRec> g_worldSafeLocsDB;

// STATIC: %0x00C9A984
WowClientDB<WorldStateUIRec> g_worldStateUIDB;

// STATIC: %0x00C9A9A0
WowClientDB<ZoneIntroMusicTableRec> g_zoneIntroMusicTableDB;

// STATIC: %0x00C9A9BC
WowClientDB<ZoneLightRec> g_zoneLightDB;

// STATIC: %0x00C9A9D8
WowClientDB<ZoneLightPointRec> g_zoneLightPointDB;

// STATIC: %0x00C9A9F4
WowClientDB<ZoneMusicRec> g_zoneMusicDB;

// STATIC: %0x00C9AA10
WowClientDB<WorldStateZoneSoundsRec> g_worldStateZoneSoundsDB;

// STATIC: %0x00C9AA2C
WowClientDB<WorldChunkSoundsRec> g_worldChunkSoundsDB;

// STATIC: %0x00C9AA48
WowClientDB<ObjectEffectRec> g_objectEffectDB;

// STATIC: %0x00C9AA64
WowClientDB<ObjectEffectGroupRec> g_objectEffectGroupDB;

// STATIC: %0x00C9AA80
WowClientDB<ObjectEffectModifierRec> g_objectEffectModifierDB;

// STATIC: %0x00C9AA9C
WowClientDB<ObjectEffectPackageRec> g_objectEffectPackageDB;

// STATIC: %0x00C9AAB8
WowClientDB<ObjectEffectPackageElemRec> g_objectEffectPackageElemDB;

// STATIC: %0x00C9AAD4
WowClientDB<SoundFilterRec> g_soundFilterDB;

// STATIC: %0x00C9AAF0
WowClientDB<SoundFilterElemRec> g_soundFilterElemDB;

// STATIC: %0x00CC802C
WowClientDB<LightSkyboxRec> g_lightSkyboxDB;

// STATIC: %0x00CC8048
WowClientDB_RLE<LightIntBandRec> g_lightIntBandDB;

// STATIC: %0x00CC8068
WowClientDB_RLE<LightFloatBandRec> g_lightFloatBandDB;

// STATIC: %0x00CC8088
WowClientDB<LightParamsRec> g_lightParamsDB;

// STATIC: %0x00CC80A4
WowClientDB<LightRec> g_lightDB;

// STATIC: %0x00C99138
WowClientDB<DeclinedWordRec> g_declinedWordDB;

// STATIC: %0x00C99154
WowClientDB<DeclinedWordCasesRec> g_declinedWordCasesDB;

// STATIC: %0x00BAEE98
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
    { &g_cinematicCameraDB, &CinematicCameraRec::s_meta },
    { &g_cinematicSequencesDB, &CinematicSequencesRec::s_meta },
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
    { &g_footprintTexturesDB, &FootprintTexturesRec::s_meta },
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
    { &g_groundEffectTextureDB, &GroundEffectTextureRec::s_meta },
    { &g_gtBarberShopCostBaseDB, &gtBarberShopCostBaseRec::s_meta },
    { &g_gtCombatRatingsDB, &gtCombatRatingsRec::s_meta },
    { &g_gtChanceToMeleeCritDB, &gtChanceToMeleeCritRec::s_meta },
    { &g_gtChanceToMeleeCritBaseDB, &gtChanceToMeleeCritBaseRec::s_meta },
    { &g_gtChanceToSpellCritDB, &gtChanceToSpellCritRec::s_meta },
    { &g_gtChanceToSpellCritBaseDB, &gtChanceToSpellCritBaseRec::s_meta },
    { &g_gtNPCManaCostScalerDB, &gtNPCManaCostScalerRec::s_meta },
    { &g_gtOCTClassCombatRatingScalarDB, &gtOCTClassCombatRatingScalarRec::s_meta },
    { &g_gtOCTHpPerStaminaDB, &gtOCTHpPerStaminaRec::s_meta },
    { &g_gtOCTRegenMPDB, &gtOCTRegenMPRec::s_meta },
    { &g_gtRegenMPPerSptDB, &gtRegenMPPerSptRec::s_meta },
    { &g_gtSpellScalingDB, &gtSpellScalingRec::s_meta },
    { &g_guildPerkSpellsDB, &GuildPerkSpellsRec::s_meta },
    { &g_holidayDescriptionsDB, &HolidayDescriptionsRec::s_meta },
    { &g_holidayNamesDB, &HolidayNamesRec::s_meta },
    { &g_holidaysDB, &HolidaysRec::s_meta },
    { &g_itemArmorQualityDB, &ItemArmorQualityRec::s_meta },
    { &g_itemArmorTotalDB, &ItemArmorTotalRec::s_meta },
    { &g_itemArmorShieldDB, &ItemArmorShieldRec::s_meta },
    { &g_itemBagFamilyDB, &ItemBagFamilyRec::s_meta },
    { &g_itemCurrencyCostDB, &ItemCurrencyCostRec::s_meta },
    { &g_itemDamageAmmoDB, &ItemDamageAmmoRec::s_meta },
    { &g_itemDamageOneHandDB, &ItemDamageOneHandRec::s_meta },
    { &g_itemDamageOneHandCasterDB, &ItemDamageOneHandCasterRec::s_meta },
    { &g_itemDamageRangedDB, &ItemDamageRangedRec::s_meta },
    { &g_itemDamageThrownDB, &ItemDamageThrownRec::s_meta },
    { &g_itemDamageTwoHandDB, &ItemDamageTwoHandRec::s_meta },
    { &g_itemDamageTwoHandCasterDB, &ItemDamageTwoHandCasterRec::s_meta },
    { &g_itemDamageWandDB, &ItemDamageWandRec::s_meta },
    { &g_itemDisenchantLootDB, &ItemDisenchantLootRec::s_meta },
    { &g_itemExtendedCostDB, &ItemExtendedCostRec::s_meta },
    { &g_itemGroupSoundsDB, &ItemGroupSoundsRec::s_meta },
    { &g_itemLimitCategoryDB, &ItemLimitCategoryRec::s_meta },
    { &g_itemPetFoodDB, &ItemPetFoodRec::s_meta },
    { &g_itemPurchaseGroupDB, &ItemPurchaseGroupRec::s_meta },
    { &g_itemRandomPropertiesDB, &ItemRandomPropertiesRec::s_meta },
    { &g_itemRandomSuffixDB, &ItemRandomSuffixRec::s_meta },
    { &g_itemReforgeDB, &ItemReforgeRec::s_meta },
    { &g_itemSetDB, &ItemSetRec::s_meta },
    { &g_itemSubClassMaskDB, &ItemSubClassMaskRec::s_meta },
    { &g_languageWordsDB, &LanguageWordsRec::s_meta },
    { &g_languagesDB, &LanguagesRec::s_meta },
    { &g_lfgDungeonExpansionDB, &LfgDungeonExpansionRec::s_meta },
    { &g_lfgDungeonGroupDB, &LfgDungeonGroupRec::s_meta },
    { &g_lfgDungeonsDB, &LfgDungeonsRec::s_meta },
    { &g_lightFloatBandDB, &LightFloatBandRec::s_meta },
    { &g_lightSkyboxDB, &LightSkyboxRec::s_meta },
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
    { &g_petPersonalityDB, &PetPersonalityRec::s_meta },
    { &g_phaseDB, &PhaseRec::s_meta },
    { &g_phaseXPhaseGroupDB, &PhaseXPhaseGroupRec::s_meta },
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
    { &g_soundEmittersDB, &SoundEmittersRec::s_meta },
    { &g_spellActivationOverlayDB, &SpellActivationOverlayRec::s_meta },
    { &g_spellAuraOptionsDB, &SpellAuraOptionsRec::s_meta },
    { &g_spellAuraRestrictionsDB, &SpellAuraRestrictionsRec::s_meta },
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
    { &g_terrainTypeDB, &TerrainTypeRec::s_meta },
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
    { &g_wmoAreaTableDB, &WMOAreaTableRec::s_meta },
    { &g_world_PVP_AreaDB, &World_PVP_AreaRec::s_meta },
    { &g_weaponImpactSoundsDB, &WeaponImpactSoundsRec::s_meta },
    { &g_weaponSwingSounds2DB, &WeaponSwingSounds2Rec::s_meta },
    { &g_weatherDB, &WeatherRec::s_meta },
    { &g_worldMapAreaDB, &WorldMapAreaRec::s_meta },
    { &g_worldMapContinentDB, &WorldMapContinentRec::s_meta },
    { &g_worldMapOverlayDB, &WorldMapOverlayRec::s_meta },
    { &g_worldMapTransformsDB, &WorldMapTransformsRec::s_meta },
    { &g_worldSafeLocsDB, &WorldSafeLocsRec::s_meta },
    { &g_worldStateUIDB, &WorldStateUIRec::s_meta },
    { &g_zoneIntroMusicTableDB, &ZoneIntroMusicTableRec::s_meta },
    { &g_zoneLightDB, &ZoneLightRec::s_meta },
    { &g_zoneLightPointDB, &ZoneLightPointRec::s_meta },
    { &g_zoneMusicDB, &ZoneMusicRec::s_meta },
    { &g_worldStateZoneSoundsDB, &WorldStateZoneSoundsRec::s_meta },
    { &g_worldChunkSoundsDB, &WorldChunkSoundsRec::s_meta },
    { &g_phaseShiftZoneSoundsDB, &PhaseShiftZoneSoundsRec::s_meta },
    { NULL, NULL },
};

// STATIC: %0x00BAF5B8
static ClientDBTable s_clientDBTables[] = {
    { &g_animKitBoneSetAliasDB, &AnimKitBoneSetAliasRec::s_meta },
    { &g_animKitBoneSetDB, &AnimKitBoneSetRec::s_meta },
    { &g_animKitDB, &AnimKitRec::s_meta },
    { &g_animKitConfigDB, &AnimKitConfigRec::s_meta },
    { &g_animKitConfigBoneSetDB, &AnimKitConfigBoneSetRec::s_meta },
    { &g_animKitPriorityDB, &AnimKitPriorityRec::s_meta },
    { &g_animKitSegmentDB, &AnimKitSegmentRec::s_meta },
    { &g_animReplacementDB, &AnimReplacementRec::s_meta },
    { &g_animReplacementSetDB, &AnimReplacementSetRec::s_meta },
    { &g_areaTableDB, &AreaTableRec::s_meta },
    { &g_bannedAddOnsDB, &BannedAddOnsRec::s_meta },
    { &g_cfg_CategoriesDB, &Cfg_CategoriesRec::s_meta },
    { &g_cfg_ConfigsDB, &Cfg_ConfigsRec::s_meta },
    { &g_charBaseInfoDB, &CharBaseInfoRec::s_meta },
    { &g_charHairGeosetsDB, &CharHairGeosetsRec::s_meta },
    { &g_charSectionsDB, &CharSectionsRec::s_meta },
    { &g_charStartOutfitDB, &CharStartOutfitRec::s_meta },
    { &g_characterFacialHairStylesDB, &CharacterFacialHairStylesRec::s_meta },
    { &g_chatProfanityDB, &ChatProfanityRec::s_meta },
    { &g_chrClassesDB, &ChrClassesRec::s_meta },
    { &g_chrRacesDB, &ChrRacesRec::s_meta },
    { &g_creatureDisplayInfoDB, &CreatureDisplayInfoRec::s_meta },
    { &g_creatureDisplayInfoExtraDB, &CreatureDisplayInfoExtraRec::s_meta },
    { &g_creatureFamilyDB, &CreatureFamilyRec::s_meta },
    { &g_creatureModelDataDB, &CreatureModelDataRec::s_meta },
    { &g_factionGroupDB, &FactionGroupRec::s_meta },
    { &g_factionTemplateDB, &FactionTemplateRec::s_meta },
    { &g_fileDataDB, &FileDataRec::s_meta },
    { &g_gameTipsDB, &GameTipsRec::s_meta },
    { &g_glueScreenEmoteDB, &GlueScreenEmoteRec::s_meta },
    { &g_groundEffectDoodadDB, &GroundEffectDoodadRec::s_meta },
    { &g_guildColorBackgroundDB, &GuildColorBackgroundRec::s_meta },
    { &g_guildColorBorderDB, &GuildColorBorderRec::s_meta },
    { &g_guildColorEmblemDB, &GuildColorEmblemRec::s_meta },
    { &g_helmetGeosetVisDataDB, &HelmetGeosetVisDataRec::s_meta },
    { &g_itemClassDB, &ItemClassRec::s_meta },
    { &g_itemDisplayInfoDB, &ItemDisplayInfoRec::s_meta },
    { &g_itemSubClassDB, &ItemSubClassRec::s_meta },
    { &g_itemVisualsDB, &ItemVisualsRec::s_meta },
    { &g_itemVisualEffectsDB, &ItemVisualEffectsRec::s_meta },
    { &g_lightDB, &LightRec::s_meta },
    { &g_lightIntBandDB, &LightIntBandRec::s_meta },
    { &g_lightParamsDB, &LightParamsRec::s_meta },
    { &g_loadingScreensDB, &LoadingScreensRec::s_meta },
    { &g_mapDB, &MapRec::s_meta },
    { &g_movieDB, &MovieRec::s_meta },
    { &g_movieFileDataDB, &MovieFileDataRec::s_meta },
    { &g_movieVariationDB, &MovieVariationRec::s_meta },
    { &g_nameGenDB, &NameGenRec::s_meta },
    { &g_namesProfanityDB, &NamesProfanityRec::s_meta },
    { &g_namesReservedDB, &NamesReservedRec::s_meta },
    { &g_objectEffectDB, &ObjectEffectRec::s_meta },
    { &g_objectEffectGroupDB, &ObjectEffectGroupRec::s_meta },
    { &g_objectEffectModifierDB, &ObjectEffectModifierRec::s_meta },
    { &g_objectEffectPackageDB, &ObjectEffectPackageRec::s_meta },
    { &g_objectEffectPackageElemDB, &ObjectEffectPackageElemRec::s_meta },
    { &g_particleColorDB, &ParticleColorRec::s_meta },
    { &g_resistancesDB, &ResistancesRec::s_meta },
    { &g_soundFilterDB, &SoundFilterRec::s_meta },
    { &g_soundFilterElemDB, &SoundFilterElemRec::s_meta },
    { &g_spamMessagesDB, &SpamMessagesRec::s_meta },
    { &g_spellEffectDB, &SpellEffectRec::s_meta },
    { &g_spellVisualEffectNameDB, &SpellVisualEffectNameRec::s_meta },
    { &g_spellVisualKitDB, &SpellVisualKitRec::s_meta },
    { &g_soundProviderPreferencesDB, &SoundProviderPreferencesRec::s_meta },
    { NULL, NULL },
};

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

// PROVEN: 836efa84 IDENTICAL/NONE
// OFFSET: 0x00495350
int32_t GetMapResetTime(int32_t mapId, uint32_t difficulty) {
    MapDifficultyRec* entry = GetMapDifficultyEntryByMapIdAndDifficulty(mapId, difficulty, NULL);

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

// OFFSET: #0x00634C60
void ClientDBShutdown() {
    s_resistances.Clear();
    s_terrainTypeSoundIDs.Clear();
}

// OFFSET: %0x0049CB30
static void ClientDBLoadTables() {
    char path[260];
    uint32_t hotfixTime = 0;

    WowClientDB2_Base::GetCacheDir(path, "Hotfix.tbl", false);
    HOSFILE file = OsCreateFile(path, 0x80000000, 1, 3, 0x80, 0x3F3F3F3F);

    if (file != OS_INVALID_FILE) {
        uint32_t buffer[4];
        uint32_t bytesRead;
        WowClientDB2_Base* table = NULL;
        uint32_t tableHash = 0;

        OsReadFile(file, buffer, 16, &bytesRead);

        if (bytesRead == 16 && buffer[0] == 'HTFX' && buffer[1] == 1 && buffer[3] == 13623) {
            hotfixTime = buffer[2];
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
        (*table)->Load(hotfixTime);
    }

    for (ClientDBTable* table = s_clientDBTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }

    for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
        SFile::MakeFileLocal(table->m_meta->m_filename, 2, false, -1, NULL);
    }
}

// OFFSET: %0x0049CF70
void ClientDBUnloadTables() {
    char path[260];
    uint32_t hotfixTime = 0;

    WowClientDB2_Base::GetCacheDir(path, "Hotfix.tbl", false);
    HOSFILE file = OsCreateFile(path, 0x40000000, 1, 2, 0x80, 0x3F3F3F3F);

    if (file != OS_INVALID_FILE) {
        uint32_t now = static_cast<uint32_t>(time(NULL));
        uint32_t header[4] = { 'HTFX', 1, now, 13623 };
        uint32_t bytesWritten;

        OsWriteFile(file, header, 16, &bytesWritten);

        if (bytesWritten == 16) {
            hotfixTime = now;

            for (WowClientDB2_Base** table = g_clientDB2Tables; *table; table++) {
                (*table)->WriteHotfixes(file);
            }
        }

        OsCloseFile(file);
    }

    for (ClientDBTable* table = s_clientDBTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }

    for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }

    for (WowClientDB2_Base** table = g_clientDB2Tables; *table; table++) {
        (*table)->Save(hotfixTime);
        (*table)->Unload();
    }
}

// OFFSET: %0x0049E570
void ClientDBInitializeInGame() {
    for (ClientDBTable* table = s_clientDBGameTables; table->m_db; table++) {
        table->m_db->Load(table->m_meta);
    }

    GameTablesInitialize();
    InitTerrainTypeFlags();
    InitSpellEffects();
    InitSpellActivationOverlays();
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

// OFFSET: %0x0049D670
void ClientDBInitialize() {
    CVar* cvar = CVar::Lookup("dbCompress");

    if (cvar) {
        g_clientDbPerformCompress = cvar->m_intValue != 0;
    }

    ClientDB2StartThread();
    ClientDBLoadTables();
    InitResistances();
    InitWeaponSubclasses();
    SDBItemSubclassInitialize();
    InitLights(1);
    g_hotfixNotifyBlobHandler = WowClientDB2_Base::HotfixNotifyBlob;
    g_hotfixNotifyMultiplexHandler = WowClientDB2_Base::HotfixNotifyMultiplex;
    g_dbReplyMultiplexHandler = WowClientDB2_Base::DBReplyMultiplex;
}

// OFFSET: #0x00635D90
int32_t LocaleNameToIndex(const char* locale) {
    for (int32_t i = 0; i < 12; i++) {
        if (!SStrCmpI(locale, g_localeNames[i], 4)) {
            return i;
        }
    }

    return 2;
}

// PROVEN: 9dcd0762 IDENTICAL/NONE
// OFFSET: 0x00495030/0x1400BA9A0
const char* ClientDBStringLookup(uint32_t lookup) {
    StringLookupsRec* rec = g_stringLookupsDB.GetRecord(lookup);
    return rec ? rec->m_string : NULL;
}

// OFFSET: #0x00634950
MapDifficultyRec* GetMapDifficultyEntryByMapIdAndDifficulty(int32_t mapId, uint32_t difficulty, int32_t* index) {
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

// PROVEN: be1740d5 IDENTICAL/NONE
// OFFSET: 0x00495380
int32_t GetMaxPlayersForMap(const MapRec* map, int32_t difficulty) {
    MapDifficultyRec* entry = GetMapDifficultyEntryByMapIdAndDifficulty(map->m_id, difficulty, NULL);

    if (!entry || !entry->m_maxPlayers) {
        return map->m_maxPlayers;
    }

    return entry->m_maxPlayers;
}
