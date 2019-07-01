#include "TrainController.h"
#include <QApplication>
#include <QDir>
#include <QTime>
#include <QDate>
#include <QDebug>
#include <math.h>
#include <QApplication>
#include "xlsxdocument.h"
#include "xlsxabstractsheet.h"
#include <QStringList>
#include <QTimer>
#include "Globals.h"
TrainController::TrainController(QObject *par):QObject(par)
  ,m_xnum(4)
  ,m_ynum(4)
  ,m_totalTrainPicCount(3)
  ,m_startTestPicIndex(0)
  ,m_isFinishSelPic(false)
  ,m_isFinishOrderMemTest(false)
  ,m_isFinishLocationMemTest(false)
  ,m_firstNullRow(0)
{
    initResources();
    buildPicGroup1();
}

QString TrainController::getPicResourcesPath()
{
    return QApplication::applicationDirPath() + QDir::separator() + "MemoryPictures";
}
void TrainController::setMatchingNum(const QString &v)
{
    m_matchingNum = v;
}

void TrainController::setExpNum(const QString &v)
{
    m_expNum = v;
}

void TrainController::setAge(const QString &v)
{
    m_age = v;
}

void TrainController::setShortName(const QString &v)
{
    m_shortName = v;
}
void TrainController::makeProject(int tarinPicCount,int startTestPicIndex)
{
    qDebug() << "makeProject tarinPicCount:" << tarinPicCount << " startTestPicIndex:"<<startTestPicIndex;
    resetTrainPram();
    m_totalTrainPicCount = tarinPicCount;
    m_startTestPicIndex = startTestPicIndex;
    buildPicGroup2();
}

QPixmap TrainController::getPixmap(uint index,GroupType group) const
{
    if (Group1 == group)
        return m_nameToPic[m_picNameShowGroup1[index]];
    else
        return m_nameToPic[m_picNameShowGroup2[index]];
}

QPixmap TrainController::getPixmap(const QString &name) const
{
    return m_nameToPic[name];
}

QString TrainController::getPixmapName(uint index,GroupType group) const
{
    if (Group1 == group)
        return m_picNameShowGroup1[index];
    else
        return m_picNameShowGroup2[index];
}

void TrainController::initResources()
{
    QString picPath = getPicResourcesPath();
    QDir dir(picPath);
    QFileInfoList il = dir.entryInfoList(QDir::Files);
    foreach (const QFileInfo& fi, il) {
        QPixmap p(fi.absoluteFilePath());
        if(p.isNull())
            continue;
        m_nameToPic[fi.baseName()] = p;
        m_names.append(fi.baseName());
    }
    qDebug() <<m_names;
}

void TrainController::buildPicGroup1()
{
    m_picNameShowGroup1.clear();
    randSelect(m_names,m_picNameShowGroup1,m_xnum*m_ynum);
    qDebug() << " buildPicGroup1 :" << m_picNameShowGroup1;
}

void TrainController::buildPicGroup2()
{
    m_picNameShowGroup2 = m_picNameShowGroup1;
    PMT::TrainInfo::shuffle(m_picNameShowGroup2.begin(),m_picNameShowGroup2.end());
}

