#!/usr/bin/env python3
# Copyright (c) 2015-2019 The Qtum Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
import os, binascii
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from test_framework.script import *
from test_framework.mininode import *
from test_framework.address import *
from test_framework.qtum import *


INVALID_UNIVERSAL = [
    "0000",
    "0100",
    "010000",
    "010045837cfcb0f1ec121b105647e2cefd0bc7be48",
    "000045837cfcb0f1ec121b105647e2cefd0bc7be48a7",
    "010045837cfcb0f1ec121b105647e2cefd0bc7be48a701",
    "010045837cfcb0f1ec121b105647e2cefd0bc7be48a7gg",
    "ffff45837cfcb0f1ec121b105647e2cefd0bc7be48a7",
    "010045837cfcb0f1ec121b105647e2c",
    "0200",
    "02000100",
    "0200d5a81c1e5a3bcb2e2cd4c174898443e99636301e11",
    "0300",
    "0300f8050a633ef61e7f652e655f749548d27da4e809gg",
    "030077871d1bdd2ab9b9a44a88c0b18ebb489161aba6023be3ba9703364adf524b00",
    "0400f8050a633ef61e7f652e655f749548d27da4e809",
    "0380989dd90ed02ad7dc699720c84457363024356f3cd",
    "0480f646adf67a051e6f15a284bcf549bc74de3223",
    "0480f646adf67a051e6f15a284bcf549bc74de32234g",
    "0480f646adf67a051e6f15a284bcf549bc74de32234500",
    "0481f646adf67a051e6f15a284bcf549bc74de322345",
]

class NeutronSendtocontractTest(BitcoinTestFramework):

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.extra_args = [['-txindex=1']]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def setup_contract(self):
        # todo, cannot really create a neutron contract for now, so use random hex
        self.contract_universal = '0380' + os.urandom(20).hex()

    def sendtocontract_params_validation_test(self):
        for invalid_contract_addr in INVALID_UNIVERSAL:
            assert_raises_rpc_error(-5, 'Incorrect contract address', self.node.neutronsendtocontract, invalid_contract_addr, '00')

        for invalid_sender_addr in INVALID_UNIVERSAL:
            assert_raises_rpc_error(-5, 'Invalid Qtum address to send from', self.node.neutronsendtocontract, self.contract_universal, '00', 0, 1000000, QTUM_MIN_GAS_PRICE_STR, invalid_sender_addr)

        # todo, check non-existent contract

        # use a contract address as sender
        invalid_sender_addr = 'tVNnwt8c9HrE63mhhacDUtJ7k4v4e6W4w2'
        assert_raises_rpc_error(-5, 'Invalid contract sender address.', self.node.neutronsendtocontract, self.contract_universal, '00', 0, 1000000, QTUM_MIN_GAS_PRICE_STR, invalid_sender_addr)

        # p2sh and bech32 addresses are not allowed for now
        p2sh_sender = self.node.getnewaddress("", "p2sh-segwit")
        assert_raises_rpc_error(-5, "Invalid contract sender address.", self.node.neutronsendtocontract, self.contract_universal, "00", 0, 1000000, QTUM_MIN_GAS_PRICE_STR, p2sh_sender)

        bech32_sender = self.node.getnewaddress("", "bech32")
        assert_raises_rpc_error(-5, "Invalid contract sender address.", self.node.neutronsendtocontract, self.contract_universal, "00", 0, 1000000, QTUM_MIN_GAS_PRICE_STR, bech32_sender)

        # invalid data
        assert_raises_rpc_error(-3, 'Invalid data (data not hex)', self.node.neutronsendtocontract, self.contract_universal, '00fg')

    def sendtocontract_specify_sender_test(self):
        sender = self.node.getnewaddress("", "legacy")
        ret = self.node.neutronsendtocontract(self.contract_universal, '0000', 0, 1000000, QTUM_MIN_GAS_PRICE_STR, sender)
        assert_equal(sender, ret['senderAddress'])
        assert_equal(self.node.touniversal(sender), ret['senderUniversal'])

    def sendtocontract_no_broadcast(self):
        sender = self.node.getnewaddress("", "legacy")
        data = '0000'
        ret = self.node.neutronsendtocontract(self.contract_universal, data, 0, 1000000, QTUM_MIN_GAS_PRICE_STR, sender, False)
        assert('raw transaction' in ret)
        raw = ret['raw transaction']

        decoded_tx = self.node.decoderawtransaction(raw)

        # verify that at least one output has a scriptPubKey of type create
        good = False
        for out in decoded_tx['vout']:
            if out['scriptPubKey']['type'] == 'call' or out['scriptPubKey']['type'] == 'call_sender':
                hex = out['scriptPubKey']["hex"]
                assert_equal(hex[-2:], "c2")            # OP_CREATE
                assert_equal(hex[-48:-2], '16' + self.contract_universal)      # contract address
                assert_equal(hex[-54:-48], "020000")    # data
                assert_equal(hex[-58:-54], "0128")      # gas price
                assert_equal(hex[-66:-58], "0340420f")  # gas limit
                good = True
        assert(good)

        txid = self.node.sendrawtransaction(raw)
        self.node.generatetoaddress(1, self.addr)
        assert_equal(decoded_tx['txid'], txid)
        tx = self.node.gettransaction(txid)
        assert_equal(tx['confirmations'], 1)

    def sendtocontract_verify_storage_test(self):
        # todo
        pass

    def sendtocontract_verify_storage_and_balance_test(self):
        # todo
        pass

    def run_test(self):
        self.node = self.nodes[0]
        self.addr = self.node.getnewaddress("", "legacy")
        self.node.generatetoaddress(COINBASE_MATURITY+50, self.addr)
        self.setup_contract()
        self.sendtocontract_params_validation_test()
        self.sendtocontract_specify_sender_test()
        self.sendtocontract_no_broadcast()
        self.sendtocontract_verify_storage_test()
        self.sendtocontract_verify_storage_and_balance_test()


if __name__ == '__main__':
    NeutronSendtocontractTest().main()
