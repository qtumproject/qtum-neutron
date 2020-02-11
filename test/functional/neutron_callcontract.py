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


class NeutronCallcontractTest(BitcoinTestFramework):

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.extra_args = [['-txindex=1']]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def run_test(self):
        self.node = self.nodes[0]
        self.addr = self.node.getnewaddress("", "legacy")
        self.node.generatetoaddress(COINBASE_MATURITY+50, self.addr)


if __name__ == '__main__':
    NeutronCallcontractTest().main()