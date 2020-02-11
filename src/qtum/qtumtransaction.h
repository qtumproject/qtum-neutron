#ifndef QTUMTRANSACTION_H
#define QTUMTRANSACTION_H

#include <libethereum/Transaction.h>

struct VersionVM{
    //this should be portable, see https://stackoverflow.com/questions/31726191/is-there-a-portable-alternative-to-c-bitfields
# if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t format : 2;
    uint8_t rootVM : 6;
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t rootVM : 6;
    uint8_t format : 2;
#endif
    uint8_t vmVersion;
    uint16_t flagOptions;
    // CONSENSUS CRITICAL!
    // Do not add any other fields to this struct

    static constexpr uint8_t ROOTVM_NOEXEC = 0;
    static constexpr uint8_t ROOTVM_EVM = 1;
    static constexpr uint8_t ROOTVM_NEUTRON_X86 = 3;
    static constexpr uint8_t ROOTVM_NEUTRON_TESTVM = 4;

    static constexpr uint8_t FORMAT_EVM = 0;
    static constexpr uint8_t FORMAT_NEUTRON = 1;

    uint32_t toRaw() const {
        return *(uint32_t*)this;
    }
    bool isValidFormat(bool neutronHardfork) const {
        return format == FORMAT_EVM || (neutronHardfork && format == FORMAT_NEUTRON);
    }

    static VersionVM fromRaw(uint32_t val){
        VersionVM x = *(VersionVM*)&val;
        return x;
    }
    static VersionVM GetNoExec(){
        VersionVM x;
        x.flagOptions = 0;
        x.rootVM = ROOTVM_NOEXEC;
        x.format = FORMAT_EVM;
        x.vmVersion = 0;
        return x;
    }
    static VersionVM GetEVMDefault(){
        VersionVM x;
        x.flagOptions = 0;
        x.rootVM = ROOTVM_EVM;
        x.format = FORMAT_EVM;
        x.vmVersion = 0;
        return x;
    }
    static VersionVM GetNeutronX86Default() {
        VersionVM x;
        x.flagOptions = 0;
        x.rootVM = ROOTVM_NEUTRON_X86;
        x.format = FORMAT_NEUTRON;
        x.vmVersion = 0;
        return x;
    }
    static VersionVM GetNeutronTestVMDefault() {
        VersionVM x;
        x.flagOptions = 0;
        x.rootVM = ROOTVM_NEUTRON_TESTVM;
        x.format = FORMAT_NEUTRON;
        x.vmVersion = 0;
        return x;
    }
    bool isNoExec() const {
        return toRaw() == GetNoExec().toRaw();
    }
    bool isEVM() const {
        return toRaw() == GetEVMDefault().toRaw();
    }
    bool isNeutronX86() const {
        return toRaw() == GetNeutronX86Default().toRaw();
    }
    bool isNeutronTestVM() const {
        return toRaw() == GetNeutronTestVMDefault().toRaw();
    }
}__attribute__((__packed__));

class QtumTransaction : public dev::eth::Transaction{

public:

    QtumTransaction() : nVout(0), hasRefundSender(false) {}

    QtumTransaction(dev::u256 const& _value, dev::u256 const& _gasPrice, dev::u256 const& _gas, dev::bytes const& _data, dev::u256 const& _nonce = dev::Invalid256):
        dev::eth::Transaction(_value, _gasPrice, _gas, _data, _nonce) , nVout(0), hasRefundSender(false) {}

    QtumTransaction(dev::u256 const& _value, dev::u256 const& _gasPrice, dev::u256 const& _gas, dev::Address const& _dest, dev::bytes const& _data, dev::u256 const& _nonce = dev::Invalid256):
        dev::eth::Transaction(_value, _gasPrice, _gas, _dest, _data, _nonce) , nVout(0), hasRefundSender(false) {}

    void setHashWith(const dev::h256 hash) { m_hashWith = hash; }

    dev::h256 getHashWith() const { return m_hashWith; }

    void setNVout(uint32_t vout) { nVout = vout; }

    uint32_t getNVout() const { return nVout; }

    void setVersion(VersionVM v){
        version=v;
    }
    VersionVM getVersion() const{
        return version;
    }

    void setRefundSender(const dev::Address _refundSender) { refundSender = _refundSender; hasRefundSender = true;}

    dev::Address getRefundSender() const { return hasRefundSender ? refundSender : sender();}

private:

    uint32_t nVout;
    VersionVM version;
    dev::Address refundSender;
    bool hasRefundSender;
};
#endif
