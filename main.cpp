using namespace std;
#include <iostream>
#include "Vectors.h"
#include "3DFunctions.h"
#include "Screen.h"
#include "Camera.h"
#include <SFML/Graphics.hpp>
#include <Eigen>
#include <stdexcept>

std::vector<Face> cube(Point bottomFrontLeft, double sideLength);

Eigen::Vector3d y,z,a,b;
Line l1, l2;

int WinMain() {
	
	y(0) = 6; y(1) = 2; y(2) = 3;
	z(0) = 2; z(1) = 6; z(2) = 2;
	b(0) = 5; b(1) = 3; b(2) = 4.5;
	Eigen::Vector3d s1Coords(40,40,100), s2Coords(40,-40,100), s3Coords(-40,-40,100), s4Coords(-40,40,100);
	Point s1(s1Coords), s2(s2Coords), s3(s3Coords), s4(s4Coords);
	std::vector<Point> squarePoints; squarePoints.push_back(s1);squarePoints.push_back(s2);squarePoints.push_back(s3);squarePoints.push_back(s4);
	Face square(squarePoints,4);
	
	Eigen::Vector3d t1Coords(30,0,100), t2Coords(0,30,100), t3Coords(-30,0,100);
	Point t1(t1Coords), t2(t2Coords), t3(t3Coords);
	Polyhedron triangle;
	triangle.addPoint(t1,"rightPoint"); triangle.addPoint(t2,"topPoint"); triangle.addPoint(t3,"leftPoint");
	triangle.addConnection("rightPoint","topPoint"); triangle.addConnection("topPoint","leftPoint"); triangle.addConnection("leftPoint","rightPoint");
	
	std::vector<Face> floor; //Visual representation of y = 0
	Face floorSquare;
	std::vector<Point> floorPoints;
	Eigen::Vector3d f1, f2, f3, f4;
	for (int i = -4; i < 4; i++){
		for (int j = -4; j < 4; j++){
			floorPoints.clear();
			f1 = Eigen::Vector3d(200*i, 0, 200*j); f2 = Eigen::Vector3d(200*(i + 1), 0, 200*j); f3 = Eigen::Vector3d(200*(i + 1), 0, 200*(j + 1)); f4 = Eigen::Vector3d(200*i, 0, 200*(j + 1));
			floorPoints.push_back(f1); floorPoints.push_back(f2); floorPoints.push_back(f3); floorPoints.push_back(f4);
			floorSquare = Face(floorPoints, 4);
			floor.push_back(floorSquare);		
		}
	}
	
	Camera testCam(Eigen::Vector3d(0,0,0),Eigen::Vector3d(0,0,1),Eigen::Vector3d(0,1,0),1000,800);
	Eigen::Vector3d cubeBottomFrontLeft(-7.5,0,22), cube2BottomFrontLeft(7.5,0,22);
	Point cubePoint(cubeBottomFrontLeft), cube2Point(cube2BottomFrontLeft);
	std::vector<Face> testCube = cube(cubePoint,15), testCube2 = cube(cube2Point, 20);
	testCam.centerMouse(false);
	double speed = 0.08333;
	bool shiftHeld = false;	
	
	while (testCam.isOpen()){
        sf::Event event;
	    while (testCam.pollEvent(&event)){    
            if (event.type == sf::Event::Closed){
                testCam.close();
            }else if (event.type == sf::Event::GainedFocus){
				testCam.centerMouse(false);	
        	} else if (event.type == sf::Event::KeyPressed){
        		if (event.key.code == sf::Keyboard::Escape){
                	testCam.close();
                } else if (event.key.code == sf::Keyboard::R){
                	testCam.reset();
				}
			} else if (event.type == sf::Event::MouseMoved){
				testCam.centerMouse();
			}	
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
			shiftHeld = true;
			speed *= 4;
		}
		//Key press events
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			testCam.moveRelative(Eigen::Vector3d(-speed,0,0));
		} 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
			testCam.moveRelative(Eigen::Vector3d(speed,0,0));
		} 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
			testCam.moveRelative(Eigen::Vector3d(0,speed,0));
		} 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
			testCam.moveRelative(Eigen::Vector3d(0,-speed,0));
		} 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			testCam.moveRelative(Eigen::Vector3d(0,0,speed));
		} 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			testCam.moveRelative(Eigen::Vector3d(0,0,-speed));
		}
		for (int i = 0; i < 6; i++){
			testCam.drawFace(testCube[i]);
			testCam.drawFace(testCube2[i]);
		}

		for (int i = 0; i < 64; i++){
			testCam.drawFace(floor[i]);
		}
	
		if (shiftHeld){
			shiftHeld = false;
			speed /= 4;
		}
		
		testCam.drawPolyhedron(triangle);
        testCam.render();
	}
	return 0;
}

