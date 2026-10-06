#ifndef WOW_SOURCE_DB_DBCLIENT_DBCLIENT_H
#define WOW_SOURCE_DB_DBCLIENT_DBCLIENT_H

#include "WoW/Source/DB/WowClientDB.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/DB/Rec/AnimationDataRec.h"
#include "WoW/Source/DB/Rec/AnimKitRec.h"
#include "WoW/Source/DB/Rec/AnimKitBoneSetRec.h"
#include "WoW/Source/DB/Rec/AnimKitBoneSetAliasRec.h"
#include "WoW/Source/DB/Rec/AnimKitConfigRec.h"
#include "WoW/Source/DB/Rec/AnimKitConfigBoneSetRec.h"
#include "WoW/Source/DB/Rec/AnimKitPriorityRec.h"
#include "WoW/Source/DB/Rec/AnimKitSegmentRec.h"
#include "WoW/Source/DB/Rec/AnimReplacementRec.h"
#include "WoW/Source/DB/Rec/AnimReplacementSetRec.h"
#include "WoW/Source/DB/Rec/AreaAssignmentRec.h"
#include "WoW/Source/DB/Rec/AreaGroupRec.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/DB/Rec/AreaPOISortedWorldStateRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/AreaTriggerRec.h"
#include "WoW/Source/DB/Rec/ArmorLocationRec.h"
#include "WoW/Source/DB/Rec/AuctionHouseRec.h"
#include "WoW/Source/DB/Rec/BankBagSlotPricesRec.h"
#include "WoW/Source/DB/Rec/BannedAddOnsRec.h"
#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/DB/Rec/CameraModeRec.h"
#include "WoW/Source/DB/Rec/CameraShakesRec.h"
#include "WoW/Source/DB/Rec/CastableRaidBuffsRec.h"
#include "WoW/Source/DB/Rec/Cfg_CategoriesRec.h"
#include "WoW/Source/DB/Rec/Cfg_ConfigsRec.h"
#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"
#include "WoW/Source/DB/Rec/CharBaseInfoRec.h"
#include "WoW/Source/DB/Rec/CharHairGeosetsRec.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"
#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/DB/Rec/ChatChannelsRec.h"
#include "WoW/Source/DB/Rec/ChatProfanityRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrClassesXPowerTypesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CinematicCameraRec.h"
#include "WoW/Source/DB/Rec/CinematicSequencesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/CreatureImmunitiesRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"
#include "WoW/Source/DB/Rec/CreatureSpellDataRec.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"
#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/DanceMovesRec.h"
#include "WoW/Source/DB/Rec/DeathThudLookupsRec.h"
#include "WoW/Source/DB/Rec/DeclinedWordRec.h"
#include "WoW/Source/DB/Rec/DeclinedWordCasesRec.h"
#include "WoW/Source/DB/Rec/DestructibleModelDataRec.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/DungeonMapChunkRec.h"
#include "WoW/Source/DB/Rec/DurabilityCostsRec.h"
#include "WoW/Source/DB/Rec/DurabilityQualityRec.h"
#include "WoW/Source/DB/Rec/EmotesRec.h"
#include "WoW/Source/DB/Rec/EmotesTextRec.h"
#include "WoW/Source/DB/Rec/EmotesTextDataRec.h"
#include "WoW/Source/DB/Rec/EmotesTextSoundRec.h"
#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"
#include "WoW/Source/DB/Rec/ExhaustionRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/FactionGroupRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/FileDataRec.h"
#include "WoW/Source/DB/Rec/FootprintTexturesRec.h"
#include "WoW/Source/DB/Rec/FootstepTerrainLookupRec.h"
#include "WoW/Source/DB/Rec/GameObjectArtKitRec.h"
#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/GameTablesRec.h"
#include "WoW/Source/DB/Rec/GameTipsRec.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlueScreenEmoteRec.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlyphSlotRec.h"
#include "WoW/Source/DB/Rec/GMSurveyAnswersRec.h"
#include "WoW/Source/DB/Rec/GMSurveyCurrentSurveyRec.h"
#include "WoW/Source/DB/Rec/GMSurveyQuestionsRec.h"
#include "WoW/Source/DB/Rec/GMSurveySurveysRec.h"
#include "WoW/Source/DB/Rec/GMTicketCategoryRec.h"
#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"
#include "WoW/Source/DB/Rec/GroundEffectTextureRec.h"
#include "WoW/Source/DB/Rec/gtBarberShopCostBaseRec.h"
#include "WoW/Source/DB/Rec/gtChanceToMeleeCritRec.h"
#include "WoW/Source/DB/Rec/gtChanceToMeleeCritBaseRec.h"
#include "WoW/Source/DB/Rec/gtChanceToSpellCritRec.h"
#include "WoW/Source/DB/Rec/gtChanceToSpellCritBaseRec.h"
#include "WoW/Source/DB/Rec/gtCombatRatingsRec.h"
#include "WoW/Source/DB/Rec/gtNPCManaCostScalerRec.h"
#include "WoW/Source/DB/Rec/gtOCTBaseHPByClassRec.h"
#include "WoW/Source/DB/Rec/gtOCTBaseMPByClassRec.h"
#include "WoW/Source/DB/Rec/gtOCTClassCombatRatingScalarRec.h"
#include "WoW/Source/DB/Rec/gtOCTHpPerStaminaRec.h"
#include "WoW/Source/DB/Rec/gtOCTRegenMPRec.h"
#include "WoW/Source/DB/Rec/gtRegenMPPerSptRec.h"
#include "WoW/Source/DB/Rec/gtSpellScalingRec.h"
#include "WoW/Source/DB/Rec/GuildColorBackgroundRec.h"
#include "WoW/Source/DB/Rec/GuildColorBorderRec.h"
#include "WoW/Source/DB/Rec/GuildColorEmblemRec.h"
#include "WoW/Source/DB/Rec/GuildPerkSpellsRec.h"
#include "WoW/Source/DB/Rec/HelmetGeosetVisDataRec.h"
#include "WoW/Source/DB/Rec/HolidayDescriptionsRec.h"
#include "WoW/Source/DB/Rec/HolidayNamesRec.h"
#include "WoW/Source/DB/Rec/HolidaysRec.h"
#include "WoW/Source/DB/Rec/ImportPriceArmorRec.h"
#include "WoW/Source/DB/Rec/ImportPriceQualityRec.h"
#include "WoW/Source/DB/Rec/ImportPriceShieldRec.h"
#include "WoW/Source/DB/Rec/ImportPriceWeaponRec.h"
#include "WoW/Source/DB/Rec/ItemArmorQualityRec.h"
#include "WoW/Source/DB/Rec/ItemArmorShieldRec.h"
#include "WoW/Source/DB/Rec/ItemArmorTotalRec.h"
#include "WoW/Source/DB/Rec/ItemBagFamilyRec.h"
#include "WoW/Source/DB/Rec/ItemClassRec.h"
#include "WoW/Source/DB/Rec/ItemCurrencyCostRec.h"
#include "WoW/Source/DB/Rec/ItemDamageAmmoRec.h"
#include "WoW/Source/DB/Rec/ItemDamageOneHandRec.h"
#include "WoW/Source/DB/Rec/ItemDamageOneHandCasterRec.h"
#include "WoW/Source/DB/Rec/ItemDamageRangedRec.h"
#include "WoW/Source/DB/Rec/ItemDamageThrownRec.h"
#include "WoW/Source/DB/Rec/ItemDamageTwoHandRec.h"
#include "WoW/Source/DB/Rec/ItemDamageTwoHandCasterRec.h"
#include "WoW/Source/DB/Rec/ItemDamageWandRec.h"
#include "WoW/Source/DB/Rec/ItemDisenchantLootRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"
#include "WoW/Source/DB/Rec/ItemGroupSoundsRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/ItemNameDescriptionRec.h"
#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"
#include "WoW/Source/DB/Rec/ItemPriceBaseRec.h"
#include "WoW/Source/DB/Rec/ItemPurchaseGroupRec.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/ItemReforgeRec.h"
#include "WoW/Source/DB/Rec/ItemSetRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"
#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"
#include "WoW/Source/DB/Rec/ItemVisualsRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterCreatureRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterItemRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterSectionRec.h"
#include "WoW/Source/DB/Rec/JournalInstanceRec.h"
#include "WoW/Source/DB/Rec/LanguagesRec.h"
#include "WoW/Source/DB/Rec/LanguageWordsRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsGroupingMapRec.h"
#include "WoW/Source/DB/Rec/LightRec.h"
#include "WoW/Source/DB/Rec/LightFloatBandRec.h"
#include "WoW/Source/DB/Rec/LightIntBandRec.h"
#include "WoW/Source/DB/Rec/LightParamsRec.h"
#include "WoW/Source/DB/Rec/LightSkyboxRec.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidObjectRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/LoadingScreensRec.h"
#include "WoW/Source/DB/Rec/LoadingScreenTaxiSplinesRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/LockTypeRec.h"
#include "WoW/Source/DB/Rec/MailTemplateRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MaterialRec.h"
#include "WoW/Source/DB/Rec/MountCapabilityRec.h"
#include "WoW/Source/DB/Rec/MountTypeRec.h"
#include "WoW/Source/DB/Rec/MovieRec.h"
#include "WoW/Source/DB/Rec/MovieFileDataRec.h"
#include "WoW/Source/DB/Rec/MovieVariationRec.h"
#include "WoW/Source/DB/Rec/NameGenRec.h"
#include "WoW/Source/DB/Rec/NamesProfanityRec.h"
#include "WoW/Source/DB/Rec/NamesReservedRec.h"
#include "WoW/Source/DB/Rec/NPCSoundsRec.h"
#include "WoW/Source/DB/Rec/NumTalentsAtLevelRec.h"
#include "WoW/Source/DB/Rec/ObjectEffectRec.h"
#include "WoW/Source/DB/Rec/ObjectEffectGroupRec.h"
#include "WoW/Source/DB/Rec/ObjectEffectModifierRec.h"
#include "WoW/Source/DB/Rec/ObjectEffectPackageRec.h"
#include "WoW/Source/DB/Rec/ObjectEffectPackageElemRec.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/PackageRec.h"
#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"
#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"
#include "WoW/Source/DB/Rec/ParticleColorRec.h"
#include "WoW/Source/DB/Rec/PetPersonalityRec.h"
#include "WoW/Source/DB/Rec/PhaseRec.h"
#include "WoW/Source/DB/Rec/PhaseShiftZoneSoundsRec.h"
#include "WoW/Source/DB/Rec/PhaseXPhaseGroupRec.h"
#include "WoW/Source/DB/Rec/PlayerConditionRec.h"
#include "WoW/Source/DB/Rec/PowerDisplayRec.h"
#include "WoW/Source/DB/Rec/PvpDifficultyRec.h"
#include "WoW/Source/DB/Rec/QuestFactionRewardRec.h"
#include "WoW/Source/DB/Rec/QuestInfoRec.h"
#include "WoW/Source/DB/Rec/QuestPOIBlobRec.h"
#include "WoW/Source/DB/Rec/QuestPOIPointRec.h"
#include "WoW/Source/DB/Rec/QuestSortRec.h"
#include "WoW/Source/DB/Rec/QuestXPRec.h"
#include "WoW/Source/DB/Rec/RandPropPointsRec.h"
#include "WoW/Source/DB/Rec/ResearchBranchRec.h"
#include "WoW/Source/DB/Rec/ResearchFieldRec.h"
#include "WoW/Source/DB/Rec/ResearchProjectRec.h"
#include "WoW/Source/DB/Rec/ResearchSiteRec.h"
#include "WoW/Source/DB/Rec/ResistancesRec.h"
#include "WoW/Source/DB/Rec/ScalingStatDistributionRec.h"
#include "WoW/Source/DB/Rec/ScalingStatValuesRec.h"
#include "WoW/Source/DB/Rec/ScreenEffectRec.h"
#include "WoW/Source/DB/Rec/ScreenLocationRec.h"
#include "WoW/Source/DB/Rec/ServerMessagesRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilitySortedSpellRec.h"
#include "WoW/Source/DB/Rec/SkillLineCategoryRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SkillTiersRec.h"
#include "WoW/Source/DB/Rec/SoundAmbienceRec.h"
#include "WoW/Source/DB/Rec/SoundAmbienceFlavorRec.h"
#include "WoW/Source/DB/Rec/SoundEmitterPillPointsRec.h"
#include "WoW/Source/DB/Rec/SoundEmittersRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesAdvancedRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesFallbacksRec.h"
#include "WoW/Source/DB/Rec/SoundFilterRec.h"
#include "WoW/Source/DB/Rec/SoundFilterElemRec.h"
#include "WoW/Source/DB/Rec/SoundProviderPreferencesRec.h"
#include "WoW/Source/DB/Rec/SpamMessagesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellActivationOverlayRec.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellAuraRestrictionsRec.h"
#include "WoW/Source/DB/Rec/SpellAuraVisibilityRec.h"
#include "WoW/Source/DB/Rec/SpellAuraVisXTalentTabRec.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellCategoryRec.h"
#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"
#include "WoW/Source/DB/Rec/SpellClassOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"
#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"
#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentConditionRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellMechanicRec.h"
#include "WoW/Source/DB/Rec/SpellMissileRec.h"
#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"
#include "WoW/Source/DB/Rec/SpellPowerRec.h"
#include "WoW/Source/DB/Rec/SpellRadiusRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/DB/Rec/SpellScalingRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SpellSpecialUnitEffectRec.h"
#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "WoW/Source/DB/Rec/StringLookupsRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/DB/Rec/TalentRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/DB/Rec/TalentTreePrimarySpellsRec.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include "WoW/Source/DB/Rec/TaxiPathRec.h"
#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"
#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"
#include "WoW/Source/DB/Rec/TerrainTypeRec.h"
#include "WoW/Source/DB/Rec/TerrainTypeSoundsRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/DB/Rec/TransportAnimationRec.h"
#include "WoW/Source/DB/Rec/TransportPhysicsRec.h"
#include "WoW/Source/DB/Rec/TransportRotationRec.h"
#include "WoW/Source/DB/Rec/UnitBloodRec.h"
#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"
#include "WoW/Source/DB/Rec/UnitPowerBarRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/DB/Rec/VehicleUIIndicatorRec.h"
#include "WoW/Source/DB/Rec/VehicleUIIndSeatRec.h"
#include "WoW/Source/DB/Rec/VocalUISoundsRec.h"
#include "WoW/Source/DB/Rec/WeaponImpactSoundsRec.h"
#include "WoW/Source/DB/Rec/WeaponSwingSounds2Rec.h"
#include "WoW/Source/DB/Rec/WeatherRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"
#include "WoW/Source/DB/Rec/WorldChunkSoundsRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"
#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"
#include "WoW/Source/DB/Rec/WorldMapTransformsRec.h"
#include "WoW/Source/DB/Rec/WorldSafeLocsRec.h"
#include "WoW/Source/DB/Rec/WorldStateUIRec.h"
#include "WoW/Source/DB/Rec/WorldStateZoneSoundsRec.h"
#include "WoW/Source/DB/Rec/ZoneIntroMusicTableRec.h"
#include "WoW/Source/DB/Rec/ZoneLightRec.h"
#include "WoW/Source/DB/Rec/ZoneLightPointRec.h"
#include "WoW/Source/DB/Rec/ZoneMusicRec.h"

