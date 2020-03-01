#ifndef QTUM_NEUTRON_NEUTRONDATA_H
#define QTUM_NEUTRON_NEUTRONDATA_H

#include <vector>

enum class NeutronDataFormatError {
    OK,
    INVALID_FORMAT,
    PAYLOAD_LENGTH_MISMATCH
};

class NeutronData {
public:
    NeutronData(const std::vector<unsigned char>& data): m_data(data) {}
    NeutronData(std::vector<unsigned char>&& data): m_data(std::move(data)) {}
    NeutronData(const unsigned char* data, size_t size): m_data(data, data + size) {}

    const std::vector<unsigned char> data() const { return m_data; }

    NeutronData& operator<<(const std::vector<unsigned char>& data);

private:
    std::vector<unsigned char> m_data;

    static NeutronDataFormatError validate(const unsigned char* data, size_t size);
};

#endif
