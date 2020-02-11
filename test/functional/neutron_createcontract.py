#!/usr/bin/env python3
# Copyright (c) 2015-2019 The Qtum Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from test_framework.script import *
from test_framework.mininode import *
from test_framework.address import *
from test_framework.qtum import *


class NeutronCreatecontractTest(BitcoinTestFramework):

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.extra_args = [['-txindex=1']]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def createcontract_simple_test(self):
        ret = self.node.neutroncreatecontract("testvm", "0000", 1000000)
        assert('txid' in ret)
        assert('senderAddress' in ret)
        assert('senderUniversal' in ret)
        contract_address = ret['contractAddress']
        contract_universal = ret['contractUniversal']
        assert(contract_address.startswith("t"))
        assert(contract_universal.startswith("0480"))
        self.node.generatetoaddress(1, self.addr)
        tx = self.node.gettransaction(ret["txid"])
        assert_equal(tx['confirmations'], 1)

        ret = self.node.neutroncreatecontract("x86", "0000", 1000000)
        assert('txid' in ret)
        assert('senderAddress' in ret)
        assert('senderUniversal' in ret)
        contract_address = ret['contractAddress']
        contract_universal = ret['contractUniversal']
        assert(contract_address.startswith("x"))
        assert(contract_universal.startswith("0380"))
        self.node.generatetoaddress(1, self.addr)
        tx = self.node.gettransaction(ret["txid"])
        assert_equal(tx['confirmations'], 1)

        # todo check accountinfo
        # ret = self.node.getaccountinfo(contract_address)
        # expected_account_info = {
        #     "contractAddress": contract_address,
        #     "contractUniversal": contract_universal,
        #     "balance": 0,
        #     "storage": {},
        #     "code": "0000"
        # }
        # assert_equal(ret, expected_account_info)

    def createcontract_invalid_vmname_test(self):
        assert_raises_rpc_error(-3, "Invalid vmname", self.node.neutroncreatecontract, "invalid", "0000", 1000000)
        assert_raises_rpc_error(-3, "Invalid vmname", self.node.neutroncreatecontract, "", "0000", 1000000)
        assert_raises_rpc_error(-3, "Invalid vmname", self.node.neutroncreatecontract, "x87", "0000", 1000000)
        assert_raises_rpc_error(-3, "Invalid vmname", self.node.neutroncreatecontract, "x86 ", "0000", 1000000)
        assert_raises_rpc_error(-3, "Invalid vmname", self.node.neutroncreatecontract, "x86"*100, "0000", 1000000)

    def createcontract_with_sender_test(self):
        assert_raises_rpc_error(-4, "Private key not available", self.node.neutroncreatecontract, "testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, "qabmqZk3re5b9UpUcznxDkCnCsnKdmPktT")
        assert_raises_rpc_error(-5, "Invalid Qtum address to send from", self.node.neutroncreatecontract, "testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, "qabmqZk3re5b9UpUcznxDkCnCsnKdmPkt")
        assert_raises_rpc_error(-4, "Private key not available", self.node.neutroncreatecontract, "testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, "010045837cfcb0f1ec121b105647e2cefd0bc7be48a7")

        sender = self.node.getnewaddress("", "legacy")
        senderUniversal = self.node.touniversal(sender)

        ret = self.node.neutroncreatecontract("testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, sender)
        assert_equal(ret["senderAddress"], sender)
        assert_equal(ret["senderUniversal"], senderUniversal)
        assert_equal(ret["contractUniversal"], self.node.touniversal(ret["contractAddress"]))

        rawtx = self.node.getrawtransaction(ret["txid"])
        tx = self.node.decoderawtransaction(rawtx)
        assert_equal(tx["txid"], ret["txid"])

        ret = self.node.neutroncreatecontract("testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, senderUniversal)
        assert_equal(ret["senderAddress"], sender)
        assert_equal(ret["senderUniversal"], senderUniversal)
        assert_equal(ret["contractUniversal"], self.node.touniversal(ret["contractAddress"]))

        # p2sh and bech32 addresses are not allowed for now
        p2sh_sender = self.node.getnewaddress("", "p2sh-segwit")
        assert_raises_rpc_error(-5, "Invalid sender address. Only P2PK or P2PKH address allowed", self.node.neutroncreatecontract, "testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, p2sh_sender)

        bech32_sender = self.node.getnewaddress("", "bech32")
        assert_raises_rpc_error(-5, "Invalid sender address. Only P2PK or P2PKH address allowed", self.node.neutroncreatecontract, "testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, bech32_sender)

    def createcontract_no_broadcast_test(self):
        sender = self.node.getnewaddress("", "legacy")
        ret = self.node.neutroncreatecontract("testvm", "0000", 1000000, QTUM_MIN_GAS_PRICE_STR, sender, False)
        raw = ret["raw transaction"]
        assert_equal(len(ret.keys()), 1)
        decoded_tx = self.node.decoderawtransaction(raw)

        # verify that at least one output has a scriptPubKey of type create
        good = False
        for out in decoded_tx['vout']:
            if out['scriptPubKey']['type'] == 'create' or out['scriptPubKey']['type'] == 'create_sender':
                hex = out['scriptPubKey']["hex"]
                assert_equal(hex[-2:], "c1")            # OP_CREATE
                assert_equal(hex[-8:-2], "020000")      # contract byte code
                assert_equal(hex[-12:-8], "0128")       # gas price
                assert_equal(hex[-20:-12], "0340420f")  # gas limit
                assert_equal(hex[-24:-20], "0111")      # vm version
                good = True
        assert(good)

        txid = self.node.sendrawtransaction(raw)
        self.node.generatetoaddress(1, sender)
        tx = self.node.gettransaction(txid)
        assert_equal(tx['confirmations'], 1)
        assert_equal(raw, tx['hex'])

    def run_test(self):
        self.addr = self.nodes[0].getnewaddress("", "legacy")
        self.nodes[0].generatetoaddress(COINBASE_MATURITY+50, self.addr)
        self.node = self.nodes[0]
        self.createcontract_simple_test()
        self.createcontract_invalid_vmname_test()
        self.createcontract_with_sender_test()
        self.createcontract_no_broadcast_test()


if __name__ == '__main__':
    NeutronCreatecontractTest().main()
