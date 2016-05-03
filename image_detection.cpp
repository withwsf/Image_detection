#include "image_detection.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include "nms.h"
using namespace  caffe;
using namespace boost::python;
using namespace std;
//#define VISUALIZE
void visualizeBlob(std::string name,Blob<float>* blob){
    int height=blob->shape()[2];
    int width=blob->shape()[3];
    cv::Mat channel(height,width,CV_32FC1,blob->mutable_cpu_data());
    cv::namedWindow(name,cv::WINDOW_AUTOSIZE);
    cv::imshow(name,channel);
    cv::waitKey(1000);

}
void visualizeMat(std::string name,cv::Mat mat){
    cv::namedWindow(name,cv::WINDOW_AUTOSIZE);
    cv::imshow(name,mat);
    cv::waitKey(1000);
}
vector<float> loadBlob(Blob<float>* blob){
    int dims=blob->shape().size();
    int length=1;
    for(int i=0;i<dims;++i){
        length*=blob->shape()[i];
    }
    vector<float> ret;
    float* ptr_data=blob->mutable_cpu_data();
    for(int j=0;j<length;++j){
        ret.push_back(ptr_data[j]);
    }
    return ret;


}

void image_detection::init(){
    if(!import_path){
        Py_Initialize();
        import("modify_path");
        std::cout<<"Modified PYTHONPATH according to module modify_path!"<<std::endl;
        import_path=true;
    }

    std::vector<float> temp{122.7717,115.9465,102.9801};
    this->_mean=temp;
    this->_net=std::make_shared<caffe::Net<float>>(_pathToProtoTxt,_phase);
#ifdef VISUALIZE
    vector<float> before_load=loadBlob(this->_net->params()[0].get());
#endif

    this->_net->CopyTrainedLayersFrom(_pathToPretrain);
#ifdef VISUALIZE
    vector<float> after_load=loadBlob(this->_net->params()[0].get());
#endif
    caffe::Blob<float> *input_layer=_net->input_blobs()[0];
    this->_num_channels=input_layer->channels();
    this->_input_geometry=cv::Size(input_layer->width(),input_layer->height());
    vector<string> classNames{"__background__",
                      "aeroplane", "bicycle", "bird", "boat",
                      "bottle", "bus", "car", "cat", "chair",
                      "cow", "diningtable", "dog", "horse",
                      "motorbike", "person", "pottedplant",
                      "sheep", "sofa", "train", "tvmonitor","garbage"};

    this->class_index_name.reset(new std::vector<std::string>(classNames));

}

void image_detection::resetNet(string pathToProtoTxt, Phase phase, string pathToPreTrained){
    _pathToProtoTxt=pathToProtoTxt;
    _pathToPretrain=pathToPreTrained;
    _phase=phase;
    _net=std::make_shared<caffe::Net<float>>(_pathToProtoTxt,_phase);
    if(!pathToPreTrained.empty()){
        _net->CopyTrainedLayersFrom(_pathToPretrain);

    }
    this->scale=0;
}

std::shared_ptr<caffe::Net<float>> image_detection::getNet(){
    return _net;
}

caffe::Phase image_detection::getPhase(){
    return _phase;
}

cv::Mat image_detection::preprocessImg(const cv::Mat& img){

    if(img.empty()){
        std::cout<<"The input image is empty!"<<endl;
        raise(SIGABRT);
    }
    cv::Mat sample;
    if (img.channels() == 3 && this->_num_channels == 1)
    cv::cvtColor(img, sample, CV_BGR2GRAY);
    else if (img.channels() == 4 && this->_num_channels == 1)
    cv::cvtColor(img, sample, CV_BGRA2GRAY);
    else if (img.channels() == 4 && this->_num_channels == 3)
    cv::cvtColor(img, sample, CV_BGRA2BGR);
    else if (img.channels() == 1 && this->_num_channels == 3)
    cv::cvtColor(img, sample, CV_GRAY2BGR);
    else
    sample = img;
    //sample=this->subtractMean(sample,this->_mean);

    int height=sample.rows;
    int width=sample.cols;
    if(width<height){
        this->scale=float(600)/float(width);
        height=int(float(height)/float(width)*600);
        width=600;

    }
    else{
        this->scale=float(600)/float(height);
        width=int(float(width)/float(height)*600);
        height=600;

    }
    cv::Size new_size(width,height);
    cv::Mat sample_resized;
    if (sample.size() != new_size)
    cv::resize(sample, sample_resized, new_size);
    else
    sample_resized = sample;



    return sample_resized;

}

void image_detection::loadInputImg(const cv::Mat& img){

    //warp input blobs
    int width=img.cols;
    int height=img.rows;
    this->updateShape(height,width);//update the shape of networks according to


    Blob<float>* input_layer=this->_net->input_blobs()[0];
    float* input_data=input_layer->mutable_cpu_data();

    cv::Mat rbg[3];
    cv::split(img,rbg);
        for(int j=0;j<height;++j){
            for(int k=0;k<width;++k){
                float r=float(rbg[0].at<uchar>(j,k));
                float b=float(rbg[1].at<uchar>(j,k));
                float g=float(rbg[2].at<uchar>(j,k));
                input_data[j*width+k]=r;
                input_data[(height*width)+j*width+k]=b;
                input_data[(height*width)*2+j*width+k]=g;
            }
        }


}

void image_detection::updateShape(int height, int width){

    caffe::Blob<float>* input_blob=_net->input_blobs()[0];
    input_blob->Reshape(1,this->_num_channels,height,width);
    _net->Reshape();
}

