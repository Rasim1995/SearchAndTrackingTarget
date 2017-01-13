#include "SearchAndTrackingTarget.h"
#include <QMutex>

using namespace cv;
using namespace std;

SearchAndTrackingTarget::SearchAndTrackingTarget()
{
    _original0.create(gl_heightOriginal, gl_widthOriginal, CV_16UC1);
    _original.create(gl_heightOriginal, gl_widthOriginal, CV_8UC1);

    _delta = 50;
    _lossTime = -1000;
    _maxLossTime = 5;
    _minLossTime = 0;
    _treshold1 = 50;
    _treshold2 = 100;
    _isFounded = false;

    setTypeOfCamera(0);
    reset();
}


void SearchAndTrackingTarget::follow()
{
    Canny(_roi, _cannyOut, this->_treshold1, this->_treshold2);

    _offset.x = _rectangleRoi.x;
    _offset.y = _rectangleRoi.y;

    findContours( _cannyOut.clone(), _contours, RETR_EXTERNAL, CHAIN_APPROX_NONE, _offset );

    if(_contours.size()>0)
    {
        int maxDiff=-1;

        for(size_t i=0; i<_contours.size(); i++)
        {
            Rect rectEngine = boundingRect(_contours[i]);

            if(rectEngine.height<=_maxSizeOfTarget.height && rectEngine.width<=_maxSizeOfTarget.width &&
                    rectEngine.height>=_minSizeOfTarget.height && rectEngine.width>= _minSizeOfTarget.width)
            {

                Mat engine = _original(rectEngine).clone();
                Point p = maxBrightness(rectEngine);
                Rect rectRegion = createRegion(rectEngine, 1);
                Mat region = _original(rectRegion).clone();
                rectEngine.x -= rectRegion.x;
                rectEngine.y -= rectRegion.y;

                fill(region, 0, rectEngine);

                int mb = maxBrightness(engine);
                if(mb<53)
                    continue;
                int ab = averageBrightnees(region);////////

                int diff = (mb - ab);




//                if(_typeOfCamera ==0 && gl_debug)
//                {
//                    qDebug()<<mb<<" - "<<ab<<" = "<<diff;
//                    imshow("cannyOut", _cannyOut);
//                    imshow("region", region);
//                    imshow("BigRegion", _original(createRegion(rectRegion, 2)));
//                    imshow("engine", engine);
//                    waitKey();
//                    waitKey(15);
//                }
                if(diff>maxDiff && distance(p, _positionTarget)<_maxDistance)
                {
                    maxDiff = diff;
                    _bufer = _contours[i];
                }
            }
        }

        if(maxDiff>10)
        {
            _rectangleTarget = boundingRect(_bufer);
            createRoi(_delta);
            _positionTarget = maxBrightness(_rectangleTarget);

            if(_lossTime<_maxLossTime)
                _lossTime++;
            return;
        }
    }

    _lossTime--;
    if(_lossTime<_minLossTime)
        _isFounded = false;
    createRoi(_delta+(_lossTime+1)*30);
}

void SearchAndTrackingTarget::search()
{
    Canny( _original, _cannyOut, _treshold1, _treshold2);



    findContours(_cannyOut.clone(), _contours, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point() );

    if(_contours.size()==0)
        return;
    int maxDiff=-1;
    int c=0;
    for(size_t i = 0; i < _contours.size(); i++)
    {
        _rectangleTarget = boundingRect(_contours[i]);
        if(_rectangleTarget.height<=_maxSizeOfTarget.height && _rectangleTarget.width<=_maxSizeOfTarget.width &&
                _rectangleTarget.height>=_minSizeOfTarget.height && _rectangleTarget.width>= _minSizeOfTarget.width)
        {
            Mat engine = _original(_rectangleTarget).clone();
            Rect rectRegion = createRegion(_rectangleTarget, 1);
            Mat region = _original(rectRegion).clone();
            Rect rectEngine;
            rectEngine.x = _rectangleTarget.x - rectRegion.x;
            rectEngine.y = _rectangleTarget.y - rectRegion.y;
            rectEngine.height = _rectangleTarget.height;
            rectEngine.width = _rectangleTarget.width;

            fill(region, 0, rectEngine);


            int mb = maxBrightness(engine);
            int ab = averageBrightnees(region);

            int diff = (mb - ab);

//            if(_typeOfCamera ==0 && gl_debug)
//            {
//                qDebug()<<mb<<" - "<<ab<<" = "<<diff;
//                imshow("cannyOut", _cannyOut);
//                imshow("region", region);
//                imshow("BigRegion", _original(createRegion(rectRegion, 2)));
//                imshow("engine", engine);
//                waitKey();
//                waitKey(15);
//            }


            if(diff>maxDiff)
            {
                maxDiff = diff;
                _bufer = _contours[i];
            }

            //qDebug()<<"maxDif: "<<diff;
        }
        c++;
    }

    if(maxDiff<10)
        return;

    _rectangleTarget = boundingRect(_bufer);
    _positionTarget = maxBrightness(_rectangleTarget);
    createRoi(_delta);
    _lossTime=0;
    _isFounded = true;

    //    imshow("Target", _roi);
    //    waitKey(5);

}

