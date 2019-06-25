#include "Globals.h"
#include <vector>
PMT::TrainInfo::TrainInfo(int startIndex, std::initializer_list<int> total)
{
    std::vector<int> totalCount;
    totalCount.resize(total.size());
    std::copy(total.begin(),total.end(),totalCount.begin());
    shuffle(totalCount.begin(),totalCount.end());
    for(int i=0;i<totalCount.size();++i)
    {
        m_task.append(qMakePair(totalCount[i],totalCount[i]-startIndex));
    }
}

QList<QPair<int, int> > PMT::TrainInfo::getTaskList() const
{
    return m_task;
}



