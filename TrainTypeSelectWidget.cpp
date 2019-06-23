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
