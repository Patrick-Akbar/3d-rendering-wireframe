//<IMPORTANT> This needs to be recompiled into an object file (.o) for it to work

//This is the file which does all the maths - intersections of planes 'n' stuff
using namespace std;
#include <math.h>
#include "Vectors.h"
#include <Eigen>
#include <stdexcept>

//Constructors

//Point Constructors
Point::Point () {
}

Point::Point (Eigen::Vector3d& position){
	coords = position;
}

//Line Constructors
Line::Line (){
}

Line::Line (Eigen::Vector3d& start, Eigen::Vector3d& end){
	pos = Point(start);
	length = end - start;	
}

Line::Line (Point& start, Point& end){
	pos = start;
	length = end.getCoords() - start.getCoords();
}

Line::Line (Point& start, Eigen::Vector3d& direction){
	pos = start;
	length = direction;
}

Line::Line (Eigen::Vector3d& direction){
	Eigen::Vector3d zeroVector = Eigen::Vector3d::Zero();
	pos = Point(zeroVector); //Sets position to (0,0,0)
	length = direction;
}

//Plane Constructors
Plane::Plane (){
}

Plane::Plane (Point& point1, Point& point2, Point& point3){
	if (point1 == point2 || point1 == point3 || point2 == point3){
		throw std::invalid_argument( "Points must all be different" );
	}
	Eigen::Vector3d line1 = point1.getCoords() - point2.getCoords();
	Eigen::Vector3d line2 = point1.getCoords() - point3.getCoords();
	try {
		generatePlane(point1,line1,line2);
	} catch (std::invalid_argument& e){
		throw std::invalid_argument("Points must not lie along the same line");
	}
}

Plane::Plane (Point& point, Eigen::Vector3d line1, Eigen::Vector3d line2){
	generatePlane(point, line1, line2);
}

Plane::Plane (Line& normal){
	normalLine = normal;
}

//Face Constructors
Face::Face(){
	
}

Face::Face(std::vector<Point> pointsIn, int numberOfPointsIn){
	numberOfPoints = numberOfPointsIn;
	if (numberOfPoints < 3){
		throw std::invalid_argument("Faces must be made up of at least 3 points");
	}
	std::vector<Line> lines;
	for (int i = 0; i < numberOfPoints; i++){
		lines.push_back(Line(pointsIn[i], pointsIn[(i+1)%numberOfPoints]));
		points.push_back(pointsIn[i]);
	}
	bool allNormal = true;
	Eigen::Vector3d n1 = lines[0].getDirection().cross(lines[1].getDirection()),n2;
	for (int i = 1; i < numberOfPoints - 1; i++){
		n2 = lines[i].getDirection().cross(lines[i+1].getDirection());
		if ((n1.normalized() != n2.normalized()) and (n1.normalized() != -1 * n2.normalized())){
			allNormal = false;
			break;
		}
	n1 = n2;
	}
	if (not allNormal){
		throw std::invalid_argument("All points must lie on the same plane");
	} 
	Line normalLine(pointsIn[0],n1);
	plane = Plane(normalLine);
} 

Face::Face(std::vector<Line> linesIn, int numberOfLines){
	numberOfPoints = numberOfLines;
	if (numberOfPoints < 3){
		throw std::invalid_argument("Faces must be made up of at least 3 points");
	}
	bool allNormal = true;
	Eigen::Vector3d n1 = linesIn[0].getDirection().cross(linesIn[1].getDirection()),n2;
	for (int i = 0; i < numberOfPoints; i++){
		points.push_back(linesIn[i].getPos());
	}
	for (int i = 1; i < numberOfLines - 1; i++){
		n2 = linesIn[i].getDirection().cross(linesIn[i+1].getDirection());
		if ((n1.normalized() != n2.normalized()) and (n1.normalized() != -1 * n2.normalized())){
			allNormal = false;
			break;
		}
	n1 = n2;
	}
	if (not allNormal){
		throw std::invalid_argument("All points must lie on the same plane");
	} else{
		Line normalLine(points[0],n1);
		plane = Plane(normalLine);
	}
}

//Polyhedron Constructors
Polyhedron::Polyhedron(){
	numberOfPoints = 0;
	numberOfLines = 0;
}

Polyhedron::Polyhedron(std::vector<Point> pointsIn, std::vector<string> aliasesIn, std::vector<string> connectionsStartIn, std::vector<string> connectionsEndIn){
	numberOfPoints = 0; //Points are added using the "addPoint" method, which requires numberOfPoints to have a value
	numberOfLines = 0;
	int numPoints, numAliases, numConStart, numConEnd;
	numPoints = pointsIn.size(); numAliases = aliasesIn.size();
	numConStart = connectionsStartIn.size(); numConEnd = connectionsEndIn.size();
	if (numPoints != numAliases){
		throw std::invalid_argument("Number of points and aliases must be the same");
	}
	if (numConStart != numConEnd){
		throw std::invalid_argument("Length of connectionsStartIn not equal to length of connectionsEndIn");
	}
	for (int i = 0; i < numPoints; i++){
		addPoint(pointsIn[i], aliasesIn[i]);
	}
	for (int i = 0; i < numConStart; i++){
		addConnection(connectionsStartIn[i], connectionsEndIn[i]);
	}
}

