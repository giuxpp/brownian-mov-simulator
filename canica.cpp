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
#define APP_MODE_RESTART_ENABLE   0
#define LOGS_ENABLED              0
#define SIZE_OF_ARRAY             200
#define SIMULATION_VELOCITY       (5)   //int {1-100..}  1=max 100..=min
#define window_width              1500
#define window_height             2200
#define Y_STEP                    (0.00195)
#define X_STEP                    (0.0016)          
#define Y_COORDENATE_INIT         (1.0)
#define X_COORDENATE_INIT         (0.5)
#define TOTAL_Y_POSITIONS         ((uint8_t)(1/Y_STEP))
#define TOTAL_X_POSITIONS         ((uint8_t)(1/X_STEP))    
#define SCREEN_BROWNIAN_RANGE     (0.52)
#define READY_FOR_NEXT_TRH        (0.935) //threshold of Y coordenates to launche next ball 
#define INITIAL_X_POSITION        14 //((uint8_t)(TOTAL_X_POSITIONS/2))   // initial position of X coordenate in the Bars vector
#define INITIAL_Y_POSITION        50

#define BIT_MAP_SIZE              16
#define BIT_MAP_IMAGE             canica16_bitmap //canica_bitmap  canica16_bitmap
#define Y_BOTTOM_COORDENATE       (2.5*Y_STEP) 
#define TOTAL_BARS                (2*INITIAL_X_POSITION)
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

