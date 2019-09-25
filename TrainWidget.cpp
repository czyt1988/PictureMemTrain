#include "TrainWidget.h"
#include "ui_TrainWidget.h"
#include <QResizeEvent>
#include <QApplication>
#include "PMTPixmapWidget.h"
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include "MemRecord.h"
#include <QSettings>
#define DEBUG_PRIN
#define FIRST_SHOW_DELAY_TIME 2000
TrainWidget::TrainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainWidget)
  ,m_trainType(PMT::TestType1)
  ,m_hspan(15)
  ,m_vspan(15)
  ,m_currentClicked(nullptr)
  ,m_whenInSecondSessionDonotShowPicToMem(true)
  ,m_autoRunMemRecordDataIndex(0)
  ,m_autoRunOneTrainRecordDataIndex(0)
  ,m_borderWidth(5)
  ,m_OKNOButtonFactor(10)
  ,m_OKNOButtonMaxWidth(150)
  ,m_isAutoCheckOrderTest(true)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    QPalette pl = palette();
    pl.setColor(QPalette::Background,Qt::white);
    setPalette(pl);
    QSettings settings("PictureMemTrain.ini", QSettings::IniFormat);
    m_borderWidth = settings.value("paint/borderWidth",5).toUInt();
    QFont f = font();
    f.setFamily(settings.value("ui/labelIllustrationFontName",QStringLiteral("微软雅黑")).toString());
    f.setPointSize(settings.value("ui/labelIllustrationFontSize",28).toInt());
    f.setBold(settings.value("ui/labelIllustrationFontBolt",false).toBool());
    m_OKNOButtonFactor = settings.value("ui/OKNOButtonFactor",10).toInt();
    m_OKNOButtonMaxWidth = settings.value("ui/OKNOButtonMaxWidth",150).toInt();
    m_hspan = settings.value("ui/hspan",15).toInt();
    m_vspan = settings.value("ui/vspan",15).toInt();
    m_isAutoCheckOrderTest = settings.value("interaction/isAutoCheckOrderTest",true).toBool();
    ui->labelIllustration->setFont(f);
    m_controller = new TrainController(this);
    m_trainOrder = PMT::TrainInfo().getTaskList();
    m_trainOrderIndex = 0;
    ui->label->hide();
    ui->labelIllustration->hide();
    ui->pushButtonNo->hide();
    ui->pushButtonOK->hide();
    ui->pushButtonSure->hide();
    setHspan(15);
    setVspan(15);
    setPicSize(calcPrefectSize());
    resetXYNum(m_controller->getXnum(),m_controller->getYnum());
    connect(m_controller,&TrainController::saveResultExcelErr
            ,this,&TrainWidget::onSaveResultExcelErr);
    connect(m_controller,&TrainController::finish
            ,this,&TrainWidget::onFinish);
}

TrainWidget::~TrainWidget()
{
    delete ui;
}

void TrainWidget::setMatchingNum(const QString &v)
{
    m_controller->setMatchingNum(v);
}

void TrainWidget::setExpNum(const QString &v)
{
    m_controller->setExpNum(v);
}

void TrainWidget::setAge(const QString &v)
{
    m_controller->setAge(v);
}

void TrainWidget::setShortName(const QString &v)
{
    m_controller->setShortName(v);
}

void TrainWidget::setGender(bool isMale)
{
    m_controller->setGender(isMale ? TrainController::Male : TrainController::Female);
}

PMT::TrainType TrainWidget::getTrainType() const
{
    return m_trainType;
}

