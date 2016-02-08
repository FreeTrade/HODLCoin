DIST_SUBDIRS = secp256k1
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)


if EMBEDDED_LEVELDB
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/include
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/helpers/memenv
LIBLEVELDB += $(builddir)/leveldb/libleveldb.a
LIBMEMENV  += $(builddir)/leveldb/libmemenv.a

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
$(LIBLEVELDB): $(LIBMEMENV)

$(LIBLEVELDB) $(LIBMEMENV):
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
          OPT="$(CXXFLAGS) $(CPPFLAGS) -D__STDC_LIMIT_MACROS"
endif

HODLCOIN_CONFIG_INCLUDES=-I$(builddir)/config
HODLCOIN_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

HODLCOIN_INCLUDES += -I$(srcdir)/secp256k1/include

LIBHODLCOIN_SERVER=libhodlcoin_server.a
LIBHODLCOIN_WALLET=libhodlcoin_wallet.a
LIBHODLCOIN_COMMON=libhodlcoin_common.a
LIBHODLCOIN_CLI=libhodlcoin_cli.a
LIBHODLCOIN_UTIL=libhodlcoin_util.a
LIBHODLCOIN_CRYPTO=crypto/libhodlcoin_crypto.a
LIBHODLCOIN_UNIVALUE=univalue/libhodlcoin_univalue.a
LIBHODLCOINQT=qt/libhodlcoinqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES = \
  crypto/libhodlcoin_crypto.a \
  libhodlcoin_util.a \
  libhodlcoin_common.a \
  univalue/libhodlcoin_univalue.a \
  libhodlcoin_server.a \
  libhodlcoin_cli.a
if ENABLE_WALLET
HODLCOIN_INCLUDES += $(BDB_CPPFLAGS)
EXTRA_LIBRARIES += libhodlcoin_wallet.a
endif

if BUILD_HODLCOIN_LIBS
lib_LTLIBRARIES = libhodlcoinconsensus.la
LIBHODLCOIN_CONSENSUS=libhodlcoinconsensus.la
else
LIBHODLCOIN_CONSENSUS=
endif

bin_PROGRAMS =
TESTS =

if BUILD_HODLCOIND
  bin_PROGRAMS += hodlcoind
endif

if BUILD_HODLCOIN_UTILS
  bin_PROGRAMS += hodlcoin-cli hodlcoin-tx
endif

.PHONY: FORCE
# hodlcoin core #
HODLCOIN_CORE_H = \
  addrman.h \
  alert.h \
  amount.h \
  arith_uint256.h \
  base58.h \
  bloom.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  consensus/params.h \
  consensus/validation.h \
  core_io.h \
  eccryptoverify.h \
  ecwrapper.h \
  hash.h \
  init.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
  memusage.h \
  merkleblock.h \
  miner.h \
  mruset.h \
  net.h \
  netbase.h \
  noui.h \
  patternsearch.h \
  policy/fees.h \
  pow.h \
  primitives/block.h \
  primitives/transaction.h \
  protocol.h \
  pubkey.h \
  random.h \
  reverselock.h \
  rpcclient.h \
  rpcprotocol.h \
  rpcserver.h \
  scheduler.h \
  script/interpreter.h \
  script/script.h \
  script/script_error.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  serialize.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/pagelocker.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utilstrencodings.h \
  utiltime.h \
  validationinterface.h \
  version.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/wallet.h \
  wallet/wallet_ismine.h \
  wallet/walletdb.h

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libhodlcoin_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between hodlcoind and hodlcoin-qt
libhodlcoin_server_a_CPPFLAGS = $(HODLCOIN_INCLUDES) $(MINIUPNPC_CPPFLAGS)
libhodlcoin_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  chain.cpp \
  checkpoints.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  patternsearch.cpp \
  policy/fees.cpp \
  pow.cpp \
  rest.cpp \
  rpcblockchain.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  script/sigcache.cpp \
  timedata.cpp \
  txdb.cpp \
  txmempool.cpp \
  validationinterface.cpp \
  $(JSON_H) \
  $(HODLCOIN_CORE_H)

# wallet: shared between hodlcoind and hodlcoin-qt, but only linked
# when wallet enabled
libhodlcoin_wallet_a_CPPFLAGS = $(HODLCOIN_INCLUDES)
libhodlcoin_wallet_a_SOURCES = \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/wallet_ismine.cpp \
  wallet/walletdb.cpp \
  $(HODLCOIN_CORE_H)

