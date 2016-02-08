Regression tests of RPC interface
=================================

### [python-hodlcoinrpc](https://github.com/jgarzik/python-hodlcoinrpc)
Git subtree of [https://github.com/jgarzik/python-hodlcoinrpc](https://github.com/jgarzik/python-hodlcoinrpc).
Changes to python-hodlcoinrpc should be made upstream, and then
pulled here using git subtree.

### [test_framework/test_framework.py](test_framework/test_framework.py)
Base class for new regression tests.

### [test_framework/util.py](test_framework/util.py)
Generally useful functions.

Notes
=====

You can run a single test by calling `qa/pull-tester/rpc-tests.sh <testname>`.

Run all possible tests with `qa/pull-tester/rpc-tests.sh -extended`.

Possible options:

````
-h, --help       show this help message and exit
  --nocleanup      Leave hodlcoinds and test.* datadir on exit or error
  --noshutdown     Don't stop hodlcoinds after the test execution
  --srcdir=SRCDIR  Source directory containing hodlcoind/hodlcoin-cli (default:
                   ../../src)
  --tmpdir=TMPDIR  Root directory for datadirs
  --tracerpc       Print out all RPC calls as they are made
```

If you set the environment variable `PYTHON_DEBUG=1` you will get some debug output (example: `PYTHON_DEBUG=1 qa/pull-tester/rpc-tests.sh wallet`). 

A 200-block -regtest blockchain and wallets for four nodes
is created the first time a regression test is run and
is stored in the cache/ directory. Each node has 25 mature
blocks (25*50=1250 BTC) in its wallet.

After the first run, the cache/ blockchain and wallets are
copied into a temporary directory and used as the initial
test state.

If you get into a bad state, you should be able
to recover with:

```bash
rm -rf cache
killall hodlcoind
```
