using namespace std;
#include "Vectors.h"
#include "3DFunctions.h"
#include "Camera.h"
#include "Polygons.h"
#include<math.h>
#include <Eigen>
#include <stdexcept>

#define PI 3.1415926535898

//Methods

Eigen::Vector3d Camera::findDistances(Point point) { //output is the distance in the forwards direction, up direction and right direction respectively
	Eigen::Vector3d forwards = forwardsDirection.getDirection(), up = upwardsDirection.getDirection(), right = rightDirection.getDirection();
	Line forwardsLine(position,forwards), upLine, rightLine;
	Eigen::Vector3d closestPointCoords, lineToPoint, output, cameraToClosestPoint; //Need to set up line to include camera position
	double forwardsLambda = closestPointOnLineLambda(forwardsLine, point), upLambda, rightLambda;
	closestPointCoords = forwards * forwardsLambda + position.getCoords();
	Point closestPoint = Point(closestPointCoords);
	upLine = Line(closestPoint, up);
	rightLine = Line(closestPoint, right);
	upLambda = closestPointOnLineLambda(upLine, point), rightLambda = closestPointOnLineLambda(rightLine, point);
	output[0] = forwardsLambda;
	output[1] = upLambda;
	output[2] = rightLambda;
	return output;
}

std::vector<Polygon> Camera::getShapes(int* numOfShapes) {
	std::vector<Polygon> shapesToDraw;
	std::vector<Point> shapePoints;
	int shapePointsNum, numPointsOnScreen, numShapesToDraw = 0;
	std::vector<Eigen::Vector3d> distances, pointsOnScreen;
	std::vector<Line2D> sides;
	Point intersect;
	Eigen::Vector3d intersectDistances;
	std::vector<Face> adjustedFaces;

	Point planeStartPoint;
	Eigen::Vector3d planeStartCoords;
	planeStartCoords = position.getCoords() + 0.001 * forwardsDirection.getDirection();
	planeStartPoint = Point(planeStartCoords);
	Eigen::Vector3d planeDirection = forwardsDirection.getDirection();
	Line planeNormal(planeStartPoint, planeDirection);
	Plane justInFrontPlane(planeNormal);
	Line sideLine;
	Eigen::Vector3d screenCorners[3] = {Eigen::Vector3d(screenWidth,0,0),Eigen::Vector3d(0,screenHeight,0), Eigen::Vector3d(screenWidth,screenHeight,0)};
	Line screenLines[4] = {Line(screenCorners[0]),Line(screenCorners[1]),Line(screenCorners[0],screenCorners[2]),Line(screenCorners[1],screenCorners[2])};
	Eigen::Vector3d point1, point2;

	for (int i = 0; i < *numOfShapes; i++) { //Make points relative
		int facePointNum;
		std::vector<Point> facePoints = facesToDraw[i].getPoints(&facePointNum);
		std::vector<Point> adjustedPoints;
		for (int j = 0; j < facePointNum; j++) {
			Eigen::Vector3d coords = facePoints[j].getCoords() - position.getCoords(); //This bit caused a problem when a pointer was used because the coords were repeatedly being subtracted
			adjustedPoints.push_back(Point(coords));
		}
		adjustedFaces.push_back(Face(adjustedPoints, facePointNum));
	}

	for (int i = 0; i < *numOfShapes; i++) {
		numPointsOnScreen = 0;
		Face currentFace = adjustedFaces[i];
		shapePoints = adjustedFaces[i].getPoints(&shapePointsNum);
		distances.clear(); //test had to clear this, sides & pointsOnScreen
		for (int j = 0; j < shapePointsNum; j++) {
			distances.push_back(findDistances(shapePoints[j]));
		}

		pointsOnScreen.clear();
		for (int j = 0; j < shapePointsNum; j++) {
			double xPos, yPos;
			double point1Distances[3] = {distances[j][0], distances[j][1], distances[j][2]};
			double point2Distances[3] = {distances[(j+1)%shapePointsNum][0], distances[(j+1)%shapePointsNum][1], distances[(j+1)%shapePointsNum][2]};
			if (point1Distances[0] > 0 or point2Distances[0] > 0) { //At least one is in front
				if (point1Distances[0] > 0 and point2Distances[0] > 0) { //Both in front of the camera
					get2DPoints(point1Distances, point2Distances, &point1, &point2);
				} else if (point1Distances[0] <= 0) { //point1 behind camera, point2 infront
					Line intersectLine(shapePoints[j],shapePoints[(j+1)%shapePointsNum]);
					intersect = getPlaneLineIntersect(justInFrontPlane, intersectLine);
					intersectDistances = findDistances(intersect);
					double point1Distances[3] = {intersectDistances[0], intersectDistances[1], intersectDistances[2]};
					get2DPoints(point1Distances, point2Distances, &point1, &point2);
				} else { //point2 behind camera, point1 infront
					Line intersectLine(shapePoints[j],shapePoints[(j+1)%shapePointsNum]);
					intersect = getPlaneLineIntersect(justInFrontPlane, intersectLine);
					intersectDistances = findDistances(intersect);
					double point2Distances[3] = {intersectDistances[0], intersectDistances[1], intersectDistances[2]};
					get2DPoints(point1Distances, point2Distances, &point1, &point2);
				}
			} else { //both behind
				point1 = Eigen::Vector3d::Zero(3); //test Added to prevent lines
				point2 = Eigen::Vector3d::Zero(3);
			}
			if (not (point1 - point2 == Eigen::Vector3d::Zero(3))) { //test Added to prevent lines
				if (numPointsOnScreen == 0) {
					pointsOnScreen.push_back(point1);
					pointsOnScreen.push_back(point2);
					numPointsOnScreen = 2;
				} else {
					if (pointsOnScreen[numPointsOnScreen-1] == point1) {
						pointsOnScreen.push_back(point2);
						numPointsOnScreen++;
					} else if (pointsOnScreen[numPointsOnScreen-1] == point2) {
						pointsOnScreen.push_back(point1);
						numPointsOnScreen++;
					} else {
						pointsOnScreen.push_back(point1);
						pointsOnScreen.push_back(point2);
						numPointsOnScreen += 2;
					}
				}
			}

		}

		if (numPointsOnScreen >= 2) {
			sides.clear();
			for (int j = 0; j < numPointsOnScreen; j++) {
				point1 = pointsOnScreen[j];
				point2 = pointsOnScreen[(j+1)%numPointsOnScreen];
				sides.push_back(Line2D(point1,point2)); //test Sides not cleared -> only drew first shape repeatedly
			}

			shapesToDraw.push_back(Polygon(sides, numPointsOnScreen));
			numShapesToDraw++;
		}
	}
	*numOfShapes = numShapesToDraw;
	return shapesToDraw;
}

