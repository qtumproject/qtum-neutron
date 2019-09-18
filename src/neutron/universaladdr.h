//
// Created by CodeFace on 2019-08-27.
//

#ifndef QTUM_NEUTRON_UNIVERSALADDR_H
#define QTUM_NEUTRON_UNIVERSALADDR_H

#include <string>
#include <script/standard.h>

class UniversalAddress {
public:
    enum Version: uint16_t {
        UNKNOWN         = 0x0000,
        P2PKH           = 0x0001,   // pay to pubkey hash
        P2SH            = 0x0002,   // pay to script hash
        P2WPKH          = 0x0003,   // pay to witness pubkey hash
        P2WSH           = 0x0004,   // pay to witness script hash
        NONSTANDARD     = 0x0005,   // non-standard
        LEVM            = 0x4001,   // legacy EVM
        NEVM            = 0x8002,   // Neutron EVM
        NX86            = 0x8003,   // Neutron X86
        NTVM            = 0x8004    // Neutron Test VM
    };

    UniversalAddress();
    UniversalAddress(Version version, std::vector<unsigned char> data);
    UniversalAddress(Version version, const unsigned char *data, size_t size);
    UniversalAddress(const UniversalAddress &another);

    ~UniversalAddress() {
    }

    UniversalAddress& operator = (const UniversalAddress &another);
    bool operator == (const UniversalAddress &another);

    inline Version version() const                          { return m_version; }
    inline const std::vector<unsigned char>& data() const   { return m_data; };

    inline void setVersion(Version version)         { m_version = version; }
    void setData(const std::vector<unsigned char> &data);
    void setData(const unsigned char *data, size_t size);
    int setHex(const char* psz);
    int setHex(const std::vector<unsigned char> &data);
    int setHex(const std::string &str);

    std::string getHex();

private:
    Version m_version;
    std::vector<unsigned char> m_data;
};

UniversalAddress DestinationToUniversal(const CTxDestination& dest);

CTxDestination UniversalToDestination(const UniversalAddress& uaddr);

bool IsValidUniversalAddress(const UniversalAddress& uaddr);

bool IsValidSenderUniversalAddress(const UniversalAddress& uaddr);

bool IsContractUniversalAddress(const UniversalAddress& uaddr);

// read base58/bech32/hex string, convert to UniversalAddress
bool ReadUniversalAddress(const std::string& str, UniversalAddress& uaddr);

#endif //QTUM_NEUTRON_UNIVERSALADDR_H
