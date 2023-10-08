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

/***** MACROS Section *****/
#define window_width       2000  
#define window_height      2000
#define FPS                50  //original=60  // This is the number of frames per second to render.
#define Y_COORDENATE_INIT  0.95
#define X_COORDENATE_INIT  0.5
#define SIZE_OF_ARRAY      100
#define TOTAL_Y_POSITIONS  30
#define BIT_MAP_SIZE       16
#define BIT_MAP_IMAGE      canica16_bitmap //canica_bitmap  canica16_bitmap

enum {
    RGB_R =  0,
    RGB_G,
    RGB_B
};

enum {
    COORDENATE_X = 0,
    COORDENATE_Y
};

// Circle bitmap  32x32 bits
GLubyte canica_bitmap[]={
0x00,    0x00,    0x00,    0x00, 0x00,    0x0F,    0xF0,    0x00, 0x00,    0x7F,    0xFC,    0x00,
0x01,    0xFF,    0xFE,    0x00, 0x01,    0xFF,    0xFF,    0x80, 0x03,    0xFF,    0xFF,    0xC0,
0x07,    0xFF,    0xFF,    0xE0, 0x1F,    0xFF,    0xFF,    0xF0, 0x1F,    0xFF,    0xFF,    0xF8,
0x3F,    0xFF,    0xFF,    0xF8, 0x3F,    0xFF,    0xFF,    0xFC, 0x7F,    0xFF,    0xFF,    0xFC,
0x7F,    0xFF,    0xFF,    0xFE, 0x7F,    0xFF,    0xFF,    0xFE, 0x7F,    0xFF,    0xFF,    0xFE,
0x7F,    0xFF,    0xFF,    0xFE, 0x7F,    0xFF,    0xFF,    0xFE, 0x7F,    0xFF,    0xFF,    0xFE,
0x7F,    0xFF,    0xFF,    0xFE, 0x7F,    0xFF,    0xFF,    0xFC, 0x3F,    0xFF,    0xFF,    0xFC,
0x3F,    0xFF,    0xFF,    0xFC, 0x1F,    0xFF,    0xFF,    0xF8, 0x0F,    0xFF,    0xFF,    0xF8,
0x07,    0xFF,    0xFF,    0xF0, 0x07,    0xFF,    0xFF,    0xE0, 0x03,    0xFF,    0xFF,    0xC0,
0x01,    0xFF,    0xFF,    0x80, 0x01,    0xFF,    0xFF,    0x80, 0x00,    0x7F,    0xFF,    0x80,
0x00,    0x0F,    0xF0,    0x00, 0x00,    0x00,    0x00,    0x00,
};



GLubyte canica16_bitmap[]={
0b00000000, 0b00000000,
0b00000111, 0b11100000,
0b00011111, 0b11111000,
0b00011111, 0b11111000,
0b00111111, 0b11111100,
0b00111111, 0b11111100,
0b00111111, 0b11111100,
0b01111111, 0b11111110,
0b01111111, 0b11111110,
0b01111111, 0b11111110,
0b01111111, 0b11111110,
0b00111111, 0b11111100,
0b00111111, 0b11111100,
0b00111111, 0b11111100,
0b00011111, 0b11111000,
0b00011111, 0b11111000,
0b00000111, 0b11100000,
0b00000000, 0b00000000,  
};

GLubyte canica16_bitmap2[]={
    0x00, 0x00,
    0x07, 0xE0,
    0x1F, 0xF8,
    0x3F, 0xFC,
    0x3F, 0xFC,
    0x7F, 0xFE,
    0x7F, 0xFE,
    0x7F, 0xFE,
    0x7F, 0xFE,
    0x7F, 0xFE,
    0x7F, 0xFE,
    0x3F, 0xFC,
    0x3F, 0xFC,
    0x1F, 0xF8,
    0x07, 0xE0,
    0x00, 0x00,  
};


// Return a random float in the range 0.0 to 1.0.
GLfloat randomFloat() {
  return (GLfloat)rand() / RAND_MAX;
}


/***** CLASSES *****/
class canica{
    private:
        float color[3];
        double x_coor;
        double y_coor;
        bool isDone;

    public:
        int matrixCoor[2];
        int barIndex;
        static int bars[20];        

