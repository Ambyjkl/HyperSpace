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
#include <string>
#include <utility>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

using namespace std;

const float step = 0.1;
int keyPress[3] = {0, 0, 0}; //left, right, shoot
vector<pair<GLfloat, GLfloat> > alienBullets, bullets, aliens1, aliens21, aliens22, aliens3, aliens4;
map<GLfloat, pair<GLfloat, int> > m;
GLfloat r = 0.15, pxl = -0.15, pxr = 0.15, pos = 0;
bool play = true, firstHit = false;
float multiplier = 0.001;
int level = 0, cf = 2, score = 0, lives = 10;

void showAliens(int);
bool aliensGone(int);
void DrawStarFilled (float cx, float cy, float radius, int numPoints);
void genAliens(int);
pair<GLfloat, GLfloat> inc(pair<GLfloat, GLfloat> p);
pair<GLfloat, GLfloat> incAlien(pair<GLfloat, GLfloat> p);
pair<GLfloat, GLfloat> incAlien21(pair<GLfloat, GLfloat> p);
pair<GLfloat, GLfloat> incAlien22(pair<GLfloat, GLfloat> p);
pair<GLfloat, GLfloat> incAlienBullet(pair<GLfloat, GLfloat> p);
GLuint LoadTextureRAW(const char * filename, bool wrap);
void FreeTexture(GLuint texture);
void checkRedundant(int formation);

pair<GLfloat, GLfloat> inc(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second+=0.1);
}
pair<GLfloat, GLfloat> incAlien(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second-=(0.005+(level*multiplier)));
}
pair<GLfloat, GLfloat> incAlien21(pair<GLfloat, GLfloat> p){
    return make_pair(p.first+=0.005+(level*multiplier), p.second-=(0.005+(level*multiplier)));
}
pair<GLfloat, GLfloat> incAlien22(pair<GLfloat, GLfloat> p){
    return make_pair(p.first-=(0.005+(level*multiplier)), p.second-=(0.005+(level*multiplier)));
}
pair<GLfloat, GLfloat> incAlienBullet(pair<GLfloat, GLfloat> p){
    return make_pair(p.first, p.second-=0.005);
}

GLuint LoadTextureRAW( const char * filename, bool wrap ){
    GLuint texture;
    int width, height;
    unsigned char * data;
    FILE * file;

    file = fopen( filename, "rb" );
    if(file == NULL)
        return 0;

    width = 256;
    height = 256;
    data = (unsigned char *) malloc( width * height * 3 );

    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    wrap ? GL_REPEAT : GL_CLAMP );

    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,
                        height, GL_RGB, GL_UNSIGNED_BYTE, data );

    free(data);

    return texture;
}

void FreeTexture( GLuint texture ){
    glDeleteTextures( 1, &texture );
}

void checkRedundant(int formation){
    switch(formation){
        case 1:
            for(int i=0;i<aliens1.size();++i)
                if(aliens1[i].second<-3){
                    aliens1.erase(aliens1.begin()+i);
                    if(lives>0)
                        lives--;
                }
            break;
        case 2:
            for(int i=0;i<aliens21.size();++i)
                if(aliens21[i].first>2){
                    if(lives>0)
                        lives--;
                    aliens21.erase(aliens21.begin()+i);
                }
            for(int i=0;i<aliens22.size();++i)
                if(aliens22[i].first<-2){
                    if(lives>0)
                        lives--;
                    aliens22.erase(aliens22.begin()+i);
                }
            break;
    }
}

void outputText(float x, float y, string s){
    //glScalef(0.005,0.005,1);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, -10);
    int len = s.length();
    for(int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
}

void outputTextOver(float x, float y, string s){
    //glScalef(0.005,0.005,1);
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos3f(x, y, -10);
    int len = s.length();
    for(int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
}

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
    if(a[formation-1]==0)
        return true;
    return false;
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
                    score++;
                    firstHit = true;
                    return true;
                }
            }
        }
    }
    checkRedundant(cf);
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
    if(formation==1)
        for(GLfloat i=-1;i<=1;i+=0.4)
            aliens1.push_back(make_pair(i, 2));
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

void kfu(unsigned char key, int x, int y){
    if(play){
        if (key == 'a')
            keyPress[1] = 0;
        if (key == 'd')
            keyPress[0] = 0;
        if(key == 's')
            keyPress[2] = 0;
        glutPostRedisplay();
    }
}

void kf(unsigned char key, int x, int y){
    if(play){
        if (key == 'a')
            if(pos<1.7)
                keyPress[1] = 1;
        if (key == 'd')
            if(pos>-1.7)
                keyPress[0] = 1;
        if(key == 's')
            keyPress[2] = 1;

        glutPostRedisplay();
    }
}

void init(){ // Called before main loop to set up the program
    glClearColor(0.0, 0.08, 0.42, 0.5);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

}

void display(){
    //outputText(0, 0, 1, 1, 1, "Hello");
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
    if(!play){
        bullets.clear();
        outputTextOver(-0.5, 0.2, "GAME OVER!");
    }

    if(play){
        if(detectCollision(cf))
            play = true;
        if(dead()){
            cout<<"Game over\n";
            play=false;
        }
        if(lives<=0)
            play = false;
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
        glBegin(GL_TRIANGLES);
            glVertex3f(bullets[i].first-r, bullets[i].second-r, -10.0);
            glVertex3f(bullets[i].first, bullets[i].second, -10.0);
            glVertex3f(bullets[i].first+r, bullets[i].second-r, -10.0);
        glEnd();
    }
    outputText(1.2, 2.5, "Score: "+to_string(score));
    outputText(-1.9, 2.5, "Lives: "+to_string(lives));
    glutSwapBuffers();
}

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
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(512, 1024);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("HyperSpace");

    genAliens(cf);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(kf);
    glutKeyboardUpFunc(kfu);
    init();

    glutMainLoop();
    return 0;
}
