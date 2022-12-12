#include "graphics.h"
#include "circle.h"
#include "rect.h"
#include "ship.h"
#include "baddie.h"
#include "baddie.cpp"
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

GLdouble width, height;
int wd;
const color silver(192/255.0, 192/255.0, 192/255.0);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color black(0, 0, 0);
const color orange(1, 163/255.0, 22/255.0);

enum Status{MENU, PLAY, GETREADY, HIGHSCORES, GAMEOVER};

bool clockStarted = false;
clock_t beginTime, endTime;
double elapsed;


Status gameStatus = MENU;
vector<Rect> bullets, cavBullets;
vector<Baddie> baddies;
vector<Ship> theCavalry;
Ship player = Ship(silver, orange, point2D(500,600));

int cavalryCharges = 1;
int playerLives = 5;
int countdown = 5;
int level = 0;

string readyMessage = "Get Ready! 5";
string scoreboard = "Remaining Lives: 5";
string levelMessage = "Level: 0";
string backupMessage = "Cavalry Charges: 2";
string scoreMessage;

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
    if (gameStatus == MENU) {
        glColor3f(1, 0, 1);
        glRasterPos2i(400, 230);
        for (const char &letter: "Ship Shootin'!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glColor3f(1, 1, 0);
        glBegin(GL_QUADS);
        glVertex2i(400, 250);
        glVertex2i(570, 250);
        glVertex2i(570, 300);
        glVertex2i(400, 300);
        glEnd();

        glColor3f(0, 1, 1);
        glBegin(GL_QUADS);
        glVertex2i(400, 320);
        glVertex2i(570, 320);
        glVertex2i(570, 370);
        glVertex2i(400, 370);
        glEnd();

        glColor3f(0, 0, 0);
        glRasterPos2i(450, 285);
        for (const char &letter: "PLAY") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }

        glColor3f(0, 0, 0);
        glRasterPos2i(400, 355);
        for (const char &letter: "HIGH SCORES") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }


    }
    if (gameStatus == GETREADY) {
        glColor3f(1, 0, 1);
        glRasterPos2i(200, 100);
        for (const char &letter: "Captain! A fleet of alien ships has been spotted on the radar!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(300, 150);
        for (const char &letter: "... We need you to defend the system!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(400, 300);
        for (const char &letter: readyMessage) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
    }

    if (gameStatus == PLAY) {
        if(clockStarted == false) {
            beginTime = clock();
            clockStarted = true;
        }

        player.draw();

        for (Rect i: bullets) {
            i.draw();
        }
        for (Rect j: cavBullets) {
            j.draw();
        }
        for (Baddie k: baddies) {
            k.draw();
        }
        for (Ship l: theCavalry) {
            l.draw();
        }
        glColor3f(1, 1, 1);
        glRasterPos2i(800, 60);
        for (const char &letter: scoreboard) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, letter);
        }
        glRasterPos2i(800, 70);
        for (const char &letter: backupMessage) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, letter);
        }
        glRasterPos2i(800, 80);
        for (const char &letter: levelMessage) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, letter);
        }

        if (playerLives == 0) {
            endTime = clock();
            elapsed = double(endTime - beginTime) * 10 / CLOCKS_PER_SEC;
            scoreMessage = "You survived: " + to_string(elapsed) + " seconds.";
            gameStatus = GAMEOVER;
        }
    }

    if (gameStatus == GAMEOVER) {

        glColor3f(1, 0, 1);
        glRasterPos2i(150, 230);
        for (const char &letter: "OH NO! THE ALIENS HAVE BREACHED THE MAIN BASE") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(280, 260);
        for (const char &letter: "DAMAGE ASSESSMENT: CATASTROPHIC!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(400, 290);
        for (const char &letter: "GAME OVER...") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(350, 320);
        for (const char &letter: scoreMessage) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }

        glColor3f(0.5, 1, 0.5);
        glBegin(GL_QUADS);
        glVertex2i(400, 370);
        glVertex2i(570, 370);
        glVertex2i(570, 420);
        glVertex2i(400, 420);
        glEnd();

        glColor3f(1, 0.5, 1);
        glBegin(GL_QUADS);
        glVertex2i(400, 440);
        glVertex2i(570, 440);
        glVertex2i(570, 490);
        glVertex2i(400, 490);
        glEnd();

        glColor3f(0, 0, 0);
        glRasterPos2i(405, 400);
        for (const char &letter: "PLAY AGAIN") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glColor3f(0, 0, 0);
        glRasterPos2i(405, 470);
        for (const char &letter: "MAIN MENU") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }

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

    if(key == 'c' && cavalryCharges > 0) {
        if (player.getCenter().x < 500) {
            for (int i = 0; i < 5; ++i) {
                theCavalry.push_back(Ship(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          point2D(550 + i*100,730)));
            }
        }
        if (player.getCenter().x >= 500) {
            for (int i = 0; i < 5; ++i) {
                theCavalry.push_back(Ship(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          point2D(50 + i*100,730)));
            }
        }
        --cavalryCharges;
        backupMessage = "Cavalry Charges: " + to_string(cavalryCharges);
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
            if (player.getGunCenter2().x > 0) {
                player.move(-30, 0);
            }
            break;
        case GLUT_KEY_RIGHT:
            if (player.getGunCenter1().x < 1000) {
                player.move(30, 0);
            }
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

    if (gameStatus == MENU && 400 < x && x < 570 && 250 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        clockStarted = false;
        playerLives = 5;
        level = 0;
        countdown = 5;
        string readyMessage = "Get Ready! " + to_string(countdown);
        string scoreboard = "Remaining Lives: " + to_string(playerLives);
        string levelMessage = "Level: " + to_string(level);
        baddies.clear();
        bullets.clear();
        gameStatus = GETREADY;
    }
    if (gameStatus == MENU && 400 < x && x < 570 && 320 < y && y < 370 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        gameStatus = HIGHSCORES;
    }
    if (gameStatus == GAMEOVER && 400 < x && x < 570 && 370 < y && y < 420 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        clockStarted = false;
        playerLives = 5;
        level = 0;
        countdown = 5;
        string readyMessage = "Get Ready! " + to_string(countdown);
        string scoreboard = "Remaining Lives: " + to_string(playerLives);
        string levelMessage = "Level: " + to_string(level);

        baddies.clear();
        bullets.clear();
        gameStatus = GETREADY;
    }
    if (gameStatus == GAMEOVER && 400 < x && x < 570 && 440 < y && y < 490 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        gameStatus = MENU;
    }

    glutPostRedisplay();
}

