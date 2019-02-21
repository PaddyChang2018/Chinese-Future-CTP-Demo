#ifndef LOGINERRORDIALOG_H
#define LOGINERRORDIALOG_H

#include <QDialog>

namespace Ui {
class LoginErrorDialog;
}

class LoginErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginErrorDialog(QWidget *parent = 0);
    ~LoginErrorDialog();

private slots:
    void on_LoginErrorBtn_clicked();

private:
    Ui::LoginErrorDialog *ui;
};

#endif // LOGINERRORDIALOG_H
