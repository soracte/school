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

#include <iostream>
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

const float EPSILON = 1e-5;

inline float sq(float x) { return x * x; }
inline bool aeq(float x, float y) { return fabs(x - y) < EPSILON; }

struct QuadraticEquation {
  float a, b, c;
  float disc;

  QuadraticEquation(float a, float b, float c) : 
    a(a), b(b), c(c), disc(sq(b) - 4 * a* c) { }

  int numOfRoots() {
    if (aeq(disc, 0.0f)) {
      return 1;
    }

    if (disc < 0.0f) {
      return 0;
    }

    return 2;
  }

  float getMinSolution() {
    if (numOfRoots() == 1) {
      return -b / (2 * a);
    }

    float first = (-b + sqrt(disc)) / (2 * a);
    float second = (-b - sqrt(disc)) / (2 * a);

    return first < second ? first : second;
  }

};

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

  virtual ~Material() { }

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
  Hit (float t, Vector pos, Vector n, Material* material) :
    t(t), pos(pos), n(n), material(material) { }
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

  Intersectable(Material* material) : material(material) {}
  virtual ~Intersectable() {}

  virtual Hit intersect(const Ray& ray) = 0;
};

struct Sphere : Intersectable {
  float r;
  Vector c;

  Sphere(Material* material, float r, Vector c) :
    Intersectable(material), r(r), c(c) { }

  Hit intersect(const Ray& ray) {
    Vector v = ray.dir;
    Vector eye = ray.origin;
    float c2  = v * v;
    float c1 = 2 * ((eye - c) * v);
    float c0 = ((eye - c)*(eye-c)) - sq(r);

    QuadraticEquation eqn(c2, c1, c0);
    int nr = eqn.numOfRoots();
    if (nr == 0) {
      return Hit();
    }
    float t = eqn.getMinSolution();
    Vector pos = eye + v * t;
    Vector n = pos - c;

    return Hit(t, pos, n.Normalize(), material);
  }
};

struct Triangle : Intersectable {
  Vector a, b, c;
  bool inverse;

  Triangle(Material* material, Vector a, Vector b, Vector c, bool inverse) :
    Intersectable(material), a(a), b(b), c(c), inverse(inverse) { }

  Hit intersect(const Ray& ray) {
    if (aeq(ray.dir.x, 0.1f) && aeq(ray.dir.y, 0.4f)) {
      std::cout << "reached" << std::endl;
    }

    Vector eye = ray.origin;
    Vector v = ray.dir;
    Vector n = ((b - a) % (c - a)).Normalize();

    if (inverse) {
      n = n * -1;
    }

    float t = ((a - eye) * n) / (v * n);
    if (t < 0.0f) {
      return Hit();
    }

    Vector intersection = eye + v * t;
    if (!isIntersectionInside(intersection, n)) {
      return Hit(); 
    }

    return Hit(t, intersection, n, material);
  }

  bool isIntersectionInside(Vector p, Vector n) {
    if (inverse) {
      n = n * -1;
    }

    return ((b - a) % (p - a)) * n > 0.0f &&
      ((c - b) % (p - b)) * n > 0.0f &&
      ((a - c) % (p - c)) * n > 0.0f;
  }
};




struct Mesh : Intersectable {
  Vector vertices[500];
  int vertCount;

  Mesh(Material* material) : Intersectable(material), vertCount(0) { }

  Hit intersect(const Ray& ray) {
    bool inverse = false;
    for (int i = 0; i < vertCount; i++) {
      Triangle triangle(material, vertices[i],
          vertices[(i + 1) % vertCount], vertices[(i + 2) % vertCount],
          inverse);

      Hit triangleHit = triangle.intersect(ray);
      if (triangleHit.t >= 0.0f) {
        return triangleHit;
      }

      inverse = !inverse;
    }

    return Hit();
  }

  //TODO DEBUG
//void draw() {
//  for (int i = 0; i < 10; i++) {
//    Triangle triangle(material, vertices[i],
//        vertices[(i + 1) % vertCount],   
//        vertices[(i + 2) % vertCount]);

//    std::cout << "triangle #" << i << std::endl;

//    glBegin(GL_LINE_STRIP); {
//      Vector a = triangle.a;
//      Vector b = triangle.b;
//      Vector c = triangle.c;
//  std::cout << a.x << ";" << a.y << ";" << a.z << 
//       std::endl; 
//  std::cout << b.x << ";" << b.y << ";" << b.z << 
//       std::endl; 
//  std::cout << c.x << ";" << c.y << ";" << c.z << 
//       std::endl; 

//      glVertex3f(a.x, a.y, a.z);
//      glVertex3f(b.x, b.y, b.z);
//      glVertex3f(c.x, c.y, c.z);
//      }
//    std::cout << std::endl;
//    glEnd();
//    }


//}



  void addVertex(Vector vertex) {
    vertices[vertCount++] = vertex;
  }

};

struct Torus : Intersectable {
  Vector c;
  float inR, bigR;
  int inRes, bigRes;
  Vector vertices[500];
  int vertCount;

