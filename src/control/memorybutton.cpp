/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     xiajing <xiajing@uniontech.com>
*
* Maintainer: jingzhou <jingzhou@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "memorybutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <DGuiApplicationHelper>

#include "dthememanager.h"

const qreal BLURRADIUS = 12; //阴影模糊半径
const qreal ROUND_XRADIUS = 8; //按钮圆角x轴半径
const qreal ROUND_YRADIUS = 8; //按钮圆角y轴半径

MemoryButton::MemoryButton(const QString &text, bool listwidgetbtn, QWidget *parent)
    : TextButton(text, parent)
    , m_effect(new QGraphicsDropShadowEffect(this))
    , m_isallgray(false)
{
    m_settings = DSettingsAlt::instance(this);
    setObjectName("MemoryButton");
    m_widgetbtn = listwidgetbtn; //是否是内存列表按键

    init();
    m_isHover = m_isPress = false;
    showtips();
    m_effect->setOffset(0, 4); //阴影偏移
    m_effect->setBlurRadius(BLURRADIUS); //阴影模糊半径
}

MemoryButton::~MemoryButton()
{
    // delete m_effect;
}

void MemoryButton::init()
{
    m_font.setPixelSize(16);
    m_font.setFamily("Noto Sans CJK SC");
    m_font.setStyleName("Light");
    if (!m_widgetbtn) {
        m_isgray = true; //初始化时按键均不可用
        this->setEnabled(false);
    }
}

/**
 * @brief 物理键盘按下动画效果
 * @param msec 100ms
 */
void MemoryButton::animate(bool isspace, int msec)
{
    if (m_isPress == false) { //edit for bug-20492 20200416
        m_isHover = false;  //edit for bug-20508 20200414
        if (!isspace)
            setDown(true);
        m_isPress = true;

        QTimer::singleShot(msec, this, [ = ] {
            if (!isspace)
                setDown(false);
            m_isPress = false;
            update();
        });
    }
}

/**
 * @brief 切换M˅，
 */
void MemoryButton::setbtnlight(bool light)
{
    int mode = m_settings->getOption("mode").toInt();
    if (mode != 1) {
        if (light == true)
            setText("M˄");
        else
            setText("M˅");
    } else {
        if (light == true)
            setText("MH˄");
        else
            setText("MH˅");
    }
}

/**
 * @brief 设置tooltip
 */
void MemoryButton::showtips()
{
    QString tooltext;
    if (this->text() == "MC") {
        if (m_widgetbtn)
            tooltext = tr("Clear memory item");
        else
            tooltext = tr("Clear all memory");
    } else if (this->text() == "MR")
        tooltext = tr("Memory recall");
    else if (this->text() == "M+") {
        if (m_widgetbtn)
            tooltext = tr("Add to memory item");
        else
            tooltext = tr("Memory add");
    } else if (this->text() == "M-") {
        if (m_widgetbtn)
            tooltext = tr("Subtract from memory item");
        else
            tooltext = tr("Memory subtract");
    } else if (this->text() == "MS")
        tooltext = tr("Memory store");
    else
        tooltext = tr("Memory");
    this->setToolTip(tooltext);
}

/**
 * @brief MemoryButton::setbuttongray
 * @param memorywidgetshow 历史区域是否显示
 * 历史区域显示，除mlist外按钮置灰
 */
void MemoryButton:: setbuttongray(bool memorywidgetshow)
{
    //20200701 精简代码
    m_isallgray = memorywidgetshow;
    update();
}

/**
 * @brief IconButton::updateWhenBtnDisable
 * 当拥有焦点时同时按下空格和鼠标后会导致问题，将其置回普通状态
 */
void MemoryButton::updateWhenBtnDisable()
{
    this->setPalette(m_palette);
    m_isPress = false;
}

/**
 * @brief 鼠标按下
 */
void MemoryButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        return;
//    setFocus();
    m_palette = this->palette();
    m_isPress = true;
    m_isHover = false; //20200722删除foucus状态
    emit updateInterface();
    DPushButton::mousePressEvent(e);
}

/**
 * @brief 鼠标松开
 */
void MemoryButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        return;
    this->setPalette(m_palette);
    m_isPress = false;
    if (this->rect().contains(e->pos())) {
        m_isacting = true;
        m_isHover = true;
    } else
        m_isacting = false;
    DPushButton::mouseReleaseEvent(e);
}

void MemoryButton::enterEvent(QEvent *e)
{
    m_font.setPixelSize(17);
    m_font.setStyleName("");
    m_isHover = true;
    DPushButton::enterEvent(e);
}

void MemoryButton::leaveEvent(QEvent *e)
{
    m_font.setPixelSize(16);
    m_font.setStyleName("Light");
    m_isHover = false;
    m_isacting = false;
    DPushButton::leaveEvent(e);
}

void MemoryButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (isEnabled() == false) {
        m_font.setPixelSize(16);
        m_font.setStyleName("Light");
        m_isHover = false;
        m_isgray = true;
    } else {
        m_isgray = false;
    }
    QRectF rect = this->rect();
    QRectF normal(rect.left() + 1, rect.top() + 1, rect.width() - 2, rect.height() - 2);
    QRectF hover(rect.left() + 1, rect.top() + 1, rect.width() - 2, rect.height() - 2);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); //反锯齿
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true); //光滑的象素映射变换算法
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setFont(m_font);
    QRectF textRect = painter.fontMetrics().boundingRect(0, 0, int(rect.width()), int(rect.height()),
                                                         Qt::AlignCenter, this->text());
    QColor actcolor = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color().name();//活动色
    QColor pressBrush, focus, hoverFrame, base, text, hoverbrush;
    QColor pressText = actcolor;
    QColor hoverShadow, focusShadow;
    hoverShadow = actcolor;
    hoverShadow.setAlphaF(0.1);
    focusShadow = QColor(0, 0, 0);
    focusShadow.setAlphaF(0.05);
    int type = DGuiApplicationHelper::instance()->themeType();
    if (type == 1) {
        pressBrush = QColor(0, 0, 0);
        pressBrush.setAlphaF(0.1);
        focus = actcolor;
        hoverFrame = actcolor;
        hoverFrame.setAlphaF(0.2);
        base = Qt::white;
        hoverbrush = Qt::white;
        text = QColor("#303030");
        if (m_isgray == true || m_isallgray == true) {
            base = QColor("#FFFFFF");
            hoverbrush = QColor("#FFFFFF");
            text = QColor("#303030");
            text.setAlphaF(0.4);
            pressBrush = QColor("#FFFFFF");
        }
        if (this->text() == "M˄" || this->text() == "MH˄") {
            text = actcolor;
        }
    } else {
        pressBrush = QColor(0, 0, 0);
        pressBrush.setAlphaF(0.5);
        focus = actcolor;
        hoverFrame = actcolor;
        hoverFrame.setAlphaF(0.2);
        if (m_widgetbtn == false)
            base = QColor(48, 48, 48);
        else
            base = QColor(65, 65, 65);
        if (m_isHover)
            text = Qt::white;
        else
            text = QColor(224, 224, 224);
        hoverbrush = QColor(60, 60, 60);
        if (m_isgray == true || m_isallgray == true) {
            base = QColor(48, 48, 48);
            base.setAlphaF(0.4);
            text = QColor(224, 224, 224);
            text.setAlphaF(0.4);
            pressText = Qt::black;
            pressBrush = QColor("#FFFFFF");
            hoverbrush = QColor(48, 48, 48);
            hoverbrush.setAlphaF(0.4);
            if (m_widgetbtn) { //内存界面中的memorybutton在深色模式下底色不同
                base = QColor(65, 65, 65);
                text = QColor(224, 224, 224);
                text.setAlphaF(0.3);
            }
        }
        if (this->text() == "M˄" || this->text() == "MH˄") {
            text = actcolor;
        }
    }
    if (hasFocus()) {
        painter.setPen(Qt::NoPen);
        if (m_isPress) {
            painter.setBrush(QBrush(pressBrush));
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素
            QPen pen;
            pen.setColor(pressText);
            painter.setPen(pen);
            painter.setFont(m_font);
            pen.setColor(focus);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //focus边框
            if (this->text() == "MH˅" || this->text() == "MH˄") {
                this->paintTextNormal(painter, normal);
            } else if (this->text() == "M˅" || this->text() == "M˄") {
                this->paintTextNormalM(painter, textRect);
            } else
                painter.drawText(textRect, this->text());
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(base));
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素
            QPen pen;
            pen.setColor(focus);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素

            pen.setColor(text);
            painter.setPen(pen);
            painter.setFont(m_font);
            if (this->text() == "MH˅" || this->text() == "MH˄") {
                this->paintTextNormal(painter, normal);
            } else if (this->text() == "M˅" || this->text() == "M˄") {
                this->paintTextNormalM(painter, textRect);
            } else
                painter.drawText(textRect, this->text());
            m_effect->setColor(focusShadow);
            this->setGraphicsEffect(m_effect);
        }
    } else {
        painter.setPen(Qt::NoPen);
        if (m_isHover) { //hover状态
            painter.setPen(QPen(hoverFrame));
            painter.setBrush(QBrush(hoverbrush));
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素

            QPen pen;
            pen.setColor(text);
            painter.setPen(pen);
            painter.setFont(m_font);
            if (this->text() == "MH˅" || this->text() == "MH˄") {
                this->paintTextHover(painter, normal);
            } else if (this->text() == "M˅" || this->text() == "M˄") {
                this->paintTextHoverM(painter, textRect);
            } else
                painter.drawText(textRect, this->text());
            m_effect->setColor(hoverShadow);
            this->setGraphicsEffect(m_effect);
        } else if (m_isPress) { //press状态
            painter.setBrush(QBrush(pressBrush));
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素
            QPen pen;
            pen.setColor(pressText);
            painter.setPen(pen);
            painter.setFont(m_font);
            if (this->text() == "MH˅" || this->text() == "MH˄") {
                this->paintTextHover(painter, normal);
            } else if (this->text() == "M˅" || this->text() == "M˄") {
                this->paintTextHoverM(painter, textRect);
            } else
                painter.drawText(textRect, this->text());
        } else { //normal状态
            painter.setBrush(QBrush(base));
            painter.drawRoundedRect(normal, ROUND_XRADIUS, ROUND_YRADIUS); //圆角半径单位为像素
            QPen pen;
            pen.setColor(text);
            painter.setPen(pen);
            painter.setFont(m_font);
            if (this->text() == "MH˅" || this->text() == "MH˄") {
                this->paintTextNormal(painter, normal);
            } else if (this->text() == "M˅" || this->text() == "M˄") {
                this->paintTextNormalM(painter, textRect);
            } else
                painter.drawText(textRect, this->text());
            m_effect->setColor(QColor(0, 0, 0, 0));
            this->setGraphicsEffect(m_effect);
        }
    }
}

