#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <math.h>
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;
const float step = 0.1;

/*
    ALIEN FORMATIONS
    1 -> simple horizontal
    2 -> 2 groups, each coming from either side of the screen
    3 -> dense horizontal from top
    4 -> 2 dense horizontal groups from either side of the screen
*/

int keyPress[3] = {0, 0, 0}; //left, right, shoot

GLuint LoadTextureRAW( const char * filename, bool wrap )
{
  GLuint texture;
  int width, height;
  unsigned char * data;
  FILE * file;

  // open texture data
  file = fopen( filename, "rb" );
  if ( file == NULL ) return 0;

  // allocate buffer
  width = 256;
  height = 256;
  data = (unsigned char *) malloc( width * height * 3 );

  // read texture data
  fread( data, width * height * 3, 1, file );
  fclose( file );

  // allocate a texture name
  glGenTextures( 1, &texture );

  // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );

  // select modulate to mix texture with color for shading
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

  // when texture area is small, bilinear filter the closest MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                   wrap ? GL_REPEAT : GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                   wrap ? GL_REPEAT : GL_CLAMP );

  // build our texture MIP maps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,
    height, GL_RGB, GL_UNSIGNED_BYTE, data );

  // free buffer
  free( data );

  return texture;

}

void FreeTexture( GLuint texture ){
    glDeleteTextures( 1, &texture );
}

vector<pair<GLfloat, GLfloat> > bullets, aliens1, aliens21, aliens22, aliens3, aliens4;
pair<GLfloat, GLfloat> alienUpdate;
map<GLfloat, pair<GLfloat, int> > m;
GLfloat r = 0.15, pxl = -0.15, pxr = 0.15, pos = 0;
bool play = true, firstHit = false;
int level = 0, cf = 2;

void checkRedundant(int formation){
    //cout<<formation<<endl;
    switch(formation){
        case 1:
            for(int i=0;i<aliens1.size();++i)
                if(aliens1[i].second<-3)
                    aliens1.erase(aliens1.begin()+i);
            break;
        case 2:
            for(int i=0;i<aliens21.size();++i)
                if(aliens21[i].first>2){
                    cout<<"remove 21 "<<i<<endl;
                    aliens21.erase(aliens21.begin()+i);
                }
            for(int i=0;i<aliens22.size();++i)
                if(aliens22[i].first<-2){
                    cout<<"remove 22 "<<i<<endl;
                    aliens22.erase(aliens22.begin()+i);
                }
            break;
    }
}

void outputText(int x, int y, float r, float g, float b, char *string){
  glColor3f( r, g, b );
  glRasterPos2f(x, y);
  int len, i;
  len = (int)strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string[i]);
  }
}

pair<GLfloat, GLfloat> inc(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second+=0.1);
}
pair<GLfloat, GLfloat> incAlien(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second-=(0.005+(level*0.0005)));
}
pair<GLfloat, GLfloat> incAlien21(pair<GLfloat, GLfloat> p){
    return make_pair(p.first+=0.005+(level*0.0005), p.second-=(0.005+(level*0.0005)));
}
pair<GLfloat, GLfloat> incAlien22(pair<GLfloat, GLfloat> p){
    return make_pair(p.first-=(0.005+(level*0.0005)), p.second-=(0.005+(level*0.0005)));
}

void displayBullets(){
    for(int i=0;i<bullets.size();++i)
        cout<<bullets[i].first<<", "<<bullets[i].second<<", pos = "<<pos<<endl;
}

void showAliens(int);
bool aliensGone(int);
void DrawStarFilled (float cx, float cy, float radius, int numPoints);
void genAliens(int);