void bulletTimer(int dummy) {
    if (gameStatus == PLAY) {

        for (int i = 0; i < bullets.size(); ++i) {
            if (bullets[i].getCenterY() > 0) {
                bullets[i].setCenter(bullets[i].getCenterX(), bullets[i].getCenterY() - 10);
            }
            else {
                bullets.erase(bullets.begin() + i);
                --i;
            }
        }

        for (int i = 0; i < bullets.size(); ++i) {
            for (int j = 0; j < baddies.size(); ++j) {
                if (baddies[j].isOverlapping(point2D(bullets[i].getCenterX(), bullets[i].getCenterY() - 5))) {
                    bullets.erase(bullets.begin() + i);
                    baddies.erase(baddies.begin() + j);
                }
            }
        }
        for (int i = 0; i < cavBullets.size(); ++i) {
            if (cavBullets[i].getCenterY() > 0) {
                if (int(cavBullets[i].getCenterX()) % 2 == 0 ) {
                    cavBullets[i].setCenter(cavBullets[i].getCenterX() -2, cavBullets[i].getCenterY() - 14);
                }
                else {
                    cavBullets[i].setCenter(cavBullets[i].getCenterX() +2, cavBullets[i].getCenterY() - 14);
                }
            } else {
                cavBullets.erase(cavBullets.begin() + i);
                --i;
            }

        }

        for (int i = 0; i < cavBullets.size(); ++i) {

            for (int j = 0; j < baddies.size(); ++j) {
                if (baddies[j].isOverlapping(point2D(cavBullets[i].getCenterX(), cavBullets[i].getCenterY() - 5))) {
                    cavBullets.erase(cavBullets.begin() + i);
                    baddies.erase(baddies.begin() + j);
                }
            }
        }
        for (int i = 0; i < theCavalry.size(); ++i) {
            if (theCavalry[i].getCenter().y > -30) {
                theCavalry[i].move(0, -8);
            } else
                theCavalry.erase(theCavalry.begin() + i);
            for (int j = 0; j < baddies.size(); ++j) {
                if (baddies[j].isOverlapping(point2D(theCavalry[i].getCenter().x, theCavalry[i].getCenter().y - 5))) {
                    theCavalry.erase(theCavalry.begin() + i);
                    baddies.erase(baddies.begin() + j);
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(30, bulletTimer, dummy);
}

void baddieTimer(int dummy2) {

    if (gameStatus == PLAY) {
        baddies.push_back(Baddie(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                 color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                 point2D(rand() % 940 + 30, 40), rand() % 15 + 15));
    }

    glutPostRedisplay();
    glutTimerFunc(1500, baddieTimer, dummy2);
}

void baddieMoveTimer(int dummy3) {
    if (gameStatus == PLAY) {
        for (int i = 0; i < baddies.size(); ++i) {
            if (baddies[i].getBody().getBottomY() < 700) {
                baddies[i].move(0, 2 + level);
            } else {
                baddies.erase(baddies.begin() + i);
                --playerLives;
                scoreboard = "Remaining Lives: " + to_string(playerLives);
            }
            if (baddies[i].isOverlapping(point2D(player.getCenter().x, player.getCenter().y - 25)) ||
                baddies[i].isOverlapping(point2D(player.getGunCenter1().x, player.getGunCenter1().y - 7)) ||
                baddies[i].isOverlapping(point2D(player.getGunCenter2().x, player.getGunCenter2().y - 7))) {
                baddies.erase(baddies.begin() + i);
                --playerLives;
                scoreboard = "Remaining Lives: " + to_string(playerLives);
            }

        }
    }
    glutPostRedisplay();
    glutTimerFunc(50, baddieMoveTimer, dummy3);
}

void levelTimer(int dummy4) {
    if (gameStatus == PLAY) {
        ++level;
        ++cavalryCharges;
        backupMessage = "Cavalry Charges: " + to_string(cavalryCharges);
        levelMessage = "Level: " + to_string(level);
    }
    glutPostRedisplay();
    glutTimerFunc(20000, levelTimer, dummy4);
}

void readyTimer(int dummy5) {

    if (gameStatus == GETREADY && countdown > 0) {
        --countdown;
        readyMessage = "Get Ready! " + to_string(countdown);
    }
    else if (gameStatus == GETREADY && countdown == 0) {
        gameStatus = PLAY;
        glutTimerFunc(0,levelTimer,0);
    }
    else
        countdown = 5;


    glutPostRedisplay();
    glutTimerFunc(1000, readyTimer, dummy5);
}

void cavBulletTimer(int dummy6) {

    for (int i = 0; i < theCavalry.size(); ++i) {
        cavBullets.push_back(Rect(brickRed,theCavalry[i].getGunCenter1().x - 1, theCavalry[i].getGunCenter1().y - 10,dimensions(5,10)));
        cavBullets.push_back(Rect(brickRed,theCavalry[i].getGunCenter2().x, theCavalry[i].getGunCenter2().y - 10,dimensions(5,10)));
    }
    glutPostRedisplay();
    glutTimerFunc(400, cavBulletTimer, dummy6);
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
    glutTimerFunc(0,readyTimer, 0);
    glutTimerFunc(0,cavBulletTimer,0);


    // Enter the event-processing loop
    glutMainLoop();
    return 0;
}
