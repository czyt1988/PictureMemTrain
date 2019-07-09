#include "PMTPixmapWidget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QTimer>
#include <QDebug>
#include <QSettings>

PMTPixmapWidget::PMTPixmapWidget(QWidget *par):QWidget(par)
  ,m_brushBackground(QColor(208,206,206))
  ,m_viewMode(ViewPicture)
  ,m_delayTime(2000)
  ,m_isTimeOut(false)
  ,m_isClicked(false)
  ,m_clickActionMode(ClickShowPicture)
  ,m_firstClickTime(QDateTime::currentDateTime())
  ,m_isSel(false)
  ,m_xr(25)
  ,m_yr(25)
{
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
    setBorderWidth(3);
    setBorderColor(QColor(175,171,171));
    setHoverBorderColor(QColor(102,207,255));
    setSelBorderColor(QColor(245,18,67));
    setRoundRatio(5);
    QSettings settings("PictureMemTrain.ini", QSettings::IniFormat);
    m_xr = settings.value("ui/xRnd",25).toInt();
    m_yr = settings.value("ui/yRnd",25).toInt();
}



QColor PMTPixmapWidget::getBorderColor() const
{
    return m_borderColor;
}

PMTPixmapWidget& PMTPixmapWidget::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
    return *this;
}

unsigned short PMTPixmapWidget::getBorderWidth() const
{
    return m_borderWidth;
}

PMTPixmapWidget& PMTPixmapWidget::setBorderWidth(unsigned short border)
{
    m_borderWidth = border;
    return *this;
}

void PMTPixmapWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor borderColor = underMouse() ? getHoverBorderColor() : getBorderColor();
    if(hasFocus())
    {
        borderColor = m_selBorderColor;
    }

//    if((ClickSelect == m_clickActionMode||ClickShowPicture==m_clickActionMode) && m_isSel)
//    {
//        borderColor = m_selBorderColor;
//    }
    if(m_isSel)
    {
        borderColor = m_selBorderColor;
    }
    else
    {
        borderColor = getBorderColor();
    }
    if(ViewDelayDisplayBlankSpaceMode == m_viewMode)
    {
        borderColor = getBorderColor();
    }
    QPen pen(borderColor);
    pen.setWidth(getBorderWidth());
    painter.setPen(pen);
    //绘制边框
    QRect r = rect().adjusted(m_borderWidth,m_borderWidth,-m_borderWidth,-m_borderWidth);
    int xRnd = m_xr;
    int yRnd = m_yr;
    painter.drawRoundedRect(r,xRnd,yRnd,Qt::RelativeSize);
    //计算绘制图片的区域
    switch(m_viewMode)
    {
    case ViewPicture:
        paintViewPicture(painter,r,xRnd,yRnd);
        break;
    case ViewBrush:
        paintViewBrush(painter,r,xRnd,yRnd);
        break;
    case ViewDelayDisplayBlankSpaceMode:
        paintViewDelayDisplayBlankSpace(painter,r,xRnd,yRnd);
        break;
    case ViewBlankSpace:
        paintViewBlankSpace(painter,r,xRnd,yRnd);
        break;
    default:
        break;
    }
    painter.drawRoundedRect(r,xRnd,yRnd,Qt::RelativeSize);
}

void PMTPixmapWidget::paintViewPicture(QPainter &painter, const QRect &rect, int xRnd, int yRnd)
{
    int offset = rect.width()*0.05;
    QRect drawRange = rect.adjusted(offset,offset,-offset,-offset);
    painter.drawPixmap(drawRange,getPixmap());
}

void PMTPixmapWidget::paintViewBrush(QPainter &painter, const QRect &rect, int xRnd, int yRnd)
{
    painter.save();
    painter.setBrush(getBrushBackground());
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect.adjusted(1,1,-1,-1),xRnd,yRnd,Qt::RelativeSize);
    painter.restore();
}

void PMTPixmapWidget::paintViewBlankSpace(QPainter &painter, const QRect &rect, int xRnd, int yRnd)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    Q_UNUSED(xRnd);
    Q_UNUSED(yRnd);
}

void PMTPixmapWidget::paintViewDelayDisplayBlankSpace(QPainter &painter, const QRect &rect, int xRnd, int yRnd)
{
    if(m_isTimeOut)
    {
        paintViewBlankSpace(painter,rect,xRnd,yRnd);
    }
    else
    {
        paintViewPicture(painter,rect,xRnd,yRnd);
    }
}

void PMTPixmapWidget::onDelayTimeOut()
{
    m_isTimeOut = true;
    setViewMode(ViewBlankSpace);
}