std::vector<Face> cube(Point bottomFrontLeft, double sideLength){ //going throught the cube doesn't work, also don't draw so many faces
	int pointNum = 4;
	std::vector<Face> output;
	Eigen::Vector3d bottomFrontLeftCoords = bottomFrontLeft.getCoords();
	Eigen::Vector3d bottomFrontRightCoords = bottomFrontLeftCoords + Eigen::Vector3d(sideLength,0,0); 
	Eigen::Vector3d topFrontLeftCoords = bottomFrontLeftCoords + Eigen::Vector3d(0,sideLength,0); 
	Eigen::Vector3d topFrontRightCoords = bottomFrontRightCoords + Eigen::Vector3d(0,sideLength,0); 
	Eigen::Vector3d bottomBackLeftCoords = bottomFrontLeftCoords + Eigen::Vector3d(0,0,sideLength); 
	Eigen::Vector3d bottomBackRightCoords = bottomFrontRightCoords + Eigen::Vector3d(0,0,sideLength); 
	Eigen::Vector3d topBackLeftCoords = topFrontLeftCoords + Eigen::Vector3d(0,0,sideLength); 
	Eigen::Vector3d topBackRightCoords = topFrontRightCoords + Eigen::Vector3d(0,0,sideLength); 
	
	std::vector<Line> faceLines(4);
	
	faceLines[0] = Line(bottomFrontLeftCoords, bottomFrontRightCoords); faceLines[1] = Line(bottomFrontRightCoords, topFrontRightCoords); faceLines[2] = Line(topFrontRightCoords, topFrontLeftCoords); faceLines[3] = Line(topFrontLeftCoords, bottomFrontLeftCoords);
	output.push_back(Face(faceLines, 4));
	
	faceLines[0] = Line(bottomBackLeftCoords, bottomBackRightCoords); faceLines[1] = Line(bottomBackRightCoords, topBackRightCoords); faceLines[2] = Line(topBackRightCoords, topBackLeftCoords); faceLines[3] = Line(topBackLeftCoords, bottomBackLeftCoords);
	output.push_back(Face(faceLines, 4));
	
	faceLines[0] = Line(bottomFrontLeftCoords, bottomFrontRightCoords); faceLines[1] = Line(bottomFrontRightCoords, bottomBackRightCoords); faceLines[2] = Line(bottomBackRightCoords, bottomBackLeftCoords); faceLines[3] = Line(bottomBackLeftCoords, bottomFrontLeftCoords);
	output.push_back(Face(faceLines, 4));
	
	faceLines[0] = Line(bottomFrontLeftCoords, topFrontLeftCoords); faceLines[1] = Line(topFrontLeftCoords, topBackLeftCoords); faceLines[2] = Line(topBackLeftCoords, bottomBackLeftCoords); faceLines[3] = Line(bottomBackLeftCoords, bottomFrontLeftCoords);
	output.push_back(Face(faceLines, 4));
	
	faceLines[0] = Line(bottomFrontRightCoords, topFrontRightCoords); faceLines[1] = Line(topFrontRightCoords, topBackRightCoords); faceLines[2] = Line(topBackRightCoords, bottomBackRightCoords); faceLines[3] = Line(bottomBackRightCoords, bottomFrontRightCoords);
	output.push_back(Face(faceLines, 4));
	
	faceLines[0] = Line(topFrontLeftCoords, topFrontRightCoords); faceLines[1] = Line(topFrontRightCoords, topBackRightCoords); faceLines[2] = Line(topBackRightCoords, topBackLeftCoords); faceLines[3] = Line(topBackLeftCoords, topFrontLeftCoords);
	output.push_back(Face(faceLines, 4));
	return output;
}


