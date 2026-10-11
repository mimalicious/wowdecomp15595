// OFFSET: 0x0097EF80/0x1406F90A0
static int32_t GetLFGDungeonSubtype(const LfgDungeonsRec* rec) {
    if (rec->m_typeID != 1) {
        return 0;
    }

    if (rec->IsRaid()) {
        return 3;
    }

    return rec->IsHeroic() ? 2 : 1;
}
