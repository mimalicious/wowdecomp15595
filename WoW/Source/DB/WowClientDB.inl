// INLINED
inline DBCRelativeString::DBCRelativeString() {
}

// INLINED
inline DBCRelativeString::DBCRelativeString(const DBCRelativeString& other) {
    this->Set(other);
}

// INLINED
inline DBCRelativeString& DBCRelativeString::operator=(const DBCRelativeString& other) {
    this->Set(other);
    return *this;
}

// INLINED
inline DBCRelativeString& DBCRelativeString::operator=(const char* value) {
    this->m_offset = static_cast<int32_t>(value - reinterpret_cast<const char*>(this));
    return *this;
}

// INLINED
inline void DBCRelativeString::Set(const DBCRelativeString& other) {
    const char* value = other.m_offset ? reinterpret_cast<const char*>(&other) + other.m_offset : NULL;
    this->m_offset = value && *value ? static_cast<int32_t>(value - reinterpret_cast<const char*>(this)) : 0;
}

// INLINED
inline DBCRelativeString::operator const char*() const {
    return this->m_offset ? reinterpret_cast<const char*>(this) + this->m_offset : "";
}

// INLINED
inline void DBCFixupString(const char*& string, const char* strings) {
    string = &strings[reinterpret_cast<uintptr_t>(string)];
}

// INLINED
inline void DBCFixupString(DBCRelativeString& string, const char* strings) {
    const char* value = &strings[string.m_offset];
    string.m_offset = value && *value ? static_cast<int32_t>(value - reinterpret_cast<const char*>(&string)) : 0;
}