//Operator override functions

//Point Operators
bool Point::operator==(Point otherPoint){
	for (int i = 0; i < 3; i++){
		if (coords[i] != otherPoint.getCoords()[i]){
			return false;
		}
	}
	return true;
}

//Line Operators
bool Line::operator==(Line otherLine){
	for (int i = 0; i < 3; i++){
		if (length[i] != otherLine.getDirection()[i]){
			return false;
		}
		if (not (pos == otherLine.getPos())){
			return false;
		}
	}
	return true;
}


//Accessor functions

//Point Accessors
Eigen::Vector3d Point::getCoords(){
	return coords;
}

//Line Accessors
Point Line::getPos(){
	return pos;
}

Eigen::Vector3d Line::getDirection(){
	return length;	
}

std::vector<Point> Line::getPoints(){
	std::vector<Point> points;
	points.push_back(pos);
	Eigen::Vector3d endCoords = pos.getCoords() + length;
	Point endPoint(endCoords);
	points.push_back(endPoint);
	return points;
}

//Plane Accessors
Line Plane::getNormal(){
	return normalLine;
}

//Face Accessors
std::vector<Point> Face::getPoints(int* numberOfPointsOut){
	*numberOfPointsOut = numberOfPoints;
	return points;
}

Plane Face::getPlane(){
	return plane;
}

//Polyhedron Accessors
std::vector<Line> Polyhedron::getLines(int *numLines){
	*numLines = numberOfLines;
	return lines;
}

//Other Functions

//Generate a normal line from a point and 2 vectors
void Plane::generatePlane(Point& point, Eigen::Vector3d& line1, Eigen::Vector3d& line2){
	if ( line1.norm() == 0 || line2.norm() == 0 ){
		throw std::invalid_argument( "Vectors must not be zero vectors" );
	}
	Eigen::Vector3d crossProduct;
	crossProduct = line1.cross(line2); //The cross product of 2 vectors results in a vector perpendicular to both...
	if (crossProduct.norm() == 0) {
			throw std::invalid_argument( "Vectors must not be parallel" );//... unless they're parallel or [0,0,0]
	}else {
		normalLine = Line(point, crossProduct);
	}
};

//Alias system
int Polyhedron::getIndexOfAlias(string alias){
	int i = 0;
	for (; i < numberOfPoints; i++){
		if (aliases[i] == alias){
			return i;
		}
	}
	throw std::invalid_argument("Alias not found");
}

void Polyhedron::addAlias(string newAlias, string currentAlias){
	int index = getIndexOfAlias(currentAlias);
	try{
		getIndexOfAlias(newAlias);
	} catch (std::invalid_argument){
		aliases[index] = newAlias;
		return;
	}
	throw std::invalid_argument("Alias already in use");
}

void Polyhedron::addPoint(Point pointToAdd, string alias){
	if (alias == ""){
		alias = static_cast<ostringstream*>( &(ostringstream() << numberOfPoints) )->str(); //Converts numberOfPoints to string
	}
	aliases.push_back(alias);
	points.push_back(pointToAdd);
	numberOfPoints++;
}

void Polyhedron::removePoint(string pointAlias){ //NB This totally removes all the lines connecting to this point
	int index = getIndexOfAlias(pointAlias);
	for (int i = 0; i < numberOfLines; i++){
		if (connectionStart[i] == index || connectionEnd[i] == index){
			removeConnection(i);
		}
	}
	points.erase(points.begin() + index);
	numberOfPoints--;
}

void Polyhedron::addConnection(string startAlias, string endAlias){
	int startIndex, endIndex;
	startIndex = getIndexOfAlias(startAlias);
	endIndex = getIndexOfAlias(endAlias);
	connectionStart.push_back(startIndex);
	connectionEnd.push_back(endIndex);
	lines.push_back(Line(points[startIndex],points[endIndex]));
	numberOfLines++;
}

void Polyhedron::removeConnection(string startAlias, string endAlias){
	int startIndex, endIndex;
	startIndex = getIndexOfAlias(startAlias);
	endIndex = getIndexOfAlias(endAlias);
	bool foundConnection = false;
	int i = 0;
	for (; i < numberOfLines; i++){
		foundConnection = (startIndex == connectionStart[i] && endIndex == connectionEnd[i]);
		if (foundConnection){
			break;
		}
	}
	if (not foundConnection){
		throw std::invalid_argument("Line not found");
	}
	removeConnection(i);
}

void Polyhedron::removeConnection(int lineIndex){
	connectionStart.erase(connectionStart.begin() + lineIndex);
	connectionEnd.erase(connectionEnd.begin() + lineIndex);
	lines.erase(lines.begin() + lineIndex);
	numberOfLines--;
}

