//
// Created by Abraham Doughty on 12/11/22.
//

#ifndef FINAL_PROJECT_ASDOUGHT_BADDIE_H
#define FINAL_PROJECT_ASDOUGHT_BADDIE_H

#include "shape.h"
#include "rect.h"
#include "circle.h"

//class for enemy ships
class Baddie {
protected:
    Circle body;
    Rect gun1;
    Rect gun2;
public:
    //constructor
    Baddie(color fill, color guns, point2D center,double radius);

    //destructor
    virtual ~Baddie() = default;

    //getters
    Circle getBody();
    Rect getGun1();
    Rect getGun2();

    //checks overlapping with a 2d point
    bool isOverlapping(point2D bullet) const;

    //moves baddie ship around map
    void move(int deltaX, int deltaY);
    //draws baddie ship
    void draw();

};

#endif //FINAL_PROJECT_ASDOUGHT_BADDIE_H
