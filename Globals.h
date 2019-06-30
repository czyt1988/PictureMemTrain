#ifndef GLOBALS_H
#define GLOBALS_H
#include <QList>
#include <QPair>
#include <QTime>
#include <math.h>
namespace PMT {
/**
 * @brief 定义训练的模式
 */
enum TrainType
{
    TestType1
    ,TestType2
    ,FormalType1
    ,FormalType2
    ,FormalType3
};

/**
 * @brief 生成一个随机的任务
 */
class TrainInfo{
public:
    TrainInfo(int trainCount = 3,std::initializer_list<int> total = {4,5,6});
    template<typename Ite1>
    static void shuffle(Ite1 begin,Ite1 end);
    QList<QPair<int,int> > getTaskList() const;
private:
    QList<QPair<int,int> > m_task;
};


}


template<typename Ite1>
void PMT::TrainInfo::shuffle(Ite1 begin, Ite1 end)
{
    auto len = std::distance(begin,end);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0;i<len;++i)
    {
        int index = round((qrand() / float(RAND_MAX)) * (len-1));
        std::swap(*(begin+i),*(begin+index));
    }
}

#endif // GLOBALS_H
