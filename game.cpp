#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <math.h>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <iostream>

// This is just an example using basic glut functionality.
// If you want specific Apple functionality, look up AGL
using namespace std;

vector<pair<GLfloat, GLfloat> > bullets, aliens;
GLfloat r = 0.15;
GLfloat pxl = -0.15, pxr = 0.15;
bool play = true;

pair<GLfloat, GLfloat> inc(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second+=0.1);
}
pair<GLfloat, GLfloat> incAlien(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second-=0.1);
}

void DrawStarFilled (float cx, float cy, float radius, int numPoints){
    /*const float DegToRad = 3.14159 / 180;

    glBegin(GL_TRIANGLE_FAN);
    int count = 1;
    glVertex3f(cx, cx, -10);
    for(int i = 0; i <= numPoints*2; i++) {
        float DegInRad = i * 360.0/(numPoints*2) * DegToRad;
        if(count%2!=0)
            glVertex3f((float)cx + cos(DegInRad) * radius, cy + sin(DegInRad) * radius, -10);
        else
            glVertex3f((cx + cos(DegInRad) * radius/2), (cy + sin(DegInRad) * radius/2), -10);
        count++;
    }

    glEnd();*/
    const float DegToRad = 3.14159 / 180;

    glBegin (GL_TRIANGLE_FAN);
    int count = 1;
    glVertex3f(cx, cy, -10);
    for (int i = 0; i <= 360; i+=360/(numPoints*2)) {
        float DegInRad = i * DegToRad;
        if(count%2!=0)
            glVertex3f (cx + cos (DegInRad) * radius, cy + sin (DegInRad) * radius, -10);
        else
            glVertex3f ((cx + cos (DegInRad) * radius/2), (cy + sin (DegInRad) * radius/2), -10);
        count++;
    }

    glEnd();
}

void DrawCircle(GLfloat cx, GLfloat cy, GLfloat r, int num_segments){
    /*glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++){
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component

        glVertex2f(x + cx, y + cy);//output vertex
        bullets.push_back(make_pair(x+cx, y+cy));
    }
    glEnd();*/
    bullets.push_back(make_pair(cx, cy));
}

void specialKeys( int key, int x, int y ){
    const float step = 0.05;
    if(play){
        if (key == GLUT_KEY_RIGHT){
            pxl += step;
            pxr += step;
        }
        else if (key == GLUT_KEY_LEFT){
            pxl -= step;
            pxr -= step;
        }
        else if(key == 32){
            cout<<(GLfloat)((pxl+pxr)/2)<<endl;
            bullets.push_back(make_pair((GLfloat)((pxl+pxr)/2), -2.4));
        }
        glutPostRedisplay();
    }
        //DrawCircle((GLfloat)((pxl+pxr)/2), -2.4, 0.1, 0.2);
}

void init(){ // Called before main loop to set up the program
    glClearColor(0.0, 0.08, 0.42, 0.5);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    DrawStarFilled(0, 2, 0.1, 5);

    glBegin(GL_TRIANGLES);
        glVertex3f(pxl, -2.6, -10.0);
        glVertex3f(float((pxl+pxr)/2), -2.4, -10.0);
        glVertex3f(pxr, -2.6, -10.0);
    glEnd();
    if(bullets.size()>=0 && play)
        transform(begin(bullets), end(bullets), begin(bullets), inc);
    for(int i=0;i<bullets.size();++i){
        //DrawCircle(bullets[i].first, bullets[i].second, 0.1, 0.2);
        //cout<<i<<" "<<bullets[i].first<<", "<<bullets[i].second<<endl;
        glBegin(GL_TRIANGLES);
            glVertex3f(bullets[i].first-r, bullets[i].second-r, -10.0);
            glVertex3f(bullets[i].first, bullets[i].second, -10.0);
            glVertex3f(bullets[i].first+r, bullets[i].second-r, -10.0);
        glEnd();
    }

    glutSwapBuffers();
}

// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int main(int argc, char **argv){
    glutInit(&argc, argv); // Initializes glut

    // Sets up a double buffer with RGBA components and a depth component
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    // Sets the window size to 512*512 square pixels
    glutInitWindowSize(512, 1024);

    // Sets the window position to the upper left
    glutInitWindowPosition(0, 0);

    // Creates a window using internal glut functionality
    glutCreateWindow("HyperSpace");

    // passes reshape and display functions to the OpenGL machine for callback
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutSpecialFunc(specialKeys);
    init();

    // Starts the program.
    glutMainLoop();
    return 0;
}

//Redundant for now
//Do not touch this part

GLuint LoadTexture( const char * filename ){
    GLuint texture;
    int width, height;
    unsigned char * data;
    FILE * file;
    file = fopen( filename, "rb" );

    if ( file == NULL )
        return 0;
    width = 1024;
    height = 512;
    data = (unsigned char *)malloc(width * height * 3);

    fread(data, width * height * 3, 1, file);
    fclose(file);

    for(int i = 0; i < width * height ; ++i){
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;

    }

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free( data );

    return texture;
}
