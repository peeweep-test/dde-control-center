#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QBitmap>
#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>

#include <libdui/dtextbutton.h>

#include "previewwindow.h"

PreviewWindow::PreviewWindow(QWidget *parent) :
    MouseArea(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initUI();
}

PreviewWindow::~PreviewWindow()
{
}

void PreviewWindow::setImages(const QStringList &list)
{
    if(list.isEmpty())
        return;

    m_imageList.clear();
    m_pixmap_x = 0;
    foreach (const QString &str, list) {
        QPixmap pixmap;
        if(pixmap.load(str))
            m_imageList << pixmap;
    }
    update();
}

void PreviewWindow::show(const QString &key)
{
    if(m_imageList.isEmpty()){
        qWarning() << "no call setImages;";
        return;
    }

    m_key = key;

    QWidget *w = qobject_cast<QWidget*>(sender());
    if(w){
        m_controlCenterWindow = w->window();
        m_controlCenterWindow->setProperty("canNotHide", true);
        bool hideInLeft = m_controlCenterWindow->property("hideInLeft").toBool();
        QRect rect = qApp->primaryScreen()->geometry();
        if(hideInLeft){
            move(qMin(rect.right() - width() - 10, m_controlCenterWindow->geometry().right() + 10), rect.top() + 10);
        }else{
            move(w->window()->x() - width() - 10, rect.top() + 10);
        }
    }
    QWidget::show();
}

void PreviewWindow::close()
{
    if(m_controlCenterWindow)
        m_controlCenterWindow->setProperty("canNotHide", false);
    MouseArea::close();
}

void PreviewWindow::updateCursor()
{
    QPoint p = mousePos();

    if(m_show_arrow_rect.contains(p)){
        QString imageName = (pressed()?"pressed":"normal")+QString(".png");
        if(p.x() > width() / 2)
            setCursor(QPixmap(":/images/right_"+imageName));
        else
            setCursor(QPixmap(":/images/left_"+imageName));
        hideTopRect();
    }else{
        setCursor(Qt::ArrowCursor);
        if(p.y() < 80)
            showTopRect();
    }
}

void PreviewWindow::paintEvent(QPaintEvent *e)
{
    MouseArea::paintEvent(e);

    QPainter pa(this);
    pa.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 3, 3);
    pa.setClipPath(path);
    pa.drawPixmap(m_pixmap_x, 0, m_imageList[m_image_index_left]);
    pa.drawPixmap(m_pixmap_x + width(), 0, m_imageList[m_image_index_right]);

    QRect bottom_rect;
    bottom_rect.setSize(QSize(width(), 50));
    bottom_rect.moveBottom(rect().bottom());
    pa.fillRect(bottom_rect, QColor("#66000000"));
    if(m_top_rect_opacity > 0){
        pa.setOpacity(m_top_rect_opacity);
        bottom_rect.moveTop(0);
        pa.fillRect(bottom_rect, Qt::black);
    }
    pa.end();
}

void PreviewWindow::initUI()
{
    setMouseTracking(true);
    resize(300, 500);

    m_show_arrow_rect.setSize(size());
    m_show_arrow_rect.setTop(80);
    m_show_arrow_rect.setBottom(rect().bottom() - 50);

    m_closeButton = new DWindowCloseButton(this);
    m_closeButton->move(260, 10);
    m_closeButton->hide();

    DTextButton *apply_button = new DTextButton(tr("Apply"), this);
    apply_button->setStyleSheet("QPushButton{background:transparent;color:#b4b4b4;border:none;padding:8px;padding-top:2px;padding-bottom:2px;}"
                                "QPushButton:hover{background:#55555555;border:1px inset #aaa;border-radius:3px;}"
                                "QPushButton:pressed{color:#2CA7F8;}");
    apply_button->move(230, 465);

    connect(m_closeButton, &DWindowCloseButton::clicked, this, &PreviewWindow::close);
    connect(apply_button, &DTextButton::clicked, this, [this]{
        emit apply(m_key);
    });
    connect(this, &PreviewWindow::positionChanged, this, &PreviewWindow::updateCursor);
    connect(this, &PreviewWindow::pressedChanged, this, &PreviewWindow::updateCursor);
    connect(this, &PreviewWindow::clicked, [this]{
        if(cursor().shape() == Qt::ArrowCursor)
            return;

        if(mousePos().x() > width() / 2)
            imageToRight();
        else
            imageToLeft();
    });
    connect(&m_animation_image_switch, &QVariantAnimation::valueChanged, [this](const QVariant &var){
        m_pixmap_x = var.toInt();
        update();
    });
    connect(&m_animation_rect_opacity, &QVariantAnimation::valueChanged, [this](const QVariant &var){
        m_top_rect_opacity = var.toReal();
        update();
    });
}

void PreviewWindow::imageToRight()
{
    if(m_animation_image_switch.state() == QVariantAnimation::Running || m_imageList.count() < 2)
        return;

    if(m_pixmap_x != 0)
        m_image_index_left = m_image_index_right;
    m_image_index_right = (m_image_index_left + m_imageList.count() + 1) % m_imageList.count();

    m_animation_image_switch.setStartValue(0);
    m_animation_image_switch.setEndValue(- width());
    m_animation_image_switch.start();
}

void PreviewWindow::imageToLeft()
{
    if(m_animation_image_switch.state() == QVariantAnimation::Running || m_imageList.count() < 2)
        return;

    if(m_pixmap_x == 0)
        m_image_index_right = m_image_index_left;
    m_image_index_left = (m_image_index_left + m_imageList.count() - 1) % m_imageList.count();

    m_animation_image_switch.setStartValue(-width());
    m_animation_image_switch.setEndValue(0);
    m_animation_image_switch.start();
}

void PreviewWindow::showTopRect()
{
    if(m_top_rect_opacity > 0)
        return;
    m_animation_rect_opacity.setStartValue(0.0);
    m_animation_rect_opacity.setEndValue(0.7);
    m_animation_rect_opacity.start();
    m_closeButton->show();
}

void PreviewWindow::hideTopRect()
{
    if(m_top_rect_opacity == 0)
        return;
    m_animation_rect_opacity.setStartValue(0.7);
    m_animation_rect_opacity.setEndValue(0.0);
    m_animation_rect_opacity.start();
    m_closeButton->hide();
}
