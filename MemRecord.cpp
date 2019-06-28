#include "MemRecord.h"
#include "xlsxabstractsheet.h"
#include <QDebug>
#include "Globals.h"
#include <QString>
#include "Globals.h"
#include <math.h>
#include <QSet>

MemRecordData::MemRecordData():isValid(false)
{

}

MemRecord::MemRecord():m_matchNumCount(3)
{

}
/**
 * @brief 加载编组结果
 * @param name
 * @return
 */
bool MemRecord::load(const QString &excel, const QString &matchNum, QString *errInfo)
{
    QXlsx::Document xlsx(excel);
    QStringList shs = xlsx.sheetNames();
    if(shs.size() <= 0)
    {
        qDebug() << "invalid excel";
        if(errInfo)
            *errInfo = QStringLiteral("无效的excel文件，请确保格式为xlsx");
        return false;
    }
    xlsx.selectSheet(shs[0]);
    //开始搜索匹配的行数
    m_rows = findRowsByMatchNum(xlsx,matchNum);
    if(m_rows.size()!=m_matchNumCount)
    {
        if(errInfo)
            *errInfo = QStringLiteral("无效的匹配编号：%1，excel搜索到%2行").arg(matchNum).arg(m_rows.size());
        return false;
    }
    //
    for (int i=0;i<m_rows.size();++i) {
        QList<int> orders;
        int r = m_rows[i];
        if(!loadOrders(xlsx,r,orders,errInfo))
        {
            return false;
        }
        for(int j=0;j<orders.size();++j)
        {
            MemRecordData mrd;
            //读取第50列获取所有的图片
            if(!loadPicNames(xlsx,r,mrd.m_picNameShowGroup1,errInfo))
            {
                return false;
            }
            if(!loadOneOrderRecord(xlsx,r,orders[j],mrd,errInfo))
            {
                return false;
            }
            mrd.isValid = true;
            m_recordDatas.append(mrd);
        }
    }
    return true;
}

QList<int> MemRecord::findRowsByMatchNum(QXlsx::Document &excel, const QString &matchNum)
{
    QList<int> res;
    int row = 2;
    QVariant v = excel.read(row,1);
    while(v.isValid())
    {
        QString vstr = v.toString();
        if(vstr == matchNum)
        {
            res.append(row);
        }
        ++row;
        v = excel.read(row,1);
    }
    return res;
}
#define COL_Table 50
#define COL_Order 51
#define COL_4_1 5
#define COL_5_1 17
#define COL_6_1 32
bool MemRecord::loadOrders(QXlsx::Document &excel, int row, QList<int> &res, QString *errInfo)
{
    QString info;
    //读取顺序
    QString str = excel.read(row,COL_Order).toString();
    QStringList strList = str.split(',');
    if(strList.size() != 3)
    {
        info = QStringLiteral("匹配excel顺序个数异常，在第%1行").arg(row);
        qDebug() << info;
        return false;
    }
    for(int i=0;i<strList.size();++i)
    {
        bool isOK = false;
        res.append(strList[i].toInt(&isOK));
        if(!isOK)
        {
            info = QStringLiteral("匹配excel顺序个数的数字异常，在第%1行").arg(row);
            qDebug() << info;
            return false;
        }
    }
    return true;
}

bool MemRecord::loadPicNames(QXlsx::Document &excel, int row, QList<QString> &res, QString *errInfo)
{
    QString info;
    QString str = excel.read(row,COL_Table).toString();
    QStringList strList = str.split(',');
    if(strList.size() != 16)
    {
        info = QStringLiteral("匹配excel图片个数异常，在第%1行,第%2列，%3").arg(row).arg(COL_Table).arg(str);
        qDebug() << info;
        if(errInfo)
            *errInfo = info;
        return false;
    }

    for(int i=0;i<strList.size();++i)
    {
        res.append(strList[i].trimmed());
    }
    auto a = res.toSet();
    if(a.size() != res.size() && a.size() != 16)
    {
        info = QStringLiteral("匹配excel图片个数异常，在第%1行,第%2列，下面存在相同内容：%3").arg(row).arg(COL_Table).arg(str);;
        qDebug() << info;
        if(errInfo)
            *errInfo = info;
        return false;
    }
    return true;
}