void TrainWidget::setTrainType(const PMT::TrainType &trainType,const MemRecordData& mr)
{
    qDebug() << "trainType:" << trainType;
    m_orderTestSelectPicNames.clear();
    m_currentClicked = nullptr;
    m_trainType = trainType;
    int trainPicCount = 3;
    int notTestCount = 0;
    m_memRecord = mr;
    m_autoRunMemRecordDataIndex = 0;
    m_autoRunOneTrainRecordDataIndex = 0;
    switch(trainType)
    {
    case PMT::TestType1:
        trainPicCount = 4;
        notTestCount = 1;
        if(mr.isValid)
        {
            QString err;
            if(!m_controller->setPicNameShowGroup1(mr.m_picNameShowGroup1,&err))
            {
                QMessageBox::warning(this,tr("警告"),QString("匹配内容中，以下图片无法找到：%1请检查匹配测试的excel").arg(err));
            }
        }
        break;
    case PMT::TestType2:
        trainPicCount = 4;
        notTestCount = 1;
        if(mr.isValid)
        {
            QString err;
            if(!m_controller->setPicNameShowGroup1(mr.m_picNameShowGroup1,&err))
            {
                QMessageBox::warning(this,tr("警告"),QString("匹配内容中，以下图片无法找到：%1请检查匹配测试的excel").arg(err));

            }
        }
        break;
    case PMT::FormalType1:
    case PMT::FormalType2:
    case PMT::FormalType3:
        if(mr.isValid)
        {
            QString err;
            if(!m_controller->setPicNameShowGroup1(mr.m_picNameShowGroup1,&err))
            {
                QMessageBox::warning(this,tr("警告"),QString("匹配内容中，以下图片无法找到：%1请检查匹配测试的excel").arg(err));
            }
            m_trainOrder.clear();
            m_trainOrderIndex = 0;
            for(int i=0;i<mr.m_selectData.size();++i)
            {
                m_trainOrder.append(qMakePair(mr.m_selectData[i].m_order,mr.m_selectData[i].m_order - 3));
            }
            trainPicCount = m_trainOrder.first().first;
            notTestCount = m_trainOrder.first().second;
        }
        else
        {
            m_trainOrder = PMT::TrainInfo().getTaskList();
            m_trainOrderIndex = 0;
            trainPicCount = m_trainOrder.first().first;
            notTestCount = m_trainOrder.first().second;
        }
        break;
    default:
        break;
    }
    qDebug() << m_trainOrder;
    if(!mr.isValid)
        m_controller->buildPicGroup1();
    m_controller->makeProject(trainPicCount,notTestCount,!mr.isValid);
    //重置图片
    resetPictureInGroup1();
    if(mr.isValid)
    {
        QTimer::singleShot(FIRST_SHOW_DELAY_TIME+500,this,[this](){
            this->onAutoRunTimeout();
        });
    }
}


void TrainWidget::resetXYNum(int xnum, int ynum)
{
    foreach (PMTPixmapWidget* w, m_picList) {
        delete w;
    }
    m_picList.clear();
    recalcSize();
    int xoffset = m_picSize.width() + getHspan();
    int yoffset = m_picSize.height() + getVspan();
    for(int j=0;j<ynum;++j)
    {
        for(int i=0;i<xnum;++i)
        {
            PMTPixmapWidget* w = new PMTPixmapWidget(this);
            w->setId(i+j*m_controller->getYnum());
            w->setBorderWidth(m_borderWidth);
            connect(w,&PMTPixmapWidget::finishPictureMem,this,&TrainWidget::onFinishPictureMem);
            connect(w,&PMTPixmapWidget::clicked,this,&TrainWidget::onClicked);
            w->setGeometry(m_topleftPoint.x()+xoffset*i,m_topleftPoint.y()+yoffset*j,m_picSize.width(),m_picSize.height());
            m_picList.append(w);
        }

    }
}


void TrainWidget::resizeEvent(QResizeEvent *e)
{
    ui->label->setGeometry(0,0,width(),height()/10);

    setPicSize(calcPrefectSize());
    recalcSize();
    int xoffset = m_picSize.width() + getHspan();
    int yoffset = m_picSize.height() + getVspan();
    int maxLeft = 0;
    for(int j=0;j<m_controller->getYnum();++j)
    {
        for(int i=0;i<m_controller->getXnum();++i)
        {
            PMTPixmapWidget* w = m_picList[i+j*m_controller->getYnum()];
            w->setGeometry(m_topleftPoint.x()+xoffset*i,m_topleftPoint.y()+yoffset*j,m_picSize.width(),m_picSize.height());
            maxLeft = qMax(maxLeft,w->geometry().right());
        }

    }
    //设置说明label
    int lh = height()/1.5;
    ui->labelIllustration->setGeometry(5,height()/2-lh/2,width()-10,lh);
    int btnH = height()/m_OKNOButtonFactor;
    int btnW = ((width() - maxLeft) > m_OKNOButtonMaxWidth) ? m_OKNOButtonMaxWidth : (width() - maxLeft - 10);
    ui->pushButtonOK->setGeometry(width()-10-btnW,height()/2-btnH-20,btnW,btnH);
    ui->pushButtonNo->setGeometry(width()-10-btnW,height()/2+btnH+20,btnW,btnH);
    ui->pushButtonSure->setGeometry(width()-10-btnW,height()/2-btnH/2,btnW,btnH);
    QFont f = font();
    f.setPixelSize(btnH/2);
    ui->pushButtonOK->setFont(f);
    ui->pushButtonNo->setFont(f);
    ui->pushButtonSure->setFont(f);
    ui->pushButtonOK->setIconSize(QSize(btnH/2,btnH/2));
    ui->pushButtonNo->setIconSize(QSize(btnH/2,btnH/2));
    ui->pushButtonSure->setIconSize(QSize(btnH/2,btnH/2));
    QWidget::resizeEvent(e);
}