# crypto primitives library
crypto_libhodlcoin_crypto_a_CPPFLAGS = $(HODLCOIN_CONFIG_INCLUDES)
crypto_libhodlcoin_crypto_a_SOURCES = \
  crypto/common.h \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

# univalue JSON library
univalue_libhodlcoin_univalue_a_SOURCES = \
  univalue/univalue.cpp \
  univalue/univalue.h \
  univalue/univalue_escapes.h \
  univalue/univalue_read.cpp \
  univalue/univalue_write.cpp

# common: shared between hodlcoind, and hodlcoin-qt and non-server tools
libhodlcoin_common_a_CPPFLAGS = $(HODLCOIN_INCLUDES)
libhodlcoin_common_a_SOURCES = \
  amount.cpp \
  arith_uint256.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  patternsearch.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  protocol.cpp \
  pubkey.cpp \
  scheduler.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/script_error.cpp \
  script/sign.cpp \
  script/standard.cpp \
  $(HODLCOIN_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libhodlcoin_util_a_CPPFLAGS = $(HODLCOIN_INCLUDES)
libhodlcoin_util_a_SOURCES = \
  support/pagelocker.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpcprotocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(HODLCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libhodlcoin_util_a_SOURCES += compat/glibc_compat.cpp
endif

# cli: shared between hodlcoin-cli and hodlcoin-qt
libhodlcoin_cli_a_CPPFLAGS = $(HODLCOIN_INCLUDES)
libhodlcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(HODLCOIN_CORE_H)

nodist_libhodlcoin_util_a_SOURCES = $(srcdir)/obj/build.h
#

# hodlcoind binary #
hodlcoind_SOURCES = hodlcoind.cpp
hodlcoind_CPPFLAGS = $(HODLCOIN_INCLUDES)
hodlcoind_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
hodlcoind_SOURCES += hodlcoind-res.rc
endif

hodlcoind_LDADD = \
  $(LIBHODLCOIN_SERVER) \
  $(LIBHODLCOIN_COMMON) \
  $(LIBHODLCOIN_UNIVALUE) \
  $(LIBHODLCOIN_UTIL) \
  $(LIBHODLCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

if ENABLE_WALLET
hodlcoind_LDADD += libhodlcoin_wallet.a
endif

hodlcoind_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS)
#

# hodlcoin-cli binary #
hodlcoin_cli_SOURCES = hodlcoin-cli.cpp
hodlcoin_cli_CPPFLAGS = $(HODLCOIN_INCLUDES)
hodlcoin_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
hodlcoin_cli_SOURCES += hodlcoin-cli-res.rc
endif

hodlcoin_cli_LDADD = \
  $(LIBHODLCOIN_CLI) \
  $(LIBHODLCOIN_UTIL)

hodlcoin_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS)
#

# hodlcoin-tx binary #
hodlcoin_tx_SOURCES = hodlcoin-tx.cpp
hodlcoin_tx_CPPFLAGS = $(HODLCOIN_INCLUDES)
hodlcoin_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
hodlcoin_tx_SOURCES += hodlcoin-tx-res.rc
endif

hodlcoin_tx_LDADD = \
  $(LIBHODLCOIN_UNIVALUE) \
  $(LIBHODLCOIN_COMMON) \
  $(LIBHODLCOIN_UTIL) \
  $(LIBHODLCOIN_CRYPTO) \
  $(LIBSECP256K1)

hodlcoin_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# hodlcoinconsensus library #
if BUILD_HODLCOIN_LIBS
include_HEADERS = script/hodlcoinconsensus.h
libhodlcoinconsensus_la_SOURCES = \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  eccryptoverify.cpp \
  ecwrapper.cpp \
  hash.cpp \
  primitives/transaction.cpp \
  pubkey.cpp \
  script/hodlcoinconsensus.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  uint256.cpp \
  utilstrencodings.cpp

if GLIBC_BACK_COMPAT
  libhodlcoinconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libhodlcoinconsensus_la_LDFLAGS = -no-undefined $(RELDFLAGS)
libhodlcoinconsensus_la_LIBADD = $(CRYPTO_LIBS)
libhodlcoinconsensus_la_CPPFLAGS = $(CRYPTO_CFLAGS) -I$(builddir)/obj -DBUILD_HODLCOIN_INTERNAL

endif
#

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES)
	$(AM_V_GEN) $(WINDRES) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(CXXFLAGS)  -c -o $@ $<

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(abspath $(<D) $<)

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
