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
    this->m_offset = value && *value ? static_cast<int32_t>(value - reinterpret_cast<const char*>(this)) : 0;
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

// OFFSET: [0x0048AA50<LiquidTypeRec>, 0x0048AA50<SpellChainEffectsRec>, 0x0048AA90/0x1400AD910<SoundEntriesRec>, 0x004978C0/0x1400C0AC0<AreaPOIRec>, 0x00497DF0/0x1400BDFC0<AreaTableRec>, 0x00497DF0/0x1400BDFC0<MountTypeRec>, 0x00497DF0/0x1400BDFC0<UnitPowerBarRec>, 0x00497D10/0x1400AD190<AnimKitBoneSetRec>, 0x00497D10/0x1400AD190<ArmorLocationRec>, 0x00497D10/0x1400AD190<CharHairGeosetsRec>, 0x00497D10/0x1400AD190<CharTitlesRec>, 0x00497D10/0x1400AD190<GemPropertiesRec>, 0x00497D10/0x1400AD190<ItemArmorTotalRec>, 0x00497D10/0x1400AD190<ItemVisualsRec>, 0x00497D10/0x1400AD190<LiquidObjectRec>, 0x00497D10/0x1400AD190<PvpDifficultyRec>, 0x00497D10/0x1400AD190<ResearchBranchRec>, 0x00497D10/0x1400AD190<SpellInterruptsRec>, 0x00497D10/0x1400AD190<SpellShapeshiftRec>, 0x00497D10/0x1400AD190<SummonPropertiesRec>, 0x00497D10/0x1400AD190<WorldSafeLocsRec>, 0x00497900<BattlemasterListRec>, 0x00497900<MapRec>, 0x00497940/0x1400BE290<CameraModeRec>, 0x00497940/0x1400BE290<CreatureDisplayInfoRec>, 0x00497940/0x1400BE290<LfgDungeonsRec>, 0x00497940/0x1400BE290<SpellReagentsRec>, 0x00497940/0x1400BE290<WorldStateUIRec>, 0x00497980/0x1400BDD30<CharStartOutfitRec>, 0x00497A40/0x1400BDDB0<ChrRacesRec>, 0x00497A40/0x1400BDDB0<DestructibleModelDataRec>, 0x00497A40/0x1400BDDB0<SoundEntriesAdvancedRec>, 0x00497A40/0x1400BDDB0<SoundProviderPreferencesRec>, 0x00497BC0/0x1400BE140<CinematicCameraRec>, 0x00497BC0/0x1400BE140<EmotesRec>, 0x00497BC0/0x1400BE140<ExhaustionRec>, 0x00497BC0/0x1400BE140<ItemDisenchantLootRec>, 0x00497BC0/0x1400BE140<MapDifficultyRec>, 0x00497BC0/0x1400BE140<SpellCastingRequirementsRec>, 0x00497BC0/0x1400BE140<SpellCategoriesRec>, 0x00497BC0/0x1400BE140<SpellClassOptionsRec>, 0x00497BC0/0x1400BE140<SpellFlyoutRec>, 0x00497BC0/0x1400BE140<SpellPowerRec>, 0x00497BC0/0x1400BE140<SpellVisualEffectNameRec>, 0x00497BC0/0x1400BE140<TerrainTypeRec>, 0x00497BC0/0x1400BE140<TransportAnimationRec>, 0x00497BC0/0x1400BE140<TransportRotationRec>, 0x00497BC0/0x1400BE140<VocalUISoundsRec>, 0x00497BC0/0x1400BE140<World_PVP_AreaRec>, 0x00497FB0<CreatureDisplayInfoExtraRec>, 0x00497FB0<GameObjectDisplayInfoRec>, 0x00497FB0<SpellShapeshiftFormRec>, 0x00497A00/0x1400BDD70<CreatureSoundDataRec>, 0x00497A00/0x1400BDD70<VehicleRec>, 0x00497A80/0x1400BDDF0<CreatureModelDataRec>, 0x00497A80/0x1400BDDF0<DurabilityCostsRec>, 0x004981A0/0x1400BE000<AnimKitBoneSetAliasRec>, 0x004981A0/0x1400BE000<AnimKitRec>, 0x004981A0/0x1400BE000<AreaPOISortedWorldStateRec>, 0x004981A0/0x1400BE000<CastableRaidBuffsRec>, 0x004981A0/0x1400BE000<ChatProfanityRec>, 0x004981A0/0x1400BE000<CreatureTypeRec>, 0x004981A0/0x1400BE000<CurrencyCategoryRec>, 0x004981A0/0x1400BE000<EnvironmentalDamageRec>, 0x004981A0/0x1400BE000<FileDataRec>, 0x004981A0/0x1400BE000<GlyphSlotRec>, 0x004981A0/0x1400BE000<GroundEffectDoodadRec>, 0x004981A0/0x1400BE000<GuildPerkSpellsRec>, 0x004981A0/0x1400BE000<LanguageWordsRec>, 0x004981A0/0x1400BE000<LightSkyboxRec>, 0x004981A0/0x1400BE000<LiquidMaterialRec>, 0x004981A0/0x1400BE000<MailTemplateRec>, 0x004981A0/0x1400BE000<MovieVariationRec>, 0x004981A0/0x1400BE000<NamesProfanityRec>, 0x004981A0/0x1400BE000<NamesReservedRec>, 0x004981A0/0x1400BE000<PhaseRec>, 0x004981A0/0x1400BE000<PhaseXPhaseGroupRec>, 0x004981A0/0x1400BE000<ResearchFieldRec>, 0x004981A0/0x1400BE000<SkillLineCategoryRec>, 0x004981A0/0x1400BE000<SoundAmbienceRec>, 0x004981A0/0x1400BE000<SoundEntriesFallbacksRec>, 0x004981A0/0x1400BE000<SpellVisualKitAreaModelRec>, 0x00497B00/0x1400BDE30<AnimKitConfigRec>, 0x00497B00/0x1400BDE30<AnimKitPriorityRec>, 0x00497B00/0x1400BDE30<AnimReplacementSetRec>, 0x00497B00/0x1400BDE30<BankBagSlotPricesRec>, 0x00497B00/0x1400BDE30<CharBaseInfoRec>, 0x00497B00/0x1400BDE30<CreatureMovementInfoRec>, 0x00497B00/0x1400BDE30<DurabilityQualityRec>, 0x00497B00/0x1400BDE30<EmotesTextDataRec>, 0x00497B00/0x1400BDE30<FootprintTexturesRec>, 0x00497B00/0x1400BDE30<GMSurveyCurrentSurveyRec>, 0x00497B00/0x1400BDE30<GMSurveyQuestionsRec>, 0x00497B00/0x1400BDE30<GMTicketCategoryRec>, 0x00497B00/0x1400BDE30<GuildColorBackgroundRec>, 0x00497B00/0x1400BDE30<GuildColorBorderRec>, 0x00497B00/0x1400BDE30<GuildColorEmblemRec>, 0x00497B00/0x1400BDE30<HolidayDescriptionsRec>, 0x00497B00/0x1400BDE30<HolidayNamesRec>, 0x00497B00/0x1400BDE30<ItemBagFamilyRec>, 0x00497B00/0x1400BDE30<ItemCurrencyCostRec>, 0x00497B00/0x1400BDE30<ItemPetFoodRec>, 0x00497B00/0x1400BDE30<ItemVisualEffectsRec>, 0x00497B00/0x1400BDE30<LanguagesRec>, 0x00497B00/0x1400BDE30<MovieFileDataRec>, 0x00497B00/0x1400BDE30<NumTalentsAtLevelRec>, 0x00497B00/0x1400BDE30<ObjectEffectGroupRec>, 0x00497B00/0x1400BDE30<ObjectEffectPackageRec>, 0x00497B00/0x1400BDE30<PageTextMaterialRec>, 0x00497B00/0x1400BDE30<QuestInfoRec>, 0x00497B00/0x1400BDE30<QuestSortRec>, 0x00497B00/0x1400BDE30<ScreenLocationRec>, 0x00497B00/0x1400BDE30<ServerMessagesRec>, 0x00497B00/0x1400BDE30<SkillLineAbilitySortedSpellRec>, 0x00497B00/0x1400BDE30<SoundFilterRec>, 0x00497B00/0x1400BDE30<SpamMessagesRec>, 0x00497B00/0x1400BDE30<SpellCategoryRec>, 0x00497B00/0x1400BDE30<SpellDescriptionVariablesRec>, 0x00497B00/0x1400BDE30<SpellFocusObjectRec>, 0x00497B00/0x1400BDE30<SpellIconRec>, 0x00497B00/0x1400BDE30<SpellMechanicRec>, 0x00497B00/0x1400BDE30<StringLookupsRec>, 0x00497B00/0x1400BDE30<VehicleUIIndicatorRec>, 0x00497B00/0x1400BDE30<gtBarberShopCostBaseRec>, 0x00497B00/0x1400BDE30<gtChanceToMeleeCritBaseRec>, 0x00497B00/0x1400BDE30<gtChanceToMeleeCritRec>, 0x00497B00/0x1400BDE30<gtChanceToSpellCritBaseRec>, 0x00497B00/0x1400BDE30<gtChanceToSpellCritRec>, 0x00497B00/0x1400BDE30<gtCombatRatingsRec>, 0x00497B00/0x1400BDE30<gtNPCManaCostScalerRec>, 0x00497B00/0x1400BDE30<gtOCTClassCombatRatingScalarRec>, 0x00497B00/0x1400BDE30<gtOCTHpPerStaminaRec>, 0x00497B00/0x1400BDE30<gtOCTRegenMPRec>, 0x00497B00/0x1400BDE30<gtRegenMPPerSptRec>, 0x00497B00/0x1400BDE30<gtSpellScalingRec>, 0x00497D60/0x1400BE440<BannedAddOnsRec>, 0x00497D60/0x1400BE440<CurrencyTypesRec>, 0x00497D60/0x1400BE440<GMSurveySurveysRec>, 0x00497D60/0x1400BE440<GroundEffectTextureRec>, 0x00497D60/0x1400BE440<QuestFactionRewardRec>, 0x00497D60/0x1400BE440<QuestXPRec>, 0x00497D60/0x1400BE440<TalentTabRec>, 0x00497D60/0x1400BE440<TaxiPathNodeRec>, 0x00497D60/0x1400BE440<TransportPhysicsRec>, 0x00497D60/0x1400BE440<WorldMapTransformsRec>, 0x00497B40/0x1400C1300<HolidaysRec>, 0x00497C90/0x1400C1450<ItemSetRec>, 0x00498070/0x1400BE350<EmotesTextRec>, 0x00498070/0x1400BE350<LoadingScreenTaxiSplinesRec>, 0x00498070/0x1400BE350<TalentRec>, 0x00497AC0/0x1400BDF60<AreaGroupRec>, 0x00497B80/0x1400BDE60<CameraShakesRec>, 0x00497CD0/0x1400BDCF0<AreaTriggerRec>, 0x00497CD0/0x1400BDCF0<ItemRandomSuffixRec>, 0x00497CD0/0x1400BDCF0<ItemSubClassRec>, 0x00497CD0/0x1400BDCF0<OverrideSpellDataRec>, 0x00497CD0/0x1400BDCF0<SoundFilterElemRec>, 0x004981E0/0x1400BE040<CharSectionsRec>, 0x004981E0/0x1400BE040<CinematicSequencesRec>, 0x004981E0/0x1400BE040<ItemPurchaseGroupRec>, 0x004981E0/0x1400BE040<LightParamsRec>, 0x004981E0/0x1400BE040<ParticleColorRec>, 0x004981E0/0x1400BE040<ScreenEffectRec>, 0x004981E0/0x1400BE040<SoundEmittersRec>, 0x004981E0/0x1400BE040<SpellActivationOverlayRec>, 0x004981E0/0x1400BE040<UnitBloodRec>, 0x004981E0/0x1400BE040<WorldChunkSoundsRec>, 0x00497E30/0x1400BE080<AchievementRec>, 0x00497E30/0x1400BE080<ChrClassesRec>, 0x00497E30/0x1400BE080<FactionTemplateRec>, 0x00497E30/0x1400BE080<PhaseShiftZoneSoundsRec>, 0x00497E30/0x1400BE080<SkillLineAbilityRec>, 0x00497E30/0x1400BE080<SpellVisualKitModelAttachRec>, 0x00497E30/0x1400BE080<WorldMapContinentRec>, 0x00497F70/0x1400BE2D0<AnimKitSegmentRec>, 0x00497F70/0x1400BE2D0<RandPropPointsRec>, 0x00497F70/0x1400BE2D0<SpellScalingRec>, 0x00497E70/0x1400BE100<ScalingStatValuesRec>, 0x00497E70/0x1400BE100<SpellRec>, 0x00498030/0x1400BDF20<LockRec>, 0x00498030/0x1400BDF20<SkillTiersRec>, 0x0048A9D0/0x1400BE190<FactionRec>, 0x0048A9D0/0x1400BE190<SpellEffectRec>, 0x00497C40/0x1400BE1D0<Achievement_CategoryRec>, 0x00497C40/0x1400BE1D0<AnimKitConfigBoneSetRec>, 0x00497C40/0x1400BE1D0<AnimReplacementRec>, 0x00497C40/0x1400BE1D0<FactionGroupRec>, 0x00497C40/0x1400BE1D0<GMSurveyAnswersRec>, 0x00497C40/0x1400BE1D0<GameTablesRec>, 0x00497C40/0x1400BE1D0<GameTipsRec>, 0x00497C40/0x1400BE1D0<GlyphPropertiesRec>, 0x00497C40/0x1400BE1D0<ItemLimitCategoryRec>, 0x00497C40/0x1400BE1D0<ItemSubClassMaskRec>, 0x00497C40/0x1400BE1D0<LoadingScreensRec>, 0x00497C40/0x1400BE1D0<MovieRec>, 0x00497C40/0x1400BE1D0<NameGenRec>, 0x00497C40/0x1400BE1D0<ObjectEffectPackageElemRec>, 0x00497C40/0x1400BE1D0<PackageRec>, 0x00497C40/0x1400BE1D0<PaperDollItemFrameRec>, 0x00497C40/0x1400BE1D0<PowerDisplayRec>, 0x00497C40/0x1400BE1D0<QuestPOIBlobRec>, 0x00497C40/0x1400BE1D0<QuestPOIPointRec>, 0x00497C40/0x1400BE1D0<ResistancesRec>, 0x00497C40/0x1400BE1D0<SpellCastTimesRec>, 0x00497C40/0x1400BE1D0<SpellCooldownsRec>, 0x00497C40/0x1400BE1D0<SpellDurationRec>, 0x00497C40/0x1400BE1D0<SpellEffectCameraShakesRec>, 0x00497C40/0x1400BE1D0<SpellEquippedItemsRec>, 0x00497C40/0x1400BE1D0<SpellFlyoutItemRec>, 0x00497C40/0x1400BE1D0<SpellLevelsRec>, 0x00497C40/0x1400BE1D0<SpellRadiusRec>, 0x00497C40/0x1400BE1D0<StationeryRec>, 0x00497C40/0x1400BE1D0<TalentTreePrimarySpellsRec>, 0x00497C40/0x1400BE1D0<TaxiPathRec>, 0x00497C40/0x1400BE1D0<TerrainMaterialRec>, 0x00497C40/0x1400BE1D0<TotemCategoryRec>, 0x00497C40/0x1400BE1D0<UnitBloodLevelsRec>, 0x00497C40/0x1400BE1D0<WeaponSwingSounds2Rec>, 0x00497C40/0x1400BE1D0<ZoneLightRec>, 0x00498120/0x1400BE400<Achievement_CriteriaRec>, 0x00498120/0x1400BE400<ScalingStatDistributionRec>, 0x00498120/0x1400BE400<SpellItemEnchantmentRec>, 0x00498120/0x1400BE400<WeaponImpactSoundsRec>, 0x00497EF0/0x1400BE210<SpellItemEnchantmentConditionRec>, 0x00497FF0/0x1400BE310<SpellVisualKitRec>, 0x004980B0/0x1400BE390<TerrainTypeSoundsRec>, 0x004980E0/0x1400BE3C0<VehicleSeatRec>, 0x00497DA0/0x1400BDCA0<AreaAssignmentRec>, 0x00497DA0/0x1400BDCA0<AuctionHouseRec>, 0x00497DA0/0x1400BDCA0<Cfg_CategoriesRec>, 0x00497DA0/0x1400BDCA0<Cfg_ConfigsRec>, 0x00497DA0/0x1400BDCA0<ChatChannelsRec>, 0x00497DA0/0x1400BDCA0<DeathThudLookupsRec>, 0x00497DA0/0x1400BDCA0<DungeonMapChunkRec>, 0x00497DA0/0x1400BDCA0<EmotesTextSoundRec>, 0x00497DA0/0x1400BDCA0<FootstepTerrainLookupRec>, 0x00497DA0/0x1400BDCA0<ItemClassRec>, 0x00497DA0/0x1400BDCA0<ItemGroupSoundsRec>, 0x00497DA0/0x1400BDCA0<ItemReforgeRec>, 0x00497DA0/0x1400BDCA0<LfgDungeonGroupRec>, 0x00497DA0/0x1400BDCA0<LockTypeRec>, 0x00497DA0/0x1400BDCA0<MaterialRec>, 0x00497DA0/0x1400BDCA0<NPCSoundsRec>, 0x00497DA0/0x1400BDCA0<ResearchSiteRec>, 0x00497DA0/0x1400BDCA0<SpellAuraOptionsRec>, 0x00497DA0/0x1400BDCA0<SpellDifficultyRec>, 0x00497DA0/0x1400BDCA0<SpellDispelTypeRec>, 0x00497DA0/0x1400BDCA0<SpellMissileMotionRec>, 0x00497DA0/0x1400BDCA0<SpellRuneCostRec>, 0x00497DA0/0x1400BDCA0<SpellTargetRestrictionsRec>, 0x00497DA0/0x1400BDCA0<SpellTotemsRec>, 0x00497DA0/0x1400BDCA0<VehicleUIIndSeatRec>, 0x00497DA0/0x1400BDCA0<ZoneIntroMusicTableRec>, 0x00497DA0/0x1400BDCA0<ZoneLightPointRec>, 0x00498220<CreatureFamilyRec>, 0x00498220<ObjectEffectRec>, 0x00498220<WorldMapAreaRec>, 0x00498160/0x1400ADAC0<LightRec>, 0x00498160/0x1400ADAC0<SpellMissileRec>, 0x00498160/0x1400ADAC0<WMOAreaTableRec>, 0x00498160/0x1400ADAC0<WorldMapOverlayRec>, 0x0048AA10/0x1400AD150<LightFloatBandRec>, 0x004979C0<ItemExtendedCostRec>, 0x00497EB0<?>, 0x00497F30<?>]
template <class T>
int32_t WowClientDB<T>::GetRecordByIndex(int32_t index, void* rec) const {
    const T* src = this->GetRecordByIndex(index);

    if (!src) {
        return 0;
    }

    *static_cast<T*>(rec) = *src;

    return 1;
}

