#ifndef PMTTESTVALUE_H
#define PMTTESTVALUE_H
#include <QDateTime>
#include <QList>

class PMTTestSelRecord
{
public:
    PMTTestSelRecord();
    QDateTime picShowTime;///< 图片开始时间
    QDateTime picDisappearTime;///< 图片消失时间
    QString picName;///< 图片的名字
    int location;///< 记录这次测试的位置
    QString testRecordName;///< 这次测试的名字
    int testPicCount;///< 记录这次测试需要测试图片的总数
    int testRange;///< 记录这是第几轮测试
    //计算间隔时间
    int intervalMs() const;
};

/**
 * @brief 位置测试的记录集
 */
class LocationTestValue
{
public:
    QString picName;///< 图片名
    int realLocation;///< 真实的位置
    int showLocation;///< 显示的位置
    bool isCorrect() const;
};

#endif // PMTTESTVALUE_H