extern WowClientDB<AchievementRec> g_achievementDB;
extern WowClientDB<Achievement_CategoryRec> g_achievement_CategoryDB;
extern WowClientDB<Achievement_CriteriaRec> g_achievement_CriteriaDB;
extern WowClientDB<AnimKitRec> g_animKitDB;
extern WowClientDB<AnimKitBoneSetRec> g_animKitBoneSetDB;
extern WowClientDB<AnimKitBoneSetAliasRec> g_animKitBoneSetAliasDB;
extern WowClientDB<AnimKitConfigRec> g_animKitConfigDB;
extern WowClientDB<AnimKitConfigBoneSetRec> g_animKitConfigBoneSetDB;
extern WowClientDB<AnimKitPriorityRec> g_animKitPriorityDB;
extern WowClientDB<AnimKitSegmentRec> g_animKitSegmentDB;
extern WowClientDB<AnimReplacementRec> g_animReplacementDB;
extern WowClientDB<AnimReplacementSetRec> g_animReplacementSetDB;
extern WowClientDB<AnimationDataRec> g_animationDataDB;
extern WowClientDB<AreaAssignmentRec> g_areaAssignmentDB;
extern WowClientDB<AreaGroupRec> g_areaGroupDB;
extern WowClientDB<AreaPOIRec> g_areaPOIDB;
extern WowClientDB<AreaPOISortedWorldStateRec> g_areaPOISortedWorldStateDB;
extern WowClientDB<AreaTableRec> g_areaTableDB;
extern WowClientDB<AreaTriggerRec> g_areaTriggerDB;
extern WowClientDB<ArmorLocationRec> g_armorLocationDB;
extern WowClientDB<AuctionHouseRec> g_auctionHouseDB;
extern WowClientDB<BankBagSlotPricesRec> g_bankBagSlotPricesDB;
extern WowClientDB<BannedAddOnsRec> g_bannedAddOnsDB;
extern WowClientDB<BarberShopStyleRec> g_barberShopStyleDB;
extern WowClientDB<BattlemasterListRec> g_battlemasterListDB;
extern WowClientDB<CameraModeRec> g_cameraModeDB;
extern WowClientDB<CameraShakesRec> g_cameraShakesDB;
extern WowClientDB<CastableRaidBuffsRec> g_castableRaidBuffsDB;
extern WowClientDB<Cfg_CategoriesRec> g_cfg_CategoriesDB;
extern WowClientDB<Cfg_ConfigsRec> g_cfg_ConfigsDB;
extern WowClientDB<CharBaseInfoRec> g_charBaseInfoDB;
extern WowClientDB<CharHairGeosetsRec> g_charHairGeosetsDB;
extern WowClientDB<CharSectionsRec> g_charSectionsDB;
extern WowClientDB<CharStartOutfitRec> g_charStartOutfitDB;
extern WowClientDB<CharTitlesRec> g_charTitlesDB;
extern WowClientDB<CharacterFacialHairStylesRec> g_characterFacialHairStylesDB;
extern WowClientDB<ChatChannelsRec> g_chatChannelsDB;
extern WowClientDB<ChatProfanityRec> g_chatProfanityDB;
extern WowClientDB<ChrClassesRec> g_chrClassesDB;
extern WowClientDB<ChrClassesXPowerTypesRec> g_chrClassesXPowerTypesDB;
extern WowClientDB<ChrRacesRec> g_chrRacesDB;
extern WowClientDB<CinematicCameraRec> g_cinematicCameraDB;
extern WowClientDB<CinematicSequencesRec> g_cinematicSequencesDB;
extern WowClientDB<CreatureDisplayInfoRec> g_creatureDisplayInfoDB;
extern WowClientDB<CreatureDisplayInfoExtraRec> g_creatureDisplayInfoExtraDB;
extern WowClientDB<CreatureFamilyRec> g_creatureFamilyDB;
extern WowClientDB<CreatureImmunitiesRec> g_creatureImmunitiesDB;
extern WowClientDB<CreatureModelDataRec> g_creatureModelDataDB;
extern WowClientDB<CreatureMovementInfoRec> g_creatureMovementInfoDB;
extern WowClientDB<CreatureSoundDataRec> g_creatureSoundDataDB;
extern WowClientDB<CreatureSpellDataRec> g_creatureSpellDataDB;
extern WowClientDB<CreatureTypeRec> g_creatureTypeDB;
extern WowClientDB<CurrencyCategoryRec> g_currencyCategoryDB;
extern WowClientDB<CurrencyTypesRec> g_currencyTypesDB;
extern WowClientDB<DanceMovesRec> g_danceMovesDB;
extern WowClientDB<DeathThudLookupsRec> g_deathThudLookupsDB;
extern WowClientDB<DeclinedWordRec> g_declinedWordDB;
extern WowClientDB<DeclinedWordCasesRec> g_declinedWordCasesDB;
extern WowClientDB<DestructibleModelDataRec> g_destructibleModelDataDB;
extern WowClientDB<DungeonEncounterRec> g_dungeonEncounterDB;
extern WowClientDB<DungeonMapRec> g_dungeonMapDB;
extern WowClientDB<DungeonMapChunkRec> g_dungeonMapChunkDB;
extern WowClientDB<DurabilityCostsRec> g_durabilityCostsDB;
extern WowClientDB<DurabilityQualityRec> g_durabilityQualityDB;
extern WowClientDB<EmotesRec> g_emotesDB;
extern WowClientDB<EmotesTextRec> g_emotesTextDB;
extern WowClientDB<EmotesTextDataRec> g_emotesTextDataDB;
extern WowClientDB<EmotesTextSoundRec> g_emotesTextSoundDB;
extern WowClientDB<EnvironmentalDamageRec> g_environmentalDamageDB;
extern WowClientDB<ExhaustionRec> g_exhaustionDB;
extern WowClientDB<FactionRec> g_factionDB;
extern WowClientDB<FactionGroupRec> g_factionGroupDB;
extern WowClientDB<FactionTemplateRec> g_factionTemplateDB;
extern WowClientDB<FileDataRec> g_fileDataDB;
extern WowClientDB<FootprintTexturesRec> g_footprintTexturesDB;
extern WowClientDB<FootstepTerrainLookupRec> g_footstepTerrainLookupDB;
extern WowClientDB<GMSurveyAnswersRec> g_gmSurveyAnswersDB;
extern WowClientDB<GMSurveyCurrentSurveyRec> g_gmSurveyCurrentSurveyDB;
extern WowClientDB<GMSurveyQuestionsRec> g_gmSurveyQuestionsDB;
extern WowClientDB<GMSurveySurveysRec> g_gmSurveySurveysDB;
extern WowClientDB<GMTicketCategoryRec> g_gmTicketCategoryDB;
extern WowClientDB<GameObjectArtKitRec> g_gameObjectArtKitDB;
extern WowClientDB<GameObjectDisplayInfoRec> g_gameObjectDisplayInfoDB;
extern WowClientDB<GameTablesRec> g_gameTablesDB;
extern WowClientDB<GameTipsRec> g_gameTipsDB;
extern WowClientDB<GemPropertiesRec> g_gemPropertiesDB;
extern WowClientDB<GlueScreenEmoteRec> g_glueScreenEmoteDB;
extern WowClientDB<GlyphPropertiesRec> g_glyphPropertiesDB;
extern WowClientDB<GlyphSlotRec> g_glyphSlotDB;
extern WowClientDB<GroundEffectDoodadRec> g_groundEffectDoodadDB;
extern WowClientDB<GroundEffectTextureRec> g_groundEffectTextureDB;
extern WowClientDB<GuildColorBackgroundRec> g_guildColorBackgroundDB;
extern WowClientDB<GuildColorBorderRec> g_guildColorBorderDB;
extern WowClientDB<GuildColorEmblemRec> g_guildColorEmblemDB;
extern WowClientDB<GuildPerkSpellsRec> g_guildPerkSpellsDB;
extern WowClientDB<HelmetGeosetVisDataRec> g_helmetGeosetVisDataDB;
extern WowClientDB<HolidayDescriptionsRec> g_holidayDescriptionsDB;
extern WowClientDB<HolidayNamesRec> g_holidayNamesDB;
extern WowClientDB<HolidaysRec> g_holidaysDB;
extern WowClientDB<ImportPriceArmorRec> g_importPriceArmorDB;
extern WowClientDB<ImportPriceQualityRec> g_importPriceQualityDB;
extern WowClientDB<ImportPriceShieldRec> g_importPriceShieldDB;
extern WowClientDB<ImportPriceWeaponRec> g_importPriceWeaponDB;
extern WowClientDB<ItemArmorQualityRec> g_itemArmorQualityDB;
extern WowClientDB<ItemArmorShieldRec> g_itemArmorShieldDB;
extern WowClientDB<ItemArmorTotalRec> g_itemArmorTotalDB;
extern WowClientDB<ItemBagFamilyRec> g_itemBagFamilyDB;
extern WowClientDB<ItemClassRec> g_itemClassDB;
extern WowClientDB<ItemCurrencyCostRec> g_itemCurrencyCostDB;
extern WowClientDB<ItemDamageAmmoRec> g_itemDamageAmmoDB;
extern WowClientDB<ItemDamageOneHandRec> g_itemDamageOneHandDB;
extern WowClientDB<ItemDamageOneHandCasterRec> g_itemDamageOneHandCasterDB;
extern WowClientDB<ItemDamageRangedRec> g_itemDamageRangedDB;
extern WowClientDB<ItemDamageThrownRec> g_itemDamageThrownDB;
extern WowClientDB<ItemDamageTwoHandRec> g_itemDamageTwoHandDB;
extern WowClientDB<ItemDamageTwoHandCasterRec> g_itemDamageTwoHandCasterDB;
extern WowClientDB<ItemDamageWandRec> g_itemDamageWandDB;
extern WowClientDB<ItemDisenchantLootRec> g_itemDisenchantLootDB;
extern WowClientDB_RLE<ItemDisplayInfoRec> g_itemDisplayInfoDB;
extern WowClientDB<ItemExtendedCostRec> g_itemExtendedCostDB;
extern WowClientDB<ItemGroupSoundsRec> g_itemGroupSoundsDB;
extern WowClientDB<ItemLimitCategoryRec> g_itemLimitCategoryDB;
extern WowClientDB<ItemNameDescriptionRec> g_itemNameDescriptionDB;
extern WowClientDB<ItemPetFoodRec> g_itemPetFoodDB;
extern WowClientDB<ItemPriceBaseRec> g_itemPriceBaseDB;
extern WowClientDB<ItemPurchaseGroupRec> g_itemPurchaseGroupDB;
extern WowClientDB<ItemRandomPropertiesRec> g_itemRandomPropertiesDB;
extern WowClientDB<ItemRandomSuffixRec> g_itemRandomSuffixDB;
extern WowClientDB<ItemReforgeRec> g_itemReforgeDB;
extern WowClientDB<ItemSetRec> g_itemSetDB;
extern WowClientDB<ItemSubClassRec> g_itemSubClassDB;
extern WowClientDB<ItemSubClassMaskRec> g_itemSubClassMaskDB;
extern WowClientDB<ItemVisualEffectsRec> g_itemVisualEffectsDB;
extern WowClientDB<ItemVisualsRec> g_itemVisualsDB;
extern WowClientDB<JournalEncounterRec> g_journalEncounterDB;
extern WowClientDB<JournalEncounterCreatureRec> g_journalEncounterCreatureDB;
extern WowClientDB<JournalEncounterItemRec> g_journalEncounterItemDB;
extern WowClientDB<JournalEncounterSectionRec> g_journalEncounterSectionDB;
extern WowClientDB<JournalInstanceRec> g_journalInstanceDB;
extern WowClientDB<LanguageWordsRec> g_languageWordsDB;
extern WowClientDB<LanguagesRec> g_languagesDB;
extern WowClientDB<LfgDungeonExpansionRec> g_lfgDungeonExpansionDB;
extern WowClientDB<LfgDungeonGroupRec> g_lfgDungeonGroupDB;
extern WowClientDB<LfgDungeonsRec> g_lfgDungeonsDB;
extern WowClientDB<LfgDungeonsGroupingMapRec> g_lfgDungeonsGroupingMapDB;
extern WowClientDB<LightRec> g_lightDB;
extern WowClientDB<LightFloatBandRec> g_lightFloatBandDB;
extern WowClientDB<LightIntBandRec> g_lightIntBandDB;
extern WowClientDB<LightParamsRec> g_lightParamsDB;
extern WowClientDB<LightSkyboxRec> g_lightSkyboxDB;
extern WowClientDB<LiquidMaterialRec> g_liquidMaterialDB;
extern WowClientDB<LiquidObjectRec> g_liquidObjectDB;
extern WowClientDB<LiquidTypeRec> g_liquidTypeDB;
extern WowClientDB<LoadingScreenTaxiSplinesRec> g_loadingScreenTaxiSplinesDB;
extern WowClientDB<LoadingScreensRec> g_loadingScreensDB;
extern WowClientDB<LockRec> g_lockDB;
extern WowClientDB<LockTypeRec> g_lockTypeDB;
extern WowClientDB<MailTemplateRec> g_mailTemplateDB;
extern WowClientDB<MapRec> g_mapDB;
extern WowClientDB<MapDifficultyRec> g_mapDifficultyDB;
extern WowClientDB<MaterialRec> g_materialDB;
extern WowClientDB<MountCapabilityRec> g_mountCapabilityDB;
extern WowClientDB<MountTypeRec> g_mountTypeDB;
extern WowClientDB<MovieRec> g_movieDB;
extern WowClientDB<MovieFileDataRec> g_movieFileDataDB;
extern WowClientDB<MovieVariationRec> g_movieVariationDB;
extern WowClientDB<NPCSoundsRec> g_npcSoundsDB;
extern WowClientDB<NameGenRec> g_nameGenDB;
extern WowClientDB<NamesProfanityRec> g_namesProfanityDB;
extern WowClientDB<NamesReservedRec> g_namesReservedDB;
extern WowClientDB<NumTalentsAtLevelRec> g_numTalentsAtLevelDB;
extern WowClientDB<ObjectEffectRec> g_objectEffectDB;
extern WowClientDB<ObjectEffectGroupRec> g_objectEffectGroupDB;
extern WowClientDB<ObjectEffectModifierRec> g_objectEffectModifierDB;
extern WowClientDB<ObjectEffectPackageRec> g_objectEffectPackageDB;
extern WowClientDB<ObjectEffectPackageElemRec> g_objectEffectPackageElemDB;
extern WowClientDB<OverrideSpellDataRec> g_overrideSpellDataDB;
extern WowClientDB<PackageRec> g_packageDB;
extern WowClientDB<PageTextMaterialRec> g_pageTextMaterialDB;
extern WowClientDB<PaperDollItemFrameRec> g_paperDollItemFrameDB;
extern WowClientDB<ParticleColorRec> g_particleColorDB;
extern WowClientDB<PetPersonalityRec> g_petPersonalityDB;
extern WowClientDB<PhaseRec> g_phaseDB;
extern WowClientDB<PhaseShiftZoneSoundsRec> g_phaseShiftZoneSoundsDB;
extern WowClientDB<PhaseXPhaseGroupRec> g_phaseXPhaseGroupDB;
extern WowClientDB<PlayerConditionRec> g_playerConditionDB;
extern WowClientDB<PowerDisplayRec> g_powerDisplayDB;
extern WowClientDB<PvpDifficultyRec> g_pvpDifficultyDB;
extern WowClientDB<QuestFactionRewardRec> g_questFactionRewardDB;
extern WowClientDB<QuestInfoRec> g_questInfoDB;
extern WowClientDB<QuestPOIBlobRec> g_questPOIBlobDB;
extern WowClientDB<QuestPOIPointRec> g_questPOIPointDB;
extern WowClientDB<QuestSortRec> g_questSortDB;
extern WowClientDB<QuestXPRec> g_questXPDB;
extern WowClientDB<RandPropPointsRec> g_randPropPointsDB;
extern WowClientDB<ResearchBranchRec> g_researchBranchDB;
extern WowClientDB<ResearchFieldRec> g_researchFieldDB;
extern WowClientDB<ResearchProjectRec> g_researchProjectDB;
extern WowClientDB<ResearchSiteRec> g_researchSiteDB;
extern WowClientDB<ResistancesRec> g_resistancesDB;
extern WowClientDB<ScalingStatDistributionRec> g_scalingStatDistributionDB;
extern WowClientDB<ScalingStatValuesRec> g_scalingStatValuesDB;
extern WowClientDB<ScreenEffectRec> g_screenEffectDB;
extern WowClientDB<ScreenLocationRec> g_screenLocationDB;
extern WowClientDB<ServerMessagesRec> g_serverMessagesDB;
extern WowClientDB<SkillLineRec> g_skillLineDB;
extern WowClientDB<SkillLineAbilityRec> g_skillLineAbilityDB;
extern WowClientDB<SkillLineAbilitySortedSpellRec> g_skillLineAbilitySortedSpellDB;
extern WowClientDB<SkillLineCategoryRec> g_skillLineCategoryDB;
extern WowClientDB<SkillRaceClassInfoRec> g_skillRaceClassInfoDB;
extern WowClientDB<SkillTiersRec> g_skillTiersDB;
extern WowClientDB<SoundAmbienceRec> g_soundAmbienceDB;
extern WowClientDB<SoundAmbienceFlavorRec> g_soundAmbienceFlavorDB;
extern WowClientDB<SoundEmitterPillPointsRec> g_soundEmitterPillPointsDB;
extern WowClientDB<SoundEmittersRec> g_soundEmittersDB;
extern WowClientDB<SoundEntriesRec> g_soundEntriesDB;
extern WowClientDB<SoundEntriesAdvancedRec> g_soundEntriesAdvancedDB;
extern WowClientDB<SoundEntriesFallbacksRec> g_soundEntriesFallbacksDB;
extern WowClientDB<SoundFilterRec> g_soundFilterDB;
extern WowClientDB<SoundFilterElemRec> g_soundFilterElemDB;
extern WowClientDB<SoundProviderPreferencesRec> g_soundProviderPreferencesDB;
extern WowClientDB<SpamMessagesRec> g_spamMessagesDB;
extern WowClientDB<SpellRec> g_spellDB;
extern WowClientDB<SpellActivationOverlayRec> g_spellActivationOverlayDB;
extern WowClientDB<SpellAuraOptionsRec> g_spellAuraOptionsDB;
extern WowClientDB<SpellAuraRestrictionsRec> g_spellAuraRestrictionsDB;
extern WowClientDB<SpellAuraVisXTalentTabRec> g_spellAuraVisXTalentTabDB;
extern WowClientDB<SpellAuraVisibilityRec> g_spellAuraVisibilityDB;
extern WowClientDB<SpellCastTimesRec> g_spellCastTimesDB;
extern WowClientDB<SpellCastingRequirementsRec> g_spellCastingRequirementsDB;
extern WowClientDB<SpellCategoriesRec> g_spellCategoriesDB;
extern WowClientDB<SpellCategoryRec> g_spellCategoryDB;
extern WowClientDB<SpellChainEffectsRec> g_spellChainEffectsDB;
extern WowClientDB<SpellClassOptionsRec> g_spellClassOptionsDB;
extern WowClientDB<SpellCooldownsRec> g_spellCooldownsDB;
extern WowClientDB<SpellDescriptionVariablesRec> g_spellDescriptionVariablesDB;
extern WowClientDB<SpellDifficultyRec> g_spellDifficultyDB;
extern WowClientDB<SpellDispelTypeRec> g_spellDispelTypeDB;
extern WowClientDB<SpellDurationRec> g_spellDurationDB;
extern WowClientDB<SpellEffectRec> g_spellEffectDB;
extern WowClientDB<SpellEffectCameraShakesRec> g_spellEffectCameraShakesDB;
extern WowClientDB<SpellEquippedItemsRec> g_spellEquippedItemsDB;
extern WowClientDB<SpellFlyoutRec> g_spellFlyoutDB;
extern WowClientDB<SpellFlyoutItemRec> g_spellFlyoutItemDB;
extern WowClientDB<SpellFocusObjectRec> g_spellFocusObjectDB;
extern WowClientDB<SpellIconRec> g_spellIconDB;
extern WowClientDB<SpellInterruptsRec> g_spellInterruptsDB;
extern WowClientDB<SpellItemEnchantmentRec> g_spellItemEnchantmentDB;
extern WowClientDB<SpellItemEnchantmentConditionRec> g_spellItemEnchantmentConditionDB;
extern WowClientDB<SpellLevelsRec> g_spellLevelsDB;
extern WowClientDB<SpellMechanicRec> g_spellMechanicDB;
extern WowClientDB<SpellMissileRec> g_spellMissileDB;
extern WowClientDB<SpellMissileMotionRec> g_spellMissileMotionDB;
extern WowClientDB<SpellPowerRec> g_spellPowerDB;
extern WowClientDB<SpellRadiusRec> g_spellRadiusDB;
extern WowClientDB<SpellRangeRec> g_spellRangeDB;
extern WowClientDB<SpellReagentsRec> g_spellReagentsDB;
extern WowClientDB<SpellRuneCostRec> g_spellRuneCostDB;
extern WowClientDB<SpellScalingRec> g_spellScalingDB;
extern WowClientDB<SpellShapeshiftRec> g_spellShapeshiftDB;
extern WowClientDB<SpellShapeshiftFormRec> g_spellShapeshiftFormDB;
extern WowClientDB<SpellSpecialUnitEffectRec> g_spellSpecialUnitEffectDB;
extern WowClientDB<SpellTargetRestrictionsRec> g_spellTargetRestrictionsDB;
extern WowClientDB<SpellTotemsRec> g_spellTotemsDB;
extern WowClientDB<SpellVisualRec> g_spellVisualDB;
extern WowClientDB<SpellVisualEffectNameRec> g_spellVisualEffectNameDB;
extern WowClientDB<SpellVisualKitRec> g_spellVisualKitDB;
extern WowClientDB<SpellVisualKitAreaModelRec> g_spellVisualKitAreaModelDB;
extern WowClientDB<SpellVisualKitModelAttachRec> g_spellVisualKitModelAttachDB;
extern WowClientDB<StationeryRec> g_stationeryDB;
extern WowClientDB<StringLookupsRec> g_stringLookupsDB;
extern WowClientDB<SummonPropertiesRec> g_summonPropertiesDB;
extern WowClientDB<TalentRec> g_talentDB;
extern WowClientDB<TalentTabRec> g_talentTabDB;
extern WowClientDB<TalentTreePrimarySpellsRec> g_talentTreePrimarySpellsDB;
extern WowClientDB<TaxiNodesRec> g_taxiNodesDB;
extern WowClientDB<TaxiPathRec> g_taxiPathDB;
extern WowClientDB<TaxiPathNodeRec> g_taxiPathNodeDB;
extern WowClientDB<TerrainMaterialRec> g_terrainMaterialDB;
extern WowClientDB<TerrainTypeRec> g_terrainTypeDB;
extern WowClientDB<TerrainTypeSoundsRec> g_terrainTypeSoundsDB;
extern WowClientDB<TotemCategoryRec> g_totemCategoryDB;
extern WowClientDB<TransportAnimationRec> g_transportAnimationDB;
extern WowClientDB<TransportPhysicsRec> g_transportPhysicsDB;
extern WowClientDB<TransportRotationRec> g_transportRotationDB;
extern WowClientDB<UnitBloodRec> g_unitBloodDB;
extern WowClientDB<UnitBloodLevelsRec> g_unitBloodLevelsDB;
extern WowClientDB<UnitPowerBarRec> g_unitPowerBarDB;
extern WowClientDB<VehicleRec> g_vehicleDB;
extern WowClientDB<VehicleSeatRec> g_vehicleSeatDB;
extern WowClientDB<VehicleUIIndSeatRec> g_vehicleUIIndSeatDB;
extern WowClientDB<VehicleUIIndicatorRec> g_vehicleUIIndicatorDB;
extern WowClientDB<VocalUISoundsRec> g_vocalUISoundsDB;
extern WowClientDB<WMOAreaTableRec> g_wmoAreaTableDB;
extern WowClientDB<WeaponImpactSoundsRec> g_weaponImpactSoundsDB;
extern WowClientDB<WeaponSwingSounds2Rec> g_weaponSwingSounds2DB;
extern WowClientDB<WeatherRec> g_weatherDB;
extern WowClientDB<WorldChunkSoundsRec> g_worldChunkSoundsDB;
extern WowClientDB<WorldMapAreaRec> g_worldMapAreaDB;
extern WowClientDB<WorldMapContinentRec> g_worldMapContinentDB;
extern WowClientDB<WorldMapOverlayRec> g_worldMapOverlayDB;
extern WowClientDB<WorldMapTransformsRec> g_worldMapTransformsDB;
extern WowClientDB<WorldSafeLocsRec> g_worldSafeLocsDB;
extern WowClientDB<WorldStateUIRec> g_worldStateUIDB;
extern WowClientDB<WorldStateZoneSoundsRec> g_worldStateZoneSoundsDB;
extern WowClientDB<World_PVP_AreaRec> g_world_PVP_AreaDB;
extern WowClientDB<ZoneIntroMusicTableRec> g_zoneIntroMusicTableDB;
extern WowClientDB<ZoneLightRec> g_zoneLightDB;
extern WowClientDB<ZoneLightPointRec> g_zoneLightPointDB;
extern WowClientDB<ZoneMusicRec> g_zoneMusicDB;
extern WowClientDB<gtBarberShopCostBaseRec> g_gtBarberShopCostBaseDB;
extern WowClientDB<gtChanceToMeleeCritRec> g_gtChanceToMeleeCritDB;
extern WowClientDB<gtChanceToMeleeCritBaseRec> g_gtChanceToMeleeCritBaseDB;
extern WowClientDB<gtChanceToSpellCritRec> g_gtChanceToSpellCritDB;
extern WowClientDB<gtChanceToSpellCritBaseRec> g_gtChanceToSpellCritBaseDB;
extern WowClientDB<gtCombatRatingsRec> g_gtCombatRatingsDB;
extern WowClientDB<gtNPCManaCostScalerRec> g_gtNPCManaCostScalerDB;
extern WowClientDB<gtOCTBaseHPByClassRec> g_gtOCTBaseHPByClassDB;
extern WowClientDB<gtOCTBaseMPByClassRec> g_gtOCTBaseMPByClassDB;
extern WowClientDB<gtOCTClassCombatRatingScalarRec> g_gtOCTClassCombatRatingScalarDB;
extern WowClientDB<gtOCTHpPerStaminaRec> g_gtOCTHpPerStaminaDB;
extern WowClientDB<gtOCTRegenMPRec> g_gtOCTRegenMPDB;
extern WowClientDB<gtRegenMPPerSptRec> g_gtRegenMPPerSptDB;
extern WowClientDB<gtSpellScalingRec> g_gtSpellScalingDB;

