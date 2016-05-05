#ifndef DETECTOR_WARPPER_H
#define DETECTOR_WARPPER_H
#include <memory>

#include <QObject>
class image_detection;
class detector_warpper : public QObject
{
    Q_OBJECT
public:
    explicit detector_warpper(QObject *parent = 0);
private:
    std::shared_ptr<image_detection> detector;


signals:

public slots:
};

#endif // DETECTOR_WARPPER_H