// OFFSET: [0x00A06F50/0x14090B320, 0x00A06F50/0x14090B320<CreatureModelDataRec>]
template <class T>
T* WowClientDB<T>::GetRecord(int32_t id) const {
    if (id < this->m_minID || id > this->m_maxID) {
        return NULL;
    }

    return reinterpret_cast<T*>(this->m_recordsById[id - this->m_minID]);
}

// OFFSET: [0x00465E00<ScalingStatValuesRec>, 0x004977F0<BattlemasterListRec>, 0x00497840<GameTipsRec>, 0x00466030<Achievement_CriteriaRec>, 0x0048A9B0<AreaTableRec>, 0x00497860<CinematicCameraRec>, 0x00497890<AchievementRec>, 0x00465DC0<AreaTriggerRec>, 0x00465DE0<CreatureDisplayInfoExtraRec>, 0x00497820<AnimKitConfigRec>]
template <class T>
T* WowClientDB<T>::GetRecordByIndex(int32_t index) const {
    if (index < 0 || index >= this->m_numRecords) {
        return NULL;
    }

    return reinterpret_cast<T*>(&this->m_records[sizeof(T) * index]);
}

// OFFSET: [0x00497C00/0x1400BDED0<ItemDisplayInfoRec>]
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

// OFFSET: [0x00554B30<ItemDisplayInfoRec>]
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
