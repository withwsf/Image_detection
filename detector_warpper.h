#ifndef DETECTOR_WARPPER_H
#define DETECTOR_WARPPER_H
#include <memory>
#include <utility>
#include <QObject>
#include <set>
class image_detection;
class detector_warpper : public QObject
{
    Q_OBJECT
public:
    explicit detector_warpper(QObject *parent = 0);
    std::vector<std::pair<std::string,std::vector<float>>> return_detec_result();
private:
    std::shared_ptr<image_detection> detector;
    std::shared_ptr<std::vector<std::string>> cls_inx_name;
    std::vector<std::vector<float>> det_ret;
    double nums_threshold;
    double score_threshold;
    bool has_result;
    std::set<int> cls_to_dis;
signals:
   void detect_finished();
public slots:
    void change_nums_threshold(double new_threshold);
    void change_score_threshold(double new_threshold);
    void detect(std::string image_path);
};//

#endif // DETECTOR_WARPPER_H