// OFFSET: [0x0048AA50<LiquidTypeRec>, 0x0048AA50<SpellChainEffectsRec>, %0x00495110<SoundEntriesRec>, 0x004978C0<AreaPOIRec>, 0x00497DF0<AreaTableRec>, 0x00497DF0<MountTypeRec>, 0x00497DF0<UnitPowerBarRec>, 0x00497D10<AnimKitBoneSetRec>, 0x00497D10<ArmorLocationRec>, 0x00497D10<CharHairGeosetsRec>, 0x00497D10<CharTitlesRec>, 0x00497D10<GemPropertiesRec>, 0x00497D10<ItemArmorTotalRec>, 0x00497D10<ItemVisualsRec>, 0x00497D10<LiquidObjectRec>, 0x00497D10<PvpDifficultyRec>, 0x00497D10<ResearchBranchRec>, 0x00497D10<SpellInterruptsRec>, 0x00497D10<SpellShapeshiftRec>, 0x00497D10<SummonPropertiesRec>, 0x00497D10<WorldSafeLocsRec>, 0x00497900<BattlemasterListRec>, 0x00497900<MapRec>, 0x00497940<CameraModeRec>, 0x00497940<CreatureDisplayInfoRec>, 0x00497940<LfgDungeonsRec>, 0x00497940<SpellReagentsRec>, 0x00497940<WorldStateUIRec>, 0x00497980<CharStartOutfitRec>, 0x00497A40<ChrRacesRec>, 0x00497A40<DestructibleModelDataRec>, 0x00497A40<SoundEntriesAdvancedRec>, 0x00497A40<SoundProviderPreferencesRec>, 0x00497BC0<CinematicCameraRec>, 0x00497BC0<EmotesRec>, 0x00497BC0<ExhaustionRec>, 0x00497BC0<ItemDisenchantLootRec>, 0x00497BC0<MapDifficultyRec>, 0x00497BC0<SpellCastingRequirementsRec>, 0x00497BC0<SpellCategoriesRec>, 0x00497BC0<SpellClassOptionsRec>, 0x00497BC0<SpellFlyoutRec>, 0x00497BC0<SpellPowerRec>, 0x00497BC0<SpellVisualEffectNameRec>, 0x00497BC0<TerrainTypeRec>, 0x00497BC0<TransportAnimationRec>, 0x00497BC0<TransportRotationRec>, 0x00497BC0<VocalUISoundsRec>, 0x00497BC0<World_PVP_AreaRec>, 0x00497FB0<CreatureDisplayInfoExtraRec>, 0x00497FB0<GameObjectDisplayInfoRec>, 0x00497FB0<SpellShapeshiftFormRec>, 0x00497A00<CreatureSoundDataRec>, 0x00497A00<VehicleRec>, 0x00497A80<CreatureModelDataRec>, 0x00497A80<DurabilityCostsRec>, 0x004981A0<AnimKitBoneSetAliasRec>, 0x004981A0<AnimKitRec>, 0x004981A0<AreaPOISortedWorldStateRec>, 0x004981A0<CastableRaidBuffsRec>, 0x004981A0<ChatProfanityRec>, 0x004981A0<CreatureTypeRec>, 0x004981A0<CurrencyCategoryRec>, 0x004981A0<EnvironmentalDamageRec>, 0x004981A0<FileDataRec>, 0x004981A0<GlyphSlotRec>, 0x004981A0<GroundEffectDoodadRec>, 0x004981A0<GuildPerkSpellsRec>, 0x004981A0<LanguageWordsRec>, 0x004981A0<LightSkyboxRec>, 0x004981A0<LiquidMaterialRec>, 0x004981A0<MailTemplateRec>, 0x004981A0<MovieVariationRec>, 0x004981A0<NamesProfanityRec>, 0x004981A0<NamesReservedRec>, 0x004981A0<PhaseRec>, 0x004981A0<PhaseXPhaseGroupRec>, 0x004981A0<ResearchFieldRec>, 0x004981A0<SkillLineCategoryRec>, 0x004981A0<SoundAmbienceRec>, 0x004981A0<SoundEntriesFallbacksRec>, 0x004981A0<SpellVisualKitAreaModelRec>, 0x00497B00<AnimKitConfigRec>, 0x00497B00<AnimKitPriorityRec>, 0x00497B00<AnimReplacementSetRec>, 0x00497B00<BankBagSlotPricesRec>, 0x00497B00<CharBaseInfoRec>, 0x00497B00<CreatureMovementInfoRec>, 0x00497B00<DurabilityQualityRec>, 0x00497B00<EmotesTextDataRec>, 0x00497B00<FootprintTexturesRec>, 0x00497B00<GMSurveyCurrentSurveyRec>, 0x00497B00<GMSurveyQuestionsRec>, 0x00497B00<GMTicketCategoryRec>, 0x00497B00<GuildColorBackgroundRec>, 0x00497B00<GuildColorBorderRec>, 0x00497B00<GuildColorEmblemRec>, 0x00497B00<HolidayDescriptionsRec>, 0x00497B00<HolidayNamesRec>, 0x00497B00<ItemBagFamilyRec>, 0x00497B00<ItemCurrencyCostRec>, 0x00497B00<ItemPetFoodRec>, 0x00497B00<ItemVisualEffectsRec>, 0x00497B00<LanguagesRec>, 0x00497B00<MovieFileDataRec>, 0x00497B00<NumTalentsAtLevelRec>, 0x00497B00<ObjectEffectGroupRec>, 0x00497B00<ObjectEffectPackageRec>, 0x00497B00<PageTextMaterialRec>, 0x00497B00<QuestInfoRec>, 0x00497B00<QuestSortRec>, 0x00497B00<ScreenLocationRec>, 0x00497B00<ServerMessagesRec>, 0x00497B00<SkillLineAbilitySortedSpellRec>, 0x00497B00<SoundFilterRec>, 0x00497B00<SpamMessagesRec>, 0x00497B00<SpellCategoryRec>, 0x00497B00<SpellDescriptionVariablesRec>, 0x00497B00<SpellFocusObjectRec>, 0x00497B00<SpellIconRec>, 0x00497B00<SpellMechanicRec>, 0x00497B00<StringLookupsRec>, 0x00497B00<VehicleUIIndicatorRec>, 0x00497B00<gtBarberShopCostBaseRec>, 0x00497B00<gtChanceToMeleeCritBaseRec>, 0x00497B00<gtChanceToMeleeCritRec>, 0x00497B00<gtChanceToSpellCritBaseRec>, 0x00497B00<gtChanceToSpellCritRec>, 0x00497B00<gtCombatRatingsRec>, 0x00497B00<gtNPCManaCostScalerRec>, 0x00497B00<gtOCTClassCombatRatingScalarRec>, 0x00497B00<gtOCTHpPerStaminaRec>, 0x00497B00<gtOCTRegenMPRec>, 0x00497B00<gtRegenMPPerSptRec>, 0x00497B00<gtSpellScalingRec>, 0x00497D60<BannedAddOnsRec>, 0x00497D60<CurrencyTypesRec>, 0x00497D60<GMSurveySurveysRec>, 0x00497D60<GroundEffectTextureRec>, 0x00497D60<QuestFactionRewardRec>, 0x00497D60<QuestXPRec>, 0x00497D60<TalentTabRec>, 0x00497D60<TaxiPathNodeRec>, 0x00497D60<TransportPhysicsRec>, 0x00497D60<WorldMapTransformsRec>, 0x00497B40<HolidaysRec>, %0x0049EAD0<CharacterFacialHairStylesRec>, %0x0049EAD0<CreatureSpellDataRec>, %0x0049EAD0<ItemArmorQualityRec>, %0x0049EAD0<ItemArmorShieldRec>, %0x0049EAD0<ItemDamageAmmoRec>, %0x0049EAD0<ItemDamageOneHandCasterRec>, %0x0049EAD0<ItemDamageOneHandRec>, %0x0049EAD0<ItemDamageRangedRec>, %0x0049EAD0<ItemDamageThrownRec>, %0x0049EAD0<ItemDamageTwoHandCasterRec>, %0x0049EAD0<ItemDamageTwoHandRec>, %0x0049EAD0<ItemDamageWandRec>, %0x0049EAD0<ResearchProjectRec>, %0x0049EAD0<SkillRaceClassInfoRec>, %0x0049EAD0<SpellAuraRestrictionsRec>, %0x0049EAD0<WorldStateZoneSoundsRec>, %0x0049EB20<ItemExtendedCostRec>, 0x00497C90<ItemSetRec>, 0x00498070<EmotesTextRec>, 0x00498070<LoadingScreenTaxiSplinesRec>, 0x00498070<TalentRec>, %0x0049EBE0<AnimationDataRec>, %0x0049EBE0<AreaGroupRec>, %0x0049EBE0<BarberShopStyleRec>, %0x0049EBE0<CameraShakesRec>, %0x0049EBE0<DanceMovesRec>, %0x0049EBE0<DungeonEncounterRec>, %0x0049EBE0<DungeonMapRec>, %0x0049EBE0<GameObjectArtKitRec>, %0x0049EBE0<GlueScreenEmoteRec>, %0x0049EBE0<HelmetGeosetVisDataRec>, %0x0049EBE0<ItemRandomPropertiesRec>, %0x0049EBE0<LfgDungeonExpansionRec>, %0x0049EBE0<MountCapabilityRec>, %0x0049EBE0<ObjectEffectModifierRec>, %0x0049EBE0<PetPersonalityRec>, %0x0049EBE0<SkillLineRec>, %0x0049EBE0<SpellRangeRec>, %0x0049EBE0<TaxiNodesRec>, %0x0049EBE0<WeatherRec>, %0x0049EBE0<ZoneMusicRec>, 0x00497CD0<AreaTriggerRec>, 0x00497CD0<ItemRandomSuffixRec>, 0x00497CD0<ItemSubClassRec>, 0x00497CD0<OverrideSpellDataRec>, 0x00497CD0<SoundFilterElemRec>, 0x004981E0<CharSectionsRec>, 0x004981E0<CinematicSequencesRec>, 0x004981E0<ItemPurchaseGroupRec>, 0x004981E0<LightParamsRec>, 0x004981E0<ParticleColorRec>, 0x004981E0<ScreenEffectRec>, 0x004981E0<SoundEmittersRec>, 0x004981E0<SpellActivationOverlayRec>, 0x004981E0<UnitBloodRec>, 0x004981E0<WorldChunkSoundsRec>, 0x00497E30<AchievementRec>, 0x00497E30<ChrClassesRec>, 0x00497E30<FactionTemplateRec>, 0x00497E30<PhaseShiftZoneSoundsRec>, 0x00497E30<SkillLineAbilityRec>, 0x00497E30<SpellVisualKitModelAttachRec>, 0x00497E30<WorldMapContinentRec>, 0x00497F70<AnimKitSegmentRec>, 0x00497F70<RandPropPointsRec>, 0x00497F70<SpellScalingRec>, 0x00497E70<ScalingStatValuesRec>, 0x00497E70<SpellRec>, 0x00498030<LockRec>, 0x00498030<SkillTiersRec>, 0x0048A9D0<FactionRec>, 0x0048A9D0<SpellEffectRec>, 0x00497C40<Achievement_CategoryRec>, 0x00497C40<AnimKitConfigBoneSetRec>, 0x00497C40<AnimReplacementRec>, 0x00497C40<FactionGroupRec>, 0x00497C40<GMSurveyAnswersRec>, 0x00497C40<GameTablesRec>, 0x00497C40<GameTipsRec>, 0x00497C40<GlyphPropertiesRec>, 0x00497C40<ItemLimitCategoryRec>, 0x00497C40<ItemSubClassMaskRec>, 0x00497C40<LoadingScreensRec>, 0x00497C40<MovieRec>, 0x00497C40<NameGenRec>, 0x00497C40<ObjectEffectPackageElemRec>, 0x00497C40<PackageRec>, 0x00497C40<PaperDollItemFrameRec>, 0x00497C40<PowerDisplayRec>, 0x00497C40<QuestPOIBlobRec>, 0x00497C40<QuestPOIPointRec>, 0x00497C40<ResistancesRec>, 0x00497C40<SpellCastTimesRec>, 0x00497C40<SpellCooldownsRec>, 0x00497C40<SpellDurationRec>, 0x00497C40<SpellEffectCameraShakesRec>, 0x00497C40<SpellEquippedItemsRec>, 0x00497C40<SpellFlyoutItemRec>, 0x00497C40<SpellLevelsRec>, 0x00497C40<SpellRadiusRec>, 0x00497C40<StationeryRec>, 0x00497C40<TalentTreePrimarySpellsRec>, 0x00497C40<TaxiPathRec>, 0x00497C40<TerrainMaterialRec>, 0x00497C40<TotemCategoryRec>, 0x00497C40<UnitBloodLevelsRec>, 0x00497C40<WeaponSwingSounds2Rec>, 0x00497C40<ZoneLightRec>, 0x00498120<Achievement_CriteriaRec>, 0x00498120<ScalingStatDistributionRec>, 0x00498120<SpellItemEnchantmentRec>, 0x00498120<WeaponImpactSoundsRec>, 0x00497EF0<SpellItemEnchantmentConditionRec>, 0x00497FF0<SpellVisualKitRec>, %0x0049EEF0<SpellVisualRec>, 0x004980B0<TerrainTypeSoundsRec>, 0x004980E0<VehicleSeatRec>, 0x00497DA0<AreaAssignmentRec>, 0x00497DA0<AuctionHouseRec>, 0x00497DA0<Cfg_CategoriesRec>, 0x00497DA0<Cfg_ConfigsRec>, 0x00497DA0<ChatChannelsRec>, 0x00497DA0<DeathThudLookupsRec>, 0x00497DA0<DungeonMapChunkRec>, 0x00497DA0<EmotesTextSoundRec>, 0x00497DA0<FootstepTerrainLookupRec>, 0x00497DA0<ItemClassRec>, 0x00497DA0<ItemGroupSoundsRec>, 0x00497DA0<ItemReforgeRec>, 0x00497DA0<LfgDungeonGroupRec>, 0x00497DA0<LockTypeRec>, 0x00497DA0<MaterialRec>, 0x00497DA0<NPCSoundsRec>, 0x00497DA0<ResearchSiteRec>, 0x00497DA0<SpellAuraOptionsRec>, 0x00497DA0<SpellDifficultyRec>, 0x00497DA0<SpellDispelTypeRec>, 0x00497DA0<SpellMissileMotionRec>, 0x00497DA0<SpellRuneCostRec>, 0x00497DA0<SpellTargetRestrictionsRec>, 0x00497DA0<SpellTotemsRec>, 0x00497DA0<VehicleUIIndSeatRec>, 0x00497DA0<ZoneIntroMusicTableRec>, 0x00497DA0<ZoneLightPointRec>, 0x00498220<CreatureFamilyRec>, 0x00498220<ObjectEffectRec>, 0x00498220<WorldMapAreaRec>, 0x00498160/0x1400ADAC0<LightRec>, 0x00498160/0x1400ADAC0<SpellMissileRec>, 0x00498160/0x1400ADAC0<WMOAreaTableRec>, 0x00498160/0x1400ADAC0<WorldMapOverlayRec>]
template <class T>
int32_t WowClientDB<T>::GetRecordByIndex(int32_t index, void* rec) const {
    const T* src = this->GetRecordByIndex(index);

    if (!src) {
        return 0;
    }

    *static_cast<T*>(rec) = *src;

    return 1;
}

