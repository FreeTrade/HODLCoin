HOdlcoin Core version 3.0.0 is now available from:

  <https://github.com/HOdlcoin/HOdlcoin/releases/>

This is a new major version release, bringing bug fixes, revised block generation
requirements, and is the second hard-fork and a required upgrade for all clients!

Please report bugs using the issue tracker at github:

  <https://github.com/HOdlcoin/HOdlcoin/issues>

Upgrading and downgrading
=========================

How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/HOdlcoin-Qt (on Mac) or
hodlcoind/hodlcoin-qt (on Linux).

Downgrade warning
------------------

Because this release issues a hard-fork of the network, downgrading to prior versions
will not be possible once the network reaches a block height of 257000.

Notable changes since 2.0.0
============================

Hard fork for revised block generation requirements
--------------------------------------------------------

Revised requirements for block generation have been introduced, effectively
reverting the requirement for generated blocks to automatically enter into a
term deposit for a period of one year.

1. This release will no longer automatically generate a term deposit for a one
   year period for all generated blocks after the fork.

2. It is still possible, though not required to generate a term deposit in the
   coinbase transaction, and no enforcement will be in effect after the fork.

3. Clients running versions prior to 3.0.0 will be forced off the network in
   order to reduce consensus conflicts between client versions.


**Notice to miners:** HOdlcoin block generation will no longer automatically
create a term deposit for a one year length for the generated block.
This affects ALL newly generated blocks after a chain height of 257000.

- If you are solo mining, this will affect you the moment you upgrade
  HOdlcoin Core, which must be done prior to the network reaching a
  height of 257000.

- If you are mining with the stratum mining protocol: this does not
  affect how you mine, however stratum operators will need to adjust
  their payout policies.

- If you are mining with the getblocktemplate protocol to a pool: this
  will affect you at the pool operator’s discretion, adjusting their
  payout policies to accommodate the revised block generation requirements.

Optiminer improved internal mining
----------------------------------------------------------------

The internal mining functions have been ported to use a lot of Optiminer's
improvements, resulting in a much faster hashrate for most systems. The hashing
algorithm still requires 1GB of memory per miner and still depends heavily on
AES hardware support, but new AVX2 optimizations have been introduced as well.

Systems that support both AES and AVX2 natively on-chip will see the greatest
performance improvement, however CPUs that only support AES will also see some
improvement.

**NOTE:** Windows systems with certain CPUs have shown issues when using the AVX2
optimizations. As such, option flags have been added to disable running with both
the AES and AVX2 optimizations, and can be used as follows:

- `-optimineraes` - Default is 1 (enabled use of Optiminer AES improvements)
- `-optimineravx2` - Default is 0 (disabled use of Optimer AVX2 improvements)

3.0.0 Change log
=================

Detailed release notes follow. This overview includes changes that affect
behavior, not code moves, refactors and string updates. For convenience in locating
the code changes and accompanying discussion, both the pull request and
git merge commit are mentioned.

* [6c78cf6] #87 [Qt] Added sorting to hodlTable in overview page
* [80e5aa1] #86 [Consensus] Fork at 257000 per Unforkening Nutocracy motion
* [56b2cd8] #84 [Miner] Optiminer's miner improvements
* [b7ff808] #81 [Gitian] Updates for using trusy with gitian builds
* [96ed62b] #80 [depends] bump OSX toolchain
* [c344fc2] #78 [RPC] Guard getblocktemplate against no-wallet builds
* [feeebb1] #68 [Mining] Teach getblocktemplate about the post-fork requirements.
* [c8e7b59] #77 [Travis] Start using Trusty environment for Travis builds.
* [de18d4d] #74 [Qt] Guard against missing BDB when compiling without wallet support
* [de52e20] #71 [Qt] Add autocomplete to hodlcoin-qt's console window.
* [b6dc155] #69 [Miner] Miner improvements

Credits
=======

Thanks to everyone who directly contributed to this release:

- FreeTrade
- Fuzzbawls
- Optiminer
- ghobson2013
- madzera
- optiminer
- pallas1

And those who contributed additional code review and/or security research, as well as all pool operators and service providers.
