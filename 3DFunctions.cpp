//Functions using points, lines and planes
using namespace std;
#include <math.h>
#include "Vectors.h"
#include <Eigen>
#include <stdexcept>

bool doPlanePointIntersect(Plane& intersectPlane, Point& intersectPoint){
	Eigen::Vector3d relativePoint(3), planeNormal(3);
	relativePoint = intersectPlane.getNormal().getPos().getCoords() - intersectPoint.getCoords(); //Vector between given point and a point on the plane
	planeNormal = intersectPlane.getNormal().getDirection();
	double dotProduct = relativePoint.dot(planeNormal);
	return (dotProduct == 0);//The dot product of the normal line and the (provided point - a point on the plane) must be 0 for the point to be on the plane
}


Eigen::Vector3d lineIntersectVector(Line& intersectLine1, Line& intersectLine2){
	Eigen::Vector3d constants(3), temp(3), output(3);
	constants = intersectLine1.getPos().getCoords() - intersectLine2.getPos().getCoords() + Eigen::Vector3d::Ones();
	Eigen::Matrix3d lineMatrix, lineInverse;
	lineMatrix.col(0) = intersectLine1.getDirection();
	lineMatrix.col(1) = intersectLine2.getDirection();
	lineMatrix.col(2) = constants;
	if (lineMatrix.determinant() == 0){ //Both lines are parallel if the determinant is zero, and the matrix has no inverse
		return Eigen::Vector3d::Zero(3);  //So we can just return a vector of zeroes
	} else{
		lineInverse = lineMatrix.inverse();
		output = lineInverse * Eigen::Vector3d::Ones();
		return output; //This consists of a vector representing (lambda, -mu, 1). If the 3rd value is not 1, the lines do not meet
	}
}

double getPlaneLineLambda(Plane& plane, Line& line){
	Eigen::Vector3d planePoint = plane.getNormal().getPos().getCoords(), linePoint = line.getPos().getCoords(),
		planeNormal = plane.getNormal().getDirection(), lineDirection = line.getDirection();
	double lambda = (planePoint - linePoint).dot(planeNormal) / planeNormal.dot(lineDirection); 
	return lambda;
}

bool doPlaneLineIntersect(Plane& plane, Line& line){
	double dotProduct = plane.getNormal().getDirection().dot(line.getDirection()); //If the dot product of the normal to the plane and the direction of the line is 0, they are parallel
	double lambda;
	if (dotProduct != 0) {
		lambda = getPlaneLineLambda(plane, line);
	} else{
		return false;
	}
	return (lambda <=1 && lambda >= 0); //Ensures value is within range of line and not parallel to plane
}

Point getPlaneLineIntersect(Plane& plane, Line& line){
	double lambda  = getPlaneLineLambda(plane,line);
	Eigen::Vector3d intersectCoords = line.getPos().getCoords() + lambda * line.getDirection();
	Point intersectPoint(intersectCoords);
	return intersectPoint;
}

double closestPointOnLineLambda(Line& line, Point& point){
	Eigen::Vector3d direction = line.getDirection(), pointCoords = point.getCoords();
	double lambda;
	Line normalPointLine(point, direction);
	Plane planeOnLine(normalPointLine);
	lambda = getPlaneLineLambda(planeOnLine, line);	
	return lambda;
}


