// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The HODLCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HODLCOIN_ECCRYPTOVERIFY_H
#define HODLCOIN_ECCRYPTOVERIFY_H

#include <vector>
#include <cstdlib>

class uint256;

namespace eccrypto {

bool Check(const unsigned char *vch);
bool CheckSignatureElement(const unsigned char *vch, int len, bool half);

} // eccrypto namespace

#endif // HODLCOIN_ECCRYPTOVERIFY_H
