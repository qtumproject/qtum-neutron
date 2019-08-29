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
        INVALID= 0x0000,
        P2PKH,  // pay to pubkey hash
        P2SH,   // pay to script hash
        TESTVM  // Neutron Test VM
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
    int setHex(const std::string &str);

    std::string getHex();

private:
    Version m_version;
    std::vector<unsigned char> m_data;
};

UniversalAddress DestinationToUniversal(const CTxDestination& dest);

CTxDestination UniversalToDestination(const UniversalAddress& ua);

#endif //QTUM_NEUTRON_UNIVERSALADDR_H
