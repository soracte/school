//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
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

   Vector Normalize() { return *this * (1 / Length()); }
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

inline float sq(float x) { return x * x; }

//--------------------------------------------------------
// Kepernyo 
//--------------------------------------------------------
struct Screen {
  static const int XM = 600;
  static const int YM = 600;
};

//--------------------------------------------------------
// Szinek 
//--------------------------------------------------------
const Color WHITE = Color(1.0f, 1.0f, 1.0f);
const Color BLACK = Color(0.0f, 0.0f, 0.0f);

//--------------------------------------------------------
// Fenyforras 
//--------------------------------------------------------
struct Light {
  float ka;
  Vector p;
  Color lout;

  Light() {}
  Light(float ka, Vector p, Color lout) : ka(ka), p(p), lout(lout) { }

  Vector getLightDir(Vector from) {
    return (p - from).Normalize();
  }

  virtual Color getInRad(Vector from) = 0;

  float getDistance(Vector from) {
    return (p - from).Length();
  }
};

struct AmbientLight : Light {
  AmbientLight() {}
  AmbientLight(float ka, Color lout) : Light(ka, Vector(), lout) {}

  Color getInRad(Vector from) {
    return lout * ka;
  }
};

struct PointLight : Light {
  PointLight() {}
  PointLight(float ka, Vector p, Color lout) : Light(ka, p, lout) {}
  Color getInRad(Vector from) {
    return lout * (1 / sq(getDistance(from))); 
  }
};



//--------------------------------------------------------
// Anyag 
//--------------------------------------------------------
struct Material {
  Color F0, n, kd, ks;
  float shininess;

  virtual bool isReflective() = 0;
  virtual Vector reflect(Vector inDir, Vector n) = 0;
  virtual Color shade(Vector n, Vector viewDir, Vector l, Color inRad) = 0;
  virtual Color fresnel(Vector inDir, Vector n) = 0;
};

struct SmoothMaterial : Material {
  bool isReflective() {
    return true;
  }
  
  Vector reflect(Vector inDir, Vector n) {
   return inDir - n * (n * inDir) * 2.0f;
  }

  Color shade(Vector n, Vector viewDir, Vector l, Color inRad) {
    return Color();
  }

  Color fresnel(Vector inDir, Vector n) {
    float cosa = fabs(n * inDir);
    return F0 + (Color(1.0f, 1.0f, 1.0f) + (F0 * -1.0f)) * pow(1 - cosa, 5);
  }
};

struct RoughMaterial : Material {
  bool isReflective() {
    return false;
  }

  Vector reflect(Vector inDir, Vector n) {
    return Vector();
  }

  Color shade(Vector n, Vector viewDir, Vector l, Color inRad) {
    Color reflRad;

    float cosTheta = n * l;
    if (cosTheta < 0) {
      return reflRad;
    }

    reflRad = inRad * kd * cosTheta;
    Vector h = (viewDir + l).Normalize();
    float cosDelta = n * h;
    if (cosDelta < 0) {
      return reflRad;
    }

    return reflRad + inRad * ks * pow(cosDelta, shininess);
  }

  Color fresnel(Vector inDir, Vector n) {
    return Color();
  }
};

//--------------------------------------------------------
// Metszespont 
//--------------------------------------------------------
struct Hit {
  float t;
  Vector pos;
  Vector n;
  Material* material;
  Hit () { t = -1; }
};


//--------------------------------------------------------
// Sugar 
//--------------------------------------------------------
struct Ray {
  Vector origin, dir;

  Ray(Vector origin, Vector dir) : origin(origin), dir(dir) { }
};

//--------------------------------------------------------
// Objektum 
//--------------------------------------------------------
struct Intersectable {
  Material* material;
  virtual Hit intersect(const Ray& ray) = 0;
};


//--------------------------------------------------------
// Sik 
//--------------------------------------------------------
struct Plane : Intersectable {
  Material* material;
  Hit intersect(const Ray& ray) {
    // TODO implement
    return Hit();
  } 
};


//--------------------------------------------------------
// Kamera 
//--------------------------------------------------------
struct Camera {
  Vector lookat, ahead, right, up, eye;

  Camera() {}
  Camera(Vector eye, Vector lookat, Vector up) {
    ahead = lookat - eye;
    right = (up % ahead).Normalize();
  }

  Ray GetRay(float x, float y) {
    Vector raydir = ahead + 
      right * (2 * x / Screen::XM- 1) +
      up * (2 * y / Screen::YM - 1);
    return Ray(eye, raydir.Normalize());
  }
};


//--------------------------------------------------------
// Jelenet 
//--------------------------------------------------------
struct Scene {
  Intersectable* objects[10];
  Camera cam;
  AmbientLight amLight;
  PointLight pointLight;
  int objCount;

  Scene() {
    cam = Camera(Vector(0.0f, 0.0f, -1.0f), Vector(0.0f, 0.0f, 0.0f), 
        Vector(0.0f, 1.0f, 0.0f));
    amLight = AmbientLight(1.0f, WHITE);
    pointLight = PointLight(1.0f, Vector(0.5f, -0.5f, 0.8f), WHITE);
    objCount = 0;
  }

  void build() {
    // TODO add objects
  }

  Hit firstIntersect(Ray ray) {
    Hit bestHit;

    for (int i = 0; i < objCount; i++) {
      Intersectable* obj = objects[i];
      Hit hit = obj->intersect(ray);
      if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t)) {
        bestHit = hit;
      }
    }

    return bestHit;
  }

  Color trace(Ray ray) {
    Hit hit = firstIntersect(ray);
    if (hit.t < 0) {
      return amLight.getInRad(Vector());
    }

    Vector n = hit.n;
    Vector v = (ray.dir * -1).Normalize();
    Vector l = pointLight.getLightDir(hit.pos);
    Color inRad = pointLight.getInRad(hit.pos);
    Color outRadiance = hit.material->shade(n, v, l, inRad);

    if (hit.material->isReflective()) {
      Vector inDir = v * -1.0;
      Vector reflectionDir = hit.material->reflect(inDir, n);
      Ray reflectedRay(hit.pos, reflectionDir);
      outRadiance = outRadiance +
        trace(reflectedRay) * hit.material->fresnel(inDir, n);
    }

    return outRadiance;
  }
};

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glViewport(0, 0, Screen::XM, Screen::YM);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

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
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
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

