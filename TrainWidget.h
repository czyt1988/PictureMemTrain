#ifndef TRAINWIDGET_H
#define TRAINWIDGET_H

#include <QWidget>
#include <QSet>
#include <QList>
#include <QHash>
#include <functional>
#include "TrainController.h"
#include "MemRecord.h"
namespace Ui {
class TrainWidget;
}

class QResizeEvent;
class PMTPixmapWidget;

class TrainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainWidget(QWidget *parent = nullptr);
    ~TrainWidget();

    void setMatchingNum(const QString& v);
    void setExpNum(const QString& v);
    void setAge(const QString& v);
    void setShortName(const QString& v);
    void setGender(bool isMale);

    PMT::TrainType getTrainType() const;
    void setTrainType(const PMT::TrainType &trainType, const MemRecordData &mr = MemRecordData());

    void resetXYNum(int xnum,int ynum);
    QRect getPicRect() const;
    void setPicRect(const QRect &picRect);

    QSize getPicSize() const;
    void setPicSize(const QSize &picSize);

    int getHspan() const;
    void setHspan(int hspan);

    int getVspan() const;
    void setVspan(int vspan);
    //显示说明
    void showIllustration(const QString& str,int ms=2000,std::function<void(void)> fn=nullptr);
    TrainController *getController() const;
    void setController(TrainController *controller);

    void setMemRecord(const MemRecordData &memRecord);

protected:
    void resizeEvent(QResizeEvent *e);
    void setPictureVisible(bool visible = true);
private:
    void recalcSize();
    QSize calcPrefectSize() const;
    //重新设置图片
    void resetPictureInGroup1();
    void resetPictureInGroup2();
    void resetPictureInGroup3();
    //
    void showTestLocation();
    //重置为测试模式
    void resetTestType();
    //设置为顺序记忆测试
    void setToOrderMemTestType();
    //设置为顺序记忆测试
    void setToLocationMemTestType();
    //重置为测试模式
    void resetFormalType();
    //根据PMTPixmapWidget查找索引
    int findIndexByPMTPixmapWidget(PMTPixmapWidget* w);

private slots:
    Q_SLOT void onFinishPictureMem(const QString& name,QDateTime clickedInTime,QDateTime clickedOutTime);
    Q_SLOT void onClicked(const QString& name);
    Q_SLOT void onSaveResultExcelErr();
    Q_SLOT void onFinish();
    void on_pushButtonOK_clicked();

    void on_pushButtonNo_clicked();

    void onAutoRunTimeout();
    void on_pushButtonSure_clicked();

signals:
    void showTooltip(const QString &message, int timeout);
    void finish(PMT::TrainType trainType);

private:
    Ui::TrainWidget *ui;
    PMT::TrainType m_trainType;///< 记录训练的类型
    QList<PMTPixmapWidget*> m_picList;
    QSize m_picSize;
    int m_hspan;///< 水平间距
    int m_vspan;///< 垂直间距
    QPoint m_topleftPoint;///< 记录第一个图形左上角坐标
    QHash<QString,PMTPixmapWidget*> m_nameToPMTWidgetGroup1;///< 记录名字对应的卡牌
    QHash<QString,PMTPixmapWidget*> m_nameToPMTWidgetGroup2;///< 记录名字对应的卡牌
    TrainController* m_controller;
    PMTPixmapWidget* m_currentClicked;
    QList<LocationTestValue> m_locationTestValues;///< 存放位置测试的内容
    int m_currentLocationTestIndex;///< 记录位置测试的索引
    QList<QPair<int,int> > m_trainOrder;///< 记录训练的顺序
    int m_trainOrderIndex;///< 训练的索引
    bool m_whenInSecondSessionDonotShowPicToMem;///< 标记当第二轮时不进行图片全部显示再次记忆
    QSet<PMTPixmapWidget*> m_selectedWidgetInRange; ///< 记录记忆阶段点击过的图片
    MemRecordData m_memRecord;
    int m_autoRunMemRecordDataIndex;///< 记录自动播放的学习轮次的位置
    int m_autoRunOneTrainRecordDataIndex;///<记录每轮，学习图片的索引位置
    //
    uint m_borderWidth;///< 边框的宽度
    int m_OKNOButtonFactor;///< 按钮和界面高度的系数
    int m_OKNOButtonMaxWidth;///< 测试模式下确定和取消按钮的最大宽度像素
    bool m_isAutoCheckOrderTest;///< 标记顺序测试是否自动确认
    //
    QList<QString> m_orderTestSelectPicNames;///< 记录顺序测试图片选择
};

#endif // TRAINWIDGET_H
