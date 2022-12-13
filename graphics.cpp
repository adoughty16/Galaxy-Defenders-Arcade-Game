#include "graphics.h"
#include "circle.h"
#include "rect.h"
#include "ship.h"
#include "baddie.h"
#include "baddie.cpp"
#include <ctime>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
using namespace std;

GLdouble width, height;
int wd;
const color silver(192/255.0, 192/255.0, 192/255.0);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color black(0, 0, 0);
const color orange(1, 163/255.0, 22/255.0);

enum Status{MENU, PLAY, GETREADY, HIGHSCORES, GAMEOVER, ENTERNAME};

bool clockStarted = false;
bool newHighScore = false;
clock_t beginTime, endTime;
double elapsed;

struct scorePair {
    string name;
    double score;

    /*constructors*/
    scorePair() {
        name = "";
        score = 0;
    }
    scorePair(string name, double score) {
        this->name = name;
        this->score = score;
    }
};

Status gameStatus = MENU;
vector<Rect> bullets, cavBullets;
vector<Baddie> baddies;
vector<Ship> theCavalry;
vector<scorePair> highScores;
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
string highScoreLine;
string playerName = "";

void initHighScores() {
    ifstream fileIn;
    fileIn.open("../highScores.csv");
    string header;

    if (fileIn) {
        getline(fileIn, header);
    }

    double score;
    string name, junk;

    while (fileIn && fileIn.peek() != EOF) {
        getline(fileIn, name, ',');
        fileIn >> score;
        getline(fileIn, junk, ',');
        highScores.push_back(scorePair(name,score));
    }
    fileIn.close();
}

void updateScores() {
    for (int i = 0; i < highScores.size(); ++i) {
        if (elapsed > highScores[i].score) {
            highScores.insert(highScores.begin() + i, scorePair(playerName,elapsed));
            highScores.pop_back();
            i = 10;
        }
    }

    remove("../highScores.csv");
    fstream fileOut;
    fileOut.open("../highScores.csv",ios_base::out);

    fileOut << "name,score,";
    for (scorePair i: highScores) {
        fileOut << i.name << "," << i.score << ",";
    }
    fileOut.close();


}

