#include "PMTTestValue.h"

PMTTestSelRecord::PMTTestSelRecord()
    :picShowTime(QDateTime::currentDateTime())
    ,picDisappearTime(QDateTime::currentDateTime())
    ,location(-1)
{

}

int PMTTestSelRecord::intervalMs() const
{
    return this->picShowTime.msecsTo(this->picDisappearTime);
}







bool LocationTestValue::isCorrect() const
{
    return this->realLocation == this->showLocation;
}
