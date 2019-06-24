#include "LoginWidget.h"
#include "ui_LoginWidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

QString LoginWidget::getMatchingNum() const
{
    return ui->lineEditMatchingNum->text();
}

QString LoginWidget::getExpNum() const
{
    return ui->lineEditExpNum->text();
}

QString LoginWidget::getAge() const
{
    return ui->lineEditAge->text();
}

QString LoginWidget::getShortName() const
{
    return ui->lineEditShortName->text();
}

void LoginWidget::on_pushButtonOK_clicked()
{
    emit ok();
}
