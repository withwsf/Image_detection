#include "mainwindow.h"
#include <QApplication>

#include <caffe/caffe.hpp>
#include <boost/python.hpp>
#include <memory>
#include <iosfwd>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "image_detection.h"
#include <string>
#include <vector>
using namespace std;
using namespace  caffe;
using namespace  boost::python;

//std::vector<int> filterScore(const std::vector<std::vector<float>>& score,const float& threshold,int cls_index){
//    vector<int> ret;
//    for(size_t i=0;i<score.size();++i){
//        if(score[i][cls_index]>threshold)
//            ret.push_back(i);
//    }
//    return ret;
//}


int main(int argc,char** argv)
{

    string prototxt="/home/wsf/store/caffes/faster/py-faster-rcnn/output/cwll_faster_rcnn_test.pt";
    string caffemodel="/home/wsf/store/caffes/faster/py-faster-rcnn/output/laji_ZF_faster_rcnn_final.caffemodel";
    string prototxt2="/home/wsf/caffe-master/models/bvlc_reference_caffenet/deploy.prototxt";
    string image_file="/home/wsf/store/data/city/garbage_annotations/test1000/lj_lj_img3.jpg";
//    //Net<float>* test_=new Net<float>(prototxt2, TEST);
//     std::shared_ptr<Net<float> > net_;
//       //bp::object module = bp::import(param.python_param().module().c_str());
//       //bp::object layer = module.attr(param.python_param().layer().c_str())(param);
//       //boost::python::object module = boost::python::import(string("rpn.proposal_layer").c_str());
//       //return boost::python::extract<shared_ptr<PythonLayer<Dtype> > >(layer)();
//     Py_Initialize();
//     object sys=import("modify_path");

//    net_=std::make_shared<caffe::Net<float>>(prototxt, TEST);
//    //net_.reset(new Net<float>(prototxt, TEST));
//    net_->CopyTrainedLayersFrom(caffemodel);

//    vector<string> classNames{"__background__",
//                      "aeroplane", "bicycle", "bird", "boat",
//                      "bottle", "bus", "car", "cat", "chair",
//                      "cow", "diningtable", "dog", "horse",
//                      "motorbike", "person", "pottedplant",
//                      "sheep", "sofa", "train", "tvmonitor","garbage"};
//    std::shared_ptr<vector<string>> class_index_name;
//    class_index_name.reset(new std::vector<std::string>{"__background__",
//                                                        "aeroplane", "bicycle", "bird", "boat",
//                                                        "bottle", "bus", "car", "cat", "chair",
//                                                        "cow", "diningtable", "dog", "horse",
//                                                        "motorbike", "person", "pottedplant",
//                                                        "sheep", "sofa", "train", "tvmonitor","garbage"});


//    image_detection detector(prototxt,TEST,caffemodel);
//    std::shared_ptr<caffe::Net<float>> _net=detector.getNet();
//    cv::Mat img=cv::imread(image_file);


////    vector<Blob<float>* > _input_blobs=_net->input_blobs();
////    _net->Reshape();
////    _net->ForwardPrefilled();
//    std::vector<std::vector<float>> ret=detector.detect(img,0.5,0.3);
//    int i=0;
//    //std::vector<int> filtered=filterScore(ret[0],0.9,21);
//    std::cout<<i<<endl;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    return a.exec();
}
