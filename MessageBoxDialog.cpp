#include "MessageBoxDialog.h"
#include "ui_MessageBoxDialog.h"
#include "FramelessHelper.h"
MessageBoxDialog::MessageBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBoxDialog)
{
    ui->setupUi(this);
    //FramelessHelper* p = new FramelessHelper(this);
    connect(ui->pushButtonOK,&QPushButton::clicked,this,&QDialog::accept);
    connect(ui->pushButtonCancel,&QPushButton::clicked,this,&QDialog::reject);
}

MessageBoxDialog::~MessageBoxDialog()
{
    delete ui;
}

void MessageBoxDialog::setLabel(const QString &str)
{
    ui->label->setText(str);
}


