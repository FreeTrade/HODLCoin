// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_DEPOSITTABLEMODEL_H
#define BITCOIN_QT_DEPOSITTABLEMODEL_H

#include "bitcoinunits.h"
#include "wallet/wallet.h"
#include "clientmodel.h"

#include <QAbstractTableModel>
#include <QStringList>

typedef struct {
	CAmount nValue;
	int curHeight;
	int lockHeight;
	int releaseBlock;
	int term;
	int blocksRemaining;
	CAmount withInterest;
	CAmount matureValue;
	//int blocksSoFar;
	//double interestRatePerBlock;
	//double interestRate;
	struct tm timeinfo;
} CacheRecord;

/** UI model for the deposit table of a wallet.
 */
class DepositTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	explicit DepositTableModel(ClientModel *clientModel, QObject *parent=0);
	// explicit DepositTableModel(ClientModel *clientModel, std::vector<COutput>& termDepositInfo, QObject *parent=0);
	~DepositTableModel();

	enum ColumnIndex {
		Status = 0,
		Principal = 1,
		AccruedInterest = 2,
		AccruedValue = 3,
		OnMaturation = 4,
		TermDays = 5,
		DepositBlock = 6,
		MaturationBlock = 7,
		EstimatedDate = 8,
	};

	/** Roles to get specific information from a row.
        These are independent of column.
	 */
	enum RoleIndex {
		/** sorting */
		SortRole = Qt::UserRole,
	};

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	int update(int unit, std::vector<COutput>& termDepositInfo);

private:
	std::vector<CacheRecord> tdiCache;
	QStringList columns;

	ClientModel *clientModel;

	int unit;

	public Q_SLOTS:
	/* New transaction, or transaction changed status */
};

#endif // BITCOIN_QT_DEPOSITTABLEMODEL_H
