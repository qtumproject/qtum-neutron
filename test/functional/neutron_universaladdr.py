#!/usr/bin/env python3
# Copyright (c) 2016-2019 The Qtum Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.qtum import *

VALID_DATA = [
    ["qPtwJEJ84trxrohBbdEMdTj4rCZEzHLMbT", "010045837cfcb0f1ec121b105647e2cefd0bc7be48a7"],
    ["qbx8rwEujG8VpeBRrSg11SiinVSAkfu15B", "0100c9bfe60908817a00b03f9cf7ee8aea34319ca9dd"],
    ["qPGVvPsc55FTAZERa6Vx9t31TE6aRK5qx4", "01003e9f549531b2db2153a608859b15bf43a62370a3"],
    ["qK72Xyxf4gAfiFZMHg8hRgTdUZdk73e1Vw", "010010f438faf28397304d8b7a503d18a6ba313fe75b"],
    ["qcJa3eSfT5GoMR5RhbqaHpdNejHcckPiym", "0100cd9d3dac285182775b36c450c0716e56fce70a28"],
    ["qdXyDH3em7RVpkkstQA9wRuRCdXt51Cwib", "0100db1e1cbbeec912d828e3e17fc976c18406506a72"],
    ["qKguE3ZjtpycbnJbZMKWj7yJoVfz4wTLs1", "0100175c4388c9bfa06e24ec18b045fa7cb2c4ee0481"],
    ["qfHFzdDnaKt6r26CQUH2upDf4Mcb72ggLg", "0100ee462217b1516e09411573c476ae64d18885bc68"],
    ["qQSxWSaH9ZBNRBp7CQPSbFGh2qdijSGTLW", "01004b91cf3ccb105c1933e8390e9c7d594dddcc1962"],
    ["qf8cmJsb37sgpHboPjVDeMpnWWa9q1y6Nt", "0100eca3ba4fddff11c752bead58b7fb0f1ce85526fc"],
    ["mTx44RJ2YdPFXR6VWwj7YNZiu4t8n56tvH", "0200811db3b36aeb0a6b551ceae9aaa0fd11478eba29"],
    ["mZu7RvyL8H6sAngnqk2Cn9NzPXbBraXw8y", "0200c26005cc843565216e15084eee774dd0fa922407"],
    ["mbf4dHHgve4jLhMAJHqGs14VLrnv2jbsKs", "0200d5a81c1e5a3bcb2e2cd4c174898443e99636301e"],
    ["mH3rCdB449CjXabbx7BLkXepA2XfwS1kDu", "0200098cea43cc362f64635c9b809a69c1e23334d845"],
    ["mGsURWf29xJj6q4daBA7g1DGJCFKtfRq1i", "020007969387943ad09910f24729c4d79daec383aaec"],
    ["mUgaNvCZfz6wGfnb13cJCSbPXtnFfy3PqR", "020089287b382fa8d830ed50aca65aaf61aa1fd88967"],
    ["mdQDMfcUiWmQB1H1LXejFBiM5yJfhCKHso", "0200e8c968f9d321cfc349a436fd40b9cbdbeec21c3e"],
    ["mPw5MhtFabAUY9c3aDRYS81bgQvfaKANGH", "0200550dddccb27ea72a48a2f693e13f6de02ecc3700"],
    ["mLKuKUSeCAKT2TJKc67T6gmzvzkZKwQ355", "02002d7e8cfa1434778da01827a5c958529d6baaf346"],
    ["mJeX2jmBpMm4no5MNuD2pKm5m5DXSVkBtZ", "02001b1398cb5ff5f41a8c424cfcb61b0414a8fb2433"],
    ["qcrt1qlcn3qxlett78z6f3k9mje033gw3eqplc4r26f2", "0300fe27101bf95afc716931b1772cbe3143a39007f8"],
    ["qcrt1qg6tkqd2wcnwnq4rxk3agxsme90cq5lg3j6862r", "0300469760354ec4dd305466b47a8343792bf00a7d11"],
    ["qcrt1qkevjwykt0pkh9vgcqvx887tswssq9wf6xwuhu5", "0300b6592712cb786d72b118030c73f970742002b93a"],
    ["qcrt1q4z8spc9ns4qx6dy3uw95sltr7kmqudvu3uvm8h", "0300a88f00e0b385406d3491e38b487d63f5b60e359c"],
    ["qcrt1q4ltmcdxpdgkgh42rqurnppf5cz7grklk4k9v6p", "0300afd7bc34c16a2c8bd5430707308534c0bc81dbf6"],
    ["qcrt1qqdwe6jxjcn5t0j66uttljdzqxms9welcjk9m4w", "0300035d9d48d2c4e8b7cb5ae2d7f9344036e05767f8"],
    ["qcrt1qpd5y0a0u0qemv3wjct5teylvs29206zzftzd6c", "03000b6847f5fc7833b645d2c2e8bc93ec828aa7e842"],
    ["qcrt1qmvjje6mjk240mvzg0z763j7hteq3ctl0fy66s7", "0300db252ceb72b2aafdb04878bda8cbd75e411c2fef"],
    ["qcrt1qkhky23u57yqyqa8knxru8q6kq56tjpflf4uzh7", "0300b5ec454794f1004074f69987c383560534b9053f"],
    ["qcrt1qlqzs5ce77c087efwv40hf92g6f76f6qfpsdh5f", "0300f8050a633ef61e7f652e655f749548d27da4e809"],
    ["qcrt1qq4jp48epyg8h98evkqyeu894kpqr96y898qe72n2e2p37ntfxeeql2ywqw", "040005641a9f21220f729f2cb0099e1cb5b04032e88729c19f2a6aca831f4d693672"],
    ["qcrt1qwpy83e03s0n84d7e7l9uak80eneaguanrk9rra8x0x6fq26gp2fsskraxy", "0400704878e5f183e67ab7d9f7cbced8efccf3d473b31d8a31f4e679b4902b480a93"],
    ["qcrt1qh2v54t52angnzqkrxhw8rq7skm2k0vyv94dccftm2wj49d5jcjsqdkdzln", "0400ba994aae8aecd13102c335dc7183d0b6d567b08c2d5b8c257b53a552b692c4a0"],
    ["qcrt1qx8ca8k6puw7ymjt78ter3fqss9rnwxxkwfx82eadg7774u4r2vlsedxqkz", "040031f1d3db41e3bc4dc97e3af238a41081473718d6724c7567ad47bdeaf2a3533f"],
    ["qcrt1qnpmfsfpvvwjxqhmh4p3890fr6ygmm0rdakws68tzfqt4guqa4n3swxcu6u", "0400987698242c63a4605f77a86272bd23d111bdbc6ded9d0d1d62481754701dace3"],
    ["qcrt1qrj0ll0cy4cfgxvy2c92vc3exaeqaj4h6hhp2tl768hy3ddqszwqq6kgj6u", "04001c9fffbf04ae1283308ac154cc4726ee41d956fabdc2a5ffda3dc916b4101380"],
    ["qcrt1qtxe0j05kkv4zluse05kem362t8l9ehxxxlundxjz7zuy9vwkagmq5d2kwn", "040059b2f93e96b32a2ff2197d2d9dc74a59fe5cdcc637f9369a42f0b842b1d6ea36"],
    ["qcrt1qstwq53xtgrz4rfxzl8f3teduf4gnhh3rphqvup4cs4vxtwcz92cs80ad95", "040082dc0a44cb40c551a4c2f9d315e5bc4d513bde230dc0ce06b8855865bb022ab1"],
    ["qcrt1q3nxqag3hpqzwq7z4lgam2wlq3vrk5tu84cw4pr2l62hp5rls2vlsy9c5jd", "04008ccc0ea2370804e07855fa3bb53be08b076a2f87ae1d508d5fd2ae1a0ff0533f"],
    ["qcrt1qw7r36x7a92umnfz23rqtrr4mfzgkr2axqga78w5hqvmy4h6jfvqqymclzr", "040077871d1bdd2ab9b9a44a88c0b18ebb489161aba6023be3ba9703364adf524b00"],
]

