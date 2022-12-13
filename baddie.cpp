//
// Created by Abraham Doughty on 12/11/22.
//
#include "baddie.h"
#include "graphics.h"

using namespace std;

Baddie::Baddie(color fill, color guns, point2D center,double radius) {
    body.setColor(fill);
    body.setCenter(center);
    body.setRadius(radius);
    gun1.setCenter(center.x - radius*(cos(PI/6)), center.y + radius*(sin(PI/6)));
    gun2.setCenter(center.x + radius*(cos(PI/6)), center.y + radius*(sin(PI/6)));
    gun1.setSize(radius/3, radius);
    gun2.setSize(radius/3, radius);
    gun1.setColor(guns);
    gun2.setColor(guns);
}

Circle Baddie::getBody() {
    return body;
}
Rect Baddie::getGun1() {
    return gun1;
}
Rect Baddie::getGun2() {
    return gun2;
}

bool Baddie::isOverlapping(point2D bullet) const {
    //if point is within one radius from the center of the body
    if (sqrt(((body.getCenterX() - bullet.x)*(body.getCenterX() - bullet.x))+((body.getCenterY()-bullet.y)*(body.getCenterY()-bullet.y))) < body.getRadius()) {
        //point is overlapping
        return true;
    }
    //else, point is not overlapping
    return false;
}

void Baddie::move(int deltaX, int deltaY) {
    //move components by delta X and Y
    body.setCenter(body.getCenterX() + deltaX, body.getCenterY() + deltaY);
    gun1.setCenter(gun1.getCenterX() + deltaX, gun1.getCenterY() + deltaY);
    gun2.setCenter(gun2.getCenterX() + deltaX, gun2.getCenterY() + deltaY);
}

void Baddie::draw() {
    //call component draw functions
    gun1.draw();
    gun2.draw();
    body.draw();
}
