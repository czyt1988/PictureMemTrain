#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FramelessHelper.h"
#include "MessageBoxDialog.h"
#include "LoginWidget.h"
#include "TrainTypeSelectWidget.h"
#include "TrainWidget.h"
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FramelessHelper *pHelper = new FramelessHelper(this);
    pHelper->setTitleHeight(25);  //设置窗体的标题栏高度
    initStyle();
    initWidget();
    initConnection();

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief 初始化连接
 */
void MainWindow::initConnection()
{
    connect(ui->pushButtonQuit,&QPushButton::clicked,this,&MainWindow::onPushBottonQuitClicked);
    connect(ui->pushButtonBack,&QPushButton::clicked,this,&MainWindow::onPushBottonBackClicked);
    connect(m_loginWidget,&LoginWidget::ok,this,&MainWindow::onLoginWidgetOK);
    connect(m_trainSelWidget,&TrainTypeSelectWidget::trainTypeSelect,this,&MainWindow::onTrainTypeSelected);
}

void MainWindow::initStyle()
{
    QFont f(QStringLiteral("微软雅黑"),12);
    qApp->setFont(f);
}

void MainWindow::initWidget()
{
    ui->labelTitle->setText(tr("图片记忆训练"));
    m_loginWidget = new LoginWidget(ui->stackedWidget);
    ui->stackedWidget->addWidget(m_loginWidget);
    m_trainSelWidget = new TrainTypeSelectWidget(ui->stackedWidget);
    ui->stackedWidget->addWidget(m_trainSelWidget);
    m_trainWidget = new TrainWidget(ui->stackedWidget);
    connect(m_trainWidget,&TrainWidget::showTooltip,this,&MainWindow::onShowTooltip);
    connect(m_trainWidget,&TrainWidget::finish,this,&MainWindow::onTrainFinish1);
    ui->stackedWidget->addWidget(m_trainWidget);
    ui->stackedWidget->setCurrentWidget(m_loginWidget);
    ui->pushButtonBack->hide();
}

void MainWindow::showLoginPage()
{
    ui->stackedWidget->setCurrentWidget(m_loginWidget);
    ui->pushButtonBack->hide();
}

void MainWindow::showTrainSelPage()
{
    ui->pushButtonBack->show();
    ui->stackedWidget->setCurrentWidget(m_trainSelWidget);
}

void MainWindow::onPushBottonQuitClicked()
{
    MessageBoxDialog dlg;
    dlg.setWindowTitle(tr("询问"));
    dlg.setLabel(tr("是否退出程序？"));
    if(QDialog::Accepted == dlg.exec())
        qApp->quit();
}

void MainWindow::onPushBottonBackClicked()
{
    QWidget* cw = ui->stackedWidget->currentWidget();
    if(cw == m_trainWidget)
    {
        showTrainSelPage();
    }
    else if(cw == m_trainSelWidget)
    {
        showLoginPage();
    }
}

void MainWindow::onLoginWidgetOK()
{
    QString v = m_loginWidget->getMatchingNum();
    m_trainWidget->setAge( m_loginWidget->getAge() );
    m_trainWidget->setExpNum(m_loginWidget->getExpNum());
    m_trainWidget->setShortName(m_loginWidget->getShortName());
    m_trainWidget->setMatchingNum(v);
    if(!v.isEmpty())
    {
        QString err;
        MemRecord mr;
        if(!mr.load(QApplication::applicationDirPath() + QDir::separator() + "match/match.xlsx",v,&err))
        {
            QMessageBox::critical(this,tr("错误"),tr("匹配excel异常，异常原因：%1").arg(err));
            return;
        }
        m_recordDatas = mr.getRecordDatas();
    }
    else {
        m_recordDatas.clear();
    }
    showTrainSelPage();
}

void MainWindow::onTrainTypeSelected(PMT::TrainType type)
{
    switch(type)
    {
    case PMT::FormalType1:
        m_trainWidget->setTrainType(type,(m_recordDatas.size() != 0) ? m_recordDatas[0] : MemRecordData());
        break;
    case PMT::FormalType2:
        m_trainWidget->setTrainType(type,(m_recordDatas.size() != 0) ? m_recordDatas[1] : MemRecordData());
        break;
    case PMT::FormalType3:
        m_trainWidget->setTrainType(type,(m_recordDatas.size() != 0) ? m_recordDatas[2] : MemRecordData());
        break;
    default:
        m_trainWidget->setTrainType(type);
        break;
    }
    ui->stackedWidget->setCurrentWidget(m_trainWidget);
}

void MainWindow::onShowTooltip(const QString &message, int timeout)
{
    statusBar()->showMessage(message,timeout);
}

void MainWindow::onTrainFinish1(PMT::TrainType trainType)
{
    m_trainSelWidget->enableTrainTypeSelBtn(trainType,false);
    showTrainSelPage();
}