/**
 * @brief 设置图片的显示
 * @param visible
 */
void TrainWidget::setPictureVisible(bool visible)
{
    for(int i=0;i<m_picList.size();++i)
    {
        m_picList[i]->setVisible(visible);
    }
}

void TrainWidget::recalcSize()
{
    //计算离上下的总间距
    int labelH = 10;
    if(ui->label->isVisible())
    {
        labelH = ui->label->height();
    }
    int v = (getPicSize().height() * m_controller->getYnum()) + (getVspan() * (m_controller->getYnum() - 1));
    m_topleftPoint.ry() = ((height() - v - labelH) / 2)+labelH;
    int h = (getPicSize().width() * m_controller->getXnum()) + (getHspan() * (m_controller->getXnum()-1));
    m_topleftPoint.rx() = (width() - h) / 2;

}

QSize TrainWidget::calcPrefectSize() const
{
    int labelH = 10;
    if(ui->label->isVisible())
    {
        labelH = ui->label->height();
    }
    int h = (height()-labelH) / m_controller->getYnum();
    int w = width() / m_controller->getXnum();
    int min = qMin(h,w);
    min = min - qMax(m_hspan,m_vspan);
    return QSize(min,min);
}


/**
 * @brief 重新设置图片
 */
void TrainWidget::resetPictureInGroup1()
{
    ui->pushButtonNo->hide();
    ui->pushButtonOK->hide();
    m_selectedWidgetInRange.clear();
    m_nameToPMTWidgetGroup1.clear();
    if(!m_whenInSecondSessionDonotShowPicToMem)
    {
        for(int i=0;i<m_picList.size();++i)
        {
            m_picList[i]->setPixmap(m_controller->getPixmap(i),m_controller->getPixmapName(i));
            m_nameToPMTWidgetGroup1[m_controller->getPixmapName(i)] = m_picList[i];
            m_picList[i]->showDelayDisplayBlankSpace(FIRST_SHOW_DELAY_TIME);
            m_picList[i]->setClickActionMode(m_memRecord.isValid ? PMTPixmapWidget::ClickNothing : PMTPixmapWidget::ClickShowPicture);
            m_picList[i]->setSelected(false);
        }
    }
    else
    {//标记当第二轮时不进行图片全部显示再次记忆
        for(int i=0;i<m_picList.size();++i)
        {
            if(0 == m_trainOrderIndex)
            {
                m_picList[i]->setPixmap(m_controller->getPixmap(i),m_controller->getPixmapName(i));
                m_nameToPMTWidgetGroup1[m_controller->getPixmapName(i)] = m_picList[i];
                m_picList[i]->showDelayDisplayBlankSpace(FIRST_SHOW_DELAY_TIME);
                m_picList[i]->setClickActionMode(m_memRecord.isValid ? PMTPixmapWidget::ClickNothing : PMTPixmapWidget::ClickShowPicture);
                m_picList[i]->setSelected(false);
            }
            else
            {
                m_picList[i]->setPixmap(m_controller->getPixmap(i),m_controller->getPixmapName(i));
                m_nameToPMTWidgetGroup1[m_controller->getPixmapName(i)] = m_picList[i];
                m_picList[i]->showBlankSpace();
                m_picList[i]->setClickActionMode(m_memRecord.isValid ? PMTPixmapWidget::ClickNothing : PMTPixmapWidget::ClickShowPicture);
                m_picList[i]->setSelected(false);
            }
        }
    }
}

