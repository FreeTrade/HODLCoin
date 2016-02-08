// Copyright (c) 2011-2014 The HODLCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HODLCOIN_QT_UTILITYDIALOG_H
#define HODLCOIN_QT_UTILITYDIALOG_H

#include <QDialog>
#include <QObject>

class HODLCoinGUI;
class ClientModel;

namespace Ui {
    class HelpMessageDialog;
}

/** "Help message" dialog box */
class HelpMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpMessageDialog(QWidget *parent, bool about);
    ~HelpMessageDialog();

    void printToConsole();
    void showOrPrint();

private:
    Ui::HelpMessageDialog *ui;
    QString text;

private Q_SLOTS:
    void on_okButton_accepted();
};


/** "Shutdown" window */
class ShutdownWindow : public QWidget
{
    Q_OBJECT

public:
    ShutdownWindow(QWidget *parent=0, Qt::WindowFlags f=0);
    static void showShutdownWindow(HODLCoinGUI *window);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // HODLCOIN_QT_UTILITYDIALOG_H
