#include "MemRecord.h"
#include "xlsxabstractsheet.h"
#include <QDebug>
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
    m_control.clear();
    QXlsx::Document xlsx(matchNum);
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
    QList<int> rows = findRowsByMatchNum(xlsx,matchNum);
    if(rows.size()!=m_matchNumCount)
    {
        if(errInfo)
            *errInfo = QStringLiteral("无效的匹配编号：%1，excel搜索到%2行").arg(matchNum).arg(rows.size());
        return false;
    }
    //
    for (int i=0;i<rows.size();++i) {
        TrainController c;
        if(!loadController(xlsx,rows[i],c,errInfo))
        {
            return false;
        }
        m_control.append(c);
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

bool MemRecord::loadController(QXlsx::Document &excel, int row, TrainController &res, QString *errInfo)
{

}
