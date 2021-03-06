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

  Color operator/(const Color& c) {
    return Color(r / c.r, g / c.g, b / c.b);
  }

  Color operator+(const Color& c) {
    return Color(r + c.r, g + c.g, b + c.b); 
  }

  Color operator-(const Color& c) {
    return Color(r - c.r, g - c.g, b - c.b);
  }

  Color operator+(float f) {
    return Color(r + f, g + f, b + f);
  }

  Color operator-(float f) {
    return Color(r - f, g - f, b - f);
  }
};

const float EPSILON = 1e-5;
const float RAY_LENGTH = 0.05;
const int TRACE_MAX_DEPTH = 100;
const float G =  6.673e-11;
const float EARTH_M = 5.97219e24;

inline float sq(float x) { return x * x; }
inline Color sq(Color col) { return col * col; }
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
  virtual bool isCheckboard() = 0;
  virtual Vector reflect(Vector inDir, Vector n) = 0;
  virtual Color shade(Vector n, Vector viewDir, Vector l, Color inRad) = 0;
  virtual Color fresnel(Vector inDir, Vector n) = 0;
  virtual bool isBlackHole() = 0;
};

struct SmoothMaterial : Material {
  bool isReflective() {
    return true;
  }

  bool isBlackHole() {
    return false;
  }

  bool isCheckboard() {
    return false;
  }

  Vector reflect(Vector inDir, Vector n) {
    return (inDir - n * (n * inDir) * 2.0f).Normalize();
  }

  Color shade(Vector n, Vector viewDir, Vector l, Color inRad) {
    return Color();
  }

  Color fresnel(Vector inDir, Vector n) {
    float cosa = fabs(n * inDir);
    return F0 + (Color(1.0f, 1.0f, 1.0f) - F0) * pow(1 - cosa, 5);
  }

  Color calcF0(Color n, Color k) {
    return (sq(n - 1.0f) + sq(k)) / (sq(n + 1.0f) + sq(k));
  }
};

struct RoughMaterial : Material {
  bool isReflective() {
    return false;
  }

  Vector reflect(Vector inDir, Vector n) {
    return Vector();
  }

  virtual bool isCheckboard() {
    return false;
  }

