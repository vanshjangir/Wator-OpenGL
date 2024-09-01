#include <GL/freeglut_std.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <time.h>
#include "fish.c"
#include "shark.c"

#ifndef GUI_H_
#define GUI_H_

unsigned long FPS = 60;

vector * fishes;
vector * sharks;

Creature ***sea;
int old = 0;

void square(int x,int y,char R,char G,char B){
  glLineWidth(0.1);
  glColor3ub(R,G,B);
  glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);
    glVertex2f(x+1,y);
    glVertex2f(x+1,y+1);
    glVertex2f(x,y+1);
  glEnd();
}


void drawGrid(){
  for (int x=0; x<SIZE; x++) {
    for (int y = 0; y < SIZE; y++) {
      square(x,y,(char)255,(char)255,(char)255);
    }
  }
}

void draw_border(int x,int y){
  glColor3ub(255.0 ,255.0 , 255.0);
  glBegin(GL_LINE_LOOP);
    glVertex2d(x, y);
    glVertex2d(x+1, y);
    glVertex2d(x+1, y+1);
    glVertex2d(x, y+1);
  glEnd(); 



}

void copy(Creature ***sea, int x, int y, int i, int j){
    int new = old^1;
    sea[new][x][y].energy = sea[old][i][j].energy;
    sea[new][x][y].breeding_time = sea[old][i][j].breeding_time;
}

void block(int x, int y){
    sea[old][x][y].kin = Blocked;
}

void moveS(Creature ***sea, int old, int i, int j){

    sea[old][i][j].energy -= 1;
    sea[old][i][j].breeding_time += 1;

    int new = old^1;
    int dt[4][2];
    int fishEaten = 0;
    
    dt[0][0] = (i+SIZE-1)%SIZE; dt[0][1] = j;
    dt[1][0] = (i+SIZE+1)%SIZE; dt[1][1] = j;
    dt[2][1] = (j+SIZE-1)%SIZE; dt[2][0] = i;
    dt[3][1] = (j+SIZE+1)%SIZE; dt[3][0] = i;


    int n = rand()%4;
    int newx, newy;
    for(int k = 0; k < 4; k++){

        int x = dt[n][0]; int y = dt[n][1];
        if(sea[old][x][y].kin == Fish){
            sea[new][x][y].kin = Shark;
            sea[new][x][y].energy = sea[old][i][j].energy;
            sea[new][x][y].breeding_time = sea[old][i][j].breeding_time;
            sea[new][x][y].energy += 3;
            newx = x, newy = y;

            block(x,y);
            block(i,j);
            fishEaten = 1;
            break;
        }
        n = (n+1)%4;
    }
    
    if(fishEaten == 0 && sea[old][i][j].energy <= 0){
        sea[new][i][j].kin = Water;
        block(i,j);
        return;
    }
    
    if(fishEaten == 1 && sea[old][i][j].breeding_time >= BREEDING_T_S &&
        sea[old][i][j].energy > 0)
    {
        sea[new][i][j].kin = Shark;
        sea[new][i][j].breeding_time = 0;
        sea[new][newx][newy].breeding_time = 0;
        sea[new][i][j].energy = ENERGY_S;
        block(i,j);
    }

    if(fishEaten == 1){
        return;
    }
    
    n = rand()%4;
    int hasMoved = 0;
    for(int k = 0; k < 4; k++){
        int x = dt[n][0]; int y = dt[n][1];
        if(sea[old][x][y].kin == Water){
            sea[new][x][y].kin = Shark;
            sea[new][x][y].energy = sea[old][i][j].energy;
            sea[new][x][y].breeding_time = sea[old][i][j].breeding_time;
            block(x,y);
            block(i,j);
            hasMoved = 1;
            newx = x;
            newy = y;
            break;
        }
        n = (n+1)%4;
    }

    if(hasMoved == 1){
        if(sea[old][i][j].breeding_time > BREEDING_T_S){
            sea[new][newx][newy].breeding_time = 0;
        }
        return;
    }

    sea[new][i][j].kin = Shark;
    copy(sea, i, j, i, j);
}

void moveF(Creature ***sea, int old, int i, int j){
    
    sea[old][i][j].breeding_time += 1;
    
    int new = old^1;
    int dt[4][2];
    int fishEaten = 0;
    int sharkx, sharky;

    dt[0][0] = (i+SIZE-1)%SIZE; dt[0][1] = j;
    dt[1][0] = (i+SIZE+1)%SIZE; dt[1][1] = j;
    dt[2][1] = (j+SIZE-1)%SIZE; dt[2][0] = i;
    dt[3][1] = (j+SIZE+1)%SIZE; dt[3][0] = i;
    
    int n = rand()%4;
    for(int k = 0; k < 4; k++){
        int x = dt[n][0]; int y = dt[n][1];
        if(sea[old][x][y].kin == Shark){
            moveS(sea, old, x, y);
            return;
        }
        n = (n+1)%4;
    }

    n = rand()%4;
    int hasMoved = 0;
    int newx, newy;
    for(int k = 0; k < 4; k++){
        int x = dt[n][0]; int y = dt[n][1];
        if(sea[old][x][y].kin == Water){
            sea[new][x][y].kin = Fish;
            copy(sea, x, y, i, j);
            block(x,y);
            block(i,j);
            newx = x;
            newy = y;
            hasMoved = 1;
            break;
        }
        n = (n+1)%4;
    }

    if(sea[old][i][j].breeding_time >= BREEDING_T_F && hasMoved == 1){
        sea[new][i][j].kin = Fish;
        sea[new][i][j].breeding_time = 0;
        sea[new][newx][newy].breeding_time = 0;
        block(i,j);
    }

    if(hasMoved == 1){
        return;
    }

    sea[new][i][j].kin = Fish;
    copy(sea, i, j, i, j);
}

