// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "deposittablemodel.h"

#include "addresstablemodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "scicon.h"
#include "transactiondesc.h"
#include "transactionrecord.h"
#include "walletmodel.h"

#include "main.h"
#include "sync.h"
#include "uint256.h"
#include "util.h"
#include "wallet/wallet.h"

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QList>

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
		Qt::AlignLeft|Qt::AlignVCenter, /* status */
		Qt::AlignRight|Qt::AlignVCenter, /* Principal */
		Qt::AlignRight|Qt::AlignVCenter, /* Interest */
		Qt::AlignRight|Qt::AlignVCenter, /* Value */
		Qt::AlignRight|Qt::AlignVCenter, /* Maturation */
		Qt::AlignRight|Qt::AlignVCenter, /* Days */
		Qt::AlignRight|Qt::AlignVCenter, /* deposit block */
		Qt::AlignRight|Qt::AlignVCenter, /* maturation */
		Qt::AlignCenter|Qt::AlignVCenter,  /* Date */
};

DepositTableModel::DepositTableModel(ClientModel *clientModel, QObject *parent)
: QAbstractTableModel(parent)
{
	this->clientModel = clientModel;

	this->columns << tr("Status") << tr("Principal") << tr("Accrued Interest") << tr("Accrued Value")
								<< tr("On Maturation") << tr("Term (Days)") << tr("Deposit Block") << tr("Maturation Block")
								<< tr("Estimated Date");

}

static bool compareByDepth(COutput& a, COutput& b) {
	return b.nDepth < a.nDepth;
}

int DepositTableModel::update(int unit, std::vector<COutput>& termDepositInfo)
{

	this->unit = unit;

	clock_t t1 = clock();

	this->beginResetModel();
	clock_t t2 = clock();

	qSort(termDepositInfo.begin(), termDepositInfo.end(), compareByDepth);
	clock_t t3 = clock();

	this->tdiCache.clear();
	this->tdiCache.reserve(termDepositInfo.size());
	for (int i=0, ii=termDepositInfo.size(); i<ii; i++) {

		COutput ctermDeposit = termDepositInfo.at(i);
		CTxOut termDeposit = ctermDeposit.tx->vout[ctermDeposit.i];

		// create new record with empty constructor
		tdiCache.push_back(CacheRecord());

		tdiCache[i].nValue = termDeposit.nValue;
		tdiCache[i].curHeight = this->clientModel->getNumBlocks();
		tdiCache[i].lockHeight = tdiCache[i].curHeight-ctermDeposit.nDepth;
		tdiCache[i].releaseBlock = termDeposit.scriptPubKey.GetTermDepositReleaseBlock();
		tdiCache[i].term = tdiCache[i].releaseBlock - tdiCache[i].lockHeight;
		tdiCache[i].blocksRemaining = tdiCache[i].releaseBlock - tdiCache[i].curHeight;
		tdiCache[i].withInterest = termDeposit.GetValueWithInterest(tdiCache[i].lockHeight,
				(tdiCache[i].curHeight<tdiCache[i].releaseBlock?tdiCache[i].curHeight:tdiCache[i].releaseBlock));
		tdiCache[i].matureValue = termDeposit.GetValueWithInterest(tdiCache[i].lockHeight,tdiCache[i].releaseBlock);
		//tdiCache[i].blocksSoFar = tdiCache[i].curHeight - tdiCache[i].lockHeight;
		//termDepositInfoCache[i].interestRatePerBlock = pow(((0.0+termDepositInfoCache[i].matureValue)/termDepositInfoCache[i].nValue),1.0/termDepositInfoCache[i].term);
		//termDepositInfoCache[i].interestRate = (pow(termDepositInfoCache[i].interestRatePerBlock,365*720)-1)*100;

		time_t rawtime;
		time(&rawtime);
		rawtime += tdiCache[i].blocksRemaining * 154;
		memcpy(&tdiCache[i].timeinfo,localtime(&rawtime),sizeof(struct tm));

	}
	clock_t t4 = clock();

	this->endResetModel();
	clock_t t5 = clock();

	LogPrintf("list of deposits updated, count=%d beginReset=%f qSort=%f cache=%f endReset=%f\n",
			tdiCache.size(),
			((double)t2 - (double)t1) / CLOCKS_PER_SEC,
			((double)t3 - (double)t2) / CLOCKS_PER_SEC,
			((double)t4 - (double)t3) / CLOCKS_PER_SEC,
			((double)t5 - (double)t4) / CLOCKS_PER_SEC
	);

	return tdiCache.size();
}