INVALID_ADDRESS = [
    "qPtwJEJ84trxrohBbdEMdT",
    "qbx8rwEujG8VpeBRrSg11SiinVSAkfu16",
    "qbx8rwEujG8VpeBRrSg11SiinVSAkfu15Bab",
    "maf4dHHgve4jLhMAJHqGs14VLrnv2jbsKs",
    "mdQDMfcUiWmQB1H1LXejFBiM5yJfhCKHs0",
    "qcrt1qlcn3qxlett78z6f3k9mje033gw3eqplc4r26f200",
    "qcrt1qlcn3qxlett78z6f3k9mje033gw3eqplc4r26f1",
    "qcrt1qw7r36x7a92umnfz23rqtrr4mfzgkr2axqga78w5hqvmy4h6jfvqqymcl",
    "qcrt1qstwq53xtgrz4rfxzl7f3teduf4gnhh3rphqvup4cs4vxtwcz92cs80ad95",
    "bcrt1qwpy83e03s0n84d7e7l9uak80eneaguanrk9rra8x0x6fq26gp2fsskraxy",
]

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
]

class UniversalAddressTest(BitcoinTestFramework):

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1

    def fromuniversal_test(self):
        self.node = self.nodes[0]
        for data in VALID_DATA:
            assert_equal(self.node.fromuniversal(data[1]), data[0])

        for data in INVALID_UNIVERSAL:
            assert_raises_rpc_error(-5, "Invalid universal address", self.node.fromuniversal, data)

    def touniversal_test(self):
        self.node = self.nodes[0]
        for data in VALID_DATA:
            assert_equal(self.node.touniversal(data[0]), data[1])

        for data in INVALID_ADDRESS:
            assert_raises_rpc_error(-5, "Invalid Qtum address", self.node.touniversal, data)

    def run_test(self):
        self.fromuniversal_test()
        self.touniversal_test()


if __name__ == '__main__':
    UniversalAddressTest().main()
