#include "SearchAndTrackingTargetLibrary.h"
#include "SearchAndTrackingTarget.h"






SearchAndTrackingTarget *initNewObject()
{
    return new SearchAndTrackingTarget;
}


void deleteObject(SearchAndTrackingTarget *obj)
{
    delete obj;
    obj=NULL;
}