void TrainWidget::resetPictureInGroup2()
{
    m_nameToPMTWidgetGroup2.clear();
    for(int i=0;i<m_picList.size();++i)
    {
        m_picList[i]->setPixmap(m_controller->getPixmap(i,TrainController::Group2),m_controller->getPixmapName(i,TrainController::Group2));
        m_picList[i]->showPicture();
        m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickSelect);
        m_picList[i]->setSelected(false);
        m_nameToPMTWidgetGroup2[m_controller->getPixmapName(i,TrainController::Group2)] = m_picList[i];
    }
}

void TrainWidget::resetPictureInGroup3()
{
    m_locationTestValues = m_controller->getLocationTestValues();
    Q_ASSERT(m_locationTestValues.size() > 0);
    m_currentLocationTestIndex = 0;
    showTestLocation();
}

void TrainWidget::showTestLocation()
{
    for(int i=0;i<m_picList.size();++i)
    {
        m_picList[i]->setShowBrush();
        m_picList[i]->setSelected(false);
        m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickNothing);
        m_picList[i]->repaint();
    }
    if(m_currentLocationTestIndex < m_locationTestValues.size())
    {
        LocationTestValue v = m_locationTestValues[m_currentLocationTestIndex];
        PMTPixmapWidget* w = m_picList[v.showLocation];
        w->showPicture();
        w->setPixmap(m_controller->getPixmap(v.picName),v.picName);
        w->setClickActionMode(PMTPixmapWidget::ClickNothing);
        qDebug() << "v.showLocation：" << v.showLocation;
    }
}
/**
 * @brief 重置为测试模式
 */
void TrainWidget::resetTestType()
{
    for(int i=0;i<m_picList.size();++i)
    {
        m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickShowPicture);
    }
}

void TrainWidget::setToOrderMemTestType()
{
    resetPictureInGroup2();
    if(!m_isAutoCheckOrderTest)
    {
        ui->pushButtonSure->show();
        ui->pushButtonSure->setEnabled(false);
    }
    //emit showTooltip(tr("顺序记忆测试"),3000);
}

void TrainWidget::setToLocationMemTestType()
{
    resetPictureInGroup3();
    ui->pushButtonSure->hide();
    ui->pushButtonSure->setEnabled(false);
    //emit showTooltip(tr("位置记忆测试"),3000);
}


/**
 * @brief 根据PMTPixmapWidget查找索引
 * @param w
 * @return
 */
int TrainWidget::findIndexByPMTPixmapWidget(PMTPixmapWidget *w)
{
    return m_picList.indexOf(w);
}

/**
 * @brief pixmap被点击触发的槽
 * @param name
 */
void TrainWidget::onFinishPictureMem(const QString &name,QDateTime clickedInTime,QDateTime clickedOutTime)
{
    if(!m_nameToPMTWidgetGroup1.contains(name))
    {
        qDebug() << "can not find widget!";
        return;
    }
    PMTPixmapWidget* w = m_nameToPMTWidgetGroup1[name];

    if (!m_controller->isFinishSelPic())
    {
        //把其余设置为可选，把这个设置为不可选
        w->setClickActionMode(PMTPixmapWidget::ClickNothing);
        m_selectedWidgetInRange.insert(w);
        for(int i=0;i<m_picList.size();++i)
        {
            if(!m_selectedWidgetInRange.contains(m_picList[i]))
                m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickShowPicture);
        }
        m_currentClicked = nullptr;
        PMTTestSelRecord ptr;
        ptr.picName = name;
        ptr.location = findIndexByPMTPixmapWidget(w);
        ptr.testRange = m_controller->getSelPicCount();
        ptr.picShowTime = clickedInTime;
        ptr.testPicCount = m_controller->getTotalTrainPicCount();
        ptr.picDisappearTime = clickedOutTime;
        ptr.testRecordName = QStringLiteral("%1-%2").arg(ptr.testPicCount).arg(ptr.testRange);
        m_controller->appendOneSelRecord(ptr);
        qDebug() << "ptr.testRecordName:" << ptr.testRecordName;
        if(m_controller->isFinishSelPic())
        {
            //如果此时选图模式结束，就进入突击测试阶段
            showIllustration(tr("突 击 测 试"),2000,[this](){
                this->setToOrderMemTestType();
            });
            m_currentClicked = nullptr;
        }
    }

//    if(!m_controller->isFinishOrderMemTest())
//    {
//        //进入顺序测试阶段
//    }
}

