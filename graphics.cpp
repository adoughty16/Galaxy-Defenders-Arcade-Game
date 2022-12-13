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
//Colors we will need
const color silver(192/255.0, 192/255.0, 192/255.0);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color black(0, 0, 0);
const color orange(1, 163/255.0, 22/255.0);
//enum so we can track where we are in the menu navigation
enum Status{MENU, PLAY, GETREADY, HIGHSCORES, GAMEOVER, ENTERNAME};

//bools serve to help menu navigation
bool clockStarted = false;
bool newHighScore = false;

//to keep track of time survived
clock_t beginTime, endTime;
double elapsed;

//struct to keep track of high scores
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

//vectors for bullets and ships
vector<Rect> bullets, cavBullets;
vector<Baddie> baddies;
vector<Ship> theCavalry;
//vector to hold high score pairs
vector<scorePair> highScores;

//player ship
Ship player = Ship(silver, orange, point2D(500,600));

//game starts in the main menu
Status gameStatus = MENU;

//initial ints for main game
int cavalryCharges = 1;
int playerLives = 5;
int countdown = 5;
int level = 0;

//strings we will need
string readyMessage = "Get Ready! 5";
string scoreboard = "Remaining Lives: 5";
string levelMessage = "Level: 0";
string backupMessage = "Cavalry Charges: 2";
string scoreMessage;
string highScoreLine;
string playerName = "";

//function loads in high scores to vector of score pairs
void initHighScores() {
    //open file
    ifstream fileIn;
    fileIn.open("../highScores.csv");
    string header;
    //consume header
    if (fileIn) {
        getline(fileIn, header);
    }
    //init variables for data
    double score;
    string name, junk;

    while (fileIn && fileIn.peek() != EOF) {
        //read in data
        getline(fileIn, name, ',');
        fileIn >> score;
        getline(fileIn, junk, ',');
        //add data to vector of scorePairs
        highScores.push_back(scorePair(name,score));
    }
    //close file
    fileIn.close();
}
//function updates high score vector and rewrites .csv file
void updateScores() {
    //loop through vector of score pairs
    for (int i = 0; i < highScores.size(); ++i) {
        //if the time is higher than the item
        if (elapsed > highScores[i].score) {
            //this is where it belongs in the vector
            highScores.insert(highScores.begin() + i, scorePair(playerName,elapsed));
            //delete last item in vector
            highScores.pop_back();
            //exit the loop
            i = 10;
        }
    }
    //delete old csv
    remove("../highScores.csv");
    //new csv with same name
    fstream fileOut;
    fileOut.open("../highScores.csv",ios_base::out);
    //write header
    fileOut << "name,score,";
    //loop through scores
    for (scorePair i: highScores) {
        //write to file with commas
        fileOut << i.name << "," << i.score << ",";
    }
    //close file
    fileOut.close();
}

