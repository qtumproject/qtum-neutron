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
    m_version(INVALID),
    m_data_size(0),
    m_data(nullptr)
{

}

UniversalAddress::UniversalAddress(Version version, const uint8_t *data, uint32_t size):
    m_version(version),
    m_data_size(size),
    m_data(nullptr)
{
    if (size > 0 && data != nullptr) {
        m_data = (uint8_t *)malloc(size);
        memcpy(m_data, data, size);
    }
}

UniversalAddress::UniversalAddress(const UniversalAddress &another) {
    m_version = another.m_version;
    if (another.m_data_size > 0 && another.m_data != nullptr) {
        m_data_size = another.m_data_size;
        m_data = (uint8_t *)malloc(m_data_size);
        memcpy(m_data, another.m_data, m_data_size);
    } else {
        m_data = nullptr;
        m_data_size = 0;
    }
}

UniversalAddress& UniversalAddress::operator=(const UniversalAddress &another) {
    if (this != &another) {
        m_version = another.m_version;
        setData(another.m_data, another.m_data_size);
    }
    return *this;
}

bool UniversalAddress::operator==(const UniversalAddress &another) {
    return (m_version == another.m_version
    && m_data_size == another.m_data_size
    && memcmp(m_data, another.m_data, m_data_size) == 0);
}

void UniversalAddress::setData(const uint8_t *data, uint32_t size) {
    if (size == 0) {
        m_data_size = 0;
        if (m_data != nullptr) free(m_data);
        m_data = nullptr;
        return;
    }
    if (m_data_size == size && m_data != nullptr) {
        memcpy(m_data, data, size);
    } else {
        if (m_data != nullptr) free(m_data);
        m_data = (uint8_t *)malloc(size);
        memcpy(m_data, data, size);
        m_data_size = size;
    }
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

    std::vector<uint8_t> data;
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
    setData(data.data() + 2, data.size() - 2);
    return 0;
}

int UniversalAddress::setHex(const std::string &str) {
    return setHex(str.c_str());
}

std::string UniversalAddress::getHex() {
    if (m_data_size == 0 || m_data == nullptr) {
        return "";
    }
    std::stringstream hex;
    hex << std::hex << std::setfill('0');

    uint16_t version = htole16(m_version);
    hex << std::setw(2) << static_cast<unsigned>(version & 0xff);
    hex << std::setw(2) << static_cast<unsigned>(version >> 8);

    for (size_t i=0; i < m_data_size; i++) {
        hex << std::setw(2) << static_cast<unsigned>(m_data[i]);
    }

    return hex.str();
}

class DestinationConverter : public boost::static_visitor<UniversalAddress>
{

public:
    UniversalAddress operator()(const CKeyID& id) const {
        std::vector<uint8_t> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::P2PKH, data.data(), (uint32_t)id.size());
        return addr;
    }

    UniversalAddress operator()(const CScriptID& id) const {
        std::vector<uint8_t> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::P2SH, data.data(), (uint32_t)id.size());
        return addr;
    }

    UniversalAddress operator()(const WitnessV0KeyHash& id) const {
        std::vector<uint8_t> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::INVALID, data.data(), (uint32_t)id.size());
        return addr;
    }

    UniversalAddress operator()(const WitnessV0ScriptHash& id) const {
        std::vector<uint8_t> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::INVALID, data.data(), (uint32_t)id.size());
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
            std::copy(ua.data(), ua.data() + ua.dataSize(), hash.begin());
            return CKeyID(hash);
        }
        case UniversalAddress::P2SH:
        {
            uint160 hash;
            std::copy(ua.data(), ua.data() + ua.dataSize(), hash.begin());
            return CScriptID(hash);
        }
        default:
        {
            return CNoDestination();
        }
    }
}