void TrainController::buildPicGroup3()
{
    m_picNameShowGroup3.clear();
    //测试的个数
    int totalLocalTestCount = m_totalTrainPicCount - m_startTestPicIndex;
    //
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QSet<int> locationSet;
    for(int i=0;i<totalLocalTestCount;++i)
    {
        PMTTestSelRecord& pmtrec = m_selRecords[m_startTestPicIndex+i];
        locationSet.insert(pmtrec.location);
    }
    for(int i=0;i<totalLocalTestCount;++i)
    {
        PMTTestSelRecord& pmtrec = m_selRecords[m_startTestPicIndex+i];
        int loc = floor((qrand() / float(RAND_MAX))*(m_xnum*m_ynum));
        while(locationSet.contains(loc))
        {
            loc = floor((qrand() / float(RAND_MAX))*(m_xnum*m_ynum));
        }
        LocationTestValue ltv;
        ltv.realLocation = pmtrec.location;
        ltv.picName = pmtrec.picName;
        ltv.showLocation = loc;
        locationSet.insert(loc);

        m_picNameShowGroup3.append(ltv);
    }
    //先判断这随机生成的是否符合：
    bool haveCorrect = false;
    bool haveIncorrect = false;
    for(int i=0;i<totalLocalTestCount;++i)
    {
        if(m_picNameShowGroup3[i].isCorrect())
        {
            haveCorrect = true;
        }
        else
        {
            haveIncorrect = true;
        }
    }
    if(!haveCorrect)
    {
        //如果没有正确的，随机选择一个序列,且这个正确的序列不能在随机序列里
        int correctIndex = floor(qrand() / float(RAND_MAX) * totalLocalTestCount);
        m_picNameShowGroup3[correctIndex].showLocation = m_picNameShowGroup3[correctIndex].realLocation;
    }
    if(!haveIncorrect)
    {
        //如果没有正确的，随机选择一个序列作为不正确值
        int correctIndex = floor(qrand() / float(RAND_MAX) * totalLocalTestCount);
        while(m_picNameShowGroup3[correctIndex].isCorrect())
        {
            m_picNameShowGroup3[correctIndex].showLocation = floor((qrand() / float(RAND_MAX))*(m_xnum*m_ynum));
        }
    }
    for(int i=0;i<totalLocalTestCount;++i)
    {
        qDebug() << i+1 << "-  name:" << m_picNameShowGroup3[i].picName
                 << "  realLocation:" << m_picNameShowGroup3[i].realLocation
                 << "  showLocation:" << m_picNameShowGroup3[i].showLocation;
    }
}
/**
 * @brief 随机循环位交换法进行洗牌
 * @param org 原牌位
 * @param shuffleLen 洗完的长度
 * @return 返回洗完的结果
 */
QList<QString> TrainController::shufflePicName(const QList<QString> &org, uint shuffleLen)
{
    Q_ASSERT(shuffleLen<=org.size());
    QList<QString> tmp = org;
    QList<QString> res;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0;i<tmp.size();++i)
    {
        int index = round((qrand() / float(RAND_MAX)) * (shuffleLen-1));
        qSwap(tmp[i],tmp[index]);
    }
    for(uint i=0;i<shuffleLen;++i)
    {
        res.append(tmp[i]);
    }
    return res;
}

void TrainController::randSelect(const QList<QString> &org, QList<QString> &res, int len)
{
    QList<QString> tmpOrg = org.toSet().toList();//去重
    if(tmpOrg.size() < len)
    {
        qDebug() << "pic size invalid";
    }
    QSet<QString> tmpRes;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    do{
        int index = round((qrand() / float(RAND_MAX)) * (tmpOrg.size()-1));
        tmpRes.insert(tmpOrg[index]);
        tmpOrg.removeAt(index);
    }while(tmpRes.size() < len);
    res = tmpRes.toList();
}

void TrainController::resetTrainPram()
{
    //训练模式下参数
    m_isFinishSelPic = m_isFinishOrderMemTest = m_isFinishLocationMemTest = false;
    m_selRecords.clear();
    m_orderMemSelName.clear();
    m_picNameShowGroup2.clear();
    m_orderMemTestResult.clear();
    m_locationMemTestResult.clear();
    m_picNameShowGroup3.clear();
}

bool TrainController::setPicNameShowGroup1(const QList<QString> &picNameShowGroup1,QString* info)
{
    QString canotFind;
    for(int i=0;i<picNameShowGroup1.size();++i)
    {
        if(!m_nameToPic.contains(picNameShowGroup1[i]))
        {
            canotFind += picNameShowGroup1[i];
            canotFind += ",";
        }
    }
    m_picNameShowGroup1 = picNameShowGroup1;
    if(!canotFind.isEmpty())
    {
        if(info)
            *info = canotFind;
        return false;
    }
    return true;
}


