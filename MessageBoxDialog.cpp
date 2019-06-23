#include "MessageBoxDialog.h"
#include "ui_MessageBoxDialog.h"

MessageBoxDialog::MessageBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBoxDialog)
{
    ui->setupUi(this);
}

MessageBoxDialog::~MessageBoxDialog()
{
    delete ui;
}