extern const char* g_localeNames[12];
MapDifficultyRec* GetMapDifficultyEntryByMapIdAndDifficulty(int32_t mapId, uint32_t difficulty, int32_t* a3);
int32_t GetMaxPlayersForMap(const MapRec* map, int32_t difficulty);
int32_t GetMapResetTime(int32_t mapId, uint32_t difficulty);
int32_t ClientDBGetNumWeaponSubClasses();
int32_t ClientDBGetUnarmedWeapon();
extern int32_t g_clientDBWorldTablesLoaded;
extern int32_t g_clientDBGlueTablesLoaded;

void ClientDBInitialize();
void ClientDBInitializeGlue();
void ClientDBInitializeInGame();
void ClientDBShutdown();
void GameTablesInitialize();
void InitLights(bool transform);
void InitResistances();
void InitTerrainTypeFlags();
void InitSpellEffects();
void InitSpellActivationOverlays();
void InitWeaponSubclasses();
void SDBItemSubclassInitialize();
const ItemSubClassRec* SDBItemSubclassGetSubClassRec(uint32_t classID, uint32_t subClassID);
int32_t GetPhysicalDamageClassID();
uint32_t GetPhysicalDamageClassMask();
int32_t GetMapResetTime(int32_t mapId, uint32_t difficulty);
int32_t LocaleNameToIndex(const char* locale);
void DBCache_ClearTemporaryCaches();
const char* ClientDBStringLookup(uint32_t lookup);

#endif
