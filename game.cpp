#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <math.h>
#include <map>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <iostream>

using namespace std;

/*
    ALIEN FORMATIONS
    1 -> simple horizontal
    2 -> 2 gorups, each coming from either side of the screen
    3 -> dense horizontal from top
    4 -> 2 dense horizontal groups from either side of the screen
*/

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

void FreeTexture( GLuint texture )
{

  glDeleteTextures( 1, &texture );

}

/*void SetupRC(void){
    GLint textureName;
    // some init gl code here

    // the texture (2x2)
    GLbyte textureData[] = { 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 0 };
    GLsizei width = 2;
    GLsizei heigth = 2;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, &textureName);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid*)textureData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTE R,GL_LINEAR);
}


void texturize(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window with current clearing color
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING); // Draw plane that the cube rests on
    glEnable(GL_TEXTURE_2D); // should use shader, but for an example fixed pipeline is ok
    glBindTexture(GL_TEXTURE_2D, m_texture[0]);
    glBegin(GL_TRIANGLE_STRIP); // draw something with the texture on
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-1.0, -1.0);

    glTexCoord2f(1.0, 0.0);
    glVertex2f(1.0, -1.0);

    glTexCoord2f(0.0, 1.0);
    glVertex2f(-1.0, 1.0);

    glTexCoord2f(1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glEnd();

    glPopMatrix();

    glutSwapBuffers();
}*/

vector<pair<GLfloat, GLfloat> > bullets, aliens1, aliens2, aliens3, aliens4;
pair<GLfloat, GLfloat> alienUpdate;
map<GLfloat, pair<GLfloat, int> > m;
GLfloat r = 0.15, pxl = -0.15, pxr = 0.15, pos = 0;
bool play = true, firstHit = false;
int level = 0, cf = 1;

void checkRedundant(){

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
    return make_pair(p.first, p.second-=0.005);
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
        for(int i=0;i<aliens1.size();i++){
            DrawStarFilled(aliens1[i].first, aliens1[i].second, 0.1, 5);
        }
        transform(begin(aliens1), end(aliens1), begin(aliens1), incAlien);
    }
}


bool aliensGone(int formation){
    int a[4];
    a[0] = aliens1.size();
    a[1] = aliens2.size();
    a[2] = aliens3.size();
    a[3] = aliens4.size();
    //cout<<a[0]<<endl;
    if(a[formation-1]==0){
        cout<<"it is 0\n";
        return true;
    }
    return false;
}

//temporary
int newFormation(){
    return 1;
}

bool detectCollision(int formation){
    m.clear();
    for(int i=0;i<aliens1.size();++i)
        m[aliens1[i].second] = make_pair(aliens1[i].first, i);

    /*for(int i=0;i<bullets.size();++i){
        //cout<<"main loop\n";
        if(m.find(bullets[i].second)!=m.end()){
            cout<<"something\n";
            if(m[bullets[i].second+r].first<=bullets[i].first+r &&
               m[bullets[i].second+r].first>=bullets[i].first-r){
                    aliens1.erase(aliens1.begin()+m[bullets[i].second].second);
                    bullets.erase(bullets.begin()+i);
                    cout<<"collision\n";
                    return true;
            }
        }
    }*/
    for(int i=0;i<bullets.size();++i){
        for(int j=0;j<aliens1.size();++j){
            if(fabs((float)(aliens1[j].second-bullets[i].second))<=0.1){
                //cout<<"something\n";
                if(fabs((float)(aliens1[j].first-bullets[i].first))<=r){
                    aliens1.erase(aliens1.begin()+j);
                    bullets.erase(bullets.begin()+i);
                    firstHit = true;
                    //cout<<"collision "<<aliens1.size()<<"\n";
                    return true;
                }
            }
        }
    }
    if(firstHit)
        if(aliensGone(cf)){
            cout<<"gone\n";
            cf = 1;
            genAliens(cf);
            firstHit = false;
            //cout<<cf<<endl;
        }
    //if(firstHit)
    //    cout<<"something is wrong\n";
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
    aliens1.clear();
    aliens2.clear();
    aliens3.clear();
    aliens4.clear();
    if(formation==1){
        for(GLfloat i=-1;i<=1;i+=0.4){
            //cout<<i<<endl;
            aliens1.push_back(make_pair(i, 2));
        }
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

void specialKeys( int key, int x, int y ){
    const float step = 0.1;
    if(play){
        if (key == GLUT_KEY_RIGHT){
            //pxl += step;
            //pxr += step;
            pos += step;
        }
        else if (key == GLUT_KEY_LEFT){
            //pxl -= step;
            //pxr -= step;
            pos -= step;
        }
        else if(key == 32){
            bullets.push_back(make_pair((GLfloat)(pos), -2.4));
        }
        else if(key==GLUT_KEY_UP)
            displayBullets();
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
        if(detectCollision(1))
            play = true;
        if(dead()){
            cout<<"Game over\n";
            play=false;
        }
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
    glutSpecialFunc(specialKeys);
    init();

    // Starts the program.
    glutMainLoop();
    return 0;
}

//Redundant for now
//Do not touch this part
/*
GLuint LoadTexture(const char *filename){
    GLuint texture;
    int width, height;
    unsigned char *data;
    FILE *file;
    file = fopen(filename, "rb");

    if(file==NULL)
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
    free(data);

    return texture;
}
*/