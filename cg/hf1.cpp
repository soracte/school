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

int determineQuarter(Vector v) {
    if (v.x > 300) {
        if (v.y > 300) {
            return 4;
        }

        return 1;
    }

    return v.y > 300 ? 3 : 2;
}

Vector conv(Vector v, int quarter) { 
    float a = (v.x - 300) / 300;
    float b = (v.y - 300) / -300;
    Vector base;
    Vector shift; 

    switch (quarter) {
        case 2:
            shift = Vector(0.5, -0.5, 0);
            base = Vector(a, b);
            break;
        case 3:
            shift = Vector(0.5, 0, 0.5);
            base = Vector(a, 0, b);
            break;
        case 4:
            shift = Vector(0, -0.5, 0.5); 
            base = Vector(0, a, b); 
            break;
    }

    return (base + shift) * 2;        
}

float computeTime(Vector pi, Vector vi, Vector pi1, Vector vi1) {
    // A Simpson-formula az ivhossz kozelitesehez, ami alapjan a kepletet
    // keszitettem, innen szarmazik (344. oldal):
    // https://books.google.com/books?id=HurESoDQljcC&pg=PA344

    float a = -1.5 * (pi.x - pi1.x);
    float b = -0.25 * vi1.x - 0.25 * vi.x;
    float c = -1.5 * (pi.y - pi1.y);
    float d = -0.25 * vi1.y - 0.25 * vi.y;
    float e = -1.5 * (pi.z - pi1.z);
    float f = -0.25 * vi1.z - 0.25 * vi.z;


    float disc = sqr(2 * (a * b + c * d + e * f)) - 4 * (sqr(b) + sqr(d) + sqr(f)- 1) * (sqr(a) + sqr(c) + sqr(e));

    float ret1 = ((-2 * (a * b  + c * d + e * f)) - sqrt(disc)) / (2 * (sqr(b) + sqr(d) + sqr(f) - 1));
    float ret2 = ((-2 * (a * b  + c * d + e * f)) + sqrt(disc)) / (2 * (sqr(b) + sqr(d) + sqr(f) - 1));

    return ret1 > ret2 ? ret1 : ret2;
}

Vector p[100];
Vector v[100];
float t[100];
int pCount;
long currentTime = 0;
bool markerVisible = false;
Vector markerPositions[3];
Vector actualMarker;
Vector actualMarkerVelocity;
int markerSection = 0;
float markerTime;
Vector markerVelocities[3];

void recalcV() {
    v[0] = Vector();
    v[1] = (p[2] - p[1]).Normalize();
    v[pCount - 2] = (p[pCount - 1] - p[pCount - 2]).Normalize();

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

enum View { TOP, FRONT, RIGHT };

Vector findVectorToDraw(Vector r, View view) {
    Vector base;
    Vector shift;

    switch (view) {
        case TOP:
            base = Vector(r.x, r.y);
            shift = Vector(-0.5, 0.5);
            break;
        case FRONT:
            base = Vector(r.x, r.z);
            shift = Vector(-0.5, -0.5);
            break;
        case RIGHT:
            base = Vector(r.y, r.z);
            shift = Vector(0.5, -0.5);
            break;
    }

    return base * 0.5 + shift;
}


bool shouldDrawVector(Vector point, View view) {
    switch (view) {
        case TOP:
            return point.x < 0 && point.y > 0;
        case FRONT:
            return point.x < 0 && point.y < 0;
        case RIGHT:
            return point.x > 0 && point.y < 0;
    }

    return false;
} 

void drawPointAtPosition(Vector p) {
    const float R = 0.02;
    const float ITER = 20;

    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < ITER; i++) {
        glVertex2f(p.x, p.y);     
        for (float deg = 0.0f; deg <= 2*M_PI; deg += 2 * M_PI / ITER) {
            glVertex2f(R * cos(deg) + p.x, R * sin(deg) + p.y);
        }

        glVertex2f(p.x + R, p.y);     
    }
    glEnd();

}

void drawPoints() {
    for (int i = 0; i < pCount; i++) {
        drawPointAtPosition(findVectorToDraw(p[i], TOP));
        drawPointAtPosition(findVectorToDraw(p[i], FRONT));
        drawPointAtPosition(findVectorToDraw(p[i], RIGHT));
    }
} 

Vector coeffs[4];

void recalcCoeffs(int i) {
    float dt = t[i + 1] - t[i];

    coeffs[0] = p[i];
    coeffs[1] = v[i];
    coeffs[2] = ((p[i + 1]-p[i])*3) * (1/sqr(dt)) - ((v[i + 1]+v[i]*2)*(1/dt));
    coeffs[3] = ((p[i]-p[i + 1])*2) * (1/cub(dt)) + ((v[i + 1]+v[i])*(1/sqr(dt)));
}