void MemoryButton::focusOutEvent(QFocusEvent *e)
{
    emit updateInterface();
    QPushButton::focusOutEvent(e);
}

void MemoryButton::paintTextNormal(QPainter &painter, QRectF normal)
{
    painter.save();
    if (this->text() == "MH˄") {
        QRectF textRect1 = QRectF(normal.left() + 22, normal.top() + 7, 10, 17);
        QFont a1;
        a1.setPixelSize(12);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = QRectF(normal.left() + 29, normal.top() + 12, 10, 17);
        painter.drawLine(textRect2.bottomLeft(), textRect2.topRight());
        QRectF textRect3 = QRectF(normal.left() + 37, normal.top() + 14, 9, 17);
        painter.drawText(textRect3, "H");
        QRectF textRect4 = QRectF(normal.left() + 45, normal.top() + 5, 9, 17);
        QFont a2;
        a2.setPixelSize(20);
        a2.setFamily("Noto Sans CJK SC");
        painter.setFont(a2);
        painter.drawText(textRect4, "˄");
    } else if (this->text() == "MH˅") {
        QRectF textRect1 = QRectF(normal.left() + 22, normal.top() + 7, 10, 17);
        QFont a1;
        a1.setPixelSize(12);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = QRectF(normal.left() + 29, normal.top() + 12, 10, 17);
        painter.drawLine(textRect2.bottomLeft(), textRect2.topRight());
        QRectF textRect3 = QRectF(normal.left() + 37, normal.top() + 14, 9, 17);
        painter.drawText(textRect3, "H");
        QRectF textRect4 = QRectF(normal.left() + 45, normal.top() + 5, 9, 17);
        QFont a2;
        a2.setPixelSize(20);
        a2.setFamily("Noto Sans CJK SC");
        painter.setFont(a2);
        painter.drawText(textRect4, "˅");
    }
    painter.restore();
}

