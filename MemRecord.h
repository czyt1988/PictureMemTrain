#ifndef MEMRECORD_H
#define MEMRECORD_H
#include <QList>
#include <QPair>
#include "xlsxdocument.h"
#include "TrainController.h"
class MemRecord
{
public:
    MemRecord();
    bool load(const QString& excel,const QString& matchNum,QString* errInfo = nullptr);
private:
    QList<int> findRowsByMatchNum(QXlsx::Document& excel,const QString& matchNum);
    bool loadController(QXlsx::Document& excel,int row,TrainController& res,QString* errInfo = nullptr);
private:
    QList<TrainController> m_control;
    int m_matchNumCount;///< 匹配的个数
};

#endif // MEMRECORD_H
