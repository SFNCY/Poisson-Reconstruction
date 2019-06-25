#pragma once

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QPixmap>
#include <QImage>

enum OperatType{
    DISPLAY,
    CROP,
    MULTICROP,
    MARKS
};

//The window for display.It also can be externed to make some extra operations
//(like croping the image) on the showing window.
class DisplayWindow : public QWidget
{
    Q_OBJECT

public:
    DisplayWindow(QWidget *parent = 0,OperatType = CROP);
    ~DisplayWindow();
    
    void display(const cv::Mat &image, bool resetDisplay = false);
    void clear();
    void setCropRectSize(QSize);
    void save(const QString &fileName);
    void getImageFromMat(cv::Mat &dstImage);
    bool isNull();

    static void matToQImage(const cv::Mat& matImage,QImage& qImage);
    static void QImageToMat( const QImage &srcImage, cv::Mat &dstImage);

protected:
    QRect getRect(QPoint beginPoint, QPoint endPoint);
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *) Q_DECL_OVERRIDE;

    void relayoutControlBTN();

    void setPixmap(const QPixmap &inputImage, bool resetDisplay = false);
    void setPixmap(const QPixmap &, QPoint);
    QPoint screenToImage(const QPoint &);
    QRect screenToImage(const QRect &);
    QSize screenToImage(const QSize &);
    QPoint imageToScreen(const QPoint &);
    QRect imageToScreen(const QRect &);
    QSize imageToScreen(const QSize &);

signals:
    void s_cropedImage(cv::Rect );
    void s_cropedImage(cv::Mat );

    void s_cropedImageVec(std::vector<cv::Rect> );
    void s_cropedImageVec(std::vector<cv::Mat> );

    void s_cropSizeChanged(QSize );

protected:
    OperatType m_operarMod;
    QSize m_size;
    QRect m_cropingRect;
    QList<QRect> m_mulitCropingRect;

    QPoint m_mousePosBefor;
    QPoint m_displayConstPos;
    double m_displayRatio;

    QPixmap m_loadPixmap;
    cv::Mat m_loadMat;

    //current operationg
    bool m_translating;
    bool m_scaling;
    bool m_croping;

    //key
    bool m_keyCtrl;
    bool m_keyAlt;
};
