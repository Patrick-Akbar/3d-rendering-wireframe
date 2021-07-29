#ifndef VECTORS_H
#define VECTORS_H

#include <Eigen>
#include <vector>
#include <string>
//Classes & methods

//Point Class
class Point{
	private:
		Eigen::Vector3d coords;
	public:
		Point ();
		Point (Eigen::Vector3d& position);
		
		Eigen::Vector3d getCoords();
		bool operator==(Point otherPoint);
};

//Line Class
class Line{
	private:
		Point pos;
		Eigen::Vector3d length;
	public: 
		Line ();
		Line (Eigen::Vector3d& start, Eigen::Vector3d& end);
		Line (Point& start, Point& end);
		Line (Point& start, Eigen::Vector3d& direction);
		Line (Eigen::Vector3d& direction);
		
		bool operator==(Line otherLine);
		Point getPos();
		Eigen::Vector3d getDirection();
		std::vector<Point> getPoints();
		
};

//Plane Class
class Plane{
	private:
		Line normalLine; //Plane is defined by a line normal to the plane & a point on the plane
		//Since an instance of the Line class has both a direction and a point, only one property is needed
		void generatePlane(Point& point, Eigen::Vector3d& line1, Eigen::Vector3d& line2);
	public:
		Plane ();
		Plane (Point& point1, Point& point2, Point& point3);
		Plane (Point& point, Eigen::Vector3d line1, Eigen::Vector3d line2);
		Plane (Line& normal);
		
		Line getNormal();
};

class Face{
	private:
		Plane plane;
		std::vector<Point> points;
		int numberOfPoints;
	public:
		Face();
		Face(std::vector<Point> pointsIn, int numberOfPointsIn);
		Face(std::vector<Line> linesIn, int numberOfLinesIn);
		
		std::vector<Point> getPoints(int* numberOfPointOut);
		Plane getPlane();
};

class Polyhedron{
	private:
		std::vector<Point> points;
		int numberOfPoints;
		std::vector<string> aliases;
		std::vector<int> connectionStart;
		std::vector<int> connectionEnd;
		std::vector<Line> lines;
		int numberOfLines;
		
		int getIndexOfAlias(string alias);
		void removeConnection(int connectionIndex);
	public:
		void addAlias(string newAlias, string currentAlias);
		std::vector<Line> getLines(int *numLines);
		void addPoint(Point pointToAdd, string alias="");
		void removePoint(string pointAlias);
		void addConnection(string startAlias, string endAlias);
		void removeConnection(string startAlias, string endAlias);
		Polyhedron();
		Polyhedron(std::vector<Point> pointsIn, std::vector<string> aliasesIn, std::vector<string> connectionsStartIn, std::vector<string> connectionsEndIn);
};

#endif
