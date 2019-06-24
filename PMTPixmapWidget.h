#ifndef PMTPIXMAPWIDGET_H
#define PMTPIXMAPWIDGET_H

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QDateTime>
/**
 * @brief 针对此项目的图片选择控件
 */
class PMTPixmapWidget : public QWidget
{
    Q_OBJECT
public:

    /**
     * @brief 点击模式
     */
    enum ClickMode{
        ClickNothing ///< 点击不做任何响应
        ,ClickShowPicture ///< 点击显示图片，再点击隐藏图片，并触发消息
        ,ClickSelect///<点击选中，不做任何动作
    };

    PMTPixmapWidget(QWidget* par = nullptr);
    unsigned short getBorderWidth() const;
    PMTPixmapWidget& setBorderWidth(unsigned short border);

    QColor getBorderColor() const;
    PMTPixmapWidget& setBorderColor(const QColor &borderColor);

    int getRoundRatio() const;
    void setRoundRatio(int roundRatio);

    QColor getHoverBorderColor() const;
    void setHoverBorderColor(const QColor &hoverBorderColor);
    
    const QPixmap& getPixmap() const;
    void setPixmap(const QPixmap &pixmap,const QString& name);

    QBrush getBrushBackground() const;
    void setBrushBackground(const QBrush &brushBackground);

    //设置选择的边框颜色
    QColor getSelBorderColor() const;
    void setSelBorderColor(const QColor &selBorderColor);

    ClickMode getClickMode() const;
    void setClickActionMode(const ClickMode &clickMode);
    //显示图片，并延时显示空白
    void showDelayDisplayBlankSpace(int ms);
    //设置为显示照片
    void setShowPicture();
    //判断是否显示图片
    bool isShowPicture() const;
    //设置为点击选中状态，点击一下，显示红色选中边框，再次点击取消
    void setSelected(bool isAct = true);
    //判定是否选中
    bool isSelected() const;
    //设置显示画刷
    void setShowBrush();
    int getId() const;
    void setId(int id);

signals:
    void finishPictureMem(const QString& name,QDateTime clickedInTime,QDateTime clickedOutTime);
    void clicked(const QString& name);
protected:
    /**
     * @brief 显示模式
     */
    enum ViewMode{
        ViewPicture
        ,ViewBrush ///< 显示纯色
        ,ViewBlankSpace ///< 显示空
        ,ViewDelayDisplayBlankSpaceMode///<延时显示图片模式
    };
    ViewMode getViewMode() const;
    void setViewMode(const ViewMode &viewMode);
    int getDelayTime() const;
    void setDelayTime(int delayTime);
    void paintEvent(QPaintEvent* e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    virtual void paintViewPicture(QPainter& painter,const QRect& rect,int xRnd, int yRnd);
    virtual void paintViewBrush(QPainter& painter,const QRect& rect,int xRnd, int yRnd);
    virtual void paintViewBlankSpace(QPainter& painter,const QRect& rect,int xRnd, int yRnd);
    virtual void paintViewDelayDisplayBlankSpace(QPainter& painter,const QRect& rect,int xRnd, int yRnd);
private slots:
    Q_SLOT void onDelayTimeOut();
private:
    unsigned short m_borderWidth;
    QColor m_borderColor;
    QColor m_selBorderColor;
    QColor m_hoverBorderColor;
    int m_roundRatio;
    QPixmap m_pixmap;
    QBrush m_brushBackground;
    ViewMode m_viewMode;
    int m_delayTime;///< 延时时间
    bool m_isTimeOut;///< 标记是否到时间
    bool m_isClicked;///< 标记是否点击过
    ClickMode m_clickActionMode;
    QDateTime m_firstClickTime;
    bool m_isSel;///< 标记是否选中
    int m_id;
};

#endif // PMTPIXMAPWIDGET_H