void init() {
    width = 1000;
    height = 700;
    srand(time(0));
    initHighScores();
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

    if (gameStatus == HIGHSCORES) {
        glColor3f(1, 1, 1);
        glRasterPos2i(300, 100);
        for (const char &letter: "HIGH SCORES:") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        for (int i = 0; i < highScores.size(); ++i) {
            glRasterPos2i(300, 130 + i*30);
            highScoreLine = highScores[i].name + " ...... " + to_string(highScores[i].score);
            for (const char &letter: highScoreLine) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
        }
        glColor3f(0, 0, 1);
        glBegin(GL_QUADS);
        glVertex2i(300, 430);
        glVertex2i(500, 430);
        glVertex2i(500, 480);
        glVertex2i(300, 480);
        glEnd();
        glColor3f(0, 0, 0);
        glRasterPos2i(305, 465);
        for (const char &letter: "BACK TO MENU") {
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
            for (scorePair n: highScores) {
                if (elapsed > n.score) {
                    newHighScore = true;
                    playerName = "";
                }
            }
            scoreMessage = "You survived: " + to_string(elapsed) + " seconds.";
            gameStatus = GAMEOVER;
        }
    }

    if (gameStatus == GAMEOVER) {

        glColor3f(1, 0, 1);
        glRasterPos2i(150, 40);
        for (const char &letter: "OH NO! THE ALIENS HAVE BREACHED THE MAIN BASE") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(280, 70);
        for (const char &letter: "DAMAGE ASSESSMENT: CATASTROPHIC!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(400, 100);
        for (const char &letter: "GAME OVER...") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(350, 130);
        for (const char &letter: scoreMessage) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }

        if (!newHighScore) {

            glColor3f(0.5, 1, 0.5);
            glBegin(GL_QUADS);
            glVertex2i(200, 230);
            glVertex2i(370, 230);
            glVertex2i(370, 280);
            glVertex2i(200, 280);
            glEnd();

            glColor3f(1, 0.5, 1);
            glBegin(GL_QUADS);
            glVertex2i(200, 300);
            glVertex2i(370, 300);
            glVertex2i(370, 350);
            glVertex2i(200, 350);
            glEnd();

            glColor3f(0, 0, 0);
            glRasterPos2i(205, 265);
            for (const char &letter: "PLAY AGAIN") {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
            glColor3f(0, 0, 0);
            glRasterPos2i(205, 335);
            for (const char &letter: "MAIN MENU") {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
        }

        if (newHighScore) {
            glColor3f(0.5, 1, 0.5);
            glBegin(GL_QUADS);
            glVertex2i(200, 430);
            glVertex2i(370, 430);
            glVertex2i(370, 480);
            glVertex2i(200, 480);
            glEnd();

            glColor3f(0, 0, 0);
            glRasterPos2i(205, 465);
            for (const char &letter: "ENTER NAME") {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
        }

        glColor3f(1, 1, 1);
        glRasterPos2i(600, 200);
        for (const char &letter: "HIGH SCORES:") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        for (int i = 0; i < highScores.size(); ++i) {
            glRasterPos2i(600, 230 + i*30);
            highScoreLine = highScores[i].name + " ...... " + to_string(highScores[i].score);
            for (const char &letter: highScoreLine) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
        }
    }

    if (gameStatus == ENTERNAME) {
        glColor3f(0.2, 0.2, 0.2);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 9; ++j) {
                glBegin(GL_QUADS);
                glVertex2i(50 + (j*100), 100 + (i*70));
                glVertex2i(50 + (j*100), 160 + (i*70));
                glVertex2i(140 + (j*100), 160 + (i*70));
                glVertex2i(140 + (j*100), 100 + (i*70));
                glEnd();
            }
        }
        glColor3f(1, 1, 1);
        glRasterPos2i(450, 500);
        for (const char &letter: playerName) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }

        glColor3f(1, 1, 1);
        glRasterPos2i(85, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
        glRasterPos2i(185, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
        glRasterPos2i(285, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'C');
        glRasterPos2i(385, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'D');
        glRasterPos2i(485, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'E');
        glRasterPos2i(585, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'F');
        glRasterPos2i(685, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'G');
        glRasterPos2i(785, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'H');
        glRasterPos2i(885, 140);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'I');
        glRasterPos2i(85, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'J');
        glRasterPos2i(185, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'K');
        glRasterPos2i(285, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'L');
        glRasterPos2i(385, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'M');
        glRasterPos2i(485, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N');
        glRasterPos2i(585, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'O');
        glRasterPos2i(685, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
        glRasterPos2i(785, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Q');
        glRasterPos2i(885, 210);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'R');
        glRasterPos2i(85, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'S');
        glRasterPos2i(185, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'T');
        glRasterPos2i(285, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'U');
        glRasterPos2i(385, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'V');
        glRasterPos2i(485, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'W');
        glRasterPos2i(585, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'X');
        glRasterPos2i(685, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Y');
        glRasterPos2i(785, 280);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Y');
        glRasterPos2i(855, 280);
        for (const char &letter: "ENTER") {
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
        newHighScore = false;
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
    if (gameStatus == GAMEOVER && !newHighScore && 200 < x && x < 370 && 230 < y && y < 280 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
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
    if (gameStatus == GAMEOVER && !newHighScore && 200 < x && x < 370 && 300 < y && y < 350 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        gameStatus = MENU;
    }
    if (gameStatus == HIGHSCORES && 300 < x && x < 500 && 430 < y && y < 480 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        gameStatus = MENU;
    }
    if (gameStatus == GAMEOVER && newHighScore && 200 < x && x < 370 && 430 < y && y < 480 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        gameStatus = ENTERNAME;
    }

    if (gameStatus == ENTERNAME && 50 < x && x < 140 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'A';
    }
    if (gameStatus == ENTERNAME && 150 < x && x < 240 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'B';
    }
    if (gameStatus == ENTERNAME && 250 < x && x < 340 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'C';
    }
    if (gameStatus == ENTERNAME && 350 < x && x < 440 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'D';
    }
    if (gameStatus == ENTERNAME && 450 < x && x < 540 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'E';
    }
    if (gameStatus == ENTERNAME && 550 < x && x < 640 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'F';
    }
    if (gameStatus == ENTERNAME && 650 < x && x < 740 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'G';
    }
    if (gameStatus == ENTERNAME && 750 < x && x < 840 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'H';
    }
    if (gameStatus == ENTERNAME && 850 < x && x < 940 && 100 < y && y < 160 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'I';
    }
    if (gameStatus == ENTERNAME && 50 < x && x < 140 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'J';
    }
    if (gameStatus == ENTERNAME && 150 < x && x < 240 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'K';
    }
    if (gameStatus == ENTERNAME && 250 < x && x < 340 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'L';
    }
    if (gameStatus == ENTERNAME && 350 < x && x < 440 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'M';
    }
    if (gameStatus == ENTERNAME && 450 < x && x < 540 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'N';
    }
    if (gameStatus == ENTERNAME && 550 < x && x < 640 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'O';
    }
    if (gameStatus == ENTERNAME && 650 < x && x < 740 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'P';
    }
    if (gameStatus == ENTERNAME && 750 < x && x < 840 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'Q';
    }
    if (gameStatus == ENTERNAME && 850 < x && x < 940 && 170 < y && y < 230 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'R';
    }
    if (gameStatus == ENTERNAME && 50 < x && x < 140 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'S';
    }
    if (gameStatus == ENTERNAME && 150 < x && x < 240 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'T';
    }
    if (gameStatus == ENTERNAME && 250 < x && x < 340 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'U';
    }
    if (gameStatus == ENTERNAME && 350 < x && x < 440 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'V';
    }
    if (gameStatus == ENTERNAME && 450 < x && x < 540 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'W';
    }
    if (gameStatus == ENTERNAME && 550 < x && x < 640 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'X';
    }
    if (gameStatus == ENTERNAME && 650 < x && x < 740 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'Y';
    }
    if (gameStatus == ENTERNAME && 750 < x && x < 840 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        playerName += 'Z';
    }
    if (gameStatus == ENTERNAME && playerName != "" && 850 < x && x < 940 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        updateScores();
        gameStatus = HIGHSCORES;
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
