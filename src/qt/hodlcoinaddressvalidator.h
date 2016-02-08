// Copyright (c) 2011-2014 The HODLCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HODLCOIN_QT_HODLCOINADDRESSVALIDATOR_H
#define HODLCOIN_QT_HODLCOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class HODLCoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HODLCoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** HODLCoin address widget validator, checks for a valid hodlcoin address.
 */
class HODLCoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HODLCoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // HODLCOIN_QT_HODLCOINADDRESSVALIDATOR_H