void TrainWidget::onClicked(const QString& name)
{
    if (!m_controller->isFinishSelPic())
    {
        //还没有选择完，触发点击事件
        if(!m_nameToPMTWidgetGroup1.contains(name))
        {
            qDebug() << "can not find widget!";
            return;
        }
        PMTPixmapWidget* w = m_nameToPMTWidgetGroup1[name];
        //还未完成选图
#ifdef DEBUG_PRIN
        qDebug() << "not finish sel pic:" << " sel :" << name << " is can find w:" << (w != nullptr)
                 << " currentClicked is not null:" << (nullptr != m_currentClicked);
#endif
        if(nullptr == m_currentClicked)
        {
            m_currentClicked = w;
            //把其余设置为不可选
            for(int i=0;i<m_picList.size();++i)
            {
               if(m_picList[i] != w)
                   m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickNothing);
            }
        }
    }
    else if(!m_controller->isFinishOrderMemTest())
    {
        if(!m_nameToPMTWidgetGroup2.contains(name))
        {
            qDebug() << "can not find widget!";
            return;
        }
        PMTPixmapWidget* w = m_nameToPMTWidgetGroup2[name];
        //进入顺序测试阶段
        //记录选中内容m_orderMemSelName
        m_currentClicked = w;
        if(m_isAutoCheckOrderTest)
        {
            //自动进入位置测试
            if(w->isSelected())
            {
                m_controller->appendOrderMemTestRecord(name);
                if(m_controller->isFinishOrderMemTest())
                {
                    QString des;
                    QList<bool> ores = m_controller->orderMemTestResult();
                    for(int i=0;i<ores.size();++i)
                    {
                        des += tr("%1、结果:%2 ; ").arg(i+1).arg(ores[i] ? tr("正确") : tr("错误"));
                    }
                    qDebug() << des;
                    //训练模式下把操作结果展示
                    if(PMT::TestType1 == m_trainType || PMT::TestType2 == m_trainType)
                    {
                        showIllustration(des,2000,[this](){
                            this->setToLocationMemTestType();
                            ui->pushButtonNo->show();
                            ui->pushButtonOK->show();
                        });
                    }
                    else
                    {
                        for(int i=0;i<m_picList.size();++i)
                        {
                            m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickNothing);
                        }

                        QTimer::singleShot(1000,this,[this](){
                            this->setToLocationMemTestType();
                            this->ui->pushButtonNo->show();
                            this->ui->pushButtonOK->show();
                        });



                    }
                }
            }
            else
            {
                if(!m_controller->orderMemTestRecordRef().isEmpty())
                {
                    if(m_controller->orderMemTestRecordRef().last() == name)
                    {
                        m_controller->popOrderMemTestRecord();
                    }
                }
            }
        }
        else
        {
            int maxSelSize = m_controller->getMaxSelPicCount();
            if(w->isSelected())
            {
                m_orderTestSelectPicNames.append(name);
                if(maxSelSize == m_orderTestSelectPicNames.size())
                {
                    //达到条件，把设置为不可选
                    ui->pushButtonSure->setEnabled(true);
                    for(int i=0;i<m_picList.size();++i)
                    {
                        if(m_picList[i]->windowTitle() != name)
                        {
                            m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickNothing);
                        }
                    }
                }
                w->repaint();
            }
            else
            {
                if(maxSelSize == m_orderTestSelectPicNames.size())
                {
                    //所有设置为可选
                    for(int i=0;i<m_picList.size();++i)
                    {
                        m_picList[i]->setClickActionMode(PMTPixmapWidget::ClickSelect);
                        w->repaint();
                    }
                    ui->pushButtonSure->setEnabled(false);
                }
                m_orderTestSelectPicNames.removeOne(name);
            }
        }
    }
    else if (!m_controller->isFinishLocationMemTest()) {

    }
}

void TrainWidget::onSaveResultExcelErr()
{
    emit showTooltip(tr("结果文件存入excel失败，请检查excel是否被占用"),2000);
    QMessageBox::StandardButton btn = QMessageBox::Retry;
    int rt = 0;
    while(btn == QMessageBox::Retry)
    {
        if(rt >3)
        {
            QMessageBox::information(this,tr("警告"),tr("重试失败"));
            return;
        }
        btn = QMessageBox::warning(this,tr("警告"),tr("结果文件存入excel失败，请检查excel是否被占用")
                             ,QMessageBox::Retry|QMessageBox::Cancel,QMessageBox::Retry);
        if(btn == QMessageBox::Retry)
        {
            m_controller->saveResult();
            ++rt;
        }
    }
}