bool MemRecord::loadOneOrderRecord(QXlsx::Document &excel, int row, int order, MemRecordData& res ,QString *errInfo)
{
    //读取第50列获取所有的图片
    QString info;
    //根据顺序加载
    int col = orderToCol(order);
    if(col<=0)
    {
        info = QStringLiteral("匹配excel顺序个数的数字异常，在第%1行").arg(row);
        qDebug() << info;
        if (errInfo)
            *errInfo = info;
        return false;
    }
    //加载第一次训练结果
    if(!loadOneOrderClickedRecord(excel,row,col,order,res,errInfo))
    {
        return false;
    }
    //加载第二次训练结果

    return true;
}

bool MemRecord::loadOneOrderClickedRecord(QXlsx::Document &excel, int row,int startCol, int order, MemRecordData &res, QString *errInfo)
{
    QString info;
    //QSet<int> clickIndex;
    OneTrainRecordData otd;
    otd.m_order = order;
    for(int i=0;i<order;++i)
    {
        QString name = excel.read(row,startCol+i*3).toString();
        if(name.isEmpty())
        {
            info = QStringLiteral("匹配excel异常，在第%1行,%2-%3").arg(row).arg(order).arg(i+1);
            qDebug() << info;
            if (errInfo)
                *errInfo = info;
            return false;
        }
        int index = res.m_picNameShowGroup1.indexOf(name);
        if(index < 0)
        {
            info = QStringLiteral("匹配excel异常，在第%1行,%2-%3 位置列，记录的图片名不在总图片队列中").arg(row).arg(order).arg(i+1);
            qDebug() << info;
            if (errInfo)
                *errInfo = info;
            return false;
        }
        //
        bool isOK = false;
        int pos = excel.read(row,startCol+i*3+1).toInt(&isOK);
        --pos;
        if(!isOK)
        {
            info = QStringLiteral("匹配excel异常，在第%1行,%2-%3").arg(row).arg(order).arg(i+1);
            qDebug() << info;
            if (errInfo)
                *errInfo = info;
            return false;
        }
        if(pos < 0 || pos >= res.m_picNameShowGroup1.size())
        {
            info = QStringLiteral("匹配excel异常，在第%1行,%2-%3,位置记录的数据超出范围").arg(row).arg(order).arg(i+1);
            qDebug() << info;
            if (errInfo)
                *errInfo = info;
            return false;
        }
        //clickIndex.insert(pos);//记录位置
        //获取间隔时间
        QVariant v = excel.read(row,startCol+i*3+2);
        int ms = abs(v.toTime().msecsTo(QTime(0,0,0)));
        qDebug() << v.typeName() << "  " <<v.toString() << " intv:" << ms;
        if(pos >=  res.m_picNameShowGroup1.size())
        {
            info = QStringLiteral("匹配excel异常，在第%1行,%2-%3 位置列，位置超出范围").arg(row).arg(order).arg(i+1);
            qDebug() << info;
            if (errInfo)
                *errInfo = info;
            return false;
        }
        if(ms > 100000)
        {
           qDebug() << QStringLiteral("超长时间警告：在第%1行,%2-%3 位置列，%4 ms").arg(row).arg(order).arg(i+1).arg(ms);
        }
        //随机交换，把图片设置进去
        if(index != pos)
        {
//            int loc = 0;
//            do{
//                loc = floor((qrand() / float(RAND_MAX))*(res.m_picNameShowGroup2.size()));
//            }while(!clickIndex.contains(loc));
//            clickIndex.insert(loc);
            std::swap(res.m_picNameShowGroup1[pos],res.m_picNameShowGroup1[index]);
        }
        otd.m_location.append(pos);
        otd.m_names.append(name);
        otd.m_intv.append(ms);
    }
    res.m_selectData.append(otd);
    return true;
}



int MemRecord::orderToCol(int order)
{
    switch(order)
    {
    case 4:return COL_4_1;
    case 5:return COL_5_1;
    case 6:return COL_6_1;
    }
    return 0;
}

const QList<MemRecordData> &MemRecord::getRecordDatas() const
{
    return m_recordDatas;
}