  virtual bool isBlackHole() {
    return false;
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

struct CheckboardMaterial : RoughMaterial {
  bool isCheckboard() {
    return true;
  };

  bool isBlackHole() {
    return false;
  }

  Color checkboardShade(Material* whiteMat, Material* blackMat, Vector p,
      Vector n, Vector viewDir, Vector l, Color inRad) {
    int i = p.x * 10;
    int j = p.y * 10;

    if ((i + j) % 2 == 0) {
      return whiteMat->shade(n, viewDir, l, inRad);
    }

    return blackMat->shade(n, viewDir, l, inRad);
  }
};

struct BlackHoleMaterial : RoughMaterial {
  bool isCheckboard() {
    return false;
  }

  bool isBlackHole() {
    return true;
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
  Vector objPos;

  Hit () { t = -1; }
  Hit (float t, Vector pos, Vector n, Material* material) :
    t(t), pos(pos), n(n), material(material) { }

  Hit (float t, Vector pos, Vector n, Material* material, Vector objPos) :
    t(t), pos(pos), n(n), material(material), objPos(objPos) { }
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
  Sphere surroundingSphere;

  Mesh(Material* material, Sphere surroundingSphere) : 
    Intersectable(material),
    vertCount(0),
    surroundingSphere(surroundingSphere) { }

  Hit intersect(const Ray& ray) {
    Vector eye = ray.origin;
    Vector v = ray.dir;
    Vector rayEnd = eye + v * RAY_LENGTH;
    Vector fromCenter = rayEnd - surroundingSphere.c;

    if (fromCenter.Length() >= surroundingSphere.r) {
      return Hit();
    }
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
    Intersectable(material), c(c), inR(inR), bigR(bigR),
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
    Sphere surroundingSphere(material, bigR + inR, c);
    Mesh* mesh = new Mesh(material, surroundingSphere);

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
  Vector p, n;

  Plane(Material* material, Vector p, Vector n) : 
    Intersectable(material), p(p), n(n) { }

  Hit intersect(const Ray& ray) {
    Vector v = ray.dir;
    Vector eye = ray.origin;
    float t = ((p - eye) * n) / (v * n);

    if (t < 0.0f) {
      return Hit();
    }

    Vector pos = eye + v * t;

    return Hit(t, pos, n, material, pos - p);
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
  Material* goldMat;
  Material* redMat; 
  Material* blackMat;
  Material* whiteMat;
  Material* greenMat;
  Material* blueMat;
  Material* checkMat;
  Material* blackHoleMat;
  Vector blackHoleCenter;
  Camera cam;
  AmbientLight amLight;
  PointLight pointLight;
  int objCount;

  Scene() {
    blackHoleCenter = Vector(-0.1f, 0.3f, -0.3f);
    cam = Camera(Vector(0.0f, 0.0f, 1.0f), Vector(0.0f, 0.0f, 0.0f), 
        Vector(0.0f, 1.0f, 0.0f));
    amLight = AmbientLight(1.0f, Color(0.1f, 0.1f, 0.1f));
    pointLight = PointLight(1.0f, Vector(0.2f, 0.4f, 0.5f), WHITE);
    objCount = 0;
  }

  ~Scene() {
    for (int i = 0; i < objCount; i++) {
      delete objects[i];
    }
  
    delete goldMat;
    delete redMat;
    delete greenMat;
    delete blueMat;
    delete blackMat;
    delete whiteMat;
    delete checkMat;
    delete blackHoleMat;
  }

  void addObject(Intersectable* object) {
    objects[objCount++] = object;
  }

  void createMaterials() {

    goldMat = new SmoothMaterial();
    Color goldN(0.17f, 0.35f, 1.5f);
    Color goldK(3.1f, 2.7f, 1.9f);
    goldMat->F0 = ((SmoothMaterial*)goldMat)->calcF0(goldN, goldK);

    redMat = new RoughMaterial();
    redMat->kd = Color(0.8f, 0.1f, 0.1f);
    redMat->ks = Color(0.5f, 0.2f, 0.1f);
    redMat->shininess = 5;

    blackMat = new RoughMaterial();
    blackMat->kd = Color(0.0f, 0.0f, 0.0f);
    blackMat->ks = Color(0.0f, 0.0f, 0.0f);
    blackMat->shininess = 50;

    whiteMat = new RoughMaterial();
    whiteMat->kd = Color(1.0f, 1.0f, 1.0f);
    whiteMat->ks = Color(1.0f, 1.0f, 1.0f);
    whiteMat->shininess = 50;

    greenMat = new RoughMaterial();
    greenMat->kd = Color(0.0f, 1.0f, 0.0f);
    greenMat->ks = Color(0.0f, 1.0f, 0.0f);
    greenMat->shininess = 50;
    
    blueMat = new RoughMaterial();
    blueMat->kd = Color(0.0f, 0.0f, 1.0f);
    blueMat->ks = Color(0.0f, 0.0f, 1.0f);
    blueMat->shininess = 50;

    checkMat = new CheckboardMaterial();
    checkMat->kd = Color(1.0f, 1.0f, 1.0f);
    checkMat->ks = Color(1.0f, 1.0f, 1.0f);
    checkMat->shininess = 20;

    blackHoleMat = new BlackHoleMaterial();
  }

  void addWalls() {
    addObject(new Plane(checkMat,
          Vector(-0.8f, 0.0f, 0.0f),
          Vector(1.0f, 0.0f, 0.0f)));

    addObject(new Plane(checkMat,
          Vector(0.0f, 0.0f, -0.8f),
          Vector(0.0f, 0.0f, 1.0f)));

    addObject(new Plane(checkMat,
          Vector(0.8f, 0.0f, 0.0f),
          Vector(-1.0f, 0.0f, 0.0f)));

    addObject(new Plane(checkMat,
          Vector(0.0f, 0.8f, 0.0f),
          Vector(0.0f, -1.0f, 0.0f)));

    addObject(new Plane(checkMat,
          Vector(-0.8f, -0.8f, 0.0f),
          Vector(0.0f, 1.0f, 0.0f)));
  }

  void addTorus() {
    Torus torus(goldMat, Vector(-0.2f, -0.3f, -0.7f),
        0.1f, 0.4f, 4, 6);

    addObject(torus.toMesh());
  }

  void addBlackHole() {
    addObject(new Sphere(blackHoleMat, 0.1f, blackHoleCenter));
  }

  void build() {
    createMaterials();
    addWalls();
    addTorus();
    addBlackHole();
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

  Vector calcDelta(Vector eye) {
    Vector toBlackHole = blackHoleCenter - eye;
    float distanceFromBlackHole = toBlackHole.Length(); 
    float distanceInM = 0.05 * distanceFromBlackHole;

    Vector blackHoleDir = toBlackHole.Normalize();
    float aMagnitude = (G * EARTH_M / sq(distanceInM));
    aMagnitude *= 1e-18;

    Vector a = blackHoleDir * aMagnitude;
    return Vector(a * RAY_LENGTH);
  }

  Ray getNextRay(Ray currentRay) {
    return Ray(currentRay.origin + currentRay.dir * RAY_LENGTH, 
        currentRay.dir + calcDelta(currentRay.origin));
  }

  Color trace(Ray ray, int depth) {
    Color outRadiance = amLight.getInRad(Vector());

    if (depth > TRACE_MAX_DEPTH) {
      return outRadiance; 
    }

    Hit hit = firstIntersect(ray);


    if (hit.t > 0 && hit.t <= RAY_LENGTH && hit.material->isBlackHole()) {
      return Color();
    };

    if (hit.t < 0 || hit.t >= RAY_LENGTH) {
      return trace(getNextRay(ray), depth + 1);
    }


    Vector n = hit.n;
    Vector v = (ray.dir * -1).Normalize();
    Vector l = pointLight.getLightDir(hit.pos);
    Color inRad = pointLight.getInRad(hit.pos);

    Material* material = hit.material;

    Color matColor;

    if (material->isCheckboard()) {
      Vector planeCoord;
      Vector p = hit.objPos;
      int planeSide;

      if (aeq(p.x, 0)) {
        planeCoord = Vector(p.y, p.z);
        planeSide = 0;
      } else if (aeq(p.y, 0)) {
        planeCoord = Vector(p.x, p.z);
        planeSide = 1;
      } else {
        planeCoord = Vector(p.x, p.y);
        planeSide = 2;
      }

      Material* colorMat;

      switch (planeSide) {
        case 0:
          colorMat = redMat;
          break;
        case 1:
          colorMat = greenMat;
          break;
        case 2:
          colorMat = blueMat;
          break;
      }

      matColor = ((CheckboardMaterial*)material)->checkboardShade(whiteMat,
          colorMat, planeCoord, n, v, l, inRad);
    } else {
      matColor = hit.material->shade(n, v, l, inRad);
    }

    outRadiance = outRadiance + matColor;

    if (hit.material->isReflective()) {
      Vector inDir = v * -1.0;
      Vector reflectionDir = hit.material->reflect(inDir, n);
      Ray reflectedRay(hit.pos + Vector(1e-4f, 1e-4f, 1e-4f),
          reflectionDir.Normalize());
      outRadiance = outRadiance +
        trace(getNextRay(reflectedRay), depth + 1) *
        hit.material->fresnel(inDir, n);
    }

    return outRadiance;
  }

  void render() {
    Color image[Screen::XM*Screen::YM];
    for (int y = 0; y < Screen::YM; y++) {
      for (int x = 0; x < Screen::XM; x ++) {
        Ray ray = cam.getRay(x, y);
        image[y * Screen::XM + x] = trace(ray, 0);
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

