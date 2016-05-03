#ifndef IMAGE_DETECTION_H
#define IMAGE_DETECTION_H
#include <boost/python.hpp>
#include <caffe/caffe.hpp>
#include <memory>
#include <string>
#include <boost/python.hpp>
#include <opencv2/core/core.hpp>
#include <map>
#include <vector>
#include <string>
#include <QObject>
class image_detection : public QObject
{
    Q_OBJECT

public:


   std::vector<std::vector<float>> detect(const cv::Mat& img,float threshold,float nms_threshlod);
   image_detection& operator=(const image_detection& )=delete;//forbid copy construct and operator = assignment copy
   image_detection(const image_detection&)=delete;
   image_detection(std::string pathToProtoTxt,caffe::Phase phase,std::string pathToPreTrained="") :
       _pathToProtoTxt(pathToProtoTxt),
       _phase(phase),
       _pathToPretrain(pathToPreTrained),
       import_path(false),
       QObject(NULL)
   {
       init();
   }
   void resetNet(std::string pathToProtoTxt,caffe::Phase,std::string pathToPreTrained="");
   void setMean(std::vector<float> mean);
   std::shared_ptr<caffe::Net<float>> getNet();
   caffe::Phase getPhase();
   ~image_detection(){}
private:
    void init();
    /// @brief adjust channel and resize img according to input size
    cv::Mat preprocessImg(const cv::Mat& img);
    void loadInputImg(const cv::Mat& img);
    void updateShape(int width,int height);
    void loadImgInfo(const cv::Mat& img);
    cv::Mat subtractMean(const cv::Mat& img,std::vector<float>mean);
    std::vector<std::vector<std::vector<float>>> get_score_tranfomedBoundingBox(const caffe::Blob<float>* rois,const caffe::Blob<float>* delta,const caffe::Blob<float>* score);
    std::vector<std::vector<float>> filterScore(const std::vector<std::vector<float>>& score,const std::vector<std::vector<float>>& corrd,float threshold,float nms_threshold);
private:


    std::shared_ptr<caffe::Net<float>> _net;
    std::string _pathToProtoTxt;
    caffe::Phase _phase;
    std::string _pathToPretrain;
    bool import_path;
    int _num_channels;//num of input channel
    cv::Size _input_geometry;//size of input image
    float scale;
    std::shared_ptr<std::vector<std::string>> class_index_name;
    std::vector<float> _mean;


};

#endif // IMAGE_DETECTION_H