int PMTPixmapWidget::getId() const
{
    return m_id;
}

void PMTPixmapWidget::setId(int id)
{
    m_id = id;
}

PMTPixmapWidget::ClickMode PMTPixmapWidget::getClickMode() const
{
    return m_clickActionMode;
}

void PMTPixmapWidget::setClickActionMode(const ClickMode &clickMode)
{
    m_clickActionMode = clickMode;
}

void PMTPixmapWidget::showDelayDisplayBlankSpace(int ms)
{
    setDelayTime(ms);
    setViewMode(ViewDelayDisplayBlankSpaceMode);
    repaint();
}

void PMTPixmapWidget::showPicture()
{
    setViewMode(ViewPicture);
}

void PMTPixmapWidget::showBlankSpace()
{
    setViewMode(ViewBlankSpace);
}

bool PMTPixmapWidget::isShowPicture() const
{
    return getViewMode() == ViewPicture;
}

void PMTPixmapWidget::setSelected(bool isAct)
{
    bool needrepain = (isSelected() != isAct);
    m_isSel = isAct;
    if(needrepain)
    {
        repaint();
    }
}

bool PMTPixmapWidget::isSelected() const
{
    return m_isSel;
}

void PMTPixmapWidget::setShowBrush()
{
    setViewMode(ViewBrush);
}

QColor PMTPixmapWidget::getSelBorderColor() const
{
    return m_selBorderColor;
}

void PMTPixmapWidget::setSelBorderColor(const QColor &selBorderColor)
{
    m_selBorderColor = selBorderColor;
}

int PMTPixmapWidget::getDelayTime() const
{
    return m_delayTime;
}

void PMTPixmapWidget::setDelayTime(int delayTime)
{
    m_delayTime = delayTime;
}

QBrush PMTPixmapWidget::getBrushBackground() const
{
    return m_brushBackground;
}

void PMTPixmapWidget::setBrushBackground(const QBrush &brushBackground)
{
    m_brushBackground = brushBackground;
}

void PMTPixmapWidget::enterEvent(QEvent *e)
{
    repaint();
    QWidget::enterEvent(e);
}

void PMTPixmapWidget::leaveEvent(QEvent *e)
{
    repaint();
    QWidget::leaveEvent(e);
}

void PMTPixmapWidget::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if(ClickNothing == getClickMode() || ViewDelayDisplayBlankSpaceMode == getViewMode())
    {
        return;
    }
    m_isSel = !m_isSel;
    emit clicked(windowTitle());
    if(ClickShowPicture == getClickMode())
    {
        //非选择模式，点击切换图形
        if(!m_isClicked)
        {
            //没有点击过，就显示图片
            m_isClicked = true;
            m_firstClickTime = QDateTime::currentDateTime();
            setViewMode(ViewPicture);
        }
        else {
            m_isClicked = false;
            setViewMode(ViewBlankSpace);
            emit finishPictureMem(windowTitle(),m_firstClickTime,QDateTime::currentDateTime());
        }
    }

}



PMTPixmapWidget::ViewMode PMTPixmapWidget::getViewMode() const
{
    return m_viewMode;
}

void PMTPixmapWidget::setViewMode(const PMTPixmapWidget::ViewMode &viewMode)
{
    m_viewMode = viewMode;
    if(ViewDelayDisplayBlankSpaceMode == viewMode)
    {
        //处于延时显示模式，先显示图片，后出发显示空白
        m_isTimeOut = false;
        QTimer::singleShot(m_delayTime,this,&PMTPixmapWidget::onDelayTimeOut);
    }
    repaint();
}

const QPixmap &PMTPixmapWidget::getPixmap() const
{
    return m_pixmap;
}

void PMTPixmapWidget::setPixmap(const QPixmap &pixmap,const QString& name)
{
    m_pixmap = pixmap;
    if(m_pixmap.isNull())
    {
        qDebug() << "PMTPixmapWidget::setPixmap is null:" << name;
    }
    setWindowTitle(name);
    repaint();
}

QColor PMTPixmapWidget::getHoverBorderColor() const
{
    return m_hoverBorderColor;
}

void PMTPixmapWidget::setHoverBorderColor(const QColor &hoverBorderColor)
{
    m_hoverBorderColor = hoverBorderColor;
}

int PMTPixmapWidget::getRoundRatio() const
{
    return m_roundRatio;
}

void PMTPixmapWidget::setRoundRatio(int roundRatio)
{
    m_roundRatio = roundRatio;
}
