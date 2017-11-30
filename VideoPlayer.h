#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <FaceDetector.h>
class PoseEstimation;

class VideoPlayer : public QThread
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr, unsigned int camIdx = 0);
    ~VideoPlayer();
    void play();
    void stop();
    bool isStopped() const;
protected:
    void run();
signals:
    void processedImage(const QImage &image);
private:
    void poseEstimate(cv::Mat &frame, QImage::Format format);
    void drawBasis(std::vector<cv::Point2d> &imgPoints);
    bool _stop;
    QImage _image;
    unsigned int _camIdx;
    FaceDetector *_detector;
    cv::VideoCapture _capture;
    QWaitCondition _condition;
    QMutex _mutex;
    PoseEstimation *_poseEstimator;
};

#endif // VIDEOPLAYER_H