// OFFSET: [%0x00564E40]
template <class T>
T* WowClientDB<T>::GetRecord(int32_t id) const {
    if (id < this->m_minID || id > this->m_maxID) {
        return NULL;
    }

    return reinterpret_cast<T*>(this->m_recordsById[id - this->m_minID]);
}

// OFFSET: [%0x0046FFB0, %0x0046FFD0, %0x0049E630]
template <class T>
T* WowClientDB<T>::GetRecordByIndex(int32_t index) const {
    if (index < 0 || index >= this->m_numRecords) {
        return NULL;
    }

    return reinterpret_cast<T*>(&this->m_records[sizeof(T) * index]);
}

// PROVEN: 9e93d92a IDENTICAL/NONE
// OFFSET: [0x00497C00<ItemDisplayInfoRec>]
template <class T>
int32_t WowClientDB_RLE<T>::GetRecordByIndex(int32_t index, void* rec) const {
    uint8_t buffer[sizeof(T)];

    if (index < 0 || index >= this->m_numRecords) {
        return 0;
    }

    this->CopyRecord(rec, this->m_records[index], buffer);

    return 1;
}

// INLINED
template <class T>
int32_t WowClientDB_RLE<T>::GetRecordByIndex(int32_t index, T* rec) const {
    uint8_t buffer[sizeof(T)];

    if (index < 0 || index >= this->m_numRecords) {
        return 0;
    }

    this->CopyRecord(rec, this->m_records[index], buffer);

    return 1;
}

// INLINED
template <class T>
int32_t WowClientDB_RLE<T>::GetRecord(int32_t id, T* rec) const {
    uint8_t buffer[sizeof(T)];

    if (id < this->m_minID || id > this->m_maxID) {
        return 0;
    }

    const uint8_t* row = this->m_recordsById[id - this->m_minID];

    if (!row) {
        return 0;
    }

    this->CopyRecord(rec, row, buffer);

    return 1;
}
