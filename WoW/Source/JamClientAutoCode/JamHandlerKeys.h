#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMHANDLERKEYS_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMHANDLERKEYS_H

#include <cstddef>
#include <cstdint>

class JamHandlerKey {
    public:
    virtual uint32_t Get() = 0;
};

class JamHandlerKeyOp : public JamHandlerKey {
    public:
    /* 0004 */ JamHandlerKey* m_left;
    /* 0008 */ JamHandlerKey* m_right;

    JamHandlerKeyOp(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeyMulXor : public JamHandlerKeyOp {
    public:
    uint32_t Get() override;

    JamHandlerKeyMulXor(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeySub : public JamHandlerKeyOp {
    public:
    uint32_t Get() override;

    JamHandlerKeySub(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeyRol : public JamHandlerKeyOp {
    public:
    uint32_t Get() override;

    JamHandlerKeyRol(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeyAdd : public JamHandlerKeyOp {
    public:
    uint32_t Get() override;

    JamHandlerKeyAdd(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeyXor : public JamHandlerKeyOp {
    public:
    uint32_t Get() override;

    JamHandlerKeyXor(JamHandlerKey* left, JamHandlerKey* right);
};

class JamHandlerKeyConst_66A430 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A440 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A450 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A460 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A470 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A480 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A490 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A4A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A4B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A4F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A500 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A510 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A5B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A5C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A5D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A5F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A600 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A620 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A630 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A670 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A680 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A690 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66A750 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCC0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BCF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD10 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD50 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD70 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BD80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BDA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BDB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BDC0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BDD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BDF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE10 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE70 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BE80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BEA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BEB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BED0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BEE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BEF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF50 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF70 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BF90 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BFB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BFD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BFE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66BFF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C000 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C010 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C020 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C030 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C060 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C0A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C0B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C0D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C100 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C150 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C160 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C180 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C1C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C1D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C1E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C1F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C200 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C210 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C220 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C230 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C240 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C250 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C280 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C2A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C2C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C2E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C300 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C320 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C350 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C370 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C380 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C3F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C470 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C490 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C4C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C4E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C530 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C560 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C5A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C5D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C630 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C670 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C6D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C6E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C700 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C710 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C740 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C750 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C780 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C790 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C7B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C7D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C7E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C7F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C800 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C810 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C830 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C890 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C8C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C8D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C940 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C950 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C9A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C9B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C9E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66C9F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CA20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CA40 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CA50 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CA90 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CAB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CAF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CB00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CB30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CB60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CB80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CBB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CC10 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CC40 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CC60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_66CCA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_670F40 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_670F50 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_670F90 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_670FC0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_670FF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671000 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671060 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6710D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671120 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671130 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671140 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671180 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671190 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6711F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671240 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671270 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6712A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671300 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671360 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6713A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6713B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6713D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671430 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671450 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671460 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6714A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6714C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671500 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671510 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671520 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671540 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671560 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671590 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6715C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6715D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6715E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6715F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671600 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671640 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671680 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6717E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671800 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671840 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671860 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671870 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671880 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6718C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6718E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6718F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671990 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6719A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6719E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671A00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671A10 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671A30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671AF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B70 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671B90 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671BB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671CE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671D30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671DB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671DE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671E00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671E20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671E60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671EB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671EE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671FE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_671FF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672040 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672050 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672100 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6721C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672200 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672240 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672290 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6722A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672360 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672390 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6723E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672470 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6724C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672530 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6725D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672640 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672670 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6727E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672850 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6728E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672940 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672960 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672970 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672A20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672A60 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672A80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672AC0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672AD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672B30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672B70 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672B80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672BA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672BB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672C20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672C30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672DD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672DE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672E00 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672E50 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672F10 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672F20 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672F80 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672FB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_672FD0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688190 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6881C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688210 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688240 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6882E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6883A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6883E0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6884C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688520 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688560 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688690 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6886B0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688740 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688790 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6887C0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688AB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688B40 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688BE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688CA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688E30 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688EA0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_688FB0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6890D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689320 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6894A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689500 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689730 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6897D0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_6898A0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689930 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689BF0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689CC0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689DE0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_689F40 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_68A0F0 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_68A340 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

class JamHandlerKeyConst_68A780 : public JamHandlerKey {
    public:
    uint32_t Get() override;
};

extern JamHandlerKeyMulXor g_jamHandlerKey_CFB670;
extern JamHandlerKeySub g_jamHandlerKey_CFB688;
extern JamHandlerKeyAdd g_jamHandlerKey_CFB6A4;
extern JamHandlerKeyRol g_jamHandlerKey_CFB708;
extern JamHandlerKeyAdd g_jamHandlerKey_CFB718;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFB738;
extern JamHandlerKeyRol g_jamHandlerKey_CFB74C;
extern JamHandlerKeyRol g_jamHandlerKey_CFB780;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFCDC0;
extern JamHandlerKeySub g_jamHandlerKey_CFCDD8;
extern JamHandlerKeyRol g_jamHandlerKey_CFCDE4;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFCE1C;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFCEA8;
extern JamHandlerKeySub g_jamHandlerKey_CFCEF8;
extern JamHandlerKeyAdd g_jamHandlerKey_CFCF04;
extern JamHandlerKeyXor g_jamHandlerKey_CFCF48;
extern JamHandlerKeySub g_jamHandlerKey_CFCF68;
extern JamHandlerKeyXor g_jamHandlerKey_CFCF9C;
extern JamHandlerKeyAdd g_jamHandlerKey_CFCFE0;
extern JamHandlerKeySub g_jamHandlerKey_CFD00C;
extern JamHandlerKeyXor g_jamHandlerKey_CFD04C;
extern JamHandlerKeyXor g_jamHandlerKey_CFD060;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD074;
extern JamHandlerKeyXor g_jamHandlerKey_CFD08C;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFD0CC;
extern JamHandlerKeyRol g_jamHandlerKey_CFD0E8;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD138;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFD158;
extern JamHandlerKeySub g_jamHandlerKey_CFD1F0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD200;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFD224;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD23C;
extern JamHandlerKeySub g_jamHandlerKey_CFD284;
extern JamHandlerKeyRol g_jamHandlerKey_CFD298;
extern JamHandlerKeySub g_jamHandlerKey_CFD2B4;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD2C0;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFD2EC;
extern JamHandlerKeyXor g_jamHandlerKey_CFD2FC;
extern JamHandlerKeyXor g_jamHandlerKey_CFD320;
extern JamHandlerKeySub g_jamHandlerKey_CFD344;
extern JamHandlerKeyXor g_jamHandlerKey_CFD350;
extern JamHandlerKeySub g_jamHandlerKey_CFD374;
extern JamHandlerKeyRol g_jamHandlerKey_CFD398;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD3AC;
extern JamHandlerKeySub g_jamHandlerKey_CFD3BC;
extern JamHandlerKeyRol g_jamHandlerKey_CFD3D4;
extern JamHandlerKeySub g_jamHandlerKey_CFD3E0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD434;
extern JamHandlerKeyRol g_jamHandlerKey_CFD454;
extern JamHandlerKeySub g_jamHandlerKey_CFD4E4;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD500;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD540;
extern JamHandlerKeySub g_jamHandlerKey_CFD57C;
extern JamHandlerKeySub g_jamHandlerKey_CFD5B0;
extern JamHandlerKeySub g_jamHandlerKey_CFD5C4;
extern JamHandlerKeySub g_jamHandlerKey_CFD5F8;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD614;
extern JamHandlerKeyRol g_jamHandlerKey_CFD658;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD66C;
extern JamHandlerKeyRol g_jamHandlerKey_CFD678;
extern JamHandlerKeySub g_jamHandlerKey_CFD694;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFD744;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD770;
extern JamHandlerKeyXor g_jamHandlerKey_CFD7F8;
extern JamHandlerKeyRol g_jamHandlerKey_CFD850;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD85C;
extern JamHandlerKeySub g_jamHandlerKey_CFD8BC;
extern JamHandlerKeySub g_jamHandlerKey_CFD8D4;
extern JamHandlerKeyXor g_jamHandlerKey_CFD8F4;
extern JamHandlerKeyAdd g_jamHandlerKey_CFD90C;
extern JamHandlerKeySub g_jamHandlerKey_CFD91C;
extern JamHandlerKeyRol g_jamHandlerKey_CFD934;
extern JamHandlerKeySub g_jamHandlerKey_CFD964;
extern JamHandlerKeyXor g_jamHandlerKey_CFD98C;
extern JamHandlerKeySub g_jamHandlerKey_CFD9B0;
extern JamHandlerKeySub g_jamHandlerKey_CFD9E0;
extern JamHandlerKeySub g_jamHandlerKey_CFD9EC;
extern JamHandlerKeySub g_jamHandlerKey_CFDA68;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDB04;
extern JamHandlerKeySub g_jamHandlerKey_CFDB10;
extern JamHandlerKeySub g_jamHandlerKey_CFDB38;
extern JamHandlerKeyRol g_jamHandlerKey_CFDB74;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDB84;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDBB4;
extern JamHandlerKeyXor g_jamHandlerKey_CFDBEC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDC44;
extern JamHandlerKeyXor g_jamHandlerKey_CFDC80;
extern JamHandlerKeyRol g_jamHandlerKey_CFDC94;
extern JamHandlerKeySub g_jamHandlerKey_CFDCEC;
extern JamHandlerKeySub g_jamHandlerKey_CFDD18;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDD28;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDD44;
extern JamHandlerKeyRol g_jamHandlerKey_CFDDA8;
extern JamHandlerKeySub g_jamHandlerKey_CFDDD4;
extern JamHandlerKeyRol g_jamHandlerKey_CFDE38;
extern JamHandlerKeySub g_jamHandlerKey_CFDE64;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDE78;
extern JamHandlerKeySub g_jamHandlerKey_CFDEA8;
extern JamHandlerKeySub g_jamHandlerKey_CFDEB4;
extern JamHandlerKeyRol g_jamHandlerKey_CFDEC4;
extern JamHandlerKeyXor g_jamHandlerKey_CFDED4;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDEF0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDF50;
extern JamHandlerKeyAdd g_jamHandlerKey_CFDFA0;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDFCC;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFDFDC;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE008;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE04C;
extern JamHandlerKeyXor g_jamHandlerKey_CFE07C;
extern JamHandlerKeyXor g_jamHandlerKey_CFE088;
extern JamHandlerKeySub g_jamHandlerKey_CFE0AC;
extern JamHandlerKeyRol g_jamHandlerKey_CFE0C0;
extern JamHandlerKeyRol g_jamHandlerKey_CFE0E8;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE110;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE11C;
extern JamHandlerKeyRol g_jamHandlerKey_CFE17C;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE198;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE1A4;
extern JamHandlerKeyXor g_jamHandlerKey_CFE1B8;
extern JamHandlerKeySub g_jamHandlerKey_CFE1D0;
extern JamHandlerKeySub g_jamHandlerKey_CFE1F8;
extern JamHandlerKeySub g_jamHandlerKey_CFE300;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE310;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE328;
extern JamHandlerKeyXor g_jamHandlerKey_CFE34C;
extern JamHandlerKeyRol g_jamHandlerKey_CFE358;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE3BC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE448;
extern JamHandlerKeySub g_jamHandlerKey_CFE4CC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE4E8;
extern JamHandlerKeySub g_jamHandlerKey_CFE4F8;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE514;
extern JamHandlerKeyRol g_jamHandlerKey_CFE57C;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE5D0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE5F0;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE5FC;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE624;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE634;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE648;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE694;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE6D0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE6DC;
extern JamHandlerKeySub g_jamHandlerKey_CFE6E8;
extern JamHandlerKeyXor g_jamHandlerKey_CFE708;
extern JamHandlerKeyXor g_jamHandlerKey_CFE748;
extern JamHandlerKeyRol g_jamHandlerKey_CFE754;
extern JamHandlerKeyRol g_jamHandlerKey_CFE774;
extern JamHandlerKeySub g_jamHandlerKey_CFE800;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE840;
extern JamHandlerKeyRol g_jamHandlerKey_CFE850;
extern JamHandlerKeySub g_jamHandlerKey_CFE8B0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE8F4;
extern JamHandlerKeySub g_jamHandlerKey_CFE94C;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE9B8;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFE9C4;
extern JamHandlerKeyAdd g_jamHandlerKey_CFE9DC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEA48;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFEA54;
extern JamHandlerKeyRol g_jamHandlerKey_CFEA68;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEAD0;
extern JamHandlerKeyRol g_jamHandlerKey_CFEB00;
extern JamHandlerKeySub g_jamHandlerKey_CFEB28;
extern JamHandlerKeyXor g_jamHandlerKey_CFEB88;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEBBC;
extern JamHandlerKeySub g_jamHandlerKey_CFEC10;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFEC58;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEC6C;
extern JamHandlerKeyRol g_jamHandlerKey_CFEC7C;
extern JamHandlerKeySub g_jamHandlerKey_CFEC9C;
extern JamHandlerKeyRol g_jamHandlerKey_CFECE8;
extern JamHandlerKeySub g_jamHandlerKey_CFED40;
extern JamHandlerKeyXor g_jamHandlerKey_CFED68;
extern JamHandlerKeySub g_jamHandlerKey_CFED90;
extern JamHandlerKeyRol g_jamHandlerKey_CFEDA0;
extern JamHandlerKeyRol g_jamHandlerKey_CFEDB4;
extern JamHandlerKeySub g_jamHandlerKey_CFEDD0;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFEF00;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFEF10;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEF80;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFEFA8;
extern JamHandlerKeyAdd g_jamHandlerKey_CFEFCC;
extern JamHandlerKeySub g_jamHandlerKey_CFEFF4;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFF000;
extern JamHandlerKeySub g_jamHandlerKey_CFF074;
extern JamHandlerKeyRol g_jamHandlerKey_CFF0F0;
extern JamHandlerKeyXor g_jamHandlerKey_CFF164;
extern JamHandlerKeyXor g_jamHandlerKey_CFF188;
extern JamHandlerKeyRol g_jamHandlerKey_CFF1A8;
extern JamHandlerKeyRol g_jamHandlerKey_CFF1D8;
extern JamHandlerKeyXor g_jamHandlerKey_CFF204;
extern JamHandlerKeySub g_jamHandlerKey_CFF248;
extern JamHandlerKeyXor g_jamHandlerKey_CFF274;
extern JamHandlerKeyXor g_jamHandlerKey_CFF2D0;
extern JamHandlerKeyRol g_jamHandlerKey_CFF304;
extern JamHandlerKeySub g_jamHandlerKey_CFF35C;
extern JamHandlerKeySub g_jamHandlerKey_CFF3B4;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF3C8;
extern JamHandlerKeyXor g_jamHandlerKey_CFF3E4;
extern JamHandlerKeyRol g_jamHandlerKey_CFF440;
extern JamHandlerKeyRol g_jamHandlerKey_CFF490;
extern JamHandlerKeySub g_jamHandlerKey_CFF49C;
extern JamHandlerKeyXor g_jamHandlerKey_CFF52C;
extern JamHandlerKeyRol g_jamHandlerKey_CFF538;
extern JamHandlerKeyRol g_jamHandlerKey_CFF554;
extern JamHandlerKeySub g_jamHandlerKey_CFF594;
extern JamHandlerKeyXor g_jamHandlerKey_CFF5A0;
extern JamHandlerKeySub g_jamHandlerKey_CFF5C0;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF628;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF638;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFF664;
extern JamHandlerKeyXor g_jamHandlerKey_CFF678;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFF694;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFF6A4;
extern JamHandlerKeyXor g_jamHandlerKey_CFF6D4;
extern JamHandlerKeyXor g_jamHandlerKey_CFF6FC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF718;
extern JamHandlerKeySub g_jamHandlerKey_CFF728;
extern JamHandlerKeyRol g_jamHandlerKey_CFF7BC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF820;
extern JamHandlerKeyRol g_jamHandlerKey_CFF894;
extern JamHandlerKeySub g_jamHandlerKey_CFF8F0;
extern JamHandlerKeyRol g_jamHandlerKey_CFF8FC;
extern JamHandlerKeyXor g_jamHandlerKey_CFF964;
extern JamHandlerKeyRol g_jamHandlerKey_CFF970;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFF9AC;
extern JamHandlerKeyAdd g_jamHandlerKey_CFF9B8;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFFA40;
extern JamHandlerKeyAdd g_jamHandlerKey_CFFA58;
extern JamHandlerKeyAdd g_jamHandlerKey_CFFAEC;
extern JamHandlerKeySub g_jamHandlerKey_CFFB4C;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFFB94;
extern JamHandlerKeySub g_jamHandlerKey_CFFBCC;
extern JamHandlerKeySub g_jamHandlerKey_CFFBFC;
extern JamHandlerKeySub g_jamHandlerKey_CFFC14;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFFC74;
extern JamHandlerKeyXor g_jamHandlerKey_CFFC8C;
extern JamHandlerKeyMulXor g_jamHandlerKey_CFFCA4;
extern JamHandlerKeyXor g_jamHandlerKey_CFFCD4;
extern JamHandlerKeySub g_jamHandlerKey_CFFD08;

#include "WoW/Source/JamClientAutoCode/JamHandlerKeys.inl"

#endif