#define COL_ExpNum 1
#define COL_ShortName 2
#define COL_Age 3
#define COL_ExpDate 4
#define COL_4_1 5
#define COL_4_1_OrderTest 25
#define COL_4_1_LocationTest 31
#define COL_5_1 43
#define COL_5_1_OrderTest 68
#define COL_5_1_LocationTest 74
#define COL_6_1 86
#define COL_6_1_OrderTest 116
#define COL_6_1_LocationTest 122
#define COL_PIC_LIST 134
#define COL_PIC_LEN_ORDER 135
void TrainController::saveResult()
{
    QXlsx::Document xlsx(QApplication::applicationDirPath() + QDir::separator() + "output/out.xlsx");
        //![0]
    QStringList shs = xlsx.sheetNames();
    if(shs.size() <= 0)
    {
        qDebug() << "invalid excel";
        return;
    }
    xlsx.selectSheet(shs[0]);
    if(0 == m_firstNullRow)
    {
        m_firstNullRow = 1;
        while(xlsx.read(m_firstNullRow,1).isValid())
        {
            ++m_firstNullRow;
        }
    }

    qDebug() << "save expNum:" <<  m_expNum << " shortName:" << m_shortName<<" age:"<<m_age
             << " m_firstNullRow:" << m_firstNullRow;
    xlsx.write(m_firstNullRow,COL_ExpNum,m_expNum);
    xlsx.write(m_firstNullRow,COL_ShortName,m_shortName);
    xlsx.write(m_firstNullRow,COL_Age,m_age);
    xlsx.write(m_firstNullRow,COL_ExpDate,QDate::currentDate());
    int colBias = COL_4_1;
    int orderCol = COL_4_1_OrderTest;
    int locationCol = COL_4_1_LocationTest;
    switch(m_totalTrainPicCount)
    {
    case 4:
        colBias = COL_4_1;
        orderCol = COL_4_1_OrderTest;
        locationCol = COL_4_1_LocationTest;
        break;
    case 5:
        colBias = COL_5_1;
        orderCol = COL_5_1_OrderTest;
        locationCol = COL_5_1_LocationTest;
        break;
    case 6:
        colBias = COL_6_1;
        orderCol = COL_6_1_OrderTest;
        locationCol = COL_6_1_LocationTest;
        break;
    default:
        colBias = COL_4_1;
        orderCol = COL_4_1_OrderTest;
        locationCol = COL_4_1_LocationTest;
        break;
    }
    //保存点击记录
    for(int i=0;i<m_selRecords.size();++i)
    {
        xlsx.write(m_firstNullRow,colBias+i*5,m_selRecords[i].picName);
        xlsx.write(m_firstNullRow,colBias+i*5+1,m_selRecords[i].location);
        QXlsx::Format format;
        format.setNumberFormat("yyyy-mm-dd hh:mm:ss.ms");
        xlsx.write(m_firstNullRow,colBias+i*5+2,m_selRecords[i].picShowTime,format);
        xlsx.write(m_firstNullRow,colBias+i*5+3,m_selRecords[i].picDisappearTime,format);
        format.setNumberFormat("mm:ss.ms");
        xlsx.write(m_firstNullRow,colBias+i*5+4,QTime(0,0,0).addMSecs(m_selRecords[i].intervalMs()),format);
    }
    //保存顺序测试结果
    for(int i=0;i<m_orderMemTestResult.size();++i)
    {
        xlsx.write(m_firstNullRow,orderCol+i*2,m_orderMemSelName[i]);
        xlsx.write(m_firstNullRow,orderCol+i*2+1,m_orderMemTestResult[i]);
    }
    //保存位置测试结果
    for(int i=0;i<m_locationMemTestResult.size();++i)
    {
        xlsx.write(m_firstNullRow,locationCol+i*4,m_picNameShowGroup3[i].picName);
        xlsx.write(m_firstNullRow,locationCol+i*4+1,m_picNameShowGroup3[i].realLocation);
        xlsx.write(m_firstNullRow,locationCol+i*4+2,m_picNameShowGroup3[i].isCorrect());
        xlsx.write(m_firstNullRow,locationCol+i*4+3,m_locationMemTestResult[i]);
    }
    //写入图片序列
    QString pics;
    for(int i=0;i<m_picNameShowGroup1.size();++i)
    {
        pics += m_picNameShowGroup1[i];
        if(i != m_picNameShowGroup1.size()-1)
        {
            pics += ",";
        }
    }
    qDebug()<<pics;
    xlsx.write(m_firstNullRow,COL_PIC_LIST,pics);
    if(!xlsx.save())
    {
        emit saveResultExcelErr();
    }

}

