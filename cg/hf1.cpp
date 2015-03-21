//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fĂźggvĂŠnyt kivĂŠve, a lefoglalt adat korrekt felszabadĂ­tĂĄsa nĂŠlkĂźl 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Fulop Daniel	 
// Neptun : PYUVXQ 
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          

#include <iostream>
#include <cstdio>



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------

struct Vector {
   float x, y, z;

   Vector( ) { 
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) { 
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) { 
	return Vector(x * a, y * a, z * a); 
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z); 
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z); 
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z); 
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }

   Vector Normalize() { return *this * (1 / this->Length()); }
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) { 
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) { 
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) { 
	return Color(r * a, g * a, b * a); 
   }
   Color operator*(const Color& c) { 
	return Color(r * c.r, g * c.g, b * c.b); 
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b); 
   }
};

const int screenWidth = 600;	// alkalmazas ablak felbontasa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazas ablaknyi kep


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);

}

void drawSeparators() {
    glBegin(GL_LINE_STRIP);
        glVertex2f(0.0f, 1.0f);
        glVertex2f(0.0f, -1.0f);
    glEnd();

    glBegin(GL_LINE_STRIP);
        glVertex2f(-1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
    glEnd();
}

inline float sqr(float x) { return x * x; }
inline float cub(float x) { return x * x * x; }
inline float convx(float c) { return (c - 300) / 300.f; }
inline float convy(float c) { return (c - 300) / -300.f; }

float computeTime(Vector pi, Vector vi, Vector pi1, Vector vi1) {
    float a = -1.5 * (pi.x - pi1.x);
    float b = -0.25 * vi1.x - 0.25 * vi.x;
    float c = -1.5 * (pi.y - pi1.y);
    float d = -0.25 * vi1.y - 0.25 * vi.y;

    float disc = sqr(2 * (a * b + c * d)) - 4 * (sqr(b) + sqr(d) - 1) * (sqr(a) + sqr(c));

//  std::cout << a << " " << b << " " << c << " " << d << " " << std::endl;
    float ret1 = ((-2 * (a * b  + c * d)) - sqrt(disc)) / (2 * (sqr(b) + sqr(d) - 1));
    float ret2 = ((-2 * (a * b  + c * d)) + sqrt(disc)) / (2 * (sqr(b) + sqr(d) - 1));

    return ret1 > ret2 ? ret1 : ret2;
}

Vector p[100];
Vector v[100];
float t[100];
int pCount;

void recalcV() {
//    v[0] = (p[1] - p[0]).Normalize();
    v[0] = Vector();
    v[1] = (p[2] - p[1]).Normalize();
    v[pCount - 2] = (p[pCount - 1] - p[pCount - 2]).Normalize();

//    v[pCount - 1] = v[pCount - 2]; 
    v[pCount - 1] = Vector();

    for (int i = 2; i < pCount - 2; i++) {
        float a = (p[i - 1] - p[i - 2]).Length();
        float b = (p[i] - p[i - 1]).Length();
        float area1 = 0.5 * a * b;

        a = (p[i + 1] - p[i + 2]).Length();
        b = (p[i + 2] - p[i + 1]).Length();
        float area = 0.5 * a * b;

        v[i] = ((p[i] - p[i - 1]) * area + (p[i + 1] - p[i]) * area1).Normalize();
    }
}

void recalcT() {
    t[0] = 0;

    for (int i = 0; i < pCount - 1; i++) {
        t[i + 1] = t[i] + computeTime(p[i], v[i], p[i + 1], v[i + 1]);
    }
}

void drawPoints() {
       //
} 
   
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// torlesi szin beallitasa
    glColor3f(1.0f, 1.0f, 1.0f);

    //drawSeparators();
    drawPoints();
//  p[0] = Vector(-0.5, 0);
//  p[1] = Vector(-0.25, 0.15);
//  p[2] = Vector(0.1, -0.1);
//  p[3] = Vector(0.35, 0.2);
//  p[4] = Vector(0.8, 0);
//  p[5] = Vector(0.6, 0.5);
//  p[6] = Vector(-0.6, -0.5);
//  p[7] = Vector(-0.8, 0.1);
//  pCount = 8;
    
    recalcV();
    recalcT();

//  p[0] = Vector(-0.25, 0.15);
//  p[1] = Vector(0.1, -0.1);
//  v[0] = Vector(0.813, -0.581);
//  v[1] = Vector(0.9241, 0.3819);
//  pCount = 2;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < pCount - 1; i++) {
        float dt = t[i + 1] - t[i];
//      std::cout << "SECTION " << i << std::endl;
//      std::cout << "tangent vector at beginning: " << v[i].x << ";" << v[i].y << std::endl;
//      std::cout << "dt " << dt << std::endl;
        for (double tt = t[i]; tt < t[i + 1]; tt += 0.02) {
            Vector d = p[i];
            Vector c = v[i];
            Vector b = ((p[i + 1]-p[i])*3) * (1/sqr(dt)) - ((v[i + 1]+v[i]*2)*(1/dt));
            Vector a = ((p[i]-p[i + 1])*2) * (1/cub(dt)) + ((v[i + 1]+v[i])*(1/sqr(dt)));

            float progressInSection = tt - t[i];
            Vector r = a * cub(progressInSection) + b * sqr(progressInSection) + c * progressInSection + d;
            Vector rd = a * 3 * sqr(progressInSection) + b * 2 * progressInSection + c; 
            glVertex2f(r.x, r.y);
//          std::cout << r.x << ";" << r.y << "@" << rd.Length() << std::endl;
        }
    }
    glEnd();

    glColor3f(0, 1, 0);
    for (int i = 0; i < pCount; i++) {
        glBegin(GL_LINE_STRIP);
//      glVertex2f(p[i].x, p[i].y);
//      glVertex2f(p[i].x + v[i].x, p[i].y + v[i].y);
        glEnd();
    }

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay(); 						 // Ilyenkor rajzold ujra a kepet
        p[pCount++] = Vector(convx(x), convy(y));
        std::cout << convx(x) << ";" << convy(y) << std::endl;
	}
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
     long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