void showAliens(int formation){
    if(formation==1){
        for(int i=0;i<aliens1.size();i++)
            DrawStarFilled(aliens1[i].first, aliens1[i].second, 0.1, 5);
        transform(begin(aliens1), end(aliens1), begin(aliens1), incAlien);
    }
    else if(formation==2){
        for(int i=0;i<aliens21.size();i++)
            DrawStarFilled(aliens21[i].first, aliens21[i].second, 0.1, 5);
        transform(begin(aliens21), end(aliens21), begin(aliens21), incAlien21);
        for(int i=0;i<aliens22.size();i++)
            DrawStarFilled(aliens22[i].first, aliens22[i].second, 0.1, 5);
        transform(begin(aliens22), end(aliens22), begin(aliens22), incAlien22);
    }
}


bool aliensGone(int formation){
    int a[4];
    a[0] = aliens1.size();
    a[1] = aliens21.size() + aliens22.size();
    a[2] = aliens3.size();
    a[3] = aliens4.size();
    //cout<<a[0]<<endl;
    if(a[formation-1]==0){
        //cout<<"it is 0\n";
        return true;
    }
    return false;
}

//temporary
int newFormation(){
    return 1;
}

void removeAliens(int index, int f){
    if(f==1)
        aliens1.erase(aliens1.begin()+index);
    else if(f==2){
        if(index>=aliens21.size())
            aliens22.erase(aliens22.begin()+index-aliens21.size());
        else
            aliens21.erase(aliens21.begin()+index);
    }
    else if(f==3)
        aliens3.erase(aliens3.begin()+index);

}

bool detectCollision(int formation){
    vector<pair<GLfloat, GLfloat> > al;
    switch(formation){
        case 1:
            al = aliens1;
            break;
        case 2:
            al.reserve(aliens21.size()+aliens22.size());
            al.insert(al.end(), aliens21.begin(), aliens21.end());
            al.insert(al.end(), aliens22.begin(), aliens22.end());
            break;
        case 3:
            al = aliens3;
            break;
    }
    m.clear();
    for(int i=0;i<al.size();++i)
        m[al[i].second] = make_pair(al[i].first, i);
    for(int i=0;i<bullets.size();++i){
        for(int j=0;j<al.size();++j){
            if(fabs((float)(al[j].second-bullets[i].second))<=0.1){
                if(fabs((float)(al[j].first-bullets[i].first))<=r){
                    //aliens1.erase(aliens1.begin()+j);
                    removeAliens(j, formation);
                    bullets.erase(bullets.begin()+i);
                    firstHit = true;
                    return true;
                }
            }
        }
    }
    checkRedundant(cf);
    //if(firstHit)
    if(aliensGone(cf)){
        level++;
        cout<<"gone\n";
        cf = rand() % 2 + 1;
        genAliens(cf);
        firstHit = false;
        //cout<<cf<<endl;
    }

    showAliens(cf);
    if(bullets.size()>=0 && play)
        transform(begin(bullets), end(bullets), begin(bullets), inc);

    return false;
}

bool dead(){
    for(int i=0;i<aliens1.size();++i)
        if(aliens1[i].first >= pos - 2*r && aliens1[i].first <= pos + 2*r && aliens1[i].second <= -2.375)
            return true;
    return false;
}

void genAliens(int formation){
    /*
        ALIEN FORMATIONS
        1 -> simple horizontal
        2 -> 2 groups, each coming from either side of the screen
        3 -> dense horizontal from top
        4 -> 2 dense horizontal groups from either side of the screen
    */
    aliens1.clear();
    aliens21.clear();
    aliens22.clear();
    aliens3.clear();
    aliens4.clear();
    if(formation==1){
        for(GLfloat i=-1;i<=1;i+=0.4)
            aliens1.push_back(make_pair(i, 2));
    }
    if(formation==2){
        for(GLfloat i=-2;i<=-1.1;i+=0.3)
            aliens21.push_back(make_pair(i, 2));
        for(GLfloat i=2;i>=1.1;i-=0.3)
            aliens22.push_back(make_pair(i, 1.5));
    }
}

