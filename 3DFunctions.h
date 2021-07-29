#ifndef THREE_D_FUNCTIONS_H
#define THREE_D_FUNCTIONS_H
//Guard cannot start with a number
#include "Vectors.h"
#include <Eigen>

bool doPlanePointIntersect(Plane& intersectPlane, Point& intersectPoint);

Eigen::Vector3d lineIntersectVector(Line& line1, Line& line2);

double getPlaneLineLambda(Plane& plane, Line& line);

bool doPlaneLineIntersect(Plane& plane, Line& line);

Point getPlaneLineIntersect(Plane& plane, Line& line);

double closestPointOnLineLambda(Line& line, Point& point);

#endif
