//
// Created by Abraham Doughty on 12/11/22.
//

#ifndef RUNNER_SHIP_H
#define RUNNER_SHIP_H

#include "shape.h"
#include "rect.h"

//class for friendly ships
class Ship {
protected:
    Rect body;
    Rect gun1;
    Rect gun2;
    point2D center;
    color bodyFill;
    color gunsFill;

public:
    //constructors
    Ship();
    Ship(color fill, color guns, point2D center);

    //destructor
    virtual ~Ship() = default;

    //getters
    point2D getCenter();
    point2D getGunCenter1();
    point2D getGunCenter2();
    color getGunFill();
    color getBodyFill();

    //setters
    void setGunFill(color guns);
    void setBodyFill(color body);
    void setCenter(point2D center);

    //moves ship around map
    void move(int deltaX, int deltaY);
    //draws ship on screen
    void draw();

};


#endif //RUNNER_SHIP_H
