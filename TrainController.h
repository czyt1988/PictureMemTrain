#ifndef TRAINCONTROLLER_H
#define TRAINCONTROLLER_H
#include <QObject>
#include <QHash>
#include <PMTTestValue.h>
#include <QPixmap>
#include "Globals.h"
#include "MemRecord.h"
/**
 * @brief 此类用于控制训练逻辑
 *
 * 时间原因不使用状态机
 */
class TrainController : public QObject
{
    Q_OBJECT
public:
    enum GroupType
    {
        Group1
        ,Group2
        ,Group3
    };
    /**
     * @brief 性别
     */
    enum Gender
    {
        Male, ///< 男
        Female ///< 女
    };

    TrainController(QObject* par = nullptr);
    static QString getPicResourcesPath();
    //生成一个训练方案
    void makeProject(int tarinPicCount, int notTestCount);
    //获取图片
    QPixmap getPixmap(uint index,GroupType group = Group1) const;
    QPixmap getPixmap(const QString& name) const;
    //获取图片名
    QString getPixmapName(uint index,GroupType group = Group1) const;
    void setMatchingNum(const QString& v);
    void setExpNum(const QString& v);
    void setAge(const QString& v);
    void setShortName(const QString& v);
    void setGender(Gender gender);
    //
    int getXnum() const;
    void setXnum(int xnum);

    int getYnum() const;
    void setYnum(int ynum);

    int getTotalTrainPicCount() const;

    int getSelPicCount() const;
    int getMaxSelPicCount() const;
    bool isFinishSelPic() const;
    void appendOneSelRecord(const PMTTestSelRecord& r);
    void appendOrderMemTestRecord(const QString& name);
    void appendLocationMemTestRecord(bool selres);
    const QList<QString>& orderMemTestRecordRef() const;
    const QList<bool>& orderMemTestResult() const;
    void popOrderMemTestRecord();
    bool isFinishOrderMemTest() const;

    int getStartTestPicIndex() const;

    QList<LocationTestValue> getLocationTestValues() const;

    bool isFinishLocationMemTest() const;
    //保存结果
    void saveResult();
    void savePicTestOrder(const QList<int>& los);
    //移除原来的照片，插入新的照片
    void removeAndResetPicture();

    //设置图片序列
    bool setPicNameShowGroup1(const QList<QString> &picNameShowGroup1, QString *info);

private:
    void initResources();
    //建立图片组
    void buildPicGroup1();
    void buildPicGroup2();
    void buildPicGroup3();
    //随机循环位交换法进行洗牌
    static QList<QString> shufflePicName(const QList<QString> &org,uint shuffleLen);

    void randSelect(const QList<QString> &org,QList<QString>& res,int len);
    //重载训练参数
    void resetTrainPram();

signals:
    void finish();
    void saveResultExcelErr();
    void error(const QString& err);
private:
    friend class MemRecord;
    QString m_matchingNum;
    QString m_expNum;
    QString m_age;
    QString m_shortName;
    Gender m_gender;
    int m_xnum;
    int m_ynum;
    int m_totalTrainPicCount;///< 记录训练的图片个数
    int m_picNotTestCount;///< 此变量记录不参与测试的图片，如果是0，代表从第一张图片开始进行验证顺序和位置测试，也就是训练5张，顺序和位置测试也测试5张，如果定义为2，就是保留最后2张不测试，也就是记忆5张，顺序和位置测试3张
    bool m_isFinishSelPic;///< 标记选择图片阶段结束
    bool m_isFinishOrderMemTest;///< 标记图片顺序记忆阶段结束
    bool m_isFinishLocationMemTest;///< 标记位置记忆阶段结束
    QList<PMTTestSelRecord> m_selRecords;///< 记录选择阶段的内容
    QList<QString> m_orderMemSelName;///< 记录顺序测试阶段的选择名字
    QList<bool> m_orderMemTestResult;///< 记录顺序测试的结果
    QHash<QString,QPixmap> m_nameToPic;///< 记录所有图片
    QList<QString> m_names;///< 所有图片的名字
    QList<QString> m_picNameShowGroup1;///< 第1次呈现的图片队列
    QList<QString> m_picNameShowGroup2;///< 第2次呈现的图片队列 - 和队列1一样，不过顺序不一致
    QList<LocationTestValue> m_picNameShowGroup3;///< 第3次呈现的图片队列 - 此队列需要完成图片选择才可以生成
    QList<bool> m_locationMemTestResult;///< 记录位置测试的结果
    int m_firstNullRow;///< 第一组有内容的行
    QString m_xlsxIntvTimeFormat;///< 定义输出excel的时间间隔的格式,默认mm:ss.000
    QString m_xlsxDateTimeFormat;///< 定义输出excel的日期的格式 默认为yyyy-mm-dd hh:mm:ss.ms
};

#endif // TRAINCONTROLLER_H
