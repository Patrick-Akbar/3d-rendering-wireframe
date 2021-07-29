#ifndef POLYGONS
#define POLYGONS

using namespace std;
#include <Eigen>
#include <SFML/Graphics.hpp>
#include <vector>

class Line2D{
	private:
		Eigen::Vector2d start, end;
	public:
		Line2D();
		Line2D(Eigen::Vector3d startPoint, Eigen::Vector3d endPoint);
		
		void draw(sf::RenderWindow* surface);
		
		Eigen::Vector2d getStart();
		Eigen::Vector2d getEnd();
};

class Polygon{
	private:
		int numberOfEdges;
		std::vector<Line2D> edges;
		sf::ConvexShape sfmlShape;
	public:
		Polygon();
		Polygon(std::vector<Line2D> lines, int numberOfLines);
		
		void draw(sf::RenderWindow* surface);
};

#endif
