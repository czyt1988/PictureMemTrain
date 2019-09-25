#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FramelessHelper.h"
#include "MessageBoxDialog.h"
#include "LoginWidget.h"
#include "TrainTypeSelectWidget.h"
#include "TrainWidget.h"
#include "TrainController.h"
#include <QDir>
#include <QSettings>
#include <QDebug>
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
    showMaximized();
    QSettings settings("PictureMemTrain.ini", QSettings::IniFormat);
    bool isShowTitle = settings.value("ui/isShowTitle",false).toBool();
    if(isShowTitle)
    {
        QString t = settings.value("ui/appTitle",QStringLiteral("记忆测试")).toString();
        ui->labelTitle->setText(t);
    }
    else {
        ui->labelTitle->setText("");
    }
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
/**
 * @brief 生成测试的记录集
 */
void MainWindow::makeTestMemRecord()
{

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
    m_trainWidget->setGender(m_loginWidget->getGender() == LoginWidget::Male);
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
        for(int i=0;i<m_recordDatas.size();++i)
        {
            qDebug() << "load match excel success:"<<m_recordDatas[i].m_picNameShowGroup1;
        }
        QSet<QString> names = TrainController::getPicNamesSet();
        for(int i=0;i<m_recordDatas.size();++i)
        {
            names -= m_recordDatas[i].m_picNameShowGroup1.toSet();
        }
        qDebug() << "pics when -= match pic:" << names;
        //生成测试的record
        QSettings settings("PictureMemTrain.ini", QSettings::IniFormat);
        int picshowintv = settings.value("interaction/picInTestShowTime",1020).toInt();
        MemRecordData d1;
        d1.isValid = true;
        for (int i=0;i<16;++i)
        {
            d1.m_picNameShowGroup1.append(*(names.begin()+i));
        }
        names -= d1.m_picNameShowGroup1.toSet();

        OneTrainRecordData trd;
        trd.m_intv = QList<int>() << picshowintv << picshowintv << picshowintv << picshowintv;
        trd.m_names.append(d1.m_picNameShowGroup1[2]);
        trd.m_location.append(2);
        trd.m_names.append(d1.m_picNameShowGroup1[5]);
        trd.m_location.append(5);
        trd.m_names.append(d1.m_picNameShowGroup1[11]);
        trd.m_location.append(11);
        trd.m_names.append(d1.m_picNameShowGroup1[9]);
        trd.m_location.append(9);
        d1.m_selectData.append(trd);
        m_recordDatas.append(d1);

        d1 = MemRecordData();
        d1.isValid = true;
        for (int i=0;i<16;++i)
        {
            d1.m_picNameShowGroup1.append(*(names.begin()+i));
        }
        trd = OneTrainRecordData();
        trd.m_intv = QList<int>() << picshowintv << picshowintv << picshowintv << picshowintv;
        trd.m_names.append(d1.m_picNameShowGroup1[4]);
        trd.m_location.append(4);
        trd.m_names.append(d1.m_picNameShowGroup1[7]);
        trd.m_location.append(7);
        trd.m_names.append(d1.m_picNameShowGroup1[14]);
        trd.m_location.append(14);
        trd.m_names.append(d1.m_picNameShowGroup1[10]);
        trd.m_location.append(10);
        d1.m_selectData.append(trd);
        m_recordDatas.append(d1);
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
    case PMT::TestType1:
        m_trainWidget->setTrainType(type,(m_recordDatas.size() != 0) ? m_recordDatas[3] : MemRecordData());
        break;
    case PMT::TestType2:
        m_trainWidget->setTrainType(type,(m_recordDatas.size() != 0) ? m_recordDatas[4] : MemRecordData());
        break;
    default:
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
