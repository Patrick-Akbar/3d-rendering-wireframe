#ifndef SCREEN
#define SCREEN

using namespace std;
#include "Polygons.h"
#include <SFML/Graphics.hpp>
#include <Eigen>
#include <vector>



class Screen{
	private:
		sf::RenderWindow* window;
	public:
		Screen();
		Screen(int w, int h);
		
		void render(std::vector<Polygon> shapesToRender, int numberOfShapes, std::vector<Line2D> linesToRender, int numberOfLines);
		bool isOpen();
		bool pollEvent(sf::Event* event);
		void close();
		int getWidth();
		int getHeight();
		Eigen::Vector2d centerMouse(bool moveCam);
};

#endif