void SearchAndTrackingTarget::PROCESSING()
{
    adjustmentContrastAndBrigtness((ushort*)_original0.data, _original.data, gl_sizeFrameOriginal, 0, 0, true);
    setTreshold();


    if(isFounded())
    {
        follow();
    }
    else
    {
        search();
    }
    qDebug();
}

int SearchAndTrackingTarget::getX()
{
    return _positionTarget.x;
}

int SearchAndTrackingTarget::getY()
{
    return _positionTarget.y;
}

bool SearchAndTrackingTarget::isFounded()
{
    return _isFounded;
}

bool SearchAndTrackingTarget::isCaptured()
{
    if(_lossTime<_maxLossTime)
        return false;
    else
        return true;
}

void SearchAndTrackingTarget::reset()
{
    _isFounded = false;
}




Rect SearchAndTrackingTarget::createRegion(Rect baseRect, float k)
{
    Rect result;
    int deltaW = baseRect.width*k+4;
    int deltaH = baseRect.height*k+4;
    result.height = baseRect.height + deltaH;
    result.width = baseRect.width + deltaW;
    result.y = baseRect.y - deltaH/2;
    result.x = baseRect.x - deltaW/2;

    if(result.x<0)
    {
        result.width +=result.x;
        result.x=0;
    }
    if(result.y<0)
    {
        result.height +=result.y;
        result.y=0;
    }
    if(result.x+result.width>_original.cols)
    {

        result.width-=(result.x+result.width-_original.cols);

    }
    if(result.y+result.height>_original.rows)
    {
        result.height-=(result.y+result.height-_original.rows);
    }

    return result;
}

int SearchAndTrackingTarget::maxBrightness(Mat img)
{
    int max = 0;
    int sizeImg = img.rows*img.cols;
    for(int i = 0; i<sizeImg; i++)
    {
        if((int)img.data[i]>max)
        {
            max = (int)img.data[i];
        }
    }

    return max;
}

int SearchAndTrackingTarget::minBrightness(Mat img)
{
    int min = 0;
    int sizeImg = img.rows*img.cols;
    for(int i = 0; i<sizeImg; i++)
    {
        if((int)img.data[i]<min)
        {
            min = (int)img.data[i];
        }
    }

    return min;
}

Point SearchAndTrackingTarget::maxBrightness(Rect roi)
{
    Mat roiImg = _original(roi);
    Point bestPoint;
    uchar maxB = 0;
    for(int y = 0; y<roiImg.rows; y++)
    {
        for(int x = 0; x<roiImg.cols; x++)
        {
            if(roiImg.at<uchar>(y, x)>maxB)
            {
                bestPoint.x =x;
                bestPoint.y =y;
                maxB = roiImg.at<uchar>(y, x);
            }
        }
    }

    bestPoint.x+=roi.x;
    bestPoint.y += roi.y;

    return bestPoint;
}

int SearchAndTrackingTarget::averageBrightnees(Mat img)
{
    int average=0;
    int sum=0;
    size_t sizeImg = img.rows*img.cols;
    int c=0;

    for(size_t i = 0; i<sizeImg; i++)
    {
        if(img.data[i]!=0)
        {
            sum +=(int)img.data[i];
            c++;
        }
    }

    if(c==0)
        return 0;
    average = (sum/c);
    return average;
}

void SearchAndTrackingTarget::setImage(ushort *img)
{
    _original0.data = (uchar*)img;
}

void SearchAndTrackingTarget::setTypeOfCamera(int cameraType)
{
    _typeOfCamera = cameraType;
    if(cameraType==0)
    {
        _maxSizeOfTarget.width=20;
        _maxSizeOfTarget.height = 20;
        _minSizeOfTarget.width = 1;
        _minSizeOfTarget.width = 1;
        _maxDistance = 50;
    }
    else
    {
        _maxSizeOfTarget.width=100;
        _maxSizeOfTarget.height = 100;
        _minSizeOfTarget.width = 2;
        _minSizeOfTarget.width = 2;
        _maxDistance = 100;
    }
}