std::vector<Line2D> Camera::getLines(int* numberOfLines) {
	Point intersect;
	std::vector<Eigen::Vector3d> distances;
	std::vector<Line2D> line2DToDraw;
	Eigen::Vector3d pointStart, pointEnd, intersectDistances;
	int numLinesToDraw = 0;
	Line adjustedLine;

	Point planeStartPoint;
	Eigen::Vector3d planeStartCoords;
	planeStartCoords = position.getCoords() + 0.001 * forwardsDirection.getDirection();
	planeStartPoint = Point(planeStartCoords);
	Eigen::Vector3d planeDirection = forwardsDirection.getDirection();
	Line planeNormal(planeStartPoint, planeDirection);
	Plane justInFrontPlane(planeNormal);

	Eigen::Vector3d t1Coords(30,0,100);

	for (int i = 0; i < *numberOfLines; i++) { //Make points relative
		std::vector<Point> linePoints = linesToDraw[i].getPoints();


		std::vector<Point> adjustedPoints;
		for (int j = 0; j < 2; j++) {
			Eigen::Vector3d coords = linePoints[j].getCoords() - position.getCoords();
			adjustedPoints.push_back(Point(coords));
		}
		distances.clear();
		distances.push_back(findDistances(adjustedPoints[0]));
		distances.push_back(findDistances(adjustedPoints[1]));

		double xPos, yPos;
		double point1Distances[3] = {distances[0][0], distances[0][1], distances[0][2]};
		double point2Distances[3] = {distances[1][0], distances[1][1], distances[1][2]};
		if (point1Distances[0] > 0 or point2Distances[0] > 0) { //At least one is in front
			if (point1Distances[0] > 0 and point2Distances[0] > 0) { //Both in front of the camera
				get2DPoints(point1Distances, point2Distances, &pointStart, &pointEnd);
			} else if (point1Distances[0] <= 0) { //point1 behind camera, point2 infront
				Line intersectLine(adjustedPoints[0],adjustedPoints[1]);
				intersect = getPlaneLineIntersect(justInFrontPlane, intersectLine);
				intersectDistances = findDistances(intersect);
				double point1Distances[3] = {intersectDistances[0], intersectDistances[1], intersectDistances[2]};
				get2DPoints(point1Distances, point2Distances, &pointStart, &pointEnd);
			} else { //point2 behind camera, point1 infront
				Line intersectLine(adjustedPoints[0],adjustedPoints[1]);
				intersect = getPlaneLineIntersect(justInFrontPlane, intersectLine);
				intersectDistances = findDistances(intersect);
				double point2Distances[3] = {intersectDistances[0], intersectDistances[1], intersectDistances[2]};
				get2DPoints(point1Distances, point2Distances, &pointStart, &pointEnd);
				if (i == 0) {
					//throw std::invalid_argument("boop");
				}
			}
		} else { //both behind
			pointStart = Eigen::Vector3d::Zero(3);
			pointEnd = Eigen::Vector3d::Zero(3);
		}

		if (not (pointStart - pointEnd == Eigen::Vector3d::Zero(3))) {
			Line2D lineToDraw(pointStart,pointEnd);
			line2DToDraw.push_back(lineToDraw);
			numLinesToDraw++;
		}
	}
	*numberOfLines = numLinesToDraw;
	return line2DToDraw;
}

