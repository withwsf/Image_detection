#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image_detection.h"
#include "iostream"
#include <QFileDialog>
#include <string>
#include <vector>
#include <QMessageBox>
#include <QDebug>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "QGraphicsTextItem"
#include "detector_warpper.h"
using std::cout;
using std::endl;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),scale(1),image_height(0),image_width(0),
    ui(new Ui::MainWindow)
{
    dock_object = new DockWidget(this);

    ui->setupUi(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,dock_object);
    this->setCentralWidget(ui->graphicsView);
    QObject::connect(this->dock_object->ui.pushButton,SIGNAL(clicked()),this,SLOT(LoadImgClicked()));
    QObject::connect(this->dock_object->ui.DetectpushButton,SIGNAL(clicked()),this,SLOT(detect()));
    QObject::connect(this,SIGNAL(detection_finshed()),this,SLOT(update_scene()));

    QObject::connect(this->dock_object->ui.scaleSlider,SIGNAL(valueChanged(int)),this,SLOT(rescale_image(int)));
    //figure out why not work!!!!
    QObject::connect(this->dock_object->ui.ScoreSpinBox,SIGNAL(valueChanged(double)),this->warpper.get(),SLOT(change_score_threshold(double)));
    QObject::connect(this->dock_object->ui.NMSSpinBox_2,SIGNAL(valueChanged(double)),this->warpper.get(),SLOT(change_nums_threshold(double)));

    QObject::connect(this->dock_object->ui.ScoreSpinBox,SIGNAL(valueChanged(double)),this,SLOT(change_score(double)));
    QObject::connect(this->dock_object->ui.NMSSpinBox_2,SIGNAL(valueChanged(double)),this,SLOT(change_nms(double)));


    //connect check boxes
    QObject::connect(dock_object->ui.checkBox_bicycle,SIGNAL(stateChanged(int)),this,SLOT(update_bicycle(int)));
    QObject::connect(dock_object->ui.checkBox_bus,SIGNAL(stateChanged(int)),this,SLOT(update_bus(int)));
    QObject::connect(dock_object->ui.checkBox_car,SIGNAL(stateChanged(int)),this,SLOT(update_car(int)));
    QObject::connect(dock_object->ui.checkBox_garbbage,SIGNAL(stateChanged(int)),this,SLOT(update_garbage(int)));
    QObject::connect(dock_object->ui.checkBox_hengfu,SIGNAL(stateChanged(int)),this,SLOT(update_biaoyu(int)));
    QObject::connect(dock_object->ui.checkBox_motor,SIGNAL(stateChanged(int)),this,SLOT(update_motor(int)));
    QObject::connect(dock_object->ui.checkBox_person,SIGNAL(stateChanged(int)),this,SLOT(update_person(int)));
    QObject::connect(dock_object->ui.checkBox_well,SIGNAL(stateChanged(int)),this,SLOT(update_jinggai(int)));
    QObject::connect(dock_object->ui.checkBox_youshang,SIGNAL(stateChanged(int)),this,SLOT(update_youshang(int)));



    this->warpper=std::make_shared<detector_warpper>(nullptr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadImgClicked(){
  this->det_ret.clear();
  QString filepath=QFileDialog::getOpenFileName(this,tr("Open Img"),"/home/vcc/测试数据",tr("Image Files(*.png *.jpg *.bmp)"));
  this->imagePath=filepath.toStdString();
  this->update_scene();
}

void MainWindow::detect(){
    this->det_ret.clear();
    this->update_scene();
    if(this->imagePath.empty()){
        QMessageBox msgBox;
        msgBox.setText("please open an image first!");
        msgBox.exec();
    }
    else{
    this->warpper->detect(this->imagePath);
    this->det_ret=this->warpper->return_detec_result();
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

void MainWindow::update_scene(){
    qDebug()<<"update result";
    if(this->scene.get()!=nullptr)
        this->scene->clear();
    //clear scene and add image
    if(!this->imagePath.empty()){
    this->imageObject=std::make_shared<QImage>(QString::fromStdString(this->imagePath));
    image_height=double(imageObject->height())*this->scale;
    image_width=double(imageObject->width())*this->scale;
    //will here memeory leak? no,will not
    QImage temp=this->imageObject->scaled(image_width,image_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    this->imageObject.reset(new QImage(temp));
    this->image=QPixmap::fromImage(*imageObject);
    this->scene=std::make_shared<QGraphicsScene>(this);
    QGraphicsPixmapItem* ImageItem=this->scene->addPixmap(image);
    this->scene->setSceneRect(image.rect());
    this->ui->graphicsView->setScene(this->scene.get());
    }
    for(auto const& vec:this->det_ret){
      QPen outlinepen(Qt::red);
      QBrush brush(Qt::NoBrush);
      if(this->cls_filter.find(vec.first)!=this->cls_filter.end()){
          std::string name_str=vec.first;
          std::vector<float> ret_vec=vec.second;
          float _w=(ret_vec[2]-ret_vec[0])*scale;
          float _h=(ret_vec[3]-ret_vec[1])*scale;
          QGraphicsRectItem* rectItem=this->scene->addRect(ret_vec[0]*scale,ret_vec[1]*scale,_w,_h,outlinepen,brush);
          std::ostringstream ss;
          ss<<ret_vec[4];
          std::string pro_str=ss.str();
          QGraphicsTextItem* text=this->scene->addText(QString::fromStdString(name_str),QFont(QString::fromStdString(std::string("Times")),_h/6));
          QGraphicsTextItem* text2=this->scene->addText(QString::fromStdString(pro_str),QFont(QString::fromStdString(std::string("Times")),_h/10));
          text->setPos(QPoint(ret_vec[0]*scale,ret_vec[1]*scale-_h/10));
          text->setDefaultTextColor(QColor(255,0,0));
          text2->setPos(QPoint(ret_vec[0]*scale,ret_vec[1]*scale-_h/5));
          text2->setDefaultTextColor(QColor(255,0,0));
          cout<<"draw a rect"<<endl;
      }
    }
}

////for thes
void MainWindow::change_score(double thres){
    this->warpper->change_score_threshold(thres);
}

void MainWindow::change_nms(double thres){
    this->warpper->change_nums_threshold(thres);
}

void MainWindow::rescale_image(int value){
    if(value<=50)
        this->scale=double(value)/50;
    else
        this->scale=(double(value)-50)/5;
    this->update_scene();
}

/////functions of change checkbox
void MainWindow::update_biaoyu(int state){
    if(state==0){
        this->cls_filter.erase("横幅");
        qDebug()<<"erase 横幅";
    }
    else if(state==2){
        this->cls_filter.insert("横幅");
        qDebug()<<"add 横幅";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();
}
void MainWindow::update_bicycle(int state){
    if(state==0){
        this->cls_filter.erase("自行车");
        qDebug()<<"erase 自行车";
    }
    else if(state==2){
        this->cls_filter.insert("自行车");
        qDebug()<<"add 自行车";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_bus(int state){
    if(state==0){
        this->cls_filter.erase("巴士");
        qDebug()<<"erase 巴士";
    }
    else if(state==2){
        this->cls_filter.insert("巴士");
        qDebug()<<"add 巴士";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_car(int state){
    if(state==0){
        this->cls_filter.erase("小汽车");
        qDebug()<<"erase 小汽车";
    }
    else if(state==2){
        this->cls_filter.insert("小汽车");
        qDebug()<<"add 小汽车";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_motor(int state){
    if(state==0){
        this->cls_filter.erase("摩托车");
        qDebug()<<"erase 摩托车";
    }
    else if(state==2){
        this->cls_filter.insert("摩托车");
        qDebug()<<"add 摩托车";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_person(int state){
    if(state==0){
        this->cls_filter.erase("行人");
        qDebug()<<"erase 行人";
    }
    else if(state==2){
        this->cls_filter.insert("行人");
        qDebug()<<"add 行人";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_garbage(int state){
    if(state==0){
        this->cls_filter.erase("垃圾");
        qDebug()<<"erase 垃圾";
    }
    else if(state==2){
        this->cls_filter.insert("垃圾");
        qDebug()<<"add 垃圾";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_jinggai(int state){
    if(state==0){
        this->cls_filter.erase("井盖");
        qDebug()<<"erase 井盖";
    }
    else if(state==2){
        this->cls_filter.insert("井盖");
        qDebug()<<"add 井盖";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
void MainWindow::update_youshang(int state){
    if(state==0){
        this->cls_filter.erase("游商");
        qDebug()<<"erase 游商";
    }
    else if(state==2){
        this->cls_filter.insert("游商");
        qDebug()<<"add 游商";
    }
    else{
        qDebug()<<"unknown state!";
    }
    update_scene();

}
