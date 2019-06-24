#ifndef TRAINTYPESELECTWIDGET_H
#define TRAINTYPESELECTWIDGET_H

#include <QWidget>
#include "Globals.h"
namespace Ui {
class TrainTypeSelectWidget;
}

class TrainTypeSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainTypeSelectWidget(QWidget *parent = 0);
    ~TrainTypeSelectWidget();
    void enableTrainTypeSelBtn(PMT::TrainType type,bool enable = true);
signals:
    void trainTypeSelect(PMT::TrainType type);
private slots:
    void on_pushButtonTest1_clicked();

    void on_pushButtonTest2_clicked();

    void on_pushButtonFormal1_clicked();

    void on_pushButtonFormal2_clicked();

    void on_pushButtonFormal3_clicked();

private:
    Ui::TrainTypeSelectWidget *ui;
};

#endif // TRAINTYPESELECTWIDGET_H
