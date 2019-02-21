#include "LoginErrorDialog.h"
#include "ui_LoginErrorDialog.h"

LoginErrorDialog::LoginErrorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginErrorDialog)
{
    ui->setupUi(this);
}

LoginErrorDialog::~LoginErrorDialog()
{
    delete ui;
}

void LoginErrorDialog::on_LoginErrorBtn_clicked()
{
    this->close();
}
