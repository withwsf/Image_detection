#include "nms.h"
#include "algorithm"
using namespace std;
template<typename T>
vector<size_t> arg_sort(const vector<T>& v){
    vector<size_t> idx(v.size());
    for(size_t i=0;i<idx.size();++i)
        idx[i]=i;
    std::sort(idx.begin(),idx.end(),[&v](size_t i1,size_t i2){return v[i1]>v[i2];});
    return idx;
}

template<typename T>
T my_max(T a,T b){
    return a>b?a:b;
}
template<typename T>
T my_min(T a, T b){
    return a<b?a:b;

}
vector<vector<float>> nms::NonMaxSpu(vector<vector<float>> corrd_score, float threshold){
    vector<float> x1,y1,x2,y2,scores,areas,classes;
    for(size_t num=0;num<corrd_score.size();++num){
        x1.push_back(corrd_score[num][0]);
        y1.push_back(corrd_score[num][1]);
        x2.push_back(corrd_score[num][2]);
        y2.push_back(corrd_score[num][3]);
        areas.push_back((x2[num]-x1[num]+1)*(y2[num]-y1[num]+1));
        scores.push_back(corrd_score[num][4]);
        classes.push_back(corrd_score[num][6]);
    }
    vector<size_t> order=arg_sort(scores);
    vector<size_t> isSupressed(corrd_score.size(),0);

    size_t i,j,_i,_j;
    float ix1,iy1,ix2,iy2,iarea;
    float xx1,yy1,xx2,yy2;
    float w,h;
    float inter,ovr;
    vector<vector<float>> ret;
    for(_i=0;_i<corrd_score.size();++_i){
        i=order[_i];
        if(isSupressed[i]==1)
                continue;
        ix1=x1[i];
        iy1=y1[i];
        ix2=x2[i];
        iy2=y2[i];
        iarea=areas[i];
        vector<float> temp;
        temp.push_back(ix1);
        temp.push_back(iy1);
        temp.push_back(ix2);
        temp.push_back(iy2);
        temp.push_back(scores[i]);
        temp.push_back(classes[i]);
        ret.push_back(temp);

        for(_j=_i+1;_j<corrd_score.size();++_j){
            j=order[_j];
            if(isSupressed[j]==1)
                continue;
            xx1=my_max(ix1,x1[j]);
            yy1=my_max(iy1,y1[j]);
            xx2=my_min(ix2,x2[j]);
            yy2=my_min(iy2,y2[j]);
            w=my_max(float(0.0),xx2-xx1+1);
            h=my_max(float(0.0),yy2-yy1+1);
            inter=w*h;
            ovr=inter/(iarea+areas[j]-inter);
            if(ovr>threshold)
                isSupressed[j]=1;
        }
    }

   return ret;

}