enum moveStates{
    moveState_Down=0,
    moveState_UpRight,
    moveState_UpLeft,
    moveState_noBrown,
    moveState_done
};


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
        int moveStep;
        moveStates moveState;

        canica(){
            color[RGB_R] = (float)(rand()%10)/10;  //randomFloat 0.0 - 1.0 ;
            color[RGB_G] = (float)(rand()%10)/10;  //randomFloat 0.0 - 1.0 ;
            color[RGB_B] = (float)(rand()%10)/10;  //randomFloat 0.0 - 1.0 ;
            x_coor = X_COORDENATE_INIT;
            y_coor = Y_COORDENATE_INIT;
            matrixCoor[COORDENATE_X] = INITIAL_X_POSITION;
            matrixCoor[COORDENATE_Y] = INITIAL_Y_POSITION;
            isDone = false;
            readyForNext = false;
            index = totalCanicas++;
            moveState = moveState_Down;
            moveStep = 0;
            //printf("Canica index=%i matrix_X=%i Matrix_Y=%i\n",index,matrixCoor[COORDENATE_X],matrixCoor[COORDENATE_Y]);
        }

        ~canica (){

        }

        void restart(){
            x_coor = X_COORDENATE_INIT;
            y_coor = Y_COORDENATE_INIT;
            matrixCoor[COORDENATE_X] = INITIAL_X_POSITION;
            matrixCoor[COORDENATE_Y] = INITIAL_Y_POSITION;
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

uint8_t canicaVector[TOTAL_BARS]={0};

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
    double dispPosition_y = canicaPtr->getDisplayPositionY();
    double dispPosition_x = canicaPtr->getDisplayPositionX(); 
    int matrixCoor_X = canicaPtr->matrixCoor[0];
    int matrixCoor_Y = canicaPtr->matrixCoor[1];    
 
    /* This conditions triggers the start of the next canica by setting "ReadyForNext" flag */
    if (dispPosition_y<READY_FOR_NEXT_TRH){
        canicaPtr->setIsReadyForNext(true);
    }

    // Move X and Y coordenates depending on moving state 
    switch (canicaPtr->moveState){
        case moveState_Down:                        
            if (dispPosition_y<SCREEN_BROWNIAN_RANGE){                        
                canicaPtr->moveState = moveState_noBrown;
            }
            else{                        
                if (canicaPtr->moveStep<18u){
                    canicaPtr->moveStep++;
                    dispPosition_y -= Y_STEP;
                }
                else{
                    canicaPtr->moveStep=0;
                    if (rand()%2){
                        canicaPtr->moveState=moveState_UpLeft;
                        matrixCoor_X -= 1;                        
                    } else {
                        canicaPtr->moveState=moveState_UpRight;
                        matrixCoor_X += 1;
                    }                                                   
                }
            }                                                                                          
            break;
        
        case moveState_UpRight:
                    if (canicaPtr->moveStep<24u){
                        canicaPtr->moveStep++;
                        dispPosition_y += 0.2595*Y_STEP;
                        dispPosition_x += 0.25*X_STEP;
                    }
                    else{
                        canicaPtr->moveStep=0;
                        //canicaPtr->matrixCoor[COORDENATE_Y] -= 1;
                        canicaPtr->moveState=moveState_Down;
                    }  
                break;

        case moveState_UpLeft:
                    if (canicaPtr->moveStep<24u){
                        canicaPtr->moveStep++;
                        dispPosition_y += 0.2595*Y_STEP;
                        dispPosition_x -= 0.25*X_STEP;
                    }
                    else{
                        canicaPtr->moveStep=0;
                        //canicaPtr->matrixCoor[COORDENATE_Y] -= 1;
                        canicaPtr->moveState=moveState_Down;
                    }  
                break;  
        
        case moveState_noBrown:                    
                if ((matrixCoor_Y>(canicaVector[matrixCoor_X]+1) ) && (dispPosition_y>Y_BOTTOM_COORDENATE) ) {                  
                        dispPosition_y -= Y_STEP;
                        if (canicaPtr->moveStep++ >= 6){                            
                            canicaPtr->moveStep = 0;
                            matrixCoor_Y -= 1;
                        }                    
                }
                else {
                    // here the Balls reach the bottom.
                    canicaPtr->moveState=moveState_done;
                    canicaVector[matrixCoor_X] = matrixCoor_Y;

                    cout<<"Canica "<<canicaPtr->index<<"is done in";
                    cout<<" CoorX="<<canicaPtr->matrixCoor[0];                         
                    cout<<" CoorY="<<canicaPtr->matrixCoor[1];
                    cout<<endl;
                }
                break;

        case moveState_done:
                /* no move to be done */
                break;

        default: 
                canicaPtr->moveState=moveState_Down;
                break;  
    }
    
    /* Update canica object with the new x and y data */
    canicaPtr->setDisplayPositionX(dispPosition_x);
    canicaPtr->setDisplayPositionY(dispPosition_y);
    canicaPtr->matrixCoor[0] = matrixCoor_X;
    canicaPtr->matrixCoor[1] = matrixCoor_Y;
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
    const float offset = 0.5 + 4.5*X_STEP;
    float x = offset-6*X_STEP;
    float y = Y_COORDENATE_INIT-Y_STEP*4;

    glColor3f(1.0, 1.0, 1.0); /* Blanco */   

    glBegin(GL_POINTS);
        for (int i=2; i<TOTAL_BARS-5; i++){            
            for (int j=0; j<i; j++){            
                glVertex2f(x, y);               
                x+=X_STEP*12;                 
            }       
            y-= Y_STEP*12;     
            x=offset-i*6*X_STEP;
        }   
    glEnd();
}

void draw_bars(){        
    float x = 0.20+11*6*X_STEP;
    float y = SCREEN_BROWNIAN_RANGE-Y_STEP/2;

    glColor3f(0.5, 0.6, 0.5); /*  */   

    glBegin(GL_LINES);
        for (int i=0; i<TOTAL_BARS-6; i++){            
            glVertex2f(x, 0.0);
            glVertex2f(x, SCREEN_BROWNIAN_RANGE);
            x += 2*6*X_STEP;
        }                        
    glEnd();
}

void display() {
    //glClearColor(0.25, 0.50, 0.10, 0.5); // green
    glClearColor(0.10, 0.0, 0.0, 0.5); // 
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
    glutTimerFunc(SIMULATION_VELOCITY, timer, 0);
    // Aqui va la función para Mouse o Keyboard
    
    glutMainLoop();
    
    return 0;
}
