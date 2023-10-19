/*****************************************************************
 *        -- BROWNIAN MOVEMENT SIMULATOR --
 *        v 1.0 
 *        By: Giux  
 *          
 *        Description:
 *           This program generates a desktop app that shows a 
 *           simulation for Brownian Movement of a bunch of balls. 
 *           The expectation is to get a Gaussian Bell at the end.
 * 
 *        Requirements:
 *           To have 
 * 
 *        Build Command:
 *           Since this code uses OpenGL library they need to be     
 *           called in the build command in the same order as in the
 *           include section. Use the next command:
 *           g++ ./canica.cpp -o out/canica.out -lGL -lGLU -lglut
 * 
 *        Notes: Find demos/examples for using GL library at:
 *           https://cs.lmu.edu/~ray/notes/openglexamples/ 
 * 
 ********************************************************************/

/***** INCLUDES Section *****/
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstring>
#include <cmath>
#include <math.h>
#include <cstdlib> //for random()

/******** Configuration Macros ********/
#define APP_MODE_RESTART_ENABLE   1
#define LOGS_ENABLED              1
#define SIZE_OF_ARRAY             300
#define SIMULATION_VELOCITY       (20)   //int {1-100..}  1=max 100..=min
#define window_width              3000
#define window_height             3000
#define Y_STEP                    (0.008)
#define X_STEP                    (0.007)          
#define Y_COORDENATE_INIT         (1-0*Y_STEP)
#define X_COORDENATE_INIT         (0.5)
#define TOTAL_Y_POSITIONS         ((int)(1/Y_STEP))
#define TOTAL_X_POSITIONS         ((int)(1/X_STEP))    //TOTAL_Y_POSITIONS //30
#define INITIAL_X_POSITION        ((int)(TOTAL_X_POSITIONS/2))   // initial position of X coordenate in the Bars vector
#define READY_FOR_NEXT_TRH        (0.95) //threshold of Y coordenates to launche next ball 
#define BIT_MAP_SIZE              16
#define BIT_MAP_IMAGE             canica16_bitmap //canica_bitmap  canica16_bitmap
#define SCREEN_BROWNIAN_RANGE     (0.448+2*Y_STEP)
/***************************************/

enum {
    RGB_R = 0,
    RGB_G,
    RGB_B
};

enum {
    COORDENATE_X = 0,
    COORDENATE_Y
};

GLubyte canica16_bitmap[]={
    0b00000000,   0b00000000,
    0b00000111,   0b11100000,
    0b00011111,   0b11111000,
    0b00011111,   0b11111000,
    0b00111111,   0b11111100,
    0b00111111,   0b11111100,
    0b00111111,   0b11111100,
    0b01111111,   0b11111110,
    0b01111111,   0b11111110,
    0b01111111,   0b11111110,
    0b01111111,   0b11111110,
    0b00111111,   0b11111100,
    0b00111111,   0b11111100,
    0b00111111,   0b11111100,
    0b00011111,   0b11111000,
    0b00011111,   0b11111000,
    0b00000111,   0b11100000,
    0b00000000,   0b00000000,  
};

// Return a random float in the range 0.0 to 1.0.
GLfloat randomFloat() {
  return (GLfloat)rand() / RAND_MAX;
}


/***** Main Canica Class *****/
class canica{
    private:
        float color[3];
        double x_coor;
        double y_coor;
        bool isDone;
        bool readyForNext;

    public:
        int matrixCoor[2];
        static int totalCanicas;
        int index;    

        canica(){
            color[RGB_R] = randomFloat();
            color[RGB_G] = randomFloat();
            color[RGB_B] = randomFloat();
            x_coor = X_COORDENATE_INIT;
            y_coor = Y_COORDENATE_INIT;
            matrixCoor[COORDENATE_X] = INITIAL_X_POSITION;
            matrixCoor[COORDENATE_Y] = TOTAL_Y_POSITIONS;
            isDone = false;
            readyForNext = false;
            index = totalCanicas++;
            //printf("Canica %i index=%i \n",test_Index++,index);
        }

        void restart(){
            x_coor = X_COORDENATE_INIT;
            y_coor = Y_COORDENATE_INIT;
            matrixCoor[COORDENATE_X] = INITIAL_X_POSITION;
            matrixCoor[COORDENATE_Y] = TOTAL_Y_POSITIONS;
            isDone = false;
            readyForNext = false;
        }

        double getDisplayPositionX(){
            return x_coor;
        }

        double getDisplayPositionY(){
            return y_coor;
        }

        void setDisplayPositionX(double c){
            if (c<255) x_coor=c;
        }

        void setDisplayPositionY(double c){
            if (c<255) y_coor=c;
        }

        float getRGB(int c){
            float ret;
            switch(c){
                case RGB_R: ret = color[RGB_R]; break;
                case RGB_G: ret = color[RGB_G]; break;
                case RGB_B: ret = color[RGB_B]; break;
                default: break;
            }
            return ret;
        }

        bool getIsDone () {
            return isDone;
        }

        void setIsDone (bool c){
            isDone = (c==false) ? false : true;
        }

        bool getIsReadyForNext () {
            return readyForNext;
        }

        void setIsReadyForNext (bool c){
            readyForNext = (c==false) ? false : true;
        }
};

int canica::totalCanicas = 0; // init static variable of class canica
canica canicas[SIZE_OF_ARRAY];

int canicaVector[TOTAL_X_POSITIONS]={0};

/***** STANDARD Namespace *****/
using namespace std;


/***** LOCAL FUNCTIONS *****/

void appRestart(canica * ptr, int size){
    /* restart every Canica */
    for (int i=0; i<SIZE_OF_ARRAY; i++){
        canicas[i].restart();
    }                
    /* restart Canicas Vector */
    for (int i=0; i<TOTAL_X_POSITIONS; i++){
        canicaVector[i] = 0;
    }        
}


