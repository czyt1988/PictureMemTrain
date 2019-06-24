#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Globals.h"

class LoginWidget;
class TrainTypeSelectWidget;
class TrainWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void initConnection();
    void initStyle();
    void initWidget();
    void showLoginPage();
    void showTrainSelPage();
private slots:
    Q_SLOT void onPushBottonQuitClicked();
    Q_SLOT void onPushBottonBackClicked();
    Q_SLOT void onLoginWidgetOK();
    Q_SLOT void onTrainTypeSelected(PMT::TrainType type);
    Q_SLOT void onShowTooltip(const QString &message, int timeout = 0);
    Q_SLOT void onTrainFinish1(PMT::TrainType trainType);
private:
    Ui::MainWindow *ui;
    LoginWidget* m_loginWidget;
    TrainTypeSelectWidget* m_trainSelWidget;
    TrainWidget* m_trainWidget;
};

#endif // MAINWINDOW_H