        canica(){
            color[RGB_R] = randomFloat();
            color[RGB_G] = randomFloat();
            color[RGB_B] = randomFloat();
            x_coor = X_COORDENATE_INIT;
            y_coor = Y_COORDENATE_INIT;
            matrixCoor[0] = 10;
            matrixCoor[1] = TOTAL_Y_POSITIONS;
            isDone = false;
            barIndex = 10;
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
};

canica canicas[SIZE_OF_ARRAY];

int canMatrix[20][200]={0};
int canicaVector[20]={0};

/***** STANDARD Namespace *****/
using namespace std;

/* LOCAL VARIABLES  Section */



/***** LOCAL FUNCTIONS Section *****/
void printXcordenates();
void printBitmap(GLubyte * ptr, int size);

void drawCanica(canica * canicaPtr){
    glColor3f(canicaPtr->getRGB(RGB_R), canicaPtr->getRGB(RGB_G), canicaPtr->getRGB(RGB_B));    
    glRasterPos3f(canicaPtr->getDisplayPositionX(), canicaPtr->getDisplayPositionY(), 0.0); //(X, Y, ?, ? )
    glBitmap(BIT_MAP_SIZE, BIT_MAP_SIZE, 0, 0, 0, 0, BIT_MAP_IMAGE);   
}

#define Y_STEP             0.03
#define X_STEP             0.015          

void moveCanica(canica * canicaPtr){
  double dispPosition_y = canicaPtr->getDisplayPositionY();
  double dispPosition_x = canicaPtr->getDisplayPositionX();  
  static int tId = 0;

  /* Validate that the ball is still in the screen by Y position */
  dispPosition_y-= Y_STEP;  
  if ((canicaPtr->matrixCoor[COORDENATE_Y]>canicaVector[canicaPtr->matrixCoor[COORDENATE_X]]) /* && (canicaPtr->getIsDone()==false)*/ /*&& (canicaVector[canicaPtr->matrixCoor[COORDENATE_Y]]  )*/  ){
         
    if (true){
        canicaPtr->matrixCoor[COORDENATE_Y] -= 1;
        // cout<<tId++<<endl;
        /* If ball is in brownian range, make the x movement */
        if (dispPosition_y>0.35){        
            //cout<<canicaPtr->matrixCoor[COORDENATE_X]<<endl;
            if (rand()%2) {
                dispPosition_x += X_STEP;      
                canicaPtr->matrixCoor[COORDENATE_X] += 1;
            } 
            else {
                dispPosition_x -= X_STEP;
                canicaPtr->matrixCoor[COORDENATE_X] -= 1;
            }
        }
    } else {
        /* Actions to take when ball finished brownian movement */
        tId=0;
        cout<<endl;

    }


    canicaPtr->setDisplayPositionX(dispPosition_x);
    canicaPtr->setDisplayPositionY(dispPosition_y);
  }
  else  { // here the Ball gas reach the bottom.
    canicaPtr->setIsDone(true);    
    canMatrix[20-canicaPtr->matrixCoor[COORDENATE_Y]][canicaPtr->matrixCoor[COORDENATE_X]] = 1;
    canicaVector[canicaPtr->matrixCoor[COORDENATE_X]]++;
  }

    printXcordenates();
}


void timer(int v) {      
  for (int i=0; i<SIZE_OF_ARRAY; i++){
    if (i==0) {
        moveCanica(&canicas[i]);    
      }
      else if  ((i>0) && (canicas[i-1].getIsDone()==true)) {
        moveCanica(&canicas[i]);          
      }
      else {/*do nothing*/}
    }

  glutPostRedisplay();
  glutTimerFunc(2000/FPS, timer, v);
}


// On reshape, uses an orthographic projection with world coordinates ranging
// from 0 to 1 in the x and y directions, and -1 to 1 in z.
void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Giux: se necesita?????
    for (int i=0; i<SIZE_OF_ARRAY; i++){
        drawCanica(&canicas[i]);    
    }    
    glFlush(); // Giux: se necesita?????
}
bool myflag=true;

int main(int argc, char** argv) {
   
    if (myflag) {
        //printBitmap(BIT_MAP_IMAGE, BIT_MAP_SIZE);
        myflag=false;
    }
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //glutInitWindowPosition(80, 80);
    glutInitWindowSize(window_width, window_height);    
    glutCreateWindow("Canica Simulation");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutTimerFunc(100, timer, 0);
    // Aqui va la función para Mouse o Keyboard
    glutMainLoop();

    //printXcordenates();
    getchar();
    return 0;
}


void printXcordenates(){
    for (int i=0; i<=SIZE_OF_ARRAY; i++){
       // cout<<"canica "<<i <<"coord X= "<<canicas[i].getDisplayPositionX() <<endl;       
    }
}

void printBitmap(GLubyte * ptr, int size)
{
    for (int i=0; i<size; i)
    {
        for (int j=0; j<size/2; j++)
        {
            char c = (ptr[i+j]&0x80)?'X':' ';
            ptr[i+j]>>=1;
            cout<<c;
        }
        cout<<endl;
    }
}