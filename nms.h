#ifndef NMS_H
#define NMS_H
#include <vector>
using namespace std;
class nms
{
public:
    nms()=delete;
    static vector<vector<float>> NonMaxSpu(vector<vector<float>> corrd_score,float threshold);

};

#endif // NMS_H
