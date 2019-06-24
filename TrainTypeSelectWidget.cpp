#include "TrainTypeSelectWidget.h"
#include "ui_TrainTypeSelectWidget.h"

TrainTypeSelectWidget::TrainTypeSelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainTypeSelectWidget)
{
    ui->setupUi(this);
}

TrainTypeSelectWidget::~TrainTypeSelectWidget()
{
    delete ui;
}

void TrainTypeSelectWidget::enableTrainTypeSelBtn(PMT::TrainType type, bool enable)
{
    switch(type)
    {
    case PMT::TestType1:
        ui->pushButtonTest1->setEnabled(enable);
        break;
    case PMT::TestType2:
        ui->pushButtonTest2->setEnabled(enable);
        break;
    case PMT::FormalType1:
        ui->pushButtonFormal1->setEnabled(enable);
        break;
    case PMT::FormalType2:
        ui->pushButtonFormal2->setEnabled(enable);
        break;
    case PMT::FormalType3:
        ui->pushButtonFormal3->setEnabled(enable);
        break;
    default:
        break;
    }
}

void TrainTypeSelectWidget::on_pushButtonTest1_clicked()
{
    emit trainTypeSelect(PMT::TestType1);
}

void TrainTypeSelectWidget::on_pushButtonTest2_clicked()
{
    emit trainTypeSelect(PMT::TestType2);
}

void TrainTypeSelectWidget::on_pushButtonFormal1_clicked()
{
    emit trainTypeSelect(PMT::FormalType1);
}

void TrainTypeSelectWidget::on_pushButtonFormal2_clicked()
{
    emit trainTypeSelect(PMT::FormalType2);
}

void TrainTypeSelectWidget::on_pushButtonFormal3_clicked()
{
    emit trainTypeSelect(PMT::FormalType3);
}
