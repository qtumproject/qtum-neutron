//
// Created by CodeFace on 2019/10/29.
//

#ifndef QTUM_NEUTRON_API_H
#define QTUM_NEUTRON_API_H

#include <stdint.h>

#define NEUTRON_SUCCESS 0
#define NEUTRON_RECOVERABLE_ERROR 1
#define NEUTRON_UNRECOVERABLE_ERROR 2
#define NEUTRON_OUT_OF_GAS 3

#define NEUTRON_EXEC 1
#define NEUTRON_CALL 2
#define NEUTRON_CREATE 3

#define VM_SUCCESS 0
#define VM_INVALID 1
#define VM_OUT_OF_GAS 2
#define VM_FAULT 3

typedef struct{
    uint16_t version;
    uint16_t payloadSize;
    void* payload;
} __attribute__((packed)) UniversalAddressABI;

typedef struct{
    uint8_t format;
    uint8_t rootVM;
    uint8_t vmVersion;
    uint16_t flagOptions;
    uint32_t qtumVersion;
} NeutronVersion;

typedef struct VMContext{
    int execType;
    NeutronVersion version;
    uint64_t gasRemaining;
    uint64_t gasPrice;
    UniversalAddressABI* contractAddress;

    //NeutronAPI function pointers follow:
    void (*log)(const VMContext* ctx, const char* msg); //null terminated messages
} VMContext;

typedef struct{
    uint32_t status;
} VMStatus;

#endif //QTUM_NEUTRON_API_H
