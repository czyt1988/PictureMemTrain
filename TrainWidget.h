#ifndef TRAINWIDGET_H
#define TRAINWIDGET_H

#include <QWidget>

namespace Ui {
class TrainWidget;
}

class TrainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainWidget(QWidget *parent = 0);
    ~TrainWidget();

private:
    Ui::TrainWidget *ui;
};

#endif // TRAINWIDGET_H
