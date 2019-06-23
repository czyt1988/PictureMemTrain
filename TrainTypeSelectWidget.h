#ifndef TRAINTYPESELECTWIDGET_H
#define TRAINTYPESELECTWIDGET_H

#include <QWidget>

namespace Ui {
class TrainTypeSelectWidget;
}

class TrainTypeSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainTypeSelectWidget(QWidget *parent = 0);
    ~TrainTypeSelectWidget();

private:
    Ui::TrainTypeSelectWidget *ui;
};

#endif // TRAINTYPESELECTWIDGET_H
