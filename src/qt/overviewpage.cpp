// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "scicon.h"
#include "transactionfilterproxy.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"
#include "wallet/wallet.h"
#include "util.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#define DECORATION_SIZE 54
#define NUM_ITEMS 5

class TxViewDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	TxViewDelegate(): QAbstractItemDelegate(), unit(BitcoinUnits::HODL)
	{

	}

	inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
			const QModelIndex &index ) const
	{
		painter->save();

		QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
		QRect mainRect = option.rect;
		QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
		int xspace = DECORATION_SIZE + 8;
		int ypad = 6;
		int halfheight = (mainRect.height() - 2*ypad)/2;
		QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace, halfheight);
		QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
		icon = SingleColorIcon(icon, SingleColor());
		icon.paint(painter, decorationRect);

		QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
		QString address = index.data(Qt::DisplayRole).toString();
		qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
		bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
		QVariant value = index.data(Qt::ForegroundRole);
		QColor foreground = option.palette.color(QPalette::Text);
		if(value.canConvert<QBrush>())
		{
			QBrush brush = qvariant_cast<QBrush>(value);
			foreground = brush.color();
		}

		painter->setPen(foreground);
		QRect boundingRect;
		painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address, &boundingRect);

		if (index.data(TransactionTableModel::WatchonlyRole).toBool())
		{
			QIcon iconWatchonly = qvariant_cast<QIcon>(index.data(TransactionTableModel::WatchonlyDecorationRole));
			QRect watchonlyRect(boundingRect.right() + 5, mainRect.top()+ypad+halfheight, 16, halfheight);
			iconWatchonly.paint(painter, watchonlyRect);
		}

		if(amount < 0)
		{
			foreground = COLOR_NEGATIVE;
		}
		else if(!confirmed)
		{
			foreground = COLOR_UNCONFIRMED;
		}
		else
		{
			foreground = option.palette.color(QPalette::Text);
		}
		painter->setPen(foreground);
		QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true, BitcoinUnits::separatorAlways);
		if(!confirmed)
		{
			amountText = QString("[") + amountText + QString("]");
		}
		painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

		painter->setPen(option.palette.color(QPalette::Text));
		painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

		painter->restore();
	}

	inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return QSize(DECORATION_SIZE, DECORATION_SIZE);
	}

	int unit;

};

DepositSortFilterProxyModel::DepositSortFilterProxyModel(QObject *parent)
: QSortFilterProxyModel(parent) {
	this->setSortRole( DepositTableModel::SortRole );
}

#include "overviewpage.moc"

OverviewPage::OverviewPage(QWidget *parent) :
    		QWidget(parent),
			ui(new Ui::OverviewPage),
			clientModel(0),
			walletModel(0),
			currentBalance(-1),
			currentUnconfirmedBalance(-1),
			currentImmatureBalance(-1),
			currentWatchOnlyBalance(-1),
			currentWatchUnconfBalance(-1),
			currentWatchImmatureBalance(-1),
			txdelegate(new TxViewDelegate()),
			filter(0)
{
	ui->setupUi(this);

	ui->hodlTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->hodlTable->setAlternatingRowColors(true);
	ui->hodlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->hodlTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->hodlTable->verticalHeader()->show(); // index column
	ui->hodlTable->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	// ui->hodlTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	depositModel = new DepositTableModel(this->clientModel, this);
	depositProxyModel = new DepositSortFilterProxyModel(this);
	depositProxyModel->setSourceModel(depositModel);
	ui->hodlTable->setModel(depositProxyModel);

	// Fetch sort flag
	bool sortflag = GetArg("-sortearnings", true);
	LogPrintf("sort earnings flag = %b\n", sortflag);

	ui->hodlTable->setSortingEnabled(sortflag);

	// use a SingleColorIcon for the "out of sync warning" icon
	QIcon icon = SingleColorIcon(":/icons/warning");
	icon.addPixmap(icon.pixmap(QSize(64,64), QIcon::Normal), QIcon::Disabled); // also set the disabled icon because we are using a disabled QPushButton to work around missing HiDPI support of QLabel (https://bugreports.qt.io/browse/QTBUG-42503)
	ui->labelTransactionsStatus->setIcon(icon);
	ui->labelWalletStatus->setIcon(icon);

	// Recent transactions
	ui->listTransactions->setItemDelegate(txdelegate);
	ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
	ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
	ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));

	// start with displaying the "out of sync" warnings
	showOutOfSyncWarning(true);
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
	if(filter)
		Q_EMIT transactionClicked(filter->mapToSource(index));
}

