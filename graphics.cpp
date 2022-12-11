#include "graphics.h"
#include "circle.h"
#include "rect.h"
#include "ship.h"
#include "baddie.h"
#include "baddie.cpp"
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

GLdouble width, height;
int wd;
const color skyBlue(77/255.0, 213/255.0, 240/255.0);
const color silver(192/255.0, 192/255.0, 192/255.0);
const color grassGreen(26/255.0, 176/255.0, 56/255.0);
const color white(1, 1, 1);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color darkBlue(1/255.0, 110/255.0, 214/255.0);
const color purple(119/255.0, 11/255.0, 224/255.0);
const color black(0, 0, 0);
const color magenta(1, 0, 1);
const color orange(1, 163/255.0, 22/255.0);
const color cyan (0, 1, 1);


vector<Rect> bullets;
vector<Baddie> baddies;
Ship player = Ship(silver, orange, point2D(500,600));


void init() {
    width = 1000;
    height = 700;
    srand(time(0));
}

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(black.red, black.green, black.blue, 1.0f);
}

/* Handler for window-repaint event. Call back when the window first appears and
 whenever the window needs to be re-painted. */
void display() {
    // Tell OpenGL to use the whole window for drawing
    glViewport(0, 0, width, height); // DO NOT CHANGE THIS LINE (unless you are running Catalina on Mac)
    
    // Do an orthographic parallel projection with the coordinate
    // system set to first quadrant, limited by screen/window size
    glMatrixMode(GL_PROJECTION); // DO NOT CHANGE THIS LINE
    glLoadIdentity(); // DO NOT CHANGE THIS LINE
    glOrtho(0.0, width, height, 0.0, -1.f, 1.f); // DO NOT CHANGE THIS LINE

    // Clear the color buffer with current clearing color
    glClear(GL_COLOR_BUFFER_BIT); // DO NOT CHANGE THIS LINE
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // DO NOT CHANGE THIS LINE
    
    /*
     * Draw here
     */


    player.draw();

    for(Rect i : bullets) {
        i.draw();
    }

    for(Baddie j : baddies) {
        j.draw();
    }

    
    glFlush();  // Render now
}

// http://www.theasciicode.com.ar/ascii-control-characters/escape-ascii-code-27.html
void kbd(unsigned char key, int x, int y) {
    // escape
    if(key == 'f') {
        bullets.push_back(Rect(brickRed,player.getGunCenter1().x, player.getGunCenter1().y - 10,dimensions(5,10)));
        bullets.push_back(Rect(brickRed,player.getGunCenter2().x, player.getGunCenter2().y - 10,dimensions(5,10)));
    }

    if (key == 27) {
        glutDestroyWindow(wd);
        exit(0);
    }
    
    glutPostRedisplay();
}

void kbdS(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_DOWN:
            if (player.getCenter().y < 670) {
                player.move(0, 20);
            }
            break;
        case GLUT_KEY_LEFT:
            player.move(-30,0);
            break;
        case GLUT_KEY_RIGHT:
            player.move(30,0);
            break;
        case GLUT_KEY_UP:
            if (player.getCenter().y > 550) {
                player.move(0, -20);
            }
            break;
    }
    
    glutPostRedisplay();
}

void cursor(int x, int y) {
    glutPostRedisplay();
}

// button will be GLUT_LEFT_BUTTON or GLUT_RIGHT_BUTTON
// state will be GLUT_UP or GLUT_DOWN
void mouse(int button, int state, int x, int y) {

    glutPostRedisplay();
}

void bulletTimer(int dummy) {

    for (int i = 0; i < bullets.size(); ++i) {
        bullets[i].setCenter(bullets[i].getCenterX(), bullets[i].getCenterY() - 10);
    }

    for(int i = 0; i < bullets.size(); ++i) {
        for (int j = 0; j < baddies.size(); ++j) {
            if (baddies[j].isOverlapping(point2D(bullets[i].getCenterX(), bullets[i].getCenterY() - 5))) {
                bullets.erase(bullets.begin()+ i);
                baddies.erase(baddies.begin() + j);
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(30, bulletTimer, dummy);
}

void baddieTimer(int dummy2) {

    baddies.push_back(Baddie(color(rand()%10/10.0,rand()%10/10.0,rand()%10/10.0),
                             color(rand()%10/10.0,rand()%10/10.0,rand()%10/10.0),
                             point2D(rand() % 1000, 40), rand() % 20 + 10));

    glutPostRedisplay();
    glutTimerFunc(1500, baddieTimer, dummy2);
}

void baddieMoveTimer(int dummy3) {
    for (int i = 0; i < baddies.size(); ++i) {
        if (baddies[i].getBody().getBottomY() < 700) {
            baddies[i].move(0, 3);
        }
        else
            baddies.erase(baddies.begin()+i);
    }
    glutPostRedisplay();
    glutTimerFunc(50, baddieMoveTimer, dummy3);
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    
    init();
    
    glutInit(&argc, argv);          // Initialize GLUT
    
    glutInitDisplayMode(GLUT_RGBA);
    
    glutInitWindowSize((int)width, (int)height);
    glutInitWindowPosition(100, 200); // Position the window's initial top-left corner
    /* create the window and store the handle to it */
    wd = glutCreateWindow("Ship Shootin'" /* title */ );
    
    // Register callback handler for window re-paint event
    glutDisplayFunc(display);
    
    // Our own OpenGL initialization
    initGL();
    
    // register keyboard press event processing function
    // works for numbers, letters, spacebar, etc.
    glutKeyboardFunc(kbd);
    
    // register special event: function keys, arrows, etc.
    glutSpecialFunc(kbdS);
    
    // handles mouse movement
    glutPassiveMotionFunc(cursor);
    
    // handles mouse click
    glutMouseFunc(mouse);
    
    // handles timer
    glutTimerFunc(0, bulletTimer, 0);
    glutTimerFunc(0, baddieTimer, 0);
    glutTimerFunc(0, baddieMoveTimer, 0);



    // Enter the event-processing loop
    glutMainLoop();
    return 0;
}
