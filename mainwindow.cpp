#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image_detection.h"
#include "iostream"
#include <QFileDialog>
#include <string>
#include <vector>
#include <QMessageBox>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "QGraphicsTextItem"
using std::cout;
using std::endl;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),score_threshlod(0.5),nms_threshold(0.5)
{
    dock_object = new DockWidget(this);

    ui->setupUi(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,dock_object);
    this->setCentralWidget(ui->graphicsView);
    QObject::connect(this->dock_object->ui.pushButton,SIGNAL(clicked()),this,SLOT(LoadImgClicked()));
    QObject::connect(this->dock_object->ui.DetectpushButton,SIGNAL(clicked()),this,SLOT(detect()));
    QObject::connect(this->dock_object->ui.ScoreSpinBox,SIGNAL(valueChanged(double)),this,SLOT(reset_score_threshold(double)));
    QObject::connect(this->dock_object->ui.NMSSpinBox_2,SIGNAL(valueChanged(double)),this,SLOT(reset_nms_threshold(double)));
    QObject::connect(this,SIGNAL(detection_finshed()),this,SLOT(show_detected()));
    std::string prototxt="/home/wsf/store/caffes/faster/py-faster-rcnn/output/faster_rcnn_test_four_class.pt";
    std::string caffemodel="/home/wsf/store/caffes/faster/py-faster-rcnn/output/ZF_faster_rcnn_final_four_classes.caffemodel";
    this->detector=std::make_shared<image_detection>(prototxt,caffe::TEST,caffemodel);



    std::vector<std::string> classNames{"__background__",
                      "aeroplane", "bicycle", "bird", "boat",
                      "bottle", "bus", "car", "cat", "chair",
                      "cow", "diningtable", "dog", "horse",
                      "motorbike", "person", "pottedplant",
                      "sheep", "sofa", "train", "tvmonitor","垃圾","标语","井盖","游商"};

    this->cls_inx_name.reset(new std::vector<std::string>(classNames));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadImgClicked(){
  cout<<"Button clicked!"<<endl;
  QString filepath=QFileDialog::getOpenFileName(this,tr("Open Img"),"/home/wsf/store/data/city/garbage_annotations/test1000",tr("Image Files(*.png *.jpg *.bmp)"));
  if(this->scene.get()!=nullptr)
      this->scene->clear();
  this->imagePath=filepath.toStdString();
  this->imageObject=std::make_shared<QImage>(filepath);
  this->image=QPixmap::fromImage(*imageObject);
  this->scene=std::make_shared<QGraphicsScene>(this);



  QGraphicsPixmapItem* ImageItem=this->scene->addPixmap(image);
  //ImageItem->

  this->scene->setSceneRect(image.rect());
  this->ui->graphicsView->setScene(this->scene.get());
}

void MainWindow::detect(){
    cout<<"detected clicked!"<<endl;
    if(this->imagePath.empty()){
        QMessageBox msgBox;
        msgBox.setText("please open an image first!");
        msgBox.exec();
    }
    else{

    this->det_ret=this->detector->detect(cv::imread(this->imagePath),score_threshlod,nms_threshold);
    if(this->det_ret.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No object detected!");
        msgBox.exec();
    }
    else{
    emit detection_finshed();
    }
    }
}

void MainWindow::reset_nms_threshold(double new_threshold){
    this->nms_threshold=new_threshold;
    cout<<"new NMS threshold is "<<new_threshold<<endl;
}

void MainWindow::reset_score_threshold(double new_threshlod){
    this->score_threshlod=new_threshlod;
    cout<<"new score threshold is "<<new_threshlod<<endl;
}
void MainWindow::show_detected(){
    cout<<"Drawing"<<endl;
    QPen outlinepen(Qt::red);
    QBrush brush(Qt::NoBrush);
    for(auto const& vec:this->det_ret){
      if(vec[5]>=21){
     QGraphicsRectItem* rectItem=this->scene->addRect(vec[0],vec[1],vec[2]-vec[0],vec[3]-vec[1],outlinepen,brush);
     std::string name_str=(*this->cls_inx_name)[vec[5]];

     std::ostringstream ss;
     ss<<vec[4];
     std::string pro_str=ss.str();
     QGraphicsTextItem* text=this->scene->addText(QString::fromStdString(name_str),QFont(QString::fromStdString(std::string("Times")),30));
     QGraphicsTextItem* text2=this->scene->addText(QString::fromStdString(pro_str),QFont(QString::fromStdString(std::string("Times")),15));
     text->setPos(QPoint(vec[0],vec[1]-60));
     text->setDefaultTextColor(QColor(255,0,0));
     text2->setPos(QPoint(vec[0],vec[1]-25));
     text2->setDefaultTextColor(QColor(255,0,0));
     cout<<"draw a rect"<<endl;
      }

    }
}