Vector findRawPositionVector(int i, float progressInSection) {
    recalcCoeffs(i);
    Vector d = coeffs[0];
    Vector c = coeffs[1];
    Vector b = coeffs[2];
    Vector a = coeffs[3];

    return a * cub(progressInSection) + b * sqr(progressInSection) + c * progressInSection + d;
}

Vector findPositionVector(int i, float progressInSection, View view) {
    Vector r = findRawPositionVector(i, progressInSection);
    return findVectorToDraw(r, view);
}

Vector findVelocity(float p) {
    Vector c = coeffs[1];
    Vector b = coeffs[2];
    Vector a = coeffs[3];


    Vector vel = a * sqr(p) * 3 + b * p * 2 + c;
//    return findVectorToDraw(vel, view);
      return vel * 0.5;
}


void drawCurve(View view) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < pCount - 1; i++) {
        for (double tt = t[i]; tt < t[i + 1]; tt += 0.02) {
            float progressInSection = tt - t[i];
            Vector point = findPositionVector(i, progressInSection, view);

            if (shouldDrawVector(point, view)) {
                glVertex2f(point.x, point.y); 
            }
        }
    }
    glEnd();
}

const Color GREY = Color(0.3f, 0.3f, 0.3f);
const Color WHITE = Color(1.0f, 1.0f, 1.0f);
const Color YELLOW = Color(1.0f, 1.0f, 0.0f);
const Color GREEN = Color(0.0f, 1.0f, 0.0f);
const Color TAN = Color(0.9f, 0.6f, 0.5f);

void changeColor(Color color) {
    glColor3f(color.r, color.g, color.b);
}

int findClosestIndex(Vector point) {
    if (pCount == 0) {
        return 0;
    }

    float minsofar = (point - p[0]).Length();
    int minindex = 0;
    float distance;

    for (int i = 1; i < pCount; i++) {
        distance = (point - p[i]).Length();
        if (distance < minsofar) {
            minsofar = distance;
            minindex = i;
        }
    }

    return minindex;
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa

    changeColor(GREY);
    drawSeparators();

    changeColor(YELLOW);
    drawPoints();

    recalcV();
    recalcT();

    changeColor(WHITE);
    drawCurve(TOP);
    drawCurve(FRONT);
    drawCurve(RIGHT);

    if (markerVisible) {
        changeColor(GREEN);
        for (int i = 0; i < 3 ; i++) {
            drawPointAtPosition(markerPositions[i]);

            changeColor(TAN);
            glBegin(GL_LINES);
            glVertex2f(markerPositions[i].x, markerPositions[i].y);
            glVertex2f(markerVelocities[i].x, markerVelocities[i].y);
            glEnd();
        }
    }

    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
    if (key == ' ') {
        markerVisible = !markerVisible;
        markerSection = 0;
        markerTime = 0;
    }

}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {

}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP

        Vector v(x, y);
        int quarter = determineQuarter(v);
        if (quarter != 1) {
            Vector actual = conv(v, quarter);
            if (quarter == 2) {
                p[pCount++] = actual;
            } else {
                int closestIndex = findClosestIndex(actual);
                p[closestIndex] = actual;
            }
        }

        glutPostRedisplay(); 						 // Ilyenkor rajzold ujra a kepet
    }
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

void refreshActualMarker() {
    actualMarker = findRawPositionVector(markerSection, markerTime - t[markerSection]);
}

void refreshActualMarkerVelocity() {
    actualMarkerVelocity = findVelocity(markerTime - t[markerSection]);
}

void setMarkerPosition(View view) {
    markerPositions[view] = findVectorToDraw(actualMarker, view); 
}

void setMarkerVelocities(View view) {
    markerVelocities[view] = findVectorToDraw(actualMarker - actualMarkerVelocity, view);
}

void simulateWorld(long tstart, long tend) {
    for (long te = tstart; te < tend; te += 1) {
        markerTime += 0.001;
        if (t[markerSection + 1] < markerTime) {
            markerSection++;
        }

        if (markerSection == pCount) {
            markerSection = 0;
            markerTime = 0;
        }

        refreshActualMarker();

        setMarkerPosition(TOP);
        setMarkerPosition(FRONT);
        setMarkerPosition(RIGHT);

        refreshActualMarkerVelocity();

        setMarkerVelocities(TOP);
        setMarkerVelocities(FRONT);
        setMarkerVelocities(RIGHT);
    }

    glutPostRedisplay();
}



// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    long oldTime = currentTime;
    currentTime = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

    if (markerVisible) {
        simulateWorld(oldTime, currentTime);
    }
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

