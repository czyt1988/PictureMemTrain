#include "TrainWidget.h"
#include "ui_TrainWidget.h"

TrainWidget::TrainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainWidget)
{
    ui->setupUi(this);
}

TrainWidget::~TrainWidget()
{
    delete ui;
}
