using namespace std;
#include <Eigen>
#include <SFML/Graphics.hpp>
#include "3DFunctions.h"
#include "Polygons.h"

Line2D::Line2D(){
	
}

Line2D::Line2D(Eigen::Vector3d startPoint, Eigen::Vector3d endPoint){
	start = Eigen::Vector2d(startPoint(0),startPoint(1));
	end = Eigen::Vector2d(endPoint(0),endPoint(1));
}

Eigen::Vector2d Line2D::getStart(){
	return start;
}

Eigen::Vector2d Line2D::getEnd(){
	return end;
}

void Line2D::draw(sf::RenderWindow* surface){
	sf::Vector2f startPoint(start[0],start[1]), endPoint(end[0],end[1]);
	sf::Vertex line[2] = {sf::Vertex(startPoint), sf::Vertex(endPoint)};
	line[0].color = sf::Color::Black;
	line[1].color = sf::Color::Black;
	surface->draw(line, 2, sf::Lines);
}


Polygon::Polygon(){
	
}

Polygon::Polygon(std::vector<Line2D> lines, int numberOfLines){
	edges = lines;
	numberOfEdges = numberOfLines;
	sfmlShape.setPointCount(numberOfLines);
	for (int i = 0; i < numberOfLines; i++){
		int x = lines[i].getStart()[0];
		int y = lines[i].getStart()[1];
		sfmlShape.setPoint(i, sf::Vector2f(x,y));
	}
	sfmlShape.setFillColor(sf::Color::Transparent);
	sfmlShape.setOutlineColor(sf::Color::Black);
	sfmlShape.setOutlineThickness(1);
}

void Polygon::draw(sf::RenderWindow* surface){
	for (int i = 0; i < numberOfEdges; i++){
		edges[i].draw(surface); 
	}
}