OverviewPage::~OverviewPage()
{
	delete ui;
}

void OverviewPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance, std::vector<COutput> termDepositInfo)
{
	int unit = walletModel->getOptionsModel()->getDisplayUnit();
	currentBalance = balance;
	currentUnconfirmedBalance = unconfirmedBalance;
	currentImmatureBalance = immatureBalance;
	currentWatchOnlyBalance = watchOnlyBalance;
	currentWatchUnconfBalance = watchUnconfBalance;
	currentWatchImmatureBalance = watchImmatureBalance;
	ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balance, false, BitcoinUnits::separatorAlways));
	ui->labelUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, unconfirmedBalance, false, BitcoinUnits::separatorAlways));
	ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, immatureBalance, false, BitcoinUnits::separatorAlways));
	ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balance + unconfirmedBalance + immatureBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchAvailable->setText(BitcoinUnits::formatWithUnit(unit, watchOnlyBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchPending->setText(BitcoinUnits::formatWithUnit(unit, watchUnconfBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchImmature->setText(BitcoinUnits::formatWithUnit(unit, watchImmatureBalance, false, BitcoinUnits::separatorAlways));
	ui->labelWatchTotal->setText(BitcoinUnits::formatWithUnit(unit, watchOnlyBalance + watchUnconfBalance + watchImmatureBalance, false, BitcoinUnits::separatorAlways));

	// only show immature (newly mined) balance if it's non-zero, so as not to complicate things
	// for the non-mining users
	bool showImmature = immatureBalance != 0;
	bool showWatchOnlyImmature = watchImmatureBalance != 0;

	// for symmetry reasons also show immature label when the watch-only one is shown
	ui->labelImmature->setVisible(showImmature || showWatchOnlyImmature);
	ui->labelImmatureText->setVisible(showImmature || showWatchOnlyImmature);
	ui->labelWatchImmature->setVisible(showWatchOnlyImmature); // show watch-only immature balance

	bool sortflag = GetArg("-sortearnings", true);
	bool sortOrder = true;
	int sortSection = -1;
	if (sortflag) {
		// save sort direction and sort order
		sortOrder = ui->hodlTable->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder;
		sortSection = ui->hodlTable->horizontalHeader()->sortIndicatorSection();
	}

	depositModel->update(unit, termDepositInfo);

	if (sortflag && sortSection >= 0) {
		// restore sort order and direction
		ui->hodlTable->sortByColumn(sortSection, sortOrder ? Qt::AscendingOrder : Qt::DescendingOrder);
	}

	// calculation of sums
	uint64_t totalLocked  = 0;
	uint64_t totalAccrued = 0;
	uint64_t totalMatured = 0;

	int maturedNowCount = 0;
	CAmount maturedNowAmount = 0;

	for(int i=0;i<termDepositInfo.size();i++){
		COutput ctermDeposit = termDepositInfo[i];
		CTxOut termDeposit = ctermDeposit.tx->vout[ctermDeposit.i];
		int curHeight = this->clientModel->getNumBlocks();
		int lockHeight = curHeight - ctermDeposit.nDepth;
		int releaseBlock = termDeposit.scriptPubKey.GetTermDepositReleaseBlock();
		// int term = releaseBlock - lockHeight;
		// int blocksRemaining = releaseBlock - curHeight;
		CAmount withInterest = termDeposit.GetValueWithInterest(lockHeight,(curHeight<releaseBlock?curHeight:releaseBlock));
		CAmount matureValue = termDeposit.GetValueWithInterest(lockHeight,releaseBlock);
		// int blocksSoFar = curHeight-lockHeight;

		//double interestRatePerBlock = pow(((0.0+matureValue)/termDeposit.nValue),1.0/term);
		//double interestRate = (pow(interestRatePerBlock,365*720)-1)*100;

		if (curHeight == releaseBlock) {
			maturedNowCount++;
			maturedNowAmount += matureValue;
		}

		if (curHeight >= releaseBlock) {
			totalMatured += matureValue;
		} else {
			totalAccrued += (withInterest - termDeposit.nValue);
			totalLocked  += termDeposit.nValue;
		}

	}

	ui->labellocked->setText(BitcoinUnits::formatWithUnit(unit, totalLocked, false, BitcoinUnits::separatorAlways));
	ui->labelaccrued->setText(BitcoinUnits::formatWithUnit(unit, totalAccrued, false, BitcoinUnits::separatorAlways));
	ui->labelMatured->setText(BitcoinUnits::formatWithUnit(unit, totalMatured, false, BitcoinUnits::separatorAlways));

	if (maturedNowCount > 0) {
		Q_EMIT maturedCoinsNotification(maturedNowCount, unit, maturedNowAmount);
	}

}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
	ui->labelSpendable->setVisible(showWatchOnly);      // show spendable label (only when watch-only is active)
	ui->labelWatchonly->setVisible(showWatchOnly);      // show watch-only label
	ui->lineWatchBalance->setVisible(showWatchOnly);    // show watch-only balance separator line
	ui->labelWatchAvailable->setVisible(showWatchOnly); // show watch-only available balance
	ui->labelWatchPending->setVisible(showWatchOnly);   // show watch-only pending balance
	ui->labelWatchTotal->setVisible(showWatchOnly);     // show watch-only total balance

	if (!showWatchOnly)
		ui->labelWatchImmature->hide();
}

void OverviewPage::setClientModel(ClientModel *model)
{
	this->clientModel = model;
	if(model)
	{
		// Show warning if this is a prerelease version
		connect(model, SIGNAL(alertsChanged(QString)), this, SLOT(updateAlerts(QString)));
		updateAlerts(model->getStatusBarWarnings());
	}
}

void OverviewPage::setWalletModel(WalletModel *model)
{
	this->walletModel = model;
	if(model && model->getOptionsModel())
	{
		// Set up transaction list
		filter = new TransactionFilterProxy();
		filter->setSourceModel(model->getTransactionTableModel());
		filter->setLimit(NUM_ITEMS);
		filter->setDynamicSortFilter(true);
		filter->setSortRole(Qt::EditRole);
		filter->setShowInactive(false);
		filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

		ui->listTransactions->setModel(filter);
		ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

		// Keep up to date with wallet
		setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance(),model->getWatchBalance(), model->getWatchUnconfirmedBalance(), model->getWatchImmatureBalance(),model->GetTermDepositInfo());
		connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,std::vector<COutput>)), this, SLOT(setBalance(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,std::vector<COutput>)));

		connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

		updateWatchOnlyLabels(model->haveWatchOnly());
		connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyLabels(bool)));
	}

	// update the display unit, to not use the default ("HODL")
	updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
	if(walletModel && walletModel->getOptionsModel())
	{
		if(currentBalance != -1)
			setBalance(currentBalance, currentUnconfirmedBalance, currentImmatureBalance,currentWatchOnlyBalance, currentWatchUnconfBalance, currentWatchImmatureBalance, walletModel->GetTermDepositInfo());

		// Update txdelegate->unit with the current unit
		txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();

		ui->listTransactions->update();
	}
}

void OverviewPage::updateAlerts(const QString &warnings)
{
	this->ui->labelAlerts->setVisible(!warnings.isEmpty());
	this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
	ui->labelWalletStatus->setVisible(fShow);
	ui->labelTransactionsStatus->setVisible(fShow);
}
