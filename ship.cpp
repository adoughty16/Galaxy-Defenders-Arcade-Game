//
// Created by Abraham Doughty on 12/11/22.
//
#include "ship.h"
#include "graphics.h"
using namespace std;

Ship::Ship() {
    body = Rect();
    gun1 = Rect();
    gun2 = Rect();

    center = point2D();
    bodyFill = color();
    gunsFill = color();
}

Ship::Ship(color fill, color guns, point2D center) {
    setCenter(center);
    setBodyFill(fill);
    setGunFill(guns);
    body.setCenter(center);
    body.setSize(20,35);
    body.setColor(fill);

    gun1.setCenter(center.x-17, center.y+5);
    gun1.setSize(5, 15);
    gun2.setCenter(center.x+17, center.y+5);
    gun2.setSize(5, 15);

    gun1.setColor(guns);
    gun2.setColor(guns);
}

point2D Ship::getCenter() {
    return center;
}
point2D Ship::getGunCenter1() {
    return gun1.getCenter();
}
point2D Ship::getGunCenter2() {
    return gun2.getCenter();
}
color Ship::getGunFill() {
    return gunsFill;
}
color Ship::getBodyFill() {
    return bodyFill;
}

void Ship::setCenter(point2D center){
    this->center = center;

    body.setCenter(center);
    //need to reposition guns as well
    gun1.setCenter(center.x-10, center.y-5);
    gun2.setCenter(center.x+10, center.y-5);
}
void Ship::setGunFill(color gunColor) {
    this->gunsFill = gunColor;
    gun1.setColor(gunColor);
    gun2.setColor(gunColor);
}
void Ship::setBodyFill(color bodyColor) {
    bodyFill = bodyColor;
    body.setColor(bodyColor);
}

void Ship::move(int deltaX, int deltaY) {
    //move body and guns by delta x and y
    center = point2D(center.x + deltaX, center.y + deltaY);
    body.setCenter(center);
    gun1.setCenter(gun1.getCenterX() + deltaX, gun1.getCenterY() + deltaY);
    gun2.setCenter(gun2.getCenterX() + deltaX, gun2.getCenterY() + deltaY);
}

void Ship::draw() {
    //call component draw functions
    gun1.draw();
    gun2.draw();
    body.draw();
    //draw triangles for the nose of the ship and the wings
    glColor3f(bodyFill.red, bodyFill.green, bodyFill.blue);
    glBegin(GL_TRIANGLES);
    glVertex2i(center.x, center.y);
    glVertex2i(center.x + 28, center.y + (body.getHeight()/2));
    glVertex2i(center.x - 28, center.y + (body.getHeight()/2));
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2i(center.x, center.y - 25);
    glVertex2i(center.x - body.getWidth()/2, center.y - (body.getHeight()/2));
    glVertex2i(center.x + body.getWidth()/2, center.y - (body.getHeight()/2));
    glEnd();
}