void TrainWidget::onFinish()
{
    if(PMT::FormalType1 == getTrainType()
            ||
       PMT::FormalType2 == getTrainType()
            ||
       PMT::FormalType3 == getTrainType())
    {
        m_controller->saveResult();
        ++m_trainOrderIndex;
        if(m_trainOrderIndex<m_trainOrder.size())
        {
            ui->pushButtonNo->hide();
            ui->pushButtonOK->hide();
            int trainPicCount = m_trainOrder[m_trainOrderIndex].first;
            int startTestIndex = m_trainOrder[m_trainOrderIndex].second;
            m_currentClicked = nullptr;
            m_orderTestSelectPicNames.clear();
            m_controller->makeProject(trainPicCount,startTestIndex,!m_memRecord.isValid);
            //重置图片
            resetPictureInGroup1();
            if(m_memRecord.isValid)
            {
                m_autoRunOneTrainRecordDataIndex = 0;
                QTimer::singleShot(FIRST_SHOW_DELAY_TIME+500,this,[this](){
                    this->onAutoRunTimeout();
                });
            }
        }
        else
        {
            //写入随机顺序
            QList<int> order;
            for(int i=0;i<m_trainOrder.size();++i)
            {
                order.append(m_trainOrder[i].first);
            }
            m_controller->savePicTestOrder(order);
            if(!m_memRecord.isValid)
                m_controller->removeAndResetPicture();
            qDebug() << "=======================Finish=====================";
            emit finish(getTrainType());
        }
    }
    else
    {
        m_controller->removeAndResetPicture();
        emit finish(getTrainType());
    }
}

int TrainWidget::getVspan() const
{
    return m_vspan;
}

void TrainWidget::setVspan(int vspan)
{
    m_vspan = vspan;
}

/**
 * @brief 显示说明
 * 显示说明阶段会把所有隐藏
 * @param str 说明内容
 * @param ms 说明内容持续时间
 */
void TrainWidget::showIllustration(const QString &str, int ms, std::function<void(void)> fn)
{
    setPictureVisible(false);//先把图片隐藏
    ui->labelIllustration->setText(str);
    ui->labelIllustration->show();
    QTimer::singleShot(ms,this,[this,fn](){
        ui->labelIllustration->hide();
        setPictureVisible(true);
        if(fn != nullptr)
        {
            fn();
        }
    });
}



int TrainWidget::getHspan() const
{
    return m_hspan;
}

void TrainWidget::setHspan(int hspan)
{
    m_hspan = hspan;
}

QSize TrainWidget::getPicSize() const
{
    return m_picSize;
}

void TrainWidget::setPicSize(const QSize &picSize)
{
    m_picSize = picSize;
}



void TrainWidget::on_pushButtonOK_clicked()
{
    if(m_currentLocationTestIndex >= m_locationTestValues.size())
    {
        qDebug() << "click too more";
        return;
    }
    if(m_currentLocationTestIndex>=m_locationTestValues.size())
    {
        ui->pushButtonNo->hide();
        ui->pushButtonOK->hide();
    }
    else
    {
        ++m_currentLocationTestIndex;
        showTestLocation();
    }
    //此函数一定要在最后
    m_controller->appendLocationMemTestRecord(true);
}

void TrainWidget::on_pushButtonNo_clicked()
{
    if(m_currentLocationTestIndex >= m_locationTestValues.size())
    {
        qDebug() << "click too more";
        return;
    }

    if(m_currentLocationTestIndex>=m_locationTestValues.size())
    {
        ui->pushButtonNo->hide();
        ui->pushButtonOK->hide();
    }
    else
    {
        ++m_currentLocationTestIndex;
        showTestLocation();
    }
    //此函数一定要在最后
    m_controller->appendLocationMemTestRecord(false);
}