  Torus(Material* material, Vector c,
      float inR, float bigR, int inRes, int bigRes) :
    Intersectable(material), inR(inR), bigR(bigR),
    inRes(inRes), bigRes(bigRes), vertCount(0) { 

      float inStep = 2 * M_PI / (float)inRes;
      float bigStep = 2 * M_PI / (float)bigRes; 

      for (int i = 0; i < bigRes; i++) {
        for (int j = 0; j < inRes; j++) {
          float phi = i * bigStep;
          float theta = j * inStep;

          float x = (bigR + inR * cosf(theta)) * cosf(phi);
          float y = (bigR + inR * cosf(theta)) * sinf(phi);
          float z = inR * sinf (theta);

          vertices[vertCount++] = Vector(x, y, z) + c;
        }
      } 
    }

  Hit intersect(const Ray& ray) { return Hit(); }

  Intersectable* toMesh() {
    Mesh* mesh = new Mesh(material);

    for (int i = 0; i < bigRes; i++) {
      for (int j = 0; j <= inRes; j++) {
        mesh->addVertex(vertices[i * inRes + j % inRes]);
        mesh->addVertex(vertices[(i + 1) % bigRes * inRes + j % inRes]);
      }
    } 

    return mesh;
  }
};


//--------------------------------------------------------
// Sik 
//--------------------------------------------------------
struct Plane : Intersectable {
  Hit intersect(const Ray& ray) {
    // TODO implement
    return Hit();
  } 
};


//--------------------------------------------------------
// Kamera 
//--------------------------------------------------------
struct Camera {
  Vector eye, lookat, up, ahead, right;

  Camera() {}
  Camera(Vector eye, Vector lookat, Vector up) :
    eye(eye), lookat(lookat), up(up) {
      ahead = lookat - eye;
      right = (ahead % up).Normalize();
    }

  Ray getRay(float x, float y) {
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
  Material* goldMaterial;
  Material* redMaterial; 
  Camera cam;
  AmbientLight amLight;
  PointLight pointLight;
  int objCount;

  Scene() {
    cam = Camera(Vector(0.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f), 
        Vector(0.0f, 1.0f, 0.0f));
    amLight = AmbientLight(1.0f, Color(0.1f, 0.1f, 0.1f));
    pointLight = PointLight(1.0f, Vector(0.0f, 0.0f, 0.2f), WHITE);
    objCount = 0;
  }

  ~Scene() {
    for (int i = 0; i < objCount; i++) {
      delete objects[i];
    }

    //delete goldMaterial;
    delete redMaterial;
  }

  void build() {
    redMaterial = new RoughMaterial();
    redMaterial->kd = Color(1.0f, 0.0f, 0.0f);
    redMaterial->ks = Color(1.0f, 0.0f, 0.0f);
    redMaterial->shininess = 50;
    Torus torus = Torus(redMaterial, Vector(0.0f, 0.0f, 0.0f),
        0.2f, 0.6f, 6, 4);
    objects[objCount++] = torus.toMesh();
//    ((Mesh*)torus.toMesh())->draw();
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
    Color outRadiance = amLight.getInRad(Vector());
    if (hit.t < 0) {
      return outRadiance;
    }

    Vector n = hit.n;
    Vector v = (ray.dir * -1).Normalize();
    Vector l = pointLight.getLightDir(hit.pos);
    Color inRad = pointLight.getInRad(hit.pos);
    outRadiance = outRadiance + hit.material->shade(n, v, l, inRad);

    if (hit.material->isReflective()) {
      Vector inDir = v * -1.0;
      Vector reflectionDir = hit.material->reflect(inDir, n);
      Ray reflectedRay(hit.pos, reflectionDir);
      outRadiance = outRadiance +
        trace(reflectedRay) * hit.material->fresnel(inDir, n);
    }

    return outRadiance;
  }

  void render() {
    Color image[Screen::XM*Screen::YM];
    for (int y = 0; y < Screen::YM; y++) {
      for (int x = 0; x < Screen::XM; x++) {
        if (x == 320 && y == 500) {
          std::cout << "now";
        }
        Ray ray = cam.getRay(x, y);
        image[y * Screen::XM + x] = trace(ray);
      }
    }

    glDrawPixels(Screen::XM, Screen::YM, GL_RGB, GL_FLOAT, image);
  }


};


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
  glViewport(0, 0, Screen::XM, Screen::YM);
}

bool run = false;

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// torlesi szin beallitasa
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

  if (run) {
    return;
  }

  Scene scene;
  scene.build();
  scene.render();

  glutSwapBuffers();     				// Buffercsere: rajzolas vege
  run = true;
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
  {
//    std::cout << "click@(" << (2*x/600.0f-1) << ";" << -(2*y/600.0f-1) 
 //     << ")" << std::endl;
    std::cout << "click@ " << x << ";" << y << std::endl;
    glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
  }
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{

}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
  //long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido

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

