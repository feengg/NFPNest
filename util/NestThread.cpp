#include "NestThread.h"
#include <QDebug>

//#define USE_QT
//#define SHOW_SLOW

NestThread::NestThread(QObject *parent) : QThread(parent)
{    
}

void NestThread::run()
{
    // 1.let polygons in an order
    SortByAreaDecreasing();

    m_stripNb = 0;
    QVector<Polygon> unPlaced = m_polygons;
    QVector<Polygon> operate;

    while(!unPlaced.isEmpty())
    {
        m_stripNb++;
        emit AddStrip();

        // 2.set the first locatioin
        unPlaced[0].SetLocation(0,0);
        unPlaced[0].stripID = m_stripNb-1;
        emit AddItem(unPlaced[0]);

        Polygon last = unPlaced[0];
        operate.clear();

        // 3.place the other polygons
        for(int i=1;i<unPlaced.size();++i) {

            if(doNestWait)
            {
                aMutex.lock();
                waitCondition.wait(&aMutex);
                aMutex.unlock();
            }

            Polygon &orb = unPlaced[i];
            QVector<Polygon> NFPS = noFitPolygon(last,orb,false,false);
            Polygon nfp;
            if(!NFPS.isEmpty()) {
                nfp = NFPS.takeFirst();
            }
            else {
                operate.append(orb);
                continue;
            }

            // iterate the nfp, to find the most left position to place the polygon
            int leftIndex = -1;
            int left = m_stripWidth;
            for(int i=0;i<nfp.size();++i) {
                orb.SetPosition(nfp[i],0);
                if(orb.x<0 || orb.x+orb.width>m_stripWidth)
                    continue;

                if(orb.y<0 || orb.y+orb.height>m_stripHeight)
                    continue;

                if(orb.x<left)
                {
                    left = orb.x;
                    leftIndex = i;
                }
            }

            if(leftIndex != -1) {
                orb.SetPosition(nfp[leftIndex],0);
                orb.stripID = m_stripNb-1;
                emit AddItem(orb);
#ifdef SHOW_SLOW
                msleep(1000);
#endif

                // get the hull of the polygons which have been placed
                bool ret1 = orb.IsAntiClockWise();
                bool ret2 = last.IsAntiClockWise();
                if(ret1 != ret2) {
                    std::reverse(orb.begin(),orb.end());
                }

#ifdef USE_QT
                last.FromPolygonF(orb.ToPolygonF().united(last.ToPolygonF()));
#else
                last = polygonUnion(last,orb);
#endif

#ifdef SHOW_SLOW
                last.stripID = m_stripNb-1;
                emit AddItem(last);
                msleep(1000);
#endif
            }
            else {
                operate.append(orb);
            }
        }
        unPlaced = operate;
    }

    emit NestEnd();
}

void NestThread::SetStripSize(double width, double height)
{
    m_stripWidth = width;
    m_stripHeight = height;
}

void NestThread::PauseNest()
{
    if(isRunning())
        doNestWait = true;
}

void NestThread::ResumeNest()
{
    if(isRunning())
    {
        doNestWait = false;
        waitCondition.wakeAll();
    }
}

void NestThread::SortByWidthDecreasing()
{
    for(int ctr = 0; ctr < m_polygons.size(); ++ctr)
    {
        double maxWid = m_polygons[ctr].Bounds().width;
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < m_polygons.size(); ++ctr2)
        {
            double wid = m_polygons[ctr2].Bounds().width;
            if(wid > maxWid)
            {
                maxWid = wid;
                maxIndex = ctr2;
            }
        }
        Polygon temp = m_polygons[ctr];
        m_polygons[ctr] = m_polygons[maxIndex];
        m_polygons[maxIndex] = temp;
    }
}

void NestThread::SortByAreaDecreasing()
{
    for(int ctr = 0; ctr < m_polygons.size(); ++ctr)
    {
        double maxArea = abs(m_polygons[ctr].Area());
        int maxIndex = ctr;
        for(int ctr2 = ctr + 1; ctr2 < m_polygons.size(); ++ctr2)
        {
            double area = abs(m_polygons[ctr2].Area());
            if(area > maxArea)
            {
                maxArea = area;
                maxIndex = ctr2;
            }
        }
        Polygon temp = m_polygons[ctr];
        m_polygons[ctr] = m_polygons[maxIndex];
        m_polygons[maxIndex] = temp;
    }
}
