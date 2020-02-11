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
#include <chainparams.h>
#include <key_io.h>

UniversalAddress::UniversalAddress():
    m_version(UNKNOWN), m_data{0}
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
    while (true) {
        if ((::HexDigit(*psz) != -1)) {
            psz++;
            continue;
        }
        // the last non-hex char of psz should be '\x00'
        if ((*psz) == '\x00') break;
        return -1;
    }
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
    return setHex(data);
}

int UniversalAddress::setHex(const std::vector<unsigned char> &data) {
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
        case P2WPKH:
        case NX86:
        {
            // size mismatch
            if (dataSize == 20) break;
            return -2;
        }
        case P2WSH:
        {
            // size mismatch
            if (dataSize == 32) break;
            return -3;
        }
        case NTVM:
        {
            // neutron test vm is valid in regtest only
            if (Params().MineBlocksOnDemand() && dataSize == 20) {
                break;
            }
            return -4;
        }
        default:
            // unknown or unsupported version
            return -5;
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
        UniversalAddress addr(UniversalAddress::P2WPKH, data);
        return addr;
    }

    UniversalAddress operator()(const WitnessV0ScriptHash& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::P2WSH, data);
        return addr;
    }

    UniversalAddress operator()(const WitnessUnknown& id) const {
        return UniversalAddress();
    }

    UniversalAddress operator()(const X86VMID& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::NX86, data);
        return addr;
    }

    UniversalAddress operator()(const TestVMID& id) const {
        std::vector<unsigned char> data;
        data.insert(data.end(), id.begin(), id.end());
        UniversalAddress addr(UniversalAddress::NTVM, data);
        return addr;
    }

    UniversalAddress operator()(const CNoDestination& no) const {
        return UniversalAddress();
    }
};

UniversalAddress DestinationToUniversal(const CTxDestination& dest) {
    return boost::apply_visitor(DestinationConverter(), dest);
}

CTxDestination UniversalToDestination(const UniversalAddress& ua) {
    UniversalAddress::Version version = ua.version();

    switch (version) {
        case UniversalAddress::P2PKH:
        case UniversalAddress::P2SH:
        case UniversalAddress::P2WPKH:
        case UniversalAddress::NX86:
        case UniversalAddress::NTVM:
        {
            uint160 hash;
            if (hash.size() == ua.data().size()) {
                std::copy(ua.data().begin(), ua.data().end(), hash.begin());
                if (version == UniversalAddress::P2PKH) return CKeyID(hash);
                if (version == UniversalAddress::P2SH) return CScriptID(hash);
                if (version == UniversalAddress::P2WPKH) return WitnessV0KeyHash(hash);
                if (version == UniversalAddress::NX86) return X86VMID(hash);
                if (Params().MineBlocksOnDemand()) return TestVMID(hash);
            }
            break;
        }
        case UniversalAddress::P2WSH:
        {
            WitnessV0ScriptHash keyid;
            if (keyid.size() == ua.data().size()) {
                std::copy(ua.data().begin(), ua.data().end(), keyid.begin());
                return keyid;
            }
            break;
        }
        default:
            break;
    }
    return CNoDestination();
}

bool IsValidUniversalAddress(const UniversalAddress& uaddr) {
    return uaddr.version() != UniversalAddress::UNKNOWN && !uaddr.data().empty();
}

bool IsValidSenderUniversalAddress(const UniversalAddress& uaddr) {
    return IsValidUniversalAddress(uaddr) && uaddr.version() == UniversalAddress::P2PKH;
}

bool IsContractUniversalAddress(const UniversalAddress& uaddr) {
    return uaddr.version() == UniversalAddress::NX86 || uaddr.version() == UniversalAddress::NTVM;
}

bool ReadUniversalAddress(const std::string& str, UniversalAddress& uaddr) {
    // check base58/bech32
    CTxDestination dest = DecodeDestination(str);
    if (dest.which() != 0) {
        uaddr = DestinationToUniversal(dest);
        return IsValidUniversalAddress(uaddr);
    }
    // check universal hex
    return uaddr.setHex(str) == 0;
}

UniversalAddress::Version VersionVMToUniversalVersion(VersionVM& v) {
    uint32_t raw = v.toRaw();
    if (raw == VersionVM::GetNeutronX86Default().toRaw()) return UniversalAddress::Version::NX86;
    if (raw == VersionVM::GetNeutronTestVMDefault().toRaw()) return UniversalAddress::Version::NTVM;
    return UniversalAddress::Version::UNKNOWN;
}
