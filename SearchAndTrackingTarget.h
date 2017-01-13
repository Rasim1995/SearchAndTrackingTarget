#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "iostream"
#include "vector"
#include <QDebug>
#include "Globals.h"
#include "searchandtrackingtargetlibrary_global.h"


using namespace cv;
using namespace std;

class SEARCHANDTRACKINGTARGETLIBRARYSHARED_EXPORT SearchAndTrackingTarget : QObject
{
        Q_OBJECT
    public:
        explicit SearchAndTrackingTarget();
        virtual void __stdcall  setImage(ushort *img);      // передать изображение для обработки
        virtual void __stdcall  setTypeOfCamera(int cameraType); // Тип камеры (0 - Болометр, 1 - Креол)// по умолчанию 0
        virtual void __stdcall  PROCESSING();             // Запустить процесс обработки
        virtual int  __stdcall  getX();
        virtual int  __stdcall  getY();
        virtual bool __stdcall  isCaptured();
        virtual void __stdcall  reset();              //сбросить


    protected:
        Size _minSizeOfTarget;
        Size _maxSizeOfTarget;
        int _maxLenghtContour;
        int _maxDistance;
        double _treshold1;
        double _treshold2;
        Mat _original, _original0;
        Rect _rectangleRoi, _rectangleTarget;
        vector<Point> _bufer;
        Mat _cannyOut;
        Mat _roi;
        Mat _show;
        Point _offset;
        vector<vector<Point> > _contours, _contours0;
        Mat _drawing;
        int _lossTime;
        int _maxLossTime;
        int _minLossTime;
        int _delta;
        Point _positionTarget;
        int _typeOfCamera;
        bool _isFounded;


        virtual void follow();
        virtual void search();

        bool   isFounded();
        Rect createRegion(Rect baseRect, float k);
        int maxBrightness(Mat img);
        int minBrightness(Mat img);
        int averageBrightnees(Mat img);
        void setTreshold();
        float congeniality();
        void createRoi(int delta);
        void drawAim(int x, int y, Mat img);
        Point maxBrightness(Rect roi);
        void fill(Mat &img, int color, Rect rect);
        float distance(Point p1, Point p2);
        void adjustmentContrastAndBrigtness(ushort *img, uchar *outputImg, uint sizeImg, int contrast, int brigtness, bool autoCorrection);
        void compressing(ushort *inputImg, uchar *outputImg, int sizeImage);
};


#endif // IMAGEPROCESSING_H
