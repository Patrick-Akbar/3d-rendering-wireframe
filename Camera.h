#ifndef CAMERA
#define CAMERA

#include "Vectors.h"
#include "3DFunctions.h"
#include "Polygons.h"
#include "Screen.h"
#include <Eigen>
#include <vector> //C++ Alternative array class

class Camera{
	private:
		Point position;
		Line forwardsDirection, upwardsDirection, rightDirection; //These should have position zero & be normalized
		double verticalFoV, horizontalFoV;
		Screen screen;
		std::vector<Face> facesToDraw;
		std::vector<Line> linesToDraw;
		int screenWidth, screenHeight;
		
		void get2DPoints(double point1Distances[3], double point2Distances[3], Eigen::Vector3d* point1, Eigen::Vector3d* point2);
		bool doIntersect(Line sideLine, Line screenLine, Eigen::Vector3d* intersect);
		bool pointOnScreen(Eigen::Vector3d point);
		std::vector<Polygon> getShapes(int* numOfShapes);
		std::vector<Line2D> getLines(int* numberOfLines);
		
	public:
		Camera();
		Camera(Eigen::Vector3d coords, Eigen::Vector3d forwards, Eigen::Vector3d up, int width, int height, double vFoV=38.66, double hFoV=45);
		void drawFace(Face face);
		void drawPolyhedron(Polyhedron polyhedron);
		void render();
		bool isOpen();
		bool pollEvent(sf::Event* event);
		void close();
		void moveRelative(Eigen::Vector3d moveBy);
		void moveAbsolute(Eigen::Vector3d moveBy);
		void centerMouse(bool moveCam = true);
		void reset();
		Eigen::Vector3d findDistances(Point point);
};


#endif
