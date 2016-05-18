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
Q_DECLARE_METATYPE(std::string)
int main(int argc,char** argv)
{


    qRegisterMetaType<std::string>("std::string");
    //you need to do above both to enable queued signal-slots

    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    return a.exec();
}
