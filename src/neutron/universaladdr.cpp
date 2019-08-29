//
// Created by CodeFace on 2019-08-27.
//

#include "universaladdr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <util/strencodings.h>
#include <key.h>
#include <iomanip>
#include <compat/endian.h>

UniversalAddress::UniversalAddress():
    m_version(INVALID), m_data{0}
{
}

UniversalAddress::UniversalAddress(UniversalAddress::Version version, std::vector<unsigned char> data):
    m_version(version), m_data(std::move(data))
{
}

UniversalAddress::UniversalAddress(Version version, const unsigned char *data, size_t size):
    m_version(version)
{
    setData(data, size);
}

UniversalAddress::UniversalAddress(const UniversalAddress &another) {
    m_version = another.m_version;
    m_data = another.m_data;
}

UniversalAddress& UniversalAddress::operator=(const UniversalAddress &another) {
    if (this != &another) {
        m_version = another.m_version;
        m_data = another.m_data;
    }
    return *this;
}

bool UniversalAddress::operator==(const UniversalAddress &another) {
    return (m_version == another.m_version && m_data == another.m_data);
}

void UniversalAddress::setData(const std::vector<unsigned char> &data) {
    m_data = data;
}

void UniversalAddress::setData(const unsigned char *data, size_t size) {
    m_data.clear();
    m_data.insert(m_data.end(), data, data + size);
}

int UniversalAddress::setHex(const char *psz) {
    // skip leading spaces
    while (IsSpace(*psz))
        psz++;

    // skip 0x
    if (psz[0] == '0' && ToLower(psz[1]) == 'x')
        psz += 2;

    // hex string to uint
    const char* pbegin = psz;
    while (::HexDigit(*psz) != -1)
        psz++;
    psz--;

    std::vector<unsigned char> data;
    while (psz >= pbegin) {
        auto p = ::HexDigit(*psz--);
        if (psz >= pbegin) {
            p |= ((unsigned char)::HexDigit(*psz--) << 4);
        }
        data.push_back(p);
    }
    std::reverse(data.begin(), data.end());

    size_t dataSize = data.size() -2 ;
    if (dataSize < 2) {
        return -1;
    }

    uint16_t version;
    memcpy(&version, data.data(), 2);
    version = le16toh(version);

    // check version and data size
    switch (version) {
        case P2PKH:
        case P2SH:
        case TESTVM:
        {
            // hash160 size mismatch
            if (dataSize != 20) return -2;
            break;
        }
        default:
            // invalid or unknown version
            return -3;
    }

    setVersion(Version(version));
    setData(data.data() + 2, dataSize);
    return 0;
}

int UniversalAddress::setHex(const std::string &str) {
    return setHex(str.c_str());
}

std::string UniversalAddress::getHex() {
    if (m_data.empty()) {
        return "";
    }
    std::stringstream hex;
    hex << std::hex << std::setfill('0');

    uint16_t version = htole16(m_version);
    hex << std::setw(2) << static_cast<unsigned>(version & 0xff);
    hex << std::setw(2) << static_cast<unsigned>(version >> 8);

    for (unsigned char i: m_data) {
        hex << std::setw(2) << static_cast<unsigned>(i);
    }

    return hex.str();
}

class DestinationConverter : public boost::static_visitor<UniversalAddress>
{

public:
    UniversalAddress operator()(const CKeyID& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::P2PKH, data);
        return addr;
    }

    UniversalAddress operator()(const CScriptID& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::P2SH, data);
        return addr;
    }

    UniversalAddress operator()(const WitnessV0KeyHash& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::INVALID, data);
        return addr;
    }

    UniversalAddress operator()(const WitnessV0ScriptHash& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::INVALID, data);
        return addr;
    }

    UniversalAddress operator()(const WitnessUnknown& id) const {
        return UniversalAddress();
    }

    UniversalAddress operator()(const CNoDestination& no) const {
        return UniversalAddress();
    }
};

UniversalAddress DestinationToUniversal(const CTxDestination& dest) {
    return boost::apply_visitor(DestinationConverter(), dest);
}

CTxDestination UniversalToDestination(const UniversalAddress& ua) {
    switch (ua.version()) {
        case UniversalAddress::P2PKH:
        {
            uint160 hash;
            std::copy(ua.data().begin(), ua.data().end(), hash.begin());
            return CKeyID(hash);
        }
        case UniversalAddress::P2SH:
        {
            uint160 hash;
            std::copy(ua.data().begin(), ua.data().end(), hash.begin());
            return CScriptID(hash);
        }
        default:
        {
            return CNoDestination();
        }
    }
}