void TrainController::savePicTestOrder(const QList<int> &los)
{
    QXlsx::Document xlsx(QApplication::applicationDirPath() + QDir::separator() + "output/out.xlsx");
        //![0]
    QStringList shs = xlsx.sheetNames();
    if(shs.size() <= 0)
    {
        qDebug() << "invalid excel";
        emit error(tr("无效的excel文件，请确保格式为xlsx"));
        return;
    }
    xlsx.selectSheet(shs[0]);
    if(0 == m_firstNullRow)
    {
        m_firstNullRow = 1;
        while(xlsx.read(m_firstNullRow,1).isValid())
        {
            ++m_firstNullRow;
        }
    }
    QString str;
    for(int i=0;i<los.size();++i)
    {
        str += QString::number(los[i]);
        if(i != los.size()-1)
        {
            str += ",";
        }
    }
    xlsx.write(m_firstNullRow,COL_PIC_LEN_ORDER,str);
    if(!xlsx.save())
    {
        emit saveResultExcelErr();
    }
    m_firstNullRow = 0;
}


void TrainController::removeAndResetPicture()
{
    int t = m_xnum*m_ynum;
    for(int i=0;i<m_picNameShowGroup1.size();++i)
    {
        if(t <= m_nameToPic.size())
        {
            m_nameToPic.remove(m_picNameShowGroup1[i]);
            m_names.removeOne(m_picNameShowGroup1[i]);
        }
    }
    buildPicGroup1();
}

bool TrainController::isFinishLocationMemTest() const
{
    return m_isFinishLocationMemTest;
}

QList<LocationTestValue> TrainController::getLocationTestValues() const
{
    return m_picNameShowGroup3;
}

int TrainController::getStartTestPicIndex() const
{
    return m_startTestPicIndex;
}

bool TrainController::isFinishOrderMemTest() const
{
    return m_isFinishOrderMemTest;
}

bool TrainController::isFinishSelPic() const
{
    return m_isFinishSelPic;
}

int TrainController::getSelPicCount() const
{
    return m_selRecords.size();
}

int TrainController::getMaxSelPicCount() const
{
    return m_totalTrainPicCount-m_startTestPicIndex;
}


void TrainController::appendOneSelRecord(const PMTTestSelRecord &r)
{
    m_selRecords.append(r);
    qDebug() << "select pic:" << r.picName << " location:" << r.location
             << " intervalMs:" << r.intervalMs();
    if(getSelPicCount() >= m_totalTrainPicCount)
    {
        //说明选择图片阶段完成
        m_isFinishSelPic = true;
        //生成位置测试用的图Group3
        buildPicGroup3();
    }
}

void TrainController::appendOrderMemTestRecord(const QString &name)
{
    m_orderMemSelName.append(name);
    Q_ASSERT(m_startTestPicIndex+m_orderMemSelName.size()-1 < m_selRecords.size());
    //判断结果
    m_orderMemTestResult.append(m_selRecords[m_startTestPicIndex+m_orderMemSelName.size()-1].picName == name);
    m_isFinishOrderMemTest = (m_orderMemSelName.size() >= (m_totalTrainPicCount-m_startTestPicIndex));
}

void TrainController::appendLocationMemTestRecord(bool selres)
{
    int index = m_locationMemTestResult.size();
    Q_ASSERT(index < m_picNameShowGroup3.size());
    m_locationMemTestResult.append(selres == m_picNameShowGroup3[index].isCorrect());
    qDebug() << "loacation test " << m_locationMemTestResult.size() << " :" << m_locationMemTestResult.back();
    m_isFinishLocationMemTest = (m_locationMemTestResult.size() >= (m_totalTrainPicCount-m_startTestPicIndex));
    if(m_isFinishLocationMemTest)
    {
        emit finish();
    }
}

const QList<QString> &TrainController::orderMemTestRecordRef() const
{
    return m_orderMemSelName;
}

const QList<bool> &TrainController::orderMemTestResult() const
{
    return m_orderMemTestResult;
}

void TrainController::popOrderMemTestRecord()
{
    m_orderMemSelName.pop_back();
    m_orderMemTestResult.pop_back();
    m_isFinishOrderMemTest = (m_orderMemSelName.size() >= (m_totalTrainPicCount-m_startTestPicIndex));
}

int TrainController::getTotalTrainPicCount() const
{
    return m_totalTrainPicCount;
}


int TrainController::getYnum() const
{
    return m_ynum;
}

void TrainController::setYnum(int ynum)
{
    m_ynum = ynum;
}

int TrainController::getXnum() const
{
    return m_xnum;
}

void TrainController::setXnum(int xnum)
{
    m_xnum = xnum;
}