void DrawStarFilled (float cx, float cy, float radius, int numPoints){
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

void kfu(unsigned char key, int x, int y){
    if(play){
        if (key == 'a'){
            keyPress[1] = 0;
        }
        if (key == 'd'){
            keyPress[0] = 0;
        }
        if(key == 's')
            keyPress[2] = 0;

        glutPostRedisplay();
    }
}

void kf(unsigned char key, int x, int y){
    if(play){
        if (key == 'a'){
            if(pos<1.7)
                //pos += step;
                keyPress[1] = 1;
        }
        if (key == 'd'){
            if(pos>-1.7)
                //pos -= step;
                keyPress[0] = 1;
        }
        if(key == 's')
            keyPress[2] = 1;

        glutPostRedisplay();
    }
    /*if(key == 'a'){
        keyPress[2] = 1;
        bullets.push_back(make_pair((GLfloat)(pos), -2.4));
    }*/
}

void init(){ // Called before main loop to set up the program
    glClearColor(0.0, 0.08, 0.42, 0.5);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display(){
    //checkRedundant(cf);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    GLuint texture = LoadTextureRAW("star.bmp", true);
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture );

    glPushMatrix();
    glBegin( GL_QUADS );
    glTexCoord2d(0.0,0.0); glVertex3f(-3.0,-3.5, -11);
    glTexCoord2d(1.0,0.0); glVertex3f(+3.0,-3.5, -11);
    glTexCoord2d(1.0,1.0); glVertex3f(+3.0,+3.5, -11);
    glTexCoord2d(0.0,1.0); glVertex3f(-3.0,+3.5, -11);
    glEnd();
    glPopMatrix();
    FreeTexture( texture );

    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f);
        //Quad 1
            glVertex3f(pos-2*r, -2.6+r, -10);//a
            glVertex3f(pos-r, -2.6+r,-10);//b
            glVertex3f(pos-r, -2.6-r, -10);//c
            glVertex3f(pos-2*r, -2.6-r,-10);//d
        //Quad 2
            glVertex3f(pos+r, -2.6+r, -10);//g
            glVertex3f(pos+2*r, -2.6+r, -10);//h
            glVertex3f(pos+2*r, -2.6-r, -10);//i
            glVertex3f(pos+r, -2.6-r, -10);//j
        //Quad 3
            glVertex3f(pos-r, -2.6, -10);//e
            glVertex3f(pos+r, -2.6, -10);//f
            glVertex3f(pos+r, -2.6-r, -10);//j
            glVertex3f(pos-r, -2.6-r, -10);//c
    glEnd();

    if(play){
        if(detectCollision(cf))
            play = true;
        if(dead()){
            cout<<"Game over\n";
            play=false;
        }
    }
    if(keyPress[0]&&pos<=1.6)
        pos += step;
    if(keyPress[1]&&pos>=-1.6)
        pos -= step;
    if(keyPress[2]){
        bullets.push_back(make_pair((GLfloat)(pos), -2.4));
        keyPress[2] = 0;
    }
    //Moving the bullets every frame
    for(int i=0;i<bullets.size();++i){
        //DrawCircle(bullets[i].first, bullets[i].second, 0.1, 0.2);
        glBegin(GL_TRIANGLES);
            glVertex3f(bullets[i].first-r, bullets[i].second-r, -10.0);
            glVertex3f(bullets[i].first, bullets[i].second, -10.0);
            glVertex3f(bullets[i].first+r, bullets[i].second-r, -10.0);
        glEnd();
    }
    //outputText(0, 0, 0, 0, 0, "Hello world");
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
    srand(time(NULL));
    glutInit(&argc, argv); // Initializes glut
    // Sets up a double buffer with RGBA components and a depth component
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    // Sets the window size to 512*512 square pixels
    glutInitWindowSize(512, 1024);
    // Sets the window position to the upper left
    glutInitWindowPosition(0, 0);
    // Creates a window using internal glut functionality
    glutCreateWindow("HyperSpace");


    //cf = newFormation();
    genAliens(cf);
    // passes reshape and display functions to the OpenGL machine for callback
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    //glutSpecialFunc(specialKeys);
    glutKeyboardFunc(kf);
    glutKeyboardUpFunc(kfu);
    init();

    // Starts the program.
    glutMainLoop();
    return 0;
}
