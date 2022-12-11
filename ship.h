//
// Created by Abraham Doughty on 12/11/22.
//

#ifndef RUNNER_SHIP_H
#define RUNNER_SHIP_H

#include "shape.h"
#include "rect.h"

class Ship {
protected:
    Rect body;
    Rect gun1;
    Rect gun2;
    point2D center;
    color bodyFill;
    color gunsFill;

public:
    Ship();
    Ship(color fill, color guns, point2D center);

    virtual ~Ship() = default;

    point2D getCenter();
    point2D getGunCenter1();
    point2D getGunCenter2();
    color getGunFill();
    color getBodyFill();

    void setGunFill(color guns);
    void setBodyFill(color body);
    void setCenter(point2D center);


    void move(int deltaX, int deltaY);
    void draw();

};


#endif //RUNNER_SHIP_H
