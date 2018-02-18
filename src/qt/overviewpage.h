// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_OVERVIEWPAGE_H
#define BITCOIN_QT_OVERVIEWPAGE_H

#include "amount.h"
#include "primitives/transaction.h"
#include "deposittablemodel.h"

#ifdef ENABLE_WALLET
#include "wallet/wallet.h" // for COutput
#endif // ENABLE_WALLET

#include <QWidget>
#include <QSortFilterProxyModel>

class ClientModel;
class TransactionFilterProxy;
class TxViewDelegate;
class WalletModel;

namespace Ui {
class OverviewPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class DepositSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit DepositSortFilterProxyModel(QObject *parent = 0);

protected:
	// bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

/** Overview ("home") page widget */
class OverviewPage : public QWidget
{
	Q_OBJECT

public:
	explicit OverviewPage(QWidget *parent = 0);
	~OverviewPage();

	void setClientModel(ClientModel *clientModel);
	void setWalletModel(WalletModel *walletModel);
	void showOutOfSyncWarning(bool fShow);

public Q_SLOTS:
#ifdef ENABLE_WALLET
	void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
			const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance, std::vector<COutput> termDepositInfo);
#endif // ENABLE_WALLET

Q_SIGNALS:
	/** When transaction is clicked */
	void transactionClicked(const QModelIndex &index);
	/** Notify that matured coins appeared */
	void maturedCoinsNotification(int count, int unit, CAmount& amount);

private:
	Ui::OverviewPage *ui;
	ClientModel *clientModel;
	WalletModel *walletModel;
	CAmount currentBalance;
	CAmount currentUnconfirmedBalance;
	CAmount currentImmatureBalance;
	CAmount currentWatchOnlyBalance;
	CAmount currentWatchUnconfBalance;
	CAmount currentWatchImmatureBalance;

	TxViewDelegate *txdelegate;
	TransactionFilterProxy *filter;

	DepositSortFilterProxyModel *depositProxyModel;
	DepositTableModel *depositModel;

private Q_SLOTS:
	void updateDisplayUnit();
	void handleTransactionClicked(const QModelIndex &index);
	void updateAlerts(const QString &warnings);
	void updateWatchOnlyLabels(bool showWatchOnly);
};

#endif // BITCOIN_QT_OVERVIEWPAGE_H
