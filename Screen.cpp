//Screen 
using namespace std;
#include "Vectors.h"
#include "3DFunctions.h"
#include "Polygons.h"
#include "Screen.h"
#include <SFML/Graphics.hpp>
//#include <Mouse.hpp>
#include <Eigen>
#include <stdexcept>

Screen::Screen(){
	
}

Screen::Screen(int w, int h){
	window = new sf::RenderWindow(sf::VideoMode(w,h), "3D Rendering");
	window->setKeyRepeatEnabled(false);//Prevents repeat calls to key presses when key is held
	window->setFramerateLimit(60);
	window->setMouseCursorVisible(false);
}

void Screen::render(std::vector<Polygon> shapesToRender, int numberOfShapes, std::vector<Line2D> linesToRender, int numberOfLines){
	window->clear(sf::Color::White);
	for (int i = 0; i < numberOfShapes; i++){ 
		shapesToRender[i].draw(window);
	}
	for (int i = 0; i < numberOfLines; i++){ 
		linesToRender[i].draw(window);
	}
	window->display();
}

bool Screen::isOpen(){
	return window->isOpen();
}

bool Screen::pollEvent(sf::Event* event){ //test Must pass event as a pointer or the original value is not changed and the event is not returned
	return window->pollEvent(*event);
}

void Screen::close(){
	window->close();
}

int Screen::getWidth(){
	sf::Vector2u size = window->getSize();
	return size.x;
}

int Screen::getHeight(){
	sf::Vector2u size = window->getSize();
	return size.y;
}

Eigen::Vector2d Screen::centerMouse(bool moveCam){
	int height = getHeight();
	int width = getWidth();
	if (height % 2 == 1){
		height -= 1;
	}
	if (width % 2 == 1){
		width -= 1;
	}
	sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	sf::Vector2i screenCenter(width/2,height/2);
	if (moveCam){
		if (not (mousePos == screenCenter)){
			sf::Mouse::setPosition(screenCenter, *window);
			double xAngle, yAngle;
			xAngle = (mousePos.x - screenCenter.x)/(1.0*screenCenter.x);
			yAngle = (mousePos.y - screenCenter.y)/(1.0*screenCenter.y);
			Eigen::Vector2d angleToMove(xAngle,yAngle);
			return angleToMove;
		} else{
			return Eigen::Vector2d::Zero(2);
		}
	} else{
		sf::Mouse::setPosition(screenCenter, *window);
		return Eigen::Vector2d::Zero(2);
	}
}
