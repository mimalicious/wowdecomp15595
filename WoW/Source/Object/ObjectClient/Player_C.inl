// OFFSET: 0x00554B80
inline PlaySpellVisuaKitData::PlaySpellVisuaKitData(const SpellRec* spellRec, const SpellVisualKitRec* impactVisualKit, uint32_t kitType) {
    this->m_spellRec = spellRec;
    this->m_impactVisualKit = impactVisualKit;
    this->m_kitType = kitType;
    this->m_position = NULL;
    this->m_orientation = NULL;
    this->m_transportGUID = WOWGUID::Make(0);
    this->m_animParam = 1;
    this->unk_0024 = 0;
    this->unk_0028 = 0;
    this->unk_002C = -1;
    this->m_someGUID = WOWGUID::Make(0);
    this->unk_0038 = 0;
    this->m_displayId = 0;
    this->m_inventoryType = 0;
    this->m_timeOffset = 0;
    this->unk_0048 = 0;
    this->m_flags = 0;
    this->m_stopCallback = NULL;
    this->m_stopParam = 0;
}
