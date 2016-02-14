#include "players.h"


players::players()
{
}

players::players(string name){

	setType(name);
	if (name == "Home player"){   //blue
		setHSVmin(Scalar(93, 99, 95));
		setHSVmax(Scalar(137, 256, 256));
		setcolor(Scalar(255, 0, 0));
	}
	if (name == "Away player"){  //red

		setHSVmin(Scalar(147, 125, 56));
		setHSVmax(Scalar(256, 255, 255));
		setcolor(Scalar(0, 0, 255));
	}
	if (name == "Ball"){

	}
}
players::~players()
{
}
int players::getxPos(){
	return players::xPos;
}
void players::setxPos(int x){
	players::xPos = x;
}
int players::getyPos(){
	return players::yPos;
}
void players::setyPos(int y){
	players::yPos = y;
}


Scalar players::getHSVmin(){
	return players::HSVmin;
}
void players::setHSVmin(Scalar min){
	players::HSVmin = min;
}


Scalar players::getHSVmax(){
	return players::HSVmax;
}
void players::setHSVmax(Scalar max){
	players::HSVmax = max;
}
