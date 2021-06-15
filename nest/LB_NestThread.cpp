#include "LB_NestThread.h"
#include "LB_NestConfig.h"
using namespace NestConfig;

LB_NestThread::LB_NestThread(QObject *parent) : QThread(parent)
{    
}

void LB_NestThread::run()
{
    // 1.let polygons in an order
    if(ENABLE_ROTATION)
        RotateToMinBounds();
    if(ITEM_GAP != 0) {
        for(int ctr = 0; ctr < polygons.size(); ++ctr) {
            polygons[ctr] = polygons[ctr].Shrinking(-ITEM_GAP);
        }
    }

    SortByAreaDecreasing();

    int stripNb = 0;
    QVector<LB_Polygon2D> unPlaced = polygons;
    QVector<LB_Polygon2D> operate;

    while(!unPlaced.isEmpty())
    {
        stripNb++;
        emit AddStrip();

        // 2.set the first locatioin
        unPlaced[0].SetLocation(0,0);
        unPlaced[0].SetID(stripNb-1);
        emit AddItem(unPlaced[0]);

        LB_Polygon2D last = unPlaced[0];
        operate.clear();

        // 3.place the other polygons
        for(int i=1;i<unPlaced.size();++i) {

            if(doNestWait)
            {
                aMutex.lock();
                waitCondition.wait(&aMutex);
                aMutex.unlock();
            }

            LB_Polygon2D &orb = unPlaced[i];
            QVector<LB_Polygon2D> NFPS = NoFitPolygon(last,orb,false,false);
            LB_Polygon2D nfp;
            if(!NFPS.isEmpty()) {
                nfp = NFPS.takeFirst();
            }
            else {
                operate.append(orb);
                continue;
            }

            // iterate the nfp, to find the most left position to place the polygon
            int leftIndex = -1;
            int left = STRIP_WIDTH;
            for(int i=0;i<nfp.size();++i) {
                orb.SetPosition(nfp[i],0);
                if(orb.X()<0 || orb.X()+orb.Width()>STRIP_WIDTH)
                    continue;

                if(orb.Y()<0 || orb.Y()+orb.Height()>STRIP_HEIGHT)
                    continue;

                if(orb.X()<left)
                {
                    left = orb.X();
                    leftIndex = i;
                }
            }

            if(leftIndex != -1) {
                orb.SetPosition(nfp[leftIndex],0);
                orb.SetID(stripNb-1);
                emit AddItem(orb);

                // get the hull of the polygons which have been placed
                bool ret1 = orb.IsAntiClockWise();
                bool ret2 = last.IsAntiClockWise();
                if(ret1 != ret2) {
                    std::reverse(orb.begin(),orb.end());
                }

                last = last.United(orb);
            }
            else {
                operate.append(orb);
            }
        }
        unPlaced = operate;
    }

    emit NestEnd();
}

void LB_NestThread::PauseNest()
{
    if(isRunning())
        doNestWait = true;
}

void LB_NestThread::ResumeNest()
{
    if(isRunning())
    {
        doNestWait = false;
        waitCondition.wakeAll();
    }
}

void LB_NestThread::SortByWidthDecreasing()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        double maxWid = polygons[ctr].Bounds().Width();
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < polygons.size(); ++ctr2)
        {
            double wid = polygons[ctr2].Bounds().Width();
            if(wid > maxWid)
            {
                maxWid = wid;
                maxIndex = ctr2;
            }
        }
        LB_Polygon2D temp = polygons[ctr];
        polygons[ctr] = polygons[maxIndex];
        polygons[maxIndex] = temp;
    }
}

void LB_NestThread::SortByAreaDecreasing()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr)
    {
        double maxArea = abs(polygons[ctr].Area());
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < polygons.size(); ++ctr2)
        {
            double area = abs(polygons[ctr2].Area());
            if(area > maxArea)
            {
                maxArea = area;
                maxIndex = ctr2;
            }
        }
        LB_Polygon2D temp = polygons[ctr];
        polygons[ctr] = polygons[maxIndex];
        polygons[maxIndex] = temp;
    }
}

void LB_NestThread::RotateToMinBounds()
{
    for(int ctr = 0; ctr < polygons.size(); ++ctr) {
        polygons[ctr].RotateToMinBndRect();
    }
}