void image_detection::loadImgInfo(const cv::Mat &img){
    int width=img.cols;
    int heigth=img.rows;
    Blob<float>* im_info=this->_net->input_blobs()[1];
    float* ptr_info=im_info->mutable_cpu_data();
    *ptr_info=heigth;
    *(++ptr_info)=width;
    *(++ptr_info)=this->scale;

}

std::vector<std::vector<float>> image_detection::detect(const cv::Mat &img,float threshold,float nms_threshold){
    cv::Mat intermedia=img;
    cv::Mat preProcessed=this->preprocessImg(intermedia);

    this->loadInputImg(preProcessed);
    this->loadImgInfo(preProcessed);
    this->_net->ForwardPrefilled();
   vector<vector<vector<float>>> ret =this->get_score_tranfomedBoundingBox(this->_net->blob_by_name("rois").get(),this->_net->blob_by_name("bbox_pred").get(),this->_net->blob_by_name("cls_prob").get());
   return this->filterScore(ret[0],ret[1],threshold,nms_threshold);
}

vector<vector<vector<float>>> image_detection::get_score_tranfomedBoundingBox(const caffe::Blob<float>* rois,const caffe::Blob<float>* delta,const caffe::Blob<float>* score){
//not mature
if(delta->shape()[0]!=rois->shape()[0]||score->shape()[0]!=rois->shape()[0])
    raise(0);


int num_of_roi=rois->shape()[0];
vector<vector<float>> _rois;
vector<vector<float>> _delta_correct;
vector<vector<float>> _score;
for(int i=0;i<num_of_roi;++i){
    vector<float> temp1;
    vector<float> temp2;
    vector<float> temp3;

    for(int j=1;j<5;++j){
       temp1.push_back(rois->cpu_data()[i*5+j]);
    }
    _rois.push_back(temp1);

    for(int k=0;k<22;++k){
        temp2.push_back(score->cpu_data()[i*22+k]);
    }
    _score.push_back(temp2);

    for(int l=0;l<88;++l){
        temp3.push_back(delta->cpu_data()[i*88+l]);
    }
    _delta_correct.push_back(temp3);
}
//the 1th edition, rescale ROI before allpy
for(auto& vec:_rois){
    for(auto& val:vec)
        val/=this->scale;
}

vector<float> widths,heights;
for(auto const& vec:_rois){
    widths.push_back(vec[2]-vec[0]);
    heights.push_back(vec[3]-vec[1]);
}

vector<float> ctr_x,ctr_y;
for(size_t i=0;i<_rois.size();++i){
    ctr_x.push_back(_rois[i][0]+widths[i]*0.5);
    ctr_y.push_back(_rois[i][1]+heights[i]*0.5);
}


int cls_num=_score[0].size();
vector<vector<float> > predicted_corrd=_delta_correct;
for(size_t i=0;i<_rois.size();++i){
    for(int j=0;j<cls_num;++j){
        float predicted_w=exp(_delta_correct[i][j*4+2])*widths[i];
        float predicted_h=exp(_delta_correct[i][j*4+3])*heights[i];
        predicted_corrd[i][j*4]=_delta_correct[i][j*4]*widths[i]+ctr_x[i]-0.5*predicted_w;
        predicted_corrd[i][j*4+1]=_delta_correct[i][j*4+1]*heights[i]+ctr_y[i]-0.5*predicted_h;
        predicted_corrd[i][j*4+2]=_delta_correct[i][j*4]*widths[i]+ctr_x[i]+0.5*predicted_w;;
        predicted_corrd[i][j*4+3]=_delta_correct[i][j*4+1]*heights[i]+ctr_y[i]+0.5*predicted_h;
    }
}

vector<vector<vector<float>>> ret;
ret.push_back(_score);
ret.push_back(predicted_corrd);
return ret;
}

cv::Mat image_detection::subtractMean(const cv::Mat& img,vector<float>mean){
    visualizeMat("before",img);
    cv::Mat subed;
    cv::Mat temp(img.rows,img.cols,CV_8UC3,cv::Scalar(mean[0],mean[1],mean[2]));
    cv::subtract(img,temp,subed);
    visualizeMat("after",subed);
    visualizeMat("img",img);
    return temp;
}

void image_detection::setMean(std::vector<float> mean){
    this->_mean=mean;
}

vector<vector<float>>  image_detection::filterScore(const std::vector<std::vector<float> > &score, const std::vector<std::vector<float> > &corrd,float threshold,float nms_threshlod){
    vector<vector<vector<float>>> inter_media(score[0].size());
    vector<vector<float>> ret;
    for(size_t i=0;i<score.size();++i){
        for(size_t j=1;j<score[0].size();++j){
            if(score[i][j]>threshold){
                vector<float> temp;
                temp.push_back(corrd[i][j*4]);
                temp.push_back(corrd[i][j*4+1]);
                temp.push_back(corrd[i][j*4+2]);
                temp.push_back(corrd[i][j*4+3]);
                temp.push_back(score[i][j]);
                temp.push_back(i);
                temp.push_back(j);
                inter_media[j].push_back(temp);

            }

        }
    }

    for(auto ele:inter_media){
        if(ele.size()>1){
        auto temp_ret=nms::NonMaxSpu(ele,nms_threshlod);
        ret.insert(ret.end(),temp_ret.begin(),temp_ret.end());
        }
    }
    return ret;
}