void MemoryButton::paintTextHover(QPainter &painter, QRectF normal)
{
    painter.save();
    if (this->text() == "MH˅") {
        QRectF textRect1 = QRectF(normal.left() + 22, normal.top() + 7, 12, 17);
        QFont a1;
        a1.setPixelSize(13);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = QRectF(normal.left() + 30, normal.top() + 12, 10, 17);
        painter.drawLine(textRect2.bottomLeft(), textRect2.topRight());
        QRectF textRect3 = QRectF(normal.left() + 37, normal.top() + 14, 9, 17);
        painter.drawText(textRect3, "H");
        QRectF textRect4 = QRectF(normal.left() + 45, normal.top() + 5, 9, 17);
        QFont a4;
        a4.setPixelSize(22);
        a4.setFamily("Noto Sans CJK SC");
        painter.setFont(a4);
        painter.drawText(textRect4, "˅");
    }
    painter.restore();
}

void MemoryButton::paintTextNormalM(QPainter &painter, QRectF normal)
{
    Q_UNUSED(normal)
    painter.save();
    QRectF rect = this->rect();
    int vpadding = rect.width() < 69 ? 4 : 2;
    if (this->text() == "M˄") {
        QRectF textRect1 = painter.fontMetrics().boundingRect(0, 0, int(rect.width()), int(rect.height()),
                                                              Qt::AlignCenter, "M²");
        QFont a1;
        a1.setPixelSize(16);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = painter.fontMetrics().boundingRect(int(rect.width()) / 2 + 4, int(rect.height() / 4 - vpadding),
                                                              int(rect.width()) / 2, int(rect.height() / 2),
                                                              Qt::AlignLeft, "^");
        QFont a2;
        a2.setPixelSize(10);
        a2.setFamily("Noto Sans CJK SC");
        painter.setFont(a2);
        painter.drawText(textRect2, "^");
    } else if (this->text() == "M˅") {
        QRectF textRect1 = painter.fontMetrics().boundingRect(0, 0, int(rect.width()), int(rect.height()),
                                                              Qt::AlignCenter, "M²");
        QFont a1;
        a1.setPixelSize(16);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = painter.fontMetrics().boundingRect(int(rect.width()) / 2 + 4, int(rect.height() / 4 - vpadding),
                                                              int(rect.width()) / 2, int(rect.height() / 2),
                                                              Qt::AlignLeft, "v");
        QFont a2;
        a2.setPixelSize(10);
        a2.setFamily("Noto Sans CJK SC");
        painter.setFont(a2);
        painter.drawText(textRect2, "v");
    }
    painter.restore();
}

void MemoryButton::paintTextHoverM(QPainter &painter, QRectF normal)
{
    Q_UNUSED(normal)
    painter.save();
    QRectF rect = this->rect();
    int vpadding = rect.width() < 69 ? 6 : 4;
    if (this->text() == "M˅") {
        QRectF textRect1 = painter.fontMetrics().boundingRect(0, 0, int(rect.width()), int(rect.height()),
                                                              Qt::AlignCenter, "M²");
        QFont a1;
        a1.setPixelSize(17);
        a1.setFamily("Noto Sans CJK SC");
        painter.setFont(a1);
        painter.drawText(textRect1, "M");
        QRectF textRect2 = painter.fontMetrics().boundingRect(int(rect.width()) / 2 + 4, int(rect.height() / 4 - vpadding),
                                                              int(rect.width()) / 2, int(rect.height() / 2),
                                                              Qt::AlignLeft, "v");
        QFont a4;
        a4.setPixelSize(11);
        a4.setFamily("Noto Sans CJK SC");
        painter.setFont(a4);
        painter.drawText(textRect2, "v");
    }
    painter.restore();
}