void Camera::get2DPoints(double point1Distances[3], double point2Distances[3], Eigen::Vector3d* point1, Eigen::Vector3d* point2) {
	double xPos, yPos;
	xPos = (point1Distances[2]/(point1Distances[0]*tan(horizontalFoV*PI/180))+1)*screenWidth/2;//point 1
	yPos = (1 - point1Distances[1]/(point1Distances[0]*tan(verticalFoV*PI/180)))*screenHeight/2;
	*point1 = Eigen::Vector3d(xPos,yPos,0);
	xPos = (point2Distances[2]/(point2Distances[0]*tan(horizontalFoV*PI/180))+1)*screenWidth/2;//point 2
	yPos = (1 - point2Distances[1]/(point2Distances[0]*tan(verticalFoV*PI/180)))*screenHeight/2;
	*point2 = Eigen::Vector3d(xPos,yPos,0);
}

bool Camera::doIntersect(Line sideLine, Line screenLine, Eigen::Vector3d* intersect) {
	Eigen::Vector3d intersectVector = lineIntersectVector(sideLine,screenLine);
	if (intersectVector(2) != 1) {
		return false;
	} else if ((intersectVector(0) < 0 || intersectVector(0) > 1) or (intersectVector(1) < 0 || intersectVector(1) > 1)) {
		return false;
	} else {
		*intersect = sideLine.getPos().getCoords() + sideLine.getDirection() * intersectVector[0];
		return true;
	}
}

bool Camera::pointOnScreen(Eigen::Vector3d point) {

	if ((point(0) > screenWidth or point(0) < 0) || (point(1) > screenHeight or point(1) < 0)) {
		return false;
	} else {
		return true;
	}
}

void Camera::drawFace(Face faceToDraw) {
	facesToDraw.push_back(faceToDraw);
}


