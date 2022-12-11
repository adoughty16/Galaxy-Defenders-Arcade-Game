//
// Created by Abraham Doughty on 12/11/22.
//

#ifndef FINAL_PROJECT_ASDOUGHT_BADDIE_H
#define FINAL_PROJECT_ASDOUGHT_BADDIE_H

#include "shape.h"
#include "rect.h"
#include "circle.h"

class Baddie {
protected:
    Circle body;
    Rect gun1;
    Rect gun2;
public:
    Baddie(color fill, color guns, point2D center,double radius);

    Circle getBody();
    Rect getGun1();
    Rect getGun2();

    bool isOverlapping(point2D bullet) const;

    void move(int deltaX, int deltaY);
    void draw();

};

#endif //FINAL_PROJECT_ASDOUGHT_BADDIE_H