void init() {
    width = 1000;
    height = 700;
    srand(time(0));
    //load in scores
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

    //if we are in the menu
    if (gameStatus == MENU) {

        //draw the intro, menu options, and rules
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

        glColor3f(0.9, 0.9, 0.9);
        glRasterPos2i(100, 410);
        for (const char &letter: "Rules: ") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, letter);
        }
        glRasterPos2i(100, 430);
        for (const char &letter: "You are the Captain of the Defender Class StarShip HSS PEW-PEW") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 450);
        for (const char &letter: "Your duty is to defend the Space Station from Alien Attacks. We are all counting on you, Captain!") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 470);
        for (const char &letter: "1) Fly the PEW-PEW with the arrow keys") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 490);
        for (const char &letter: "2) Fire the ship's laser cannons with 'F'") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 510);
        for (const char &letter: "3) If you need backup, use 'C' and base will send a squadron to clear the") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 530);
        for (const char &letter: "   opposite side of the map") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 550);
        for (const char &letter: "4) Every time you clear a level the Aliens will move faster and you will replenish") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 570);
        for (const char &letter: "   a squadron attack") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
        glRasterPos2i(100, 590);
        for (const char &letter: "5) Earth has informed us we are on our own out here. Protect us for as long as you can...") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, letter);
        }
    }

    //if we are in the high score menu
    if (gameStatus == HIGHSCORES) {
        glColor3f(1, 1, 1);
        glRasterPos2i(300, 100);
        for (const char &letter: "HIGH SCORES:") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        //display the high scores
        for (int i = 0; i < highScores.size(); ++i) {
            glRasterPos2i(300, 130 + i*30);
            highScoreLine = highScores[i].name + " ...... " + to_string(highScores[i].score);
            for (const char &letter: highScoreLine) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
            }
        }
        //draw navigation options
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

    //If we are in the get ready screen
    if (gameStatus == GETREADY) {
        glColor3f(1, 0, 1);
        glRasterPos2i(200, 100);
        //draw intro
        for (const char &letter: "Captain! A fleet of alien ships has been spotted on the radar!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(300, 150);
        for (const char &letter: "... We need you to defend the system!") {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        //draw countdown
        glRasterPos2i(400, 300);
        for (const char &letter: readyMessage) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
    }

    //if we are playing the game
    if (gameStatus == PLAY) {
        //if the clock isn't running yet
        if(clockStarted == false) {
            //start the clock
            beginTime = clock();
            clockStarted = true;
        }
        //draw player ship
        player.draw();
        //draw player bullets
        for (Rect i: bullets) {
            i.draw();
        }
        //draw cavalry bullets
        for (Rect j: cavBullets) {
            j.draw();
        }
        //draw enemies
        for (Baddie k: baddies) {
            k.draw();
        }
        //draw friendly ships
        for (Ship l: theCavalry) {
            l.draw();
        }
        //Display HUD
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
        //if the player is out of lives
        if (playerLives == 0) {
            //stop the clock
            endTime = clock();
            elapsed = double(endTime - beginTime) * 10 / CLOCKS_PER_SEC;
            //check for high score
            for (scorePair n: highScores) {
                if (elapsed > n.score) {
                    //set high score boolean
                    newHighScore = true;
                    //empty playerName
                    playerName = "";
                }
            }
            //set score message
            scoreMessage = "You survived: " + to_string(elapsed) + " seconds.";
            //end game
            gameStatus = GAMEOVER;
        }
    }
    //If the game has ended
    if (gameStatus == GAMEOVER) {
        glColor3f(1, 0, 1);
        glRasterPos2i(150, 40);
        //display game over message
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
        //if there is not a new high score
        if (!newHighScore) {
            //display regular menu options
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
        //if there is a new high score
        if (newHighScore) {
            //draw enter name box
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
        //display old high scores
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
    //if the player is entering their name
    if (gameStatus == ENTERNAME) {
        glColor3f(0.2, 0.2, 0.2);
        //draw keyboard
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
    //if f is pressed
    if(key == 'f') {
        //add new bullets to vector at the end of the player's guns
        bullets.push_back(Rect(brickRed,player.getGunCenter1().x, player.getGunCenter1().y - 10,dimensions(5,10)));
        bullets.push_back(Rect(brickRed,player.getGunCenter2().x, player.getGunCenter2().y - 10,dimensions(5,10)));
    }
    //if c is pressed and the player has charges
    if(key == 'c' && cavalryCharges > 0) {
        //decide what side the player is on
        if (player.getCenter().x < 500) {
            for (int i = 0; i < 5; ++i) {
                //add squadron to other side
                theCavalry.push_back(Ship(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          point2D(550 + i*100,730)));
            }
        }
        //decide what side the player is on
        if (player.getCenter().x >= 500) {
            for (int i = 0; i < 5; ++i) {
                //add squadron to other side
                theCavalry.push_back(Ship(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                          point2D(50 + i*100,730)));
            }
        }
        //decrement charges
        --cavalryCharges;
        //update HUD
        backupMessage = "Cavalry Charges: " + to_string(cavalryCharges);
    }
    //escape
    if (key == 27) {
        glutDestroyWindow(wd);
        exit(0);
    }
    glutPostRedisplay();
}
void kbdS(int key, int x, int y) {
    //arrow keys move player around in the valid area
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
    //this function is taking the day off
    glutPostRedisplay();
}
// button will be GLUT_LEFT_BUTTON or GLUT_RIGHT_BUTTON
// state will be GLUT_UP or GLUT_DOWN
void mouse(int button, int state, int x, int y) {
    //if the play button is clicked
    if (gameStatus == MENU && 400 < x && x < 570 && 250 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //clear the game data in case this is not the first game of the sesison
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
        //send game to ready screen
        gameStatus = GETREADY;
    }
    //if the high score button is clicked
    if (gameStatus == MENU && 400 < x && x < 570 && 320 < y && y < 370 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //send game to high score screen
        gameStatus = HIGHSCORES;
    }
    //if the play again button is clicked
    if (gameStatus == GAMEOVER && !newHighScore && 200 < x && x < 370 && 230 < y && y < 280 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //reset game data
        clockStarted = false;
        playerLives = 5;
        level = 0;
        countdown = 5;
        string readyMessage = "Get Ready! " + to_string(countdown);
        string scoreboard = "Remaining Lives: " + to_string(playerLives);
        string levelMessage = "Level: " + to_string(level);
        baddies.clear();
        bullets.clear();
        //send game to ready screen
        gameStatus = GETREADY;
    }
    //if menu button is clicked
    if (gameStatus == GAMEOVER && !newHighScore && 200 < x && x < 370 && 300 < y && y < 350 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //send game to menu
        gameStatus = MENU;
    }
    //if menu button is clicked
    if (gameStatus == HIGHSCORES && 300 < x && x < 500 && 430 < y && y < 480 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //send game to menu
        gameStatus = MENU;
    }
    //if enter name button is clicked
    if (gameStatus == GAMEOVER && newHighScore && 200 < x && x < 370 && 430 < y && y < 480 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //send game to enter name screen
        gameStatus = ENTERNAME;
    }
    //The next 26 blocks check for keyboard clicks in the enter name menu, and then add characters to playerName
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
    //if enter is clicked and the name is not empty
    if (gameStatus == ENTERNAME && playerName != "" && 850 < x && x < 940 && 240 < y && y < 300 && button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        //update the high score vector
        updateScores();
        //send game to high score screen
        gameStatus = HIGHSCORES;
    }
    glutPostRedisplay();
}
void bulletTimer(int dummy) {
    //if we are playing
    if (gameStatus == PLAY) {
        //loop through bullets
        for (int i = 0; i < bullets.size(); ++i) {
            //if they are on the screen
            if (bullets[i].getCenterY() > 0) {
                //move them up
                bullets[i].setCenter(bullets[i].getCenterX(), bullets[i].getCenterY() - 10);
            }
            //otherwise delete them
            else {
                bullets.erase(bullets.begin() + i);
                //ensure we dont skip a bullet by decrementing i
                --i;
            }
        }
        //for every bullet
        for (int i = 0; i < bullets.size(); ++i) {
            //for every bad guy
            for (int j = 0; j < baddies.size(); ++j) {
                //check if the bullet hits the bad guy
                if (baddies[j].isOverlapping(point2D(bullets[i].getCenterX(), bullets[i].getCenterY() - 5))) {
                    //delete the bullet and the bad guy
                    bullets.erase(bullets.begin() + i);
                    baddies.erase(baddies.begin() + j);
                }
            }
        }
        //for every cavalry bullet
        for (int i = 0; i < cavBullets.size(); ++i) {
            //check if it is on the screen
            if (cavBullets[i].getCenterY() > 0) {
                //modulus operator sends some bullets diagonally to the left, and some bullets diagonally to the right
                if (int(cavBullets[i].getCenterX()) % 2 == 0 ) {
                    cavBullets[i].setCenter(cavBullets[i].getCenterX() -2, cavBullets[i].getCenterY() - 14);
                }
                else {
                    cavBullets[i].setCenter(cavBullets[i].getCenterX() +2, cavBullets[i].getCenterY() - 14);
                }
            }
            //if it isn't on the screen, delete it.
            else {
                cavBullets.erase(cavBullets.begin() + i);
                --i;
            }
        }
        //for all the cavalry bullets
        for (int i = 0; i < cavBullets.size(); ++i) {
            //for all the bad guys
            for (int j = 0; j < baddies.size(); ++j) {
                //check if the bullet is hitting the bad guy
                if (baddies[j].isOverlapping(point2D(cavBullets[i].getCenterX(), cavBullets[i].getCenterY() - 5))) {
                    //delete the bullet and the bad guy
                    cavBullets.erase(cavBullets.begin() + i);
                    baddies.erase(baddies.begin() + j);
                }
            }
        }
        //for all the friendly ships
        for (int i = 0; i < theCavalry.size(); ++i) {
            //if they are on the screen move them up
            if (theCavalry[i].getCenter().y > -30) {
                theCavalry[i].move(0, -8);
            }
            //otherwise delete them
            else
                theCavalry.erase(theCavalry.begin() + i);
            //for all the bad guys
            for (int j = 0; j < baddies.size(); ++j) {
                //check if the friendly ship is hitting the bad guy
                if (baddies[j].isOverlapping(point2D(theCavalry[i].getCenter().x, theCavalry[i].getCenter().y - 5))) {
                    //if it is, delete them both
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
    //if we are playing
    if (gameStatus == PLAY) {
        //generate a new bad guy
        baddies.push_back(Baddie(color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                 color(rand() % 10 / 10.0, rand() % 10 / 10.0, rand() % 10 / 10.0),
                                 point2D(rand() % 940 + 30, 40), rand() % 15 + 15));
    }
    glutPostRedisplay();
    //repeat every 1.5 seconds
    glutTimerFunc(1500, baddieTimer, dummy2);
}
void baddieMoveTimer(int dummy3) {
    //if we are playing
    if (gameStatus == PLAY) {
        //for every bad guy
        for (int i = 0; i < baddies.size(); ++i) {
            //if they are on the screen, move them.
            if (baddies[i].getBody().getBottomY() < 700) {
                baddies[i].move(0, 2 + level);
            }
            //otherwise, delete them and decrement lives, because they have reached the bottom
            else {
                baddies.erase(baddies.begin() + i);
                --playerLives;
                scoreboard = "Remaining Lives: " + to_string(playerLives);
            }
            //if they slam into the player ship
            if (baddies[i].isOverlapping(point2D(player.getCenter().x, player.getCenter().y - 25)) ||
                baddies[i].isOverlapping(point2D(player.getGunCenter1().x, player.getGunCenter1().y - 7)) ||
                baddies[i].isOverlapping(point2D(player.getGunCenter2().x, player.getGunCenter2().y - 7))) {
                //delete the ship and decrement lives
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
    //if we are playing
    if (gameStatus == PLAY) {
        //up the difficulty
        ++level;
        //give the player another cavalry charge
        ++cavalryCharges;
        backupMessage = "Cavalry Charges: " + to_string(cavalryCharges);
        levelMessage = "Level: " + to_string(level);
    }
    glutPostRedisplay();
    //repeat every 20 seconds
    glutTimerFunc(20000, levelTimer, dummy4);
}
void readyTimer(int dummy5) {
    //if we are at the ready screen
    if (gameStatus == GETREADY && countdown > 0) {
        //update the countdown
        --countdown;
        readyMessage = "Get Ready! " + to_string(countdown);
    }
    //if the countdown is at 0
    else if (gameStatus == GETREADY && countdown == 0) {
        //send game to play screen
        gameStatus = PLAY;
        //start the level timer
        glutTimerFunc(0,levelTimer,0);
    }
    //otherwise reset the countdown
    else
        countdown = 5;
    glutPostRedisplay();
    //repeat every second
    glutTimerFunc(1000, readyTimer, dummy5);
}
void cavBulletTimer(int dummy6) {
    //every 0.4 seconds, have every friendly ship shoot their blasters
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
    
    // handles timers
    glutTimerFunc(0, bulletTimer, 0);
    glutTimerFunc(0, baddieTimer, 0);
    glutTimerFunc(0, baddieMoveTimer, 0);
    glutTimerFunc(0,readyTimer, 0);
    glutTimerFunc(0,cavBulletTimer,0);


    // Enter the event-processing loop
    glutMainLoop();
    return 0;
}