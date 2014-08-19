# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import os

VERSION='0.1'
APPNAME="ndn-tlv-ping"

def options(opt):
    opt.load('compiler_cxx gnu_dirs')

def configure(conf):
    conf.load("compiler_cxx gnu_dirs")

    if not os.environ.has_key('PKG_CONFIG_PATH'):
        os.environ['PKG_CONFIG_PATH'] = ':'.join([
            '/usr/lib/pkgconfig',
            '/usr/local/lib/pkgconfig',
            '/opt/local/lib/pkgconfig'])
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

def build(bld):
    bld.program(
        features='cxx',
        target='ndnping',
        source='src/ndn-tlv-ping.cpp',
        use='NDN_CXX',
        )

    bld.program(
        features='cxx',
        target='ndnpingserver',
        source='src/ndn-tlv-pingserver.cpp',
        use='NDN_CXX',
        )
