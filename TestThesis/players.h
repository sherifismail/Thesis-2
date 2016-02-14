#pragma once
#include <string>
#include <opencv\highgui.h>
#include <opencv\cv.h>
using namespace std;
using namespace cv;
class players
{
public:
	players();
	~players();
	players(string name);
	int getxPos();
	void setxPos(int x);

	int getyPos();
	void setyPos(int y);

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar min);
	void setHSVmax(Scalar max);

	string getType(){ return type; }

	void setType(string t){ type = t; }

	Scalar getcolor(){
		return color;
	}

	void setcolor(Scalar c){
		color = c;
	}
private:
	int xPos, yPos;
	string type;
	Scalar HSVmin, HSVmax;
	Scalar color;
};

