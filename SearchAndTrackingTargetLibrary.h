#ifndef SEARCHANDTRACKINGTARGETLIBRARY_H
#define SEARCHANDTRACKINGTARGETLIBRARY_H

#include "searchandtrackingtargetlibrary_global.h"
#include "SearchAndTrackingTarget.h"

extern "C" SearchAndTrackingTarget* SEARCHANDTRACKINGTARGETLIBRARYSHARED_EXPORT  initNewObject();
extern "C" void SEARCHANDTRACKINGTARGETLIBRARYSHARED_EXPORT  deleteObject(SearchAndTrackingTarget* obj);


#endif // SEARCHANDTRACKINGTARGETLIBRARY_H