void SearchAndTrackingTarget::createRoi(int delta)
{
    _rectangleRoi.height = _rectangleTarget.height + delta;
    _rectangleRoi.width = _rectangleTarget.width + delta;
    _rectangleRoi.y = _rectangleTarget.y - delta/2;
    _rectangleRoi.x = _rectangleTarget.x - delta/2;

    if(_rectangleRoi.x<0)
    {
        _rectangleRoi.x=0;
    }
    if(_rectangleRoi.y<0)
    {
        _rectangleRoi.y=0;
    }
    if(_rectangleRoi.x+_rectangleRoi.width>_original.cols)
    {
        if(_rectangleRoi.x>(_rectangleRoi.x+_rectangleRoi.width-_original.cols))
            _rectangleRoi.x-= (_rectangleRoi.x+_rectangleRoi.width-_original.cols);
        else
        {
            _rectangleRoi.x=0;
            _rectangleRoi.width-=(_rectangleRoi.x+_rectangleRoi.width-_original.cols);
        }
    }
    if(_rectangleRoi.y+_rectangleRoi.height>_original.rows)
    {
        if(_rectangleRoi.y>(_rectangleRoi.y+_rectangleRoi.height-_original.rows))
            _rectangleRoi.y-=(_rectangleRoi.y+_rectangleRoi.height-_original.rows);
        else
        {
            _rectangleRoi.y=0;
            _rectangleRoi.height-=(_rectangleRoi.y+_rectangleRoi.height-_original.rows);
        }
    }

    try
    {
        _roi = _original(_rectangleRoi);
    }
    catch(Exception e)
    {
        qDebug()<<QString::fromStdString(e.msg);
        qDebug()<<_rectangleRoi.x<<" "<<_rectangleRoi.y<<" "<<_rectangleRoi.width<<" "<<_rectangleRoi.height;
    }
}

float SearchAndTrackingTarget::congeniality()
{
    return 0;
}

void SearchAndTrackingTarget::drawAim(int x, int y, Mat img)
{
    int sizeAim= 10;
    Point p1, p2;
    p1.x = x;
    p1.y = y-sizeAim;

    p2.x = x;
    p2.y = y+sizeAim;

    line(img, p1, p2, Scalar(0), 1);//вертикальная линия

    p1.x = x-sizeAim;
    p1.y = y;

    p2.x = x+sizeAim;
    p2.y = y;

    line(img, p1, p2, Scalar(0), 1);//горизонтальная линия
}

void SearchAndTrackingTarget::fill(Mat &img, int color, Rect rect)
{
    for(int i=rect.y; i < rect.y + rect.height; i++)
    {
        for(int j= rect.x; j < rect.x + rect.width; j++)
        {
            img.at<uchar>(i,j) = (uchar)color;
        }
    }
}

float SearchAndTrackingTarget::distance(Point p1, Point p2)
{
    return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
}

void SearchAndTrackingTarget::setTreshold()
{    
    if(isFounded())
    {

        int meanValue = averageBrightnees(_roi);
        _treshold1 = meanValue*0.66;
        _treshold2 = meanValue*1.44;
    }
    else
    {
        uint meanValue = averageBrightnees(_original);
        _treshold1 = meanValue*0.66;
        _treshold2 = meanValue*1.44;
    }
}

void SearchAndTrackingTarget::adjustmentContrastAndBrigtness(ushort *img, uchar *outputImg, uint sizeImg, int contrast, int brigtness, bool autoCorrection)
{
    long AverageLong=0;
    for (uint i=0; i<sizeImg; i++)
        AverageLong += (long)img[i];

    long Average=(long)(AverageLong/(sizeImg));

    if(autoCorrection)
    {
        long AverageSignal_local=0;

        for (uint i=0; i< (sizeImg); i++)
            AverageSignal_local += abs((long long)img[i]-Average);

        float std= (float)AverageSignal_local/sizeImg;
        float delta=std*8;
        if (delta<2050)  delta=2050;
        contrast = (int)((16384*128)/delta);
        brigtness = 5000;
    }

    for (uint i=0; i<sizeImg; i++)
    {
        int pix_int=(int)((img[i]-Average)*contrast/128)+brigtness;

        if (pix_int>16383)  pix_int=16383;
        if (pix_int<0)  pix_int=0;
        outputImg[i]=(uchar)((pix_int/64));
    }
}

void SearchAndTrackingTarget::compressing(ushort *inputImg, uchar *outputImg, int sizeImage)
{
    for(int i=0; i<sizeImage; i++)
    {

        outputImg[i] = qRound(inputImg[i]/257.0);
    }
}
