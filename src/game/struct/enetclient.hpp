#pragma once
#include <cstdint>

#pragma pack(push, 1)
class ENetClient
{
  public:
    ENetClient();
    virtual ~ENetClient();

    uint8_t pad[184];
    void* m_pEnetHost;
    void* m_pEnetPeer;
    int m_unk;       // offset_0xc8
    int m_unk2;      //
    void* m_unk3;    // offset_0xd0
    void* m_unk4;    // offset_0xd8
    void* m_unk5;    //
    void* m_unk6;    // offset_0xe8
    void* m_unk7;    // offset_0xf0
    int m_unk8;      // offset_0xf8
    uint8_t m_unk9;  // offset_0xfc
    uint8_t m_unk10; // offset_0xfd
    short m_unk11;   //
    int m_unk12;     // offset_0x100
    int m_unk13;     // offset_0x104
    void* m_unk14;   // offset_0x108
    void* m_unk15;   //
    void* m_unk16;   // offset_0x118
    void* m_unk17;   // offset_0x120
    int m_unk18;     // offset_0x128
    int m_unk19;
};
#pragma pack(pop)
static_assert(sizeof(ENetClient) == 312, "ENetClient class size mismatch");