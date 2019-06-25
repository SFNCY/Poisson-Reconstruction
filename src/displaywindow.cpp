#include "displaywindow.h"

DisplayWindow::DisplayWindow(QWidget *parent, OperatType mode)
    :   QWidget(parent),
      m_operarMod(mode),
      m_translating(false),
      m_scaling(true),
      m_croping(false),
      m_keyCtrl(false),
      m_keyAlt(false)
{
    m_displayConstPos=QPoint(0, 0);
    m_displayRatio=1;
    m_cropingRect=QRect();
    m_size=size();
    setFocusPolicy(Qt::StrongFocus);
}

DisplayWindow::~DisplayWindow()
{
}

void DisplayWindow::display(
        const cv::Mat &image,
        bool resetDisplay)
{
    // qDebug()<<"DisplayWindow::display()";
    if (image.channels()>1) {
        cvtColor(image,m_loadMat,CV_BGR2RGB);
    } else {
        cvtColor(image,m_loadMat,CV_GRAY2RGB);
    }
    QImage qImage;
    matToQImage(m_loadMat,qImage);
    setPixmap(QPixmap::fromImage(qImage),resetDisplay);
}

void DisplayWindow::setPixmap(
        const QPixmap &inputImage,
        bool resetDisplay)
{
    // qDebug()<<"DisplayWindow::setPixmap()";
    if (m_loadPixmap.size()==inputImage.size() &&
            m_loadPixmap.depth()==inputImage.depth() &&
            !resetDisplay) {
        m_loadPixmap=inputImage.copy(inputImage.rect());
    } else {
        m_loadPixmap=inputImage.copy(inputImage.rect());
        double pictureWidth=m_loadPixmap.size().width();
        double pictureHeight=m_loadPixmap.size().height();
        m_displayConstPos=QPoint(0, 0);
        if (size().width()<size().height()) {
            m_displayRatio=double(size().width())/pictureWidth;
        } else {
            m_displayRatio=double(size().height())/pictureHeight;
        }
    }
    m_cropingRect=QRect();
    m_mulitCropingRect.clear();
    update();
}

void DisplayWindow::setPixmap(
        const QPixmap &inputImage,
        QPoint centerPointForShow)
{
    // qDebug()<<"DisplayWindow::setPixmap()";
    if (m_loadPixmap.size()==inputImage.size() &&
            m_loadPixmap.depth()==inputImage.depth()) {
        m_loadPixmap=inputImage.copy(inputImage.rect());
        m_displayConstPos=centerPointForShow*m_displayRatio-
                QPoint(100,
                       100);
        if (m_displayConstPos.x()<0) {
            m_displayConstPos.setX(1);
        }
        if (m_displayConstPos.y()<0) {
            m_displayConstPos.setY(1);
        }
    } else {
        m_loadPixmap=inputImage.copy(inputImage.rect());
        double pictureWidth=m_loadPixmap.size().width();
        double pictureHeight=m_loadPixmap.size().height();
        m_displayConstPos=QPoint(0, 0);
        if (size().width()<size().height()) {
            m_displayRatio=double(size().width())/pictureWidth;
        } else {
            m_displayRatio=double(size().height())/pictureHeight;
        }
    }
    m_cropingRect=QRect();
    m_mulitCropingRect.clear();
    update();
}

void DisplayWindow::clear()
{
    m_loadPixmap=QPixmap();
}

void DisplayWindow::mousePressEvent(QMouseEvent *event)
{
    //    qDebug()<<"DisplayWindow::mousePressEvent()";
    if (!m_loadPixmap.isNull()) {
        switch (event->button()) {
        case Qt::LeftButton:
            if (m_keyCtrl) {
                switch (m_operarMod) {
                case DISPLAY:
                    break;
                case CROP:
                case MULTICROP:
                    qDebug()<<"Croping init";
                    m_cropingRect=QRect();
                    m_mousePosBefor= event->pos();
                    m_croping=true;
                    break;
                case MARKS:
                    break;
                default:
                    break;
                }
                break;
            } else if (m_keyAlt) {
                break;
            } else {
                m_translating=true;
                m_mousePosBefor= event->pos();
                break;
            }
        case Qt::RightButton:
            break;
        case Qt::MidButton:
            break;
        default:
            break;
        }
        m_scaling=false;
    }
    return QWidget::mousePressEvent(event);
}

void DisplayWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_translating) {
        QPoint dd = m_mousePosBefor-event->pos();
        m_mousePosBefor = event->pos();
        QSize scaledSize=m_loadPixmap.size()*m_displayRatio;
        QRect windowRect=QRect(
                    QPoint(0,0),
                    size()
                    );
        QPoint tempP=m_displayConstPos+dd;
        QRect movedScaledImageRect=QRect(
                    -tempP,
                    scaledSize
                    );
        QRect overlapRect=windowRect.intersected(movedScaledImageRect);
        if ( !overlapRect.isEmpty() ) {
            m_displayConstPos += dd;
        }
        update();
    }
    if (m_croping) {
        m_cropingRect=QRect(m_mousePosBefor,event->pos());
        emit s_cropSizeChanged(screenToImage(m_cropingRect.size()));
        update();
    }
    return QWidget::mouseMoveEvent(event);
}

void DisplayWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //    qDebug()<<"DisplayWindow::mouseReleaseEvent()";
    m_scaling=true;
    m_translating=false;
    m_croping=false;
    return QWidget::mouseReleaseEvent(event);
}

void DisplayWindow::wheelEvent(QWheelEvent * event)
{
    if (!m_loadPixmap.isNull()
            && m_scaling) {
        if(event->delta() > 0){
            QSize scaledSize=m_loadPixmap.size()*(m_displayRatio+0.05);
            QRect scaledImageRect=QRect(
                        QPoint(-scaledSize.width(),-scaledSize.height()),
                        2*scaledSize
                        );
            if (scaledImageRect.contains(m_displayConstPos)){
                m_displayRatio+=0.05;
            }
        }else{
            QSize scaledSize=m_loadPixmap.size()*(m_displayRatio-0.05);
            QRect scaledImageRect=QRect(
                        QPoint(-scaledSize.width(),-scaledSize.height()),
                        2*scaledSize
                        );
            if (scaledImageRect.contains(m_displayConstPos)){
                m_displayRatio-=0.05;
            }
        }
        if (!m_cropingRect.isNull()){
            m_cropingRect=QRect();
        }
        update();
    }
}

void DisplayWindow::keyPressEvent(QKeyEvent *event)
{
    if (!m_loadPixmap.isNull()) {
        QRect tempSceneQRect;
        std::vector<cv::Rect> cropedImageRectVec;
        std::vector<cv::Mat> cropedImageVec;
        cv::Rect cropedImageRect;
        cv::Mat cropedImage;
        switch (event->key()) {
        case Qt::Key_Control:
            qDebug()<<"Qt::Key_Control";
            m_keyCtrl=true;
            break;
        case Qt::Key_Alt:
            m_keyAlt=true;
            break;
        case Qt::Key_S:
            switch (m_operarMod) {
            case DISPLAY:
                break;
            case CROP:
                if (m_keyCtrl &&
                        !m_cropingRect.isNull()) {
                    qDebug()<<"DisplayWindow::keyPressEvent():saving croped iamge;";
                    tempSceneQRect = screenToImage(m_cropingRect);
                    if (!m_loadPixmap.rect().contains(tempSceneQRect)) {
                        tempSceneQRect=m_loadPixmap.rect()&tempSceneQRect;
                    }
                    cropedImageRect=cv::Rect(
                                tempSceneQRect.x(),
                                tempSceneQRect.y(),
                                tempSceneQRect.width(),
                                tempSceneQRect.height()
                                );
                    emit s_cropedImage(cropedImageRect);
                    cvtColor(m_loadMat(cropedImageRect),cropedImage,CV_BGR2RGB);
                    emit s_cropedImage(cropedImage);
                }
                break;
            case MULTICROP:
                if (m_keyCtrl &&
                        m_mulitCropingRect.size()>0) {
                    qDebug()<<"DisplayWindow::keyPressEvent():saving multi crop image;";
                    for (int rectIndex=0;rectIndex<m_mulitCropingRect.size();rectIndex++) {
                        tempSceneQRect = screenToImage(m_mulitCropingRect[rectIndex]);
                        if (!m_loadPixmap.rect().contains(tempSceneQRect)) {
                            tempSceneQRect=m_loadPixmap.rect()&tempSceneQRect;
                        }
                        cropedImageRect=cv::Rect(
                                    tempSceneQRect.x(),
                                    tempSceneQRect.y(),
                                    tempSceneQRect.width(),
                                    tempSceneQRect.height()
                                    );
                        cropedImageRectVec.push_back(cropedImageRect);
                        cvtColor(m_loadMat(cropedImageRect),cropedImage,CV_BGR2RGB);
                        cropedImageVec.push_back(cropedImage);
                    }
                    emit s_cropedImageVec(cropedImageRectVec);
                    emit s_cropedImageVec(cropedImageVec);
                }
                break;
            case MARKS:
                break;
            default:
                break;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            switch (m_operarMod) {
            case MULTICROP:
                if (!m_cropingRect.isNull()) {
                    m_mulitCropingRect.push_back(m_cropingRect);
                }
                break;
            default:
                break;
            }
            update();
            break;
        case Qt::Key_Escape:
            m_keyCtrl=false;
            m_cropingRect=QRect();
            if (m_mulitCropingRect.size()) {
                m_mulitCropingRect.pop_back();
            }
            update();
            break;
        default:
            break;
        }
    }
    return QWidget::keyPressEvent(event);
}

void DisplayWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_loadPixmap.isNull()) {
        switch (event->key()) {
        case Qt::Key_Control:
            m_keyCtrl=false;
            m_croping=false;
            break;
        case Qt::Key_Alt:
            m_keyAlt=false;
            break;
        default:
            break;
        }
    }
    return QWidget::keyReleaseEvent(event);
}

void DisplayWindow::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
    if (!m_loadPixmap.isNull()) {
        QPixmap scaledImg= m_loadPixmap.scaled(m_loadPixmap.size()*m_displayRatio);
        QPixmap showImg=scaledImg.copy(QRect(QPoint(m_displayConstPos.x(), m_displayConstPos.y()), size()));
        QPoint offsetP=QPoint(0,0);
        //        if (!scaledImg.rect().contains(m_displayConstPos)) {
        //            if (showImg.size().width()<size().width() &&
        //                    m_displayConstPos.x()<0) {
        //                offsetP.setX(size().width()-showImg.size().width());
        //            }
        //            if (showImg.size().height()<size().height()&&
        //                    m_displayConstPos.y()<0) {
        //                offsetP.setY(size().height()-showImg.size().height());
        //            }
        //        }
        if (m_displayConstPos.x()<0) {
            offsetP.setX(0-m_displayConstPos.x());
        }
        if (m_displayConstPos.y()<0) {
            offsetP.setY(0-m_displayConstPos.y());
        }
        painter->drawPixmap(offsetP, showImg);
        if (m_cropingRect.isValid()) {
            painter->setPen(QPen(
                                Qt::red,
                                1,
                                Qt::SolidLine,
                                Qt::FlatCap
                                ));
            painter->drawRect(m_cropingRect);
        }
        if (m_mulitCropingRect.size()>0) {
            for (int rectIndex=0;rectIndex<m_mulitCropingRect.size();rectIndex++) {
                painter->setPen(QPen(
                                    Qt::blue,
                                    1,
                                    Qt::SolidLine,
                                    Qt::FlatCap
                                    ));
                painter->drawRect(m_mulitCropingRect[rectIndex]);
            }
        }
    } else {
        painter->setBrush(QColor(100, 100, 100));
        painter->setPen( QPen(QColor(200, 200, 200), 2));
        painter->drawRoundedRect(this->rect(),10,10);
        painter->drawText( this->rect(), Qt::AlignCenter, "NO SIGNAL" );
    }
    delete painter;
}

