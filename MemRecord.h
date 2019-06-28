#ifndef MEMRECORD_H
#define MEMRECORD_H
#include <QList>
#include <QPair>
#include <QObject>
#include "xlsxdocument.h"

class OneTrainRecordData
{
public:
    int m_order;
    QList<int> m_location;///< 记录点击的位置
    QList<int> m_intv;///< 记录点击的时间间隔
    QList<QString> m_names;///< 记录点击的名字
};


class MemRecordData
{
public:
    MemRecordData();
    bool isValid;
    QList<QString> m_picNameShowGroup1;///< 第1次呈现的图片队列
    QList<OneTrainRecordData> m_selectData;///< 记录顺序测试阶段的选择名字
};

class MemRecord
{
public:
    MemRecord();
    bool load(const QString& excel,const QString& matchNum,QString* errInfo = nullptr);
    const QList<MemRecordData>& getRecordDatas() const;

private:
    QList<int> findRowsByMatchNum(QXlsx::Document& excel,const QString& matchNum);
    bool loadOrders(QXlsx::Document& excel, int row,QList<int>& res,QString* errInfo = nullptr);
    bool loadPicNames(QXlsx::Document& excel, int row,QList<QString>& res,QString* errInfo = nullptr);
    bool loadOneOrderRecord(QXlsx::Document& excel, int row,int order, MemRecordData& res ,QString* errInfo = nullptr);
    bool loadOneOrderClickedRecord(QXlsx::Document& excel, int row, int col, int order, MemRecordData& res , QString* errInfo = nullptr);
    int orderToCol(int order);
private:
    QList<MemRecordData> m_recordDatas;
    int m_matchNumCount;///< 匹配的个数
    QList<int> m_rows;
};

#endif // MEMRECORD_H