void Camera::drawPolyhedron(Polyhedron polyhedron) {
	int numLines;
	std::vector<Line> lines = polyhedron.getLines(&numLines);
	for (int i = 0; i < numLines; i++) {
		linesToDraw.push_back(lines[i]);
	}
}

void Camera::render() {
	screenWidth = screen.getWidth();
	screenHeight = screen.getHeight();
	int numOfShapes = facesToDraw.size(), numOfLines = linesToDraw.size();
	std::vector<Polygon> shapes = getShapes(&numOfShapes);
	std::vector<Line2D> lines = getLines(&numOfLines);
	screen.render(shapes, numOfShapes, lines, numOfLines);
	facesToDraw.clear();
	linesToDraw.clear();
}

bool Camera::isOpen() {
	return screen.isOpen();
}

bool Camera::pollEvent(sf::Event* event) {
	return screen.pollEvent(event);
}

void Camera::close() {
	screen.close();
}

void Camera::moveRelative(Eigen::Vector3d moveBy) {
	Eigen::Vector3d newCoords = position.getCoords(), up = upwardsDirection.getDirection(),
	                forwards = forwardsDirection.getDirection(), right = rightDirection.getDirection();
	newCoords += moveBy(0) * right + moveBy(1) * up + moveBy(2) * forwards;
	Point newPosition(newCoords);
	position = newPosition;
}

void Camera::moveAbsolute(Eigen::Vector3d moveBy) {
	Eigen::Vector3d newCoords = position.getCoords() + moveBy;
	Point newPosition(newCoords);
	position = newPosition;
}

void Camera::centerMouse(bool moveCam) {
	Eigen::Vector2d rotateCam = screen.centerMouse(moveCam);
	if (moveCam) {
		Eigen::Vector3d forwards = forwardsDirection.getDirection(), upwards = upwardsDirection.getDirection(), right = rightDirection.getDirection();
		Eigen::Matrix3d xRotation,yRotation, overallRotation;
		rotateCam(0) *=  horizontalFoV * PI / 180;
		rotateCam(1) *= -1 * verticalFoV * PI / 180; //Make relative to FoV and in radians
		xRotation.col(0) = Eigen::Vector3d(cos(rotateCam(0)),0,-sin(rotateCam(0)));
		xRotation.col(1) = Eigen::Vector3d(0,1,0);
		xRotation.col(2) = Eigen::Vector3d(sin(rotateCam(0)),0,cos(rotateCam(0)));
		forwards = xRotation * forwards;
		upwards = xRotation * upwards;
		right = xRotation * right;
		//Rotation perpendicular to the z axis can be handled globally

		forwards = forwards * cos(rotateCam(1)) + upwards * sin(rotateCam(1));
		upwards = upwards * cos(rotateCam(1)) - forwards * sin(rotateCam(1));
		//Rotation perpendicular to the x axis is handled locally, otherwise extra rotation is added unintentionally

		forwards.normalize();
		upwards.normalize();
		right.normalize();
		//If the vectors aren't normalized, the image starts to distort

		forwardsDirection = Line(forwards);
		upwardsDirection = Line(upwards);
		rightDirection = Line(right);
	}
}

void Camera::reset() {
	Eigen::Vector3d initialPosition(0,0,0), initialForwards(0,0,1), initialUp(0,1,0), initialRight(1,0,0);
	position = Point(initialPosition);
	forwardsDirection = Line(initialForwards);
	upwardsDirection = Line(initialUp);
	rightDirection = Line(initialRight);

}

Camera::Camera() {
}

Camera::Camera(Eigen::Vector3d coords, Eigen::Vector3d forwards, Eigen::Vector3d up, int width, int height, double vFoV, double hFoV) { //no need to give default parameters here
	position = Point(coords);
	forwards = forwards.normalized();
	up = up.normalized();
	Eigen::Vector3d right = up.cross(forwards);
	forwardsDirection = Line(forwards);
	upwardsDirection = Line(up);
	rightDirection  = Line(right);
	horizontalFoV = hFoV;
	verticalFoV = vFoV;
	screen = Screen(width,height);

}