DepositTableModel::~DepositTableModel()
{
}

int DepositTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return tdiCache.size();
}

int DepositTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return columns.length();
}

QVariant DepositTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= tdiCache.size() || index.row() < 0)
		return QVariant();

	int nDisplayUnit = this->unit;

	if (role == Qt::TextAlignmentRole)
	{
		return column_alignments[index.column()];
	}
	else if (role == Qt::DisplayRole || role == SortRole || role == Qt::ToolTipRole)
	{
		CacheRecord cache = tdiCache.at(index.row());

		switch(index.column()) {
		case Status:
			if (cache.curHeight >= cache.releaseBlock) {
				if (role == Qt::DisplayRole || role == SortRole)
					return tr("Matured");
				else if (role == Qt::ToolTipRole) {
					return tr("Warning: this amount is no longer earning interest of any kind");
				}
			} else {
				if (role == Qt::DisplayRole || role == SortRole)
					return tr("Earned");
			}
			break;
		case Principal:
			if (role == SortRole)
				return qint64(cache.nValue);
			else if (role == Qt::DisplayRole)
				return BitcoinUnits::format(nDisplayUnit, cache.nValue);
			break;
		case AccruedInterest:
			if (role == SortRole)
				return qint64(cache.withInterest-cache.nValue);
			else if (role == Qt::DisplayRole)
				return BitcoinUnits::format(nDisplayUnit, cache.withInterest-cache.nValue);
			break;
		case AccruedValue:
			if (role == SortRole)
				return qint64(cache.withInterest);
			else if (role == Qt::DisplayRole)
				return BitcoinUnits::format(nDisplayUnit, cache.withInterest);
			break;
		case OnMaturation:
			if (role == SortRole)
				return qint64(cache.matureValue);
			else if (role == Qt::DisplayRole)
				return BitcoinUnits::format(nDisplayUnit, cache.matureValue);
			break;
		case TermDays:
			if (role == SortRole)
				return (cache.term)/720;
			else if (role == Qt::DisplayRole)
				return QString::number((cache.term)/720);
			break;
		case DepositBlock:
			if (role == SortRole)
				return cache.lockHeight;
			else if (role == Qt::DisplayRole)
				return QString::number(cache.lockHeight); // .rightJustified(7,'0');
			break;
		case MaturationBlock:
			if (role == SortRole)
				return cache.releaseBlock;
			else if (role == Qt::DisplayRole)
				return QString::number(cache.releaseBlock); // .rightJustified(7,'0');
			break;
		case EstimatedDate:

			char buffer[16];
			strftime(buffer, 16, "%Y/%m/%d", &cache.timeinfo);

			if (role == SortRole)
				return qint64(mktime(&cache.timeinfo));
			else if (role == Qt::DisplayRole)
				return QString(buffer);
			else if (role == Qt::ToolTipRole) {
				if (cache.curHeight >= cache.releaseBlock) {
					int days = (int)(-cache.blocksRemaining/720);
					switch (days) {
					case 0:
						return tr("Matured today");
					case 1:
						return tr("Matured yesterday");
					default:
						return tr("Matured %1 days ago").arg(days);
					}
				} else {
					int days = (int)(cache.blocksRemaining/720);
					switch (days) {
					case 0:
						return tr("Matures today");
					case 1:
						return tr("Matures tomorrow");
					default:
						return tr("%1 days to go").arg(days);
					}
				}
			}

			break;
		}
	}

	return QVariant();
}

QVariant DepositTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
	{
		if (role == Qt::DisplayRole)
		{
			return columns[section];
		}
		else if (role == Qt::TextAlignmentRole)
		{
			return column_alignments[section];
		}
		else if (role == Qt::ToolTipRole)
		{
			switch(section)
			{

			case 0:
				return tr("Deposit Status");
			case 1:
				return tr("Principal");
			case 2:
				return tr("Accrued Interest");
			case 3:
				return tr("Accrued Value");
			case 4:
				return tr("Value On Maturation");
			case 5:
				return tr("Term (Days)");
			case 6:
				return tr("Deposit Block");
			case 7:
				return tr("Maturation Block");
			case 8:
				return tr("Estimated Date");
			}
		}
	}

	// return QVariant();
	return QAbstractTableModel::headerData(section, orientation, role);
}

