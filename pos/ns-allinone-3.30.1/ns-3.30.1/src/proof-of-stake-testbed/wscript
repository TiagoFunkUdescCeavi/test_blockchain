# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('proof-of-stake-testbed', ['internet', 'config-store','stats', 'network', 'core'])
    module.source = [
        'model/constants.cc',
        'model/blockchain.cc',
        'model/node-helper.cc',
        'model/blockchain-node.cc',
        'model/ouroboros-helper.cc',
        'model/ouroboros-node.cc',
        'model/algorand-helper.cc',
        'model/algorand-node.cc',
        'model/casper-helper.cc',
        'model/casper-node.cc',
        'helper/network-helper.cc',
        'utils/rsa.cc',
        ]

    module_test = bld.create_ns3_module_test_library('proof-of-stake-testbed')
    module_test.source = [
        'test/proof-of-stake-testbed-test-suite.cc',
        'test/blockchain-test.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'proof-of-stake-testbed'
    headers.source = [
        'model/constants.h',
        'model/blockchain.h',
        'model/node-helper.h',
        'model/blockchain-node.h',
        'model/ouroboros-helper.h',
        'model/ouroboros-node.h',
        'model/algorand-helper.h',
        'model/algorand-node.h',
        'model/casper-helper.h',
        'model/casper-node.h',
        'helper/network-helper.h',
        'utils/rsa.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    bld.ns3_python_bindings()