/* drawCanica: Print a bitmap in the screen */
void drawCanica(canica * canicaPtr){
    glColor3f(canicaPtr->getRGB(RGB_R), canicaPtr->getRGB(RGB_G), canicaPtr->getRGB(RGB_B));    
    glRasterPos3f(canicaPtr->getDisplayPositionX(), canicaPtr->getDisplayPositionY(), 0.0); //(X, Y, ?, ? )
    glBitmap(BIT_MAP_SIZE, BIT_MAP_SIZE, 0, 0, 0, 0, BIT_MAP_IMAGE);   
}

/* moveCanica: Update X and Y coordenates of a canica Object */
void moveCanica(canica * canicaPtr){
    double dispPosition_y = canicaPtr->getDisplayPositionY() - Y_STEP;
    double dispPosition_x = canicaPtr->getDisplayPositionX();  
 
    /* This conditions triggers the start of the next canica by setting "ReadyForNext" flag */
    if (dispPosition_y<READY_FOR_NEXT_TRH){
        canicaPtr->setIsReadyForNext(true);
    }

    canicaPtr->matrixCoor[COORDENATE_Y]--;

    /* Check if the canica can continue moving ... */
    if ((canicaPtr->matrixCoor[COORDENATE_Y]>(canicaVector[canicaPtr->matrixCoor[COORDENATE_X]]) ) ) {                  
        
        /* If ball is in brownian range, make the x movement */
        if (dispPosition_y>SCREEN_BROWNIAN_RANGE){        
            //cout<<canicaPtr->matrixCoor[COORDENATE_X]<<endl;
            if (rand()%2) {
                dispPosition_x += X_STEP;      
                canicaPtr->matrixCoor[COORDENATE_X] += 1;
            } 
            else {
                dispPosition_x -= X_STEP;
                canicaPtr->matrixCoor[COORDENATE_X] -= 1;
            }
        
        } else {
            /* Actions to take when ball finished brownian movement 
             * Nothing to do. Just keep going down in the same X position. */
        }

        /* Update canica object with the new x and y data */
        canicaPtr->setDisplayPositionX(dispPosition_x);
        canicaPtr->setDisplayPositionY(dispPosition_y);
    }
    else  { // here the Balls reach the bottom.
        /* Next code should be executed only the first time */
        if (canicaPtr->getIsDone()==false)
        {
            canicaPtr->setIsDone(true);    
            canicaPtr->setDisplayPositionY(dispPosition_y);
            /* Increase vector in the X position */
            canicaVector[canicaPtr->matrixCoor[COORDENATE_X]]++;
            if (LOGS_ENABLED) cout<<"Canica "<<canicaPtr->index<<" llega al fondo en casilla (X): "<<canicaPtr->matrixCoor[COORDENATE_X]<<endl; 

            if (APP_MODE_RESTART_ENABLE!=0){
                if ((canicas[SIZE_OF_ARRAY-1].getIsDone()==true)){ // if last canica is Done then restart app()
                    appRestart(&canicas[0], SIZE_OF_ARRAY);
                }
            }      
        }      
        else{
                  
        }
    }  
}

void timer(int v) {      
  for (int i=0; i<SIZE_OF_ARRAY; i++){
    if ((i==0) || ((i>0) && (canicas[i-1].getIsReadyForNext()==true))){
        moveCanica(&canicas[i]);    
    }
  }  

  glutPostRedisplay();
  glutTimerFunc(SIMULATION_VELOCITY, timer, v);
}

// On reshape, uses an orthographic projection with world coordinates ranging
// from 0 to 1 in the x and y directions, and -1 to 1 in z.
void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_MODELVIEW);  //original: GL_PROJECTION
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
}

void draw_nails(){        
    float x = 0 + X_STEP/2 + 0.0005;
    float y = SCREEN_BROWNIAN_RANGE-Y_STEP/2;

    glColor3f(1.0, 1.0, 1.0); /* Blanco */   

    glBegin(GL_POINTS);
        for (int i=0; i<TOTAL_X_POSITIONS; i++){            
            for (int j=0; j<TOTAL_Y_POSITIONS; j++){
                y += Y_STEP*2;
                glVertex2f(x, y);
            }            
            x += X_STEP*2;
            y = SCREEN_BROWNIAN_RANGE-Y_STEP/2;
        }        
    glEnd();
}



void draw_bars(){        
    float x = 0 + X_STEP/2 + 0.001;
    float y = SCREEN_BROWNIAN_RANGE-Y_STEP/2;

    glColor3f(0.4, 0.0, 1.0); /* Magenta */   

    glBegin(GL_LINES);
        for (int i=0; i<TOTAL_X_POSITIONS; i++){            
            glVertex2f(x, -1.0);
            glVertex2f(x, SCREEN_BROWNIAN_RANGE-Y_STEP/2);
            x += X_STEP*2;
        }            
            
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT); 
    
    /* Draw nails */
    draw_nails();

    /* Draw bars */
    draw_bars();
    
    /* Draw Canicas */
    for (int i=0; i<SIZE_OF_ARRAY; i++){
        drawCanica(&canicas[i]);    
    }    

    glFlush(); // Giux: se necesita?????
}

int main(int argc, char** argv) {
    cout<<"Size of Vector (x) = "<<sizeof(canicaVector)/sizeof(int)<<endl;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //glutInitWindowPosition(80, 80);
    glutInitWindowSize(window_width, window_height);    
    glutCreateWindow("Brownian move Simulation");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutTimerFunc(100, timer, 0);
    // Aqui va la función para Mouse o Keyboard
    
    glutMainLoop();
    
    return 0;
}