QPoint DisplayWindow::screenToImage(const QPoint &pos)
{
    QPointF pp=pos+m_displayConstPos;
    pp /= m_displayRatio;
    return QPoint(int(pp.x()),int(pp.y()));
}

QRect DisplayWindow::screenToImage(const QRect &rec)
{
    QPoint leftTopPoint=screenToImage(rec.topLeft());
    double width=rec.width();
    double height=rec.height();
    width /= m_displayRatio;
    height /= m_displayRatio;
    return QRect(leftTopPoint,QSize(int(width),int(height)));
}

QSize DisplayWindow::screenToImage(const QSize &size)
{
    double width=size.width();
    double height=size.height();
    width /= m_displayRatio;
    height /= m_displayRatio;
    return QSize(int(width),int(height));
}

QPoint DisplayWindow::imageToScreen(const QPoint &pos)
{
    QPointF pp=pos*m_displayRatio;
    pp -= m_displayConstPos;
    return QPoint(int(pp.x()),int(pp.y()));
}

QRect DisplayWindow::imageToScreen(const QRect &rec)
{
    QPoint leftTopPoint=imageToScreen(rec.topLeft());
    double width=rec.width();
    double height=rec.height();
    width *= m_displayRatio;
    height *= m_displayRatio;
    return QRect(leftTopPoint,QSize(int(width),int(height)));
}

QSize DisplayWindow::imageToScreen(const QSize &size)
{
    double width=size.width();
    double height=size.height();
    width *= m_displayRatio;
    height *= m_displayRatio;
    return QSize(int(width),int(height));
}

void DisplayWindow::save(const QString &fileName)
{
    if (!m_loadPixmap.isNull()) {
        m_loadPixmap.save(fileName,"BMP");
    }
}

void DisplayWindow::getImageFromMat(cv::Mat &dstImage)
{
    dstImage=m_loadMat.clone();
}

bool DisplayWindow::isNull()
{
    if (m_loadPixmap.isNull()) {
        return true;
    }
    return false;
}

void DisplayWindow::matToQImage(const cv::Mat& matImage,QImage& qImage)
{
    // qDebug()<<"DisplayWindow::matToQImage()";
    if (matImage.channels() == 3) {
        qImage = QImage(
                    (const unsigned char*)(matImage.data),
                    matImage.cols,
                    matImage.rows,
                    matImage.step,
                    QImage::Format_RGB888
                    );
    } else {
        qImage = QImage(
                    (const unsigned char*)(matImage.data),
                    matImage.cols,
                    matImage.rows,
                    matImage.step,
                    QImage::Format_Grayscale8
                    );
    }
}

void DisplayWindow::QImageToMat(const QImage &srcImage, cv::Mat &dstImage)
{
    switch(srcImage.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        dstImage = cv::Mat(srcImage.height(), srcImage.width(), CV_8UC4, (void*)srcImage.constBits(), srcImage.bytesPerLine());
        cv::cvtColor(dstImage, dstImage, cv::COLOR_BGRA2BGR);
        break;
    case QImage::Format_RGB888:
        dstImage = cv::Mat(srcImage.height(), srcImage.width(), CV_8UC3, (void*)srcImage.constBits(), srcImage.bytesPerLine());
        cv::cvtColor(dstImage, dstImage, CV_RGB2BGR);
        break;
    case QImage::Format_Indexed8:
        dstImage = cv::Mat(srcImage.height(), srcImage.width(), CV_8UC1, (void*)srcImage.constBits(), srcImage.bytesPerLine());
        break;
    default:
        dstImage=cv::Mat();
        break;
    }
}

void DisplayWindow::setCropRectSize(QSize size)
{
    if (m_cropingRect.isValid()) {
        QSize curSize= imageToScreen(size);
        if (curSize.width()<1) {
            curSize.setWidth(1);
        }
        if (curSize.height()<1) {
            curSize.setHeight(1);
        }
        m_cropingRect.setSize(curSize);
    }
}