void draw(){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(sea[old][i][j].kin == Shark){
                glColor3ub(255,139,97);
                glRectd(i,j,i+1,j+1);
            }
            else if(sea[old][i][j].kin == Fish){
                glColor3ub(121,183,205);
                glRectd(i,j,i+1,j+1);
            }
            draw_border(i, j);
        }
    }
}

void reset(){
    draw();

    int nShark = 0;
    int nFish = 0;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(sea[old][i][j].kin == Shark){
                nShark++;
            }
            else if(sea[old][i][j].kin == Fish){
                nFish++;
            }
        }
    }
    printf("nShark %d, nFish %d\n", nShark, nFish);

    int new = old^1;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            sea[new][i][j].kin = Water;
        }
    }

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(sea[old][i][j].kin == Shark){
                moveS(sea, old, i, j);
            }
            else if(sea[old][i][j].kin == Fish){
                moveF(sea, old, i, j);
            }
        }
    }
    old ^= 1;
}

void display() {  // Display function will draw the image. baiscally the main drawing loop
    
    #ifdef DEBUG_ON
    
      printf("CURRENT FISHES %lu\n",fishes->size);
      printf("CURRENT SHARK %lu\n",sharks->size);
  
    #endif /* ifndef DEBUG_ON */

    glClearColor( 1, 1, 1, 1 );  // (In fact, this is the default.)
    glClear( GL_COLOR_BUFFER_BIT );
    drawGrid();

    //Draw Fish
    //for (size_t i = 0; i < fishes->size; i++) {
    //  glColor3ub(121,183,205);
    //  moveFish(&(fishes->data[i]), fishes, sharks);
    //  drawFish(fishes->data[i].coord);
    //  float x = fishes->data[i].coord.x;
    //  float y = fishes->data[i].coord.y;
    //  draw_border(x, y); 
    //}

    ////Draw Shark
    //for (size_t i = 0; i < sharks->size; ++i) {
    //  glColor3ub(255,139,97);
    //  if(!moveShark(&(sharks->data[i]), fishes, sharks)){
    //    --i;
    //  };
    //  drawShark(sharks->data[i].coord);
    //  float x = sharks->data[i].coord.x;
    //  float y = sharks->data[i].coord.y;
    //  draw_border(x, y);
    //}

    reset();
    
    glutSwapBuffers(); 
 
}

void reshape_callback(int width,int height){
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, SIZE, 0.0, SIZE, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void timer_func(){ //For setting up FPS
  glutPostRedisplay();
  glutTimerFunc(1000/FPS,timer_func,0);
}

void keyboard_callback(int key,int _,int __){
  switch (key) {
    case GLUT_KEY_UP:
      FPS++;
      break;
    case GLUT_KEY_DOWN:
      if (FPS==1) break;
        FPS--;
      break;
  }
}

void set_spawn_rate(unsigned char key,int _,int __){
  switch (key) {
    case 'a' | 'A':
      if (ENERGY_F==0) break;
        ENERGY_F--;
    case 'd' | 'D':
      ENERGY_F++;
    case 'w' | 'W':
      ENERGY_S++;
    case 's' | 'S':
      if (ENERGY_S==0) break;
        ENERGY_S--;
    
  }
}

int main_loop( int argc, char** argv) {  // Initialize GLUT and 
    srand(time(0));
    
    sea = (Creature ***)malloc(2 * sizeof(int **));

    for (int i = 0; i < 2; i++) {
        sea[i] = (Creature **)malloc(SIZE * sizeof(Creature *));
        for (int j = 0; j < SIZE; j++) {
            sea[i][j] = (Creature *)malloc(SIZE * sizeof(Creature));
        }
    }
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {
                sea[i][j][k].kin = Water;
            }
        }
    }

    gen_sharks(sea, 10);
    gen_fish(sea, 300);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);    
    glutInitWindowSize(1000,1000);


    
    glutCreateWindow("WatorGL");
    glutDisplayFunc(display); // Called when the window needs to be redrawn.
    glutReshapeFunc(reshape_callback); // In order to preserve the grid
    glutTimerFunc(0, timer_func,0);
    //glutSpecialFunc(keyboard_callback);
    //glutKeyboardFunc(set_spawn_rate);
    glutMainLoop(); // Run the event loop!  This function does not return.
    return 0;
}


#endif /* ifndef GUI_H_ */
