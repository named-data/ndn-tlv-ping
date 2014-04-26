# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION='0.1'
APPNAME="ndn-tlv-ping"

def options(opt):
    opt.load('compiler_cxx gnu_dirs')

def configure(conf):
    conf.load("compiler_cxx gnu_dirs")
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

def build (bld):
    bld.program (
        features = 'cxx',
        target   = 'ndnping',
        source   = 'src/ndn-tlv-ping.cpp',
        use      = 'NDN_CXX',
        )

    bld.program (
        features = 'cxx',
        target   = 'ndnpingserver',
        source   = 'src/ndn-tlv-pingserver.cpp',
        use      = 'NDN_CXX',
        )

