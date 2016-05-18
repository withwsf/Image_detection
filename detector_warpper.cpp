#include "detector_warpper.h"
#include "image_detection.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include<QMessageBox>
detector_warpper::detector_warpper(QObject *parent) : QObject(parent),
    nums_threshold(0.5),score_threshold(0.5),has_result(false),
    cls_to_dis({2,6,7,14,15,21,22,23,24})
{

    std::string prototxt="/home/vcc/caffe_depen/faster_rcnn_test_four_class.pt";
    std::string caffemodel="/home/vcc/caffe_depen/ZF_faster_rcnn_final_four_classes.caffemodel";
    this->detector=std::make_shared<image_detection>(prototxt,caffe::TEST,caffemodel);
    std::vector<std::string> classNames{"__background__",
                      "aeroplane", "自行车", "bird", "boat",
                      "bottle", "巴士", "小汽车", "cat", "chair",
                      "cow", "diningtable", "dog", "horse",
                      "摩托车", "行人", "pottedplant",
                      "sheep", "sofa", "train", "tvmonitor","垃圾","横幅","井盖","游商"};

    this->cls_inx_name.reset(new std::vector<std::string>(classNames));
}


void detector_warpper::change_nums_threshold(double new_threshold){
    this->nums_threshold=new_threshold;
    qDebug()<<"new nms threshold"<<this->nums_threshold;
}
void detector_warpper::change_score_threshold(double new_threshold){
    this->score_threshold=new_threshold;
     qDebug()<<"new score threshold"<<this->score_threshold;
}

void detector_warpper::detect(std::string image_path){
    cv::Mat image=cv::imread(image_path);
    if(! image.data )
        {
         this->has_result=false;
         QMessageBox msgBox;
         msgBox.setText("图像不存在或无法打开!");
         msgBox.exec();
        }
    else{
    this->det_ret=this->detector->detect(image,this->score_threshold,this->nums_threshold);
    this->has_result=true;
    emit detect_finished();
    }
}

std::vector<std::pair<std::string,std::vector<float>>> detector_warpper::return_detec_result(){
    std::vector<std::pair<std::string,std::vector<float>>> result;
    if(has_result){
     for(auto const& ele:this->det_ret){
         if(this->cls_to_dis.find(int(ele[5]))!=this->cls_to_dis.end()){
             std::string name=(*this->cls_inx_name)[ele[5]];
             result.push_back({name,ele});
         }
     }

    }
    else{
        QMessageBox msgBox;
        msgBox.setText("没有检测结果!");
        msgBox.exec();
    }
    return result;
}
