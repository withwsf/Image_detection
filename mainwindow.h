#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QGraphicsScene>
#include "dockwidget.h"
#include <utility>
#include <set>
class image_detection;
namespace Ui {
class MainWindow;

}
class detector_warpper;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPixmap image;
    std::shared_ptr<QImage> imageObject;
    std::shared_ptr<QGraphicsScene> scene;
    DockWidget* dock_object;
    std::string imagePath;
    std::shared_ptr<detector_warpper> warpper;
    std::vector<std::pair<std::string,std::vector<float>>> det_ret;
    std::set<std::string> cls_filter;
    double scale;
    int image_height;
    int image_width;

public slots:
    void LoadImgClicked();
    void detect();
    void change_score(double thres);
    void change_nms(double thres);
    void update_scene();
    void rescale_image(int value);
    void update_bus(int state);
    void update_bicycle(int state);
    void update_car(int state);
    void update_motor(int state);
    void update_person(int state);
    void update_garbage(int state);
    void update_biaoyu(int state);
    void update_jinggai(int state);
    void update_youshang(int state);



signals:
   void detection_finshed();
};

#endif // MAINWINDOW_H
