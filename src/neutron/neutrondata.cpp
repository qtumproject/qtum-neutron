#include <utility>
#include "neutrondata.h"

NeutronDataFormatError NeutronData::validate(const unsigned char* data, size_t size) {
    size_t index = 0;
    while (index < size) {
        if (index == size - 1) {
            return NeutronDataFormatError::INVALID_FORMAT;
        }
        size_t length = static_cast<size_t>(data[index + 1]) << 8 | static_cast<size_t>(data[index]);
        if (index + 2 + length > size) {
            return NeutronDataFormatError::PAYLOAD_LENGTH_MISMATCH;
        }
        index += length + 2;
    }
    return NeutronDataFormatError::OK;
}

NeutronData& NeutronData::operator<<(const std::vector<unsigned char>& data) {
    m_data.push_back(data.size());
    m_data.insert(m_data.end(), data.begin(), data.end());
    return *this;
}