void TrainWidget::onAutoRunTimeout()
{
    if(!m_memRecord.isValid)
    {
        return;
    }
    if(!m_controller->isFinishSelPic())
    {
        const OneTrainRecordData& rd = m_memRecord.m_selectData[m_autoRunMemRecordDataIndex];
        int index = rd.m_location[m_autoRunOneTrainRecordDataIndex];
        int intv = rd.m_intv[m_autoRunOneTrainRecordDataIndex];
        m_picList[index]->showDelayDisplayBlankSpace(intv);
        ++m_autoRunOneTrainRecordDataIndex;
        qDebug() << "auto show:m_picList["<<index<<"] intv:" << intv;
        //把其余设置为可选，把这个设置为不可选
        PMTTestSelRecord ptr;
        ptr.picName = m_picList[index]->windowTitle();
        ptr.location = index;
        ptr.testRange = m_controller->getSelPicCount();
        ptr.picShowTime = QDateTime::currentDateTime();
        ptr.testPicCount = m_controller->getTotalTrainPicCount();
        ptr.picDisappearTime = QDateTime::currentDateTime().addMSecs(intv);
        ptr.testRecordName = QStringLiteral("%1-%2").arg(ptr.testPicCount).arg(ptr.testRange);
        m_controller->appendOneSelRecord(ptr);
        qDebug() << "ptr.testRecordName:" << ptr.testRecordName;

        QTimer::singleShot(intv+50,this,[this](){
            this->onAutoRunTimeout();
        });

    }
    else
    {
        //如果此时选图模式结束，就进入突击测试阶段
        showIllustration(tr("突 击 测 试"),2000,[this](){
            this->setToOrderMemTestType();
        });
        ++m_autoRunMemRecordDataIndex;
        m_currentClicked = nullptr;
    }
}

void TrainWidget::setMemRecord(const MemRecordData &memRecord)
{
    m_memRecord = memRecord;
}

TrainController *TrainWidget::getController() const
{
    return m_controller;
}

/**
 * @brief 对原来设置的指针不进行管理，需要用户手动删除
 * @param controller
 */
void TrainWidget::setController(TrainController *controller)
{
    if(m_controller)
    {
        disconnect(m_controller,&TrainController::saveResultExcelErr
                ,this,&TrainWidget::onSaveResultExcelErr);
        disconnect(m_controller,&TrainController::finish
                ,this,&TrainWidget::onFinish);
    }
    m_controller = controller;
    resetXYNum(m_controller->getXnum(),m_controller->getYnum());
    connect(m_controller,&TrainController::saveResultExcelErr
            ,this,&TrainWidget::onSaveResultExcelErr);
    connect(m_controller,&TrainController::finish
            ,this,&TrainWidget::onFinish);
}

void TrainWidget::on_pushButtonSure_clicked()
{
    if(m_orderTestSelectPicNames.size() != m_controller->getMaxSelPicCount())
    {
        qDebug() << QStringLiteral("选择图片不在3张范围里");
    }


    if(PMT::TestType1 == m_trainType || PMT::TestType2 == m_trainType)
    {
        //显示正确情况
        for(int i=0;i<m_orderTestSelectPicNames.size();++i)
        {
            m_controller->appendOrderMemTestRecord(m_orderTestSelectPicNames[i]);
        }
        ui->label->show();
        QResizeEvent e(size(),size());
        QApplication::sendEvent(this,&e);
        ui->label->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        QFont f = font();
        f.setPixelSize(70);
        ui->label->setFont(f);
        QList<bool> ores = m_controller->orderMemTestResult();
        bool isRigth = true;
        for(int i=0;i<ores.size();++i)
        {
            isRigth &= ores[i];
        }
        QString des;
        des = isRigth ? tr("结果正确") : tr("结果错误");
        ui->label->setText(des);
        this->ui->pushButtonSure->setEnabled(false);
        QTimer::singleShot(2000,this,[this](){
            this->ui->pushButtonSure->setEnabled(true);
            this->ui->label->hide();
            QResizeEvent e(size(),size());
            QApplication::sendEvent(this,&e);
            this->setToLocationMemTestType();
            this->ui->pushButtonNo->show();
            this->ui->pushButtonOK->show();
        });
    }
    else
    {
        this->setToLocationMemTestType();
        this->ui->pushButtonNo->show();
        this->ui->pushButtonOK->show();
        for(int i=0;i<m_orderTestSelectPicNames.size();++i)
        {
            m_controller->appendOrderMemTestRecord(m_orderTestSelectPicNames[i]);
        }
    }
}
