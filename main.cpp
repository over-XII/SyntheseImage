// This code is highly based on smallpt
// http://www.kevinbeason.com/smallpt/
#include <cmath>
#include <algorithm>
#include <cassert>
#include <random>
#include <memory>
#include <fstream>
#include <string>
#include <iostream>

// GLM (vector / matrix)
#define GLM_FORCE_RADIANS

#include <glm/glm/vec4.hpp>
#include <glm/glm/vec3.hpp>
#include <glm/glm/mat4x4.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

const float pi = 3.1415927f;
const float noIntersect = std::numeric_limits<float>::infinity();





bool isIntersect(float t)
{
    return t < noIntersect;
}

struct Ray
{
    const glm::vec3 origin, direction;
};

struct Sphere
{
    const float radius;
    const glm::vec3 center;
};

struct Triangle
{
    const glm::vec3 v0, v1, v2;
};

struct Quadra
{
    const glm::vec3 v0, v1, v2, v3;
};

struct Mesh
{
    const std::vector<glm::vec3> Sommets;
    const std::vector<Triangle> Faces;
    const std::vector<glm::vec3> Normals;
};


//Calcule normal d'un Triangle
glm::vec3 TriangleNormal(Triangle q){
        glm::vec3 v12(q.v1.x-q.v0.x,q.v1.y-q.v0.y,q.v1.z-q.v0.z);
        glm::vec3 v13(q.v2.x-q.v0.x,q.v2.y-q.v0.y,q.v2.z-q.v0.z);

        glm::vec3 normal(0.,0.,0.);
        normal.x = v12.y*v13.z - v12.z*v13.y;
        normal.y = v12.z*v13.x - v12.x*v13.z;
        normal.z = v12.x*v13.y - v12.y*v13.x;

        return glm::normalize(normal);
}


//Calcule Normal d'un Quadra
/*glm::vec3 QuadraNormal(Quadra q){
    glm::vec3 n1 = TriangleNormal(q.v0,q.v1,q.v2);
    glm::vec3 n2 = TriangleNormal(q.v2,q.v3,q.v0);

    return (n1+n2)/2.f;
}*/

std::vector<glm::vec3> Sommets;
std::vector<Triangle> Faces;
std::vector<glm::vec3> Normals;


//****Open a file and save the data****//
void ManageFile(std::string File){

   std::ifstream fichier(File.c_str(),std::ios::in);

   std::string test;

   if(fichier){
       std::cout<<"Ouverture du fichier (lecture) : "<<File.c_str()<<std::endl;

       while (true){
           fichier>>test;
           if(test == "v"){
               glm::vec3 som(0.);
                fichier>>som.x>>som.y>>som.z;
                som += glm::vec3(50,16.5,50);
                if(fichier.good()){
                    Sommets.push_back(som);
                    //std::cout<<"Test : "<<Sommets.back().y<<std::endl;
                }
           }
           if(test == "f"){
                int face[4];
                fichier>>face[0]>>face[1]>>face[2]>>face[3];
                if(fichier.good()){
                    //Sommets.push_back(som);
                    Triangle q = Triangle{Sommets[face[0]-1],Sommets[face[1]-1],Sommets[face[2]-1]};
                    Triangle q2 = Triangle{Sommets[face[2]-1],Sommets[face[3]-1],Sommets[face[0]-1]};
                    Faces.push_back(q);
                    Faces.push_back(q2);
                    Normals.push_back(TriangleNormal(q));
                    Normals.push_back(TriangleNormal(q2));
                    //std::cout<<"Test : "<<Faces.back().v1.x<<std::endl;
                }else{
                    break;
                }
           }
       }

       std::cout<<" Fermeture du fichier (lecture) : "<<File.c_str()<<std::endl;
       fichier.close();

   }else{
       std::cout<<"Erreur d'ouverture du fichier (lecture) : "<<File.c_str()<<std::endl;
   }
}


// WARRING: works only if r.d is normalized
float intersect (const Ray & ray, const Sphere &sphere)
{				// returns distance, 0 if nohit
    glm::vec3 op = sphere.center - ray.origin;		// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    float t, b = glm:: dot(ray.direction, op), det =
        b * b - glm::dot(op, op) + sphere.radius * sphere.radius;
    if (det < 0)
        return noIntersect;
    else
        det = std::sqrt (det);
    return (t = b - det) >= 0 ? t : ((t = b + det) >= 0 ? t : noIntersect);
}

float intersect(const Ray & ray, const Triangle &triangle)
{
    auto e1 = triangle.v1 - triangle.v0;
    auto e2 = triangle.v2 - triangle.v0;

    auto h = glm::cross(ray.direction, e2);
    auto a = glm::dot(e1, h);

    auto f = 1.f / a;
    auto s = ray.origin - triangle.v0;

    auto u = f * glm::dot(s, h);
    auto q = glm::cross(s, e1);
    auto v = f * glm::dot(ray.direction, q);
    auto t = f * glm::dot(e2, q);

    if(std::abs(a) < 0.00001)
        return noIntersect;
    if(u < 0 || u > 1)
        return noIntersect;
    if(v < 0 || (u+v) > 1)
        return noIntersect;
    if(t < 0)
        return noIntersect;

    return t;
}


float intersect(const Ray & ray, const Mesh &mesh){

    for(int i = 0 ; i < mesh.Faces.size();i++){

       intersect(ray,mesh.Faces[i]);

    }

}

//Check if a point in in the Triangle
bool IsIn(Triangle t,glm::vec3 _p){
    /**************************/
    //Calcule
    //(xa-xm)(yb-ym)-(ya-ym)(xb-xm)
    //(xb-xm)(yc-ym)-(yb-ym)(xc-xm)
    //(xc-xm)(ya-ym)-(yc-ym)(xa-xm)
}


/*Normal function*/
glm::vec3 normal(Sphere s,glm::vec3 p){
    return glm::normalize(p-s.center);
}

 glm::vec3 normal(Triangle t,glm::vec3 p){
    return glm::normalize(glm::cross((t.v1-t.v0),(t.v2-t.v1)));
}

 glm::vec3 normal(Mesh m,glm::vec3 p){

     for(int i=0;i<m.Faces.size();i++){

         if(IsIn(m.Faces[i],p)){
             return m.Normals[i];
         }else{
             return glm::vec3(0.,0.,0.);
         }

     }
}


struct Diffuse
{
    const glm::vec3 color;
    glm::vec3 Direct(){


    }
    glm::vec3 Indirect(){


    }
};

struct Glass
{
    const glm::vec3 color;
    glm::vec3 Direct(){


    }
    glm::vec3 Indirect(){


    }
};

struct Mirror
{
    const glm::vec3 color;
    glm::vec3 Direct(){


    }
    glm::vec3 Indirect(){


    }
};

template<typename T>
glm::vec3 albedo(const T &t)
{
    return t.color;
}

struct Object
{
    virtual float intersect(const Ray &r) const = 0;
    virtual glm::vec3 albedo() const = 0;
    virtual glm::vec3 normal(glm::vec3 p)const =0;
};

template<typename P, typename M>
struct ObjectTpl final : Object
{
    ObjectTpl(const P &_p, const M &_m)
        :primitive(_p), material(_m)
    {}

    float intersect(const Ray &ray) const
    {
        return ::intersect(ray, primitive);
    }

    glm::vec3 normal(glm::vec3 p)const{
        return ::normal(primitive,p);
    }

    glm::vec3 albedo() const
    {
        return ::albedo(material);
    }

    const P &primitive;
    const M &material;
};


template<typename P, typename M>
std::unique_ptr<Object> makeObject(const P&p, const M&m)
{
    return std::unique_ptr<Object>(new ObjectTpl<P, M>{p, m});
}


 std::vector<std::unique_ptr<Object>> DrawObj( std::vector<std::unique_ptr<Object>> _ret){

    return _ret;
 }


// Scene
namespace scene
{
    // Primitives

    // Left Wall
    const Triangle leftWallA{{0, 0, 0}, {0, 100, 0}, {0, 0, 150}};
    const Triangle leftWallB{{0, 100, 0},{0, 100, 150},  {0, 0, 150}};

    // Right Wall
    const Triangle rightWallA{{100, 100, 0},{100, 0, 0},  {100, 0, 150}};
    const Triangle rightWallB{{100, 100, 150}, {100, 100, 0}, {100, 0, 150}};

    // Back wall
    const Triangle backWallA{{0, 0, 0}, {100, 0, 0}, {100, 100, 0}};
    const Triangle backWallB{{0, 100, 0},{0, 0, 0},  {100, 100, 0}};

    // Bottom Floor
    const Triangle bottomWallA{{100, 0, 0},{0, 0, 0},  {100, 0, 150}};
    const Triangle bottomWallB{{0, 0, 0}, {0, 0, 150}, {100, 0, 150}};

    // Top Ceiling
    const Triangle topWallA{{0, 100, 0}, {100, 100, 0}, {0, 100, 150}};
    const Triangle topWallB{ {100, 100, 0},{100, 100, 150}, {0, 100, 150}};

    //Mesh
    const Mesh m{Sommets,Faces,Normals};

    const Sphere leftSphere{16.5, glm::vec3 {27, 16.5, 47}};
    const Sphere rightSphere{16.5, glm::vec3 {73, 16.5, 78}};

    const glm::vec3 light{50, 70, 81.6};

    // Materials
    const Diffuse white{{.75, .75, .75}};
    const Diffuse red{{.75, .25, .25}};
    const Diffuse blue{{.25, .25, .75}};

    const Glass glass{{.9, .1, .9}};
    const Mirror mirror{{.9, .9, .1}};

    // Objects
    // Note: this is a rather convoluted way of initialising a vector of unique_ptr ;)
    const std::vector<std::unique_ptr<Object>> objects = [] (){
        std::vector<std::unique_ptr<Object>> ret;
        ret.push_back(makeObject(backWallA, white));
        ret.push_back(makeObject(backWallB, white));
        ret.push_back(makeObject(topWallA, white));
        ret.push_back(makeObject(topWallB, white));
        ret.push_back(makeObject(bottomWallA, white));
        ret.push_back(makeObject(bottomWallB, white));
        ret.push_back(makeObject(rightWallA, blue));
        ret.push_back(makeObject(rightWallB, blue));
        ret.push_back(makeObject(leftWallA, red));
        ret.push_back(makeObject(leftWallB, red));
        ret.push_back(makeObject(m,red));
        ret.push_back(makeObject(leftSphere, mirror));
        ret.push_back(makeObject(rightSphere, glass));

        return ret;
    }();
}

thread_local std::default_random_engine generator;
thread_local std::uniform_real_distribution<float> distribution(0.0,1.0);

float random_u()
{
    return distribution(generator);
}

glm::vec3 sample_cos(const float u, const float v, const glm::vec3 n)
{
    // Ugly: create an ornthogonal base
    glm::vec3 basex, basey, basez;

    basez = n;
    basey = glm::vec3(n.y, n.z, n.x);

    basex = glm::cross(basez, basey);
    basex = glm::normalize(basex);

    basey = glm::cross(basez, basex);

    // cosinus sampling. Pdf = cosinus
    return  basex * (std::cos(2.f * pi * u) * std::sqrt(1.f - v)) +
        basey * (std::sin(2.f * pi * u) * std::sqrt(1.f - v)) +
        basez * std::sqrt(v);
}

int toInt (const float x)
{
    return int (std::pow (glm::clamp (x, 0.f, 1.f), 1.f / 2.2f) * 255 + .5);
}

// WARNING: ASSUME NORMALIZED RAY
// Compute the intersection ray / scene.
// Returns true if intersection
// t is defined as the abscisce along the ray (i.e
//             p = r.o + t * r.d
// id is the id of the intersected object
Object* intersect (const Ray & r, float &t)
{
    t = noIntersect;
    Object *ret = nullptr;

    for(auto &object : scene::objects)
    {
        float d = object->intersect(r);
        if (isIntersect(d) && d < t)
        {
            t = d;
            ret = object.get();
        }
    }

    return ret;
}

// Reflect the ray i along the normal.
// i should be oriented as "leaving the surface"
glm::vec3 reflect(const glm::vec3 i, const glm::vec3 n)
{
    return n * (glm::dot(n, i)) * 2.f - i;
}

float sin2cos (const float x)
{
    return std::sqrt(std::max(0.0f, 1.0f-x*x));
}

// Fresnel coeficient of transmission.
// Normal point outside the surface
// ior is n0 / n1 where n0 is inside and n1 is outside
float fresnelR(const glm::vec3 i, const glm::vec3 n, const float ior)
{
    if(glm::dot(n, i) < 0)
        return fresnelR(i, n * -1.f, 1.f / ior);

    float R0 = (ior - 1.f) / (ior + 1.f);
    R0 *= R0;

    return R0 + (1.f - R0) * std::pow(1.f - glm::dot(i, n), 5.f);
}

// compute refraction vector.
// return true if refraction is possible.
// i and n are normalized
// output wo, the refracted vector (normalized)
// n point oitside the surface.
// ior is n00 / n1 where n0 is inside and n1 is outside
//
// i point outside of the surface
bool refract(glm::vec3 i, glm::vec3 n, float ior, glm::vec3 &wo)
{
    i = i * -1.f;

    if(glm::dot(n, i) > 0)
    {
        n = n * -1.f;
    }
    else
    {
        ior = 1.f / ior;
    }

    float k = 1.f - ior * ior * (1.f - glm::dot(n, i) * glm::dot(n, i));
    if (k < 0.)
        return false;

    wo = i * ior - n * (ior * glm::dot(n, i) + std::sqrt(k));

    return true;
}

glm::vec3 sample_sphere(const float r, const float u, const float v, float &pdf, const glm::vec3 normal)
{
    pdf = 1.f / (pi * r * r);
    glm::vec3 sample_p = sample_cos(u, v, normal);

    float cos = glm::dot(sample_p, normal);

    pdf *= cos;
    return sample_p * r;
}

glm::vec3 radiance (const Ray & r , int recurance )
{

    float t=1.0,eps=0.1;
    glm::vec3 indirect(0.);

    Object *obj= intersect(r,t);
    if(obj){


        glm::vec3 intersection(r.origin+glm::normalize(r.direction)* t);

        float u = random_u();
        float v = random_u();

        if(recurance < 5){
            //To know if it's a mirror or not.
            if(obj->albedo()==scene::mirror.color){
                glm::vec3 intercamera(r.origin-intersection);
                glm::vec3 reflection = glm::normalize(reflect(glm::normalize(intercamera),obj->normal(intersection)));
                glm::vec3 color = radiance(Ray{intersection+(eps*reflection),reflection},recurance+1);
                return color;

            //to know if its a glass
            }else if(obj->albedo() == scene::glass.color){

                glm::vec3 intercamera(r.origin-intersection);
                float ior=1.33;
                float fre = fresnelR(glm::normalize(intercamera),obj->normal(intersection),ior);
                glm::vec3 wo = glm::vec3(0.,0.,0.);

                if(random_u()>fre){

                    if( refract(glm::normalize(intercamera),obj->normal(intersection),ior,wo)){
                        glm::vec3 color = radiance(Ray{intersection+(eps*glm::normalize(wo)),glm::normalize(wo)},recurance+1);
                        return color;
                    }else{
                        return glm::vec3(0.);
                    }
                }else{

                    wo = reflect(glm::normalize(intercamera),obj->normal(intersection));
                    glm::vec3 color = radiance(Ray{intersection+(eps*glm::normalize(wo)),glm::normalize(wo)},recurance+1);
                    return color;

                }

            //The other object.
            }else{

                glm::vec3 w = sample_cos(u,v,obj->normal(intersection));
                indirect = radiance(Ray{intersection+(eps*glm::normalize(w)),glm::normalize(w)},recurance+1)*obj->albedo();
            }
       }


            float pdf=1.0;
            glm::vec3 origin= scene::light;
            glm::vec3 direction = sample_sphere(10., u, v,pdf, glm::vec3(0.,1.,0.));
            Ray lightvec = Ray{origin,direction};


            if(intersect(lightvec,t)){

                glm::vec3 intersectionlight(lightvec.origin+lightvec.direction*t);
                ///*Distance light et premier intersection
                float distance1=glm::distance(lightvec.origin,intersectionlight);
                //Distance light point dintersection du premeir ray
                float distance2=glm::distance(lightvec.origin,intersection);

                if(distance1+eps<distance2){

                    return indirect;

                }else{

                    glm::vec3 L = glm::normalize(scene::light-intersection); // => L
                    glm::vec3 N = obj->normal(intersection); //N
                    float diff = glm::dot(L,N);
                    diff=diff*10./pi; //J'ai enlevé abs(diff) //*10 pour plus de lumiére



                    return indirect + obj->albedo()*diff;
               }
           }



}
    return glm::vec3(0.0f);        
}

int main (int, char **)
{
    int w = 768, h = 768;
    std::vector<glm::vec3> colors(w * h, glm::vec3{0.f, 0.f, 0.f});

    Ray cam {{50, 52, 295.6}, glm::normalize(glm::vec3{0, -0.042612, -1})};	// cam pos, dir
    float near = 1.f;
    float far = 10000.f;

    glm::mat4 camera =
        glm::scale(glm::mat4(1.f), glm::vec3(float(w), float(h), 1.f))
        * glm::translate(glm::mat4(1.f), glm::vec3(0.5, 0.5, 0.f))
        * glm::perspective(float(54.5f * pi / 180.f), float(w) / float(h), near, far)
        * glm::lookAt(cam.origin, cam.origin + cam.direction, glm::vec3(0, 1, 0))
        ;

    glm::mat4 screenToRay = glm::inverse(camera);

    //Utilisation file
    ManageFile("Cube.obj");



    unsigned nbray = 10;
#pragma omp parallel for
    for (int y = 0; y < h; y++)
    {
        std::cerr << "\rRendering: " << 100 * y / (h - 1) << "%";

        for (unsigned short x = 0; x < w; x++)
        {

            for(unsigned k=0; k< nbray ; k++){

                float u = random_u();
                float v = random_u();
                float dx= sqrt(-2*log(u))*cos(2*pi*v)*0.2;
                float dy= sqrt(-2*log(u))*sin(2*pi*v)*0.2;

                glm::vec4 p0 = screenToRay * glm::vec4{float(x+dx), float(h - (y+dy)), 0.f, 1.f};
                glm::vec4 p1 = screenToRay * glm::vec4{float(x+dx), float(h - (y+dy)), 1.f, 1.f};

                glm::vec3 pp0 = glm::vec3(p0 / p0.w);
                glm::vec3 pp1 = glm::vec3(p1 / p1.w);

                glm::vec3 d = glm::normalize(pp1 - pp0);

                glm::vec3 r = radiance (Ray{pp0, d},0);
                colors[y * w + x] += r; // a changer
            }
            colors[y * w + x]/=nbray;
            colors[y * w + x]=glm::clamp( colors[y * w + x],glm::vec3(0.f),glm::vec3(1.f));
        }
    }

    {
        std::fstream f("image.ppm", std::fstream::out);
        f << "P3\n" << w << " " << h << std::endl << "255" << std::endl;

        for (auto c : colors)
            f << toInt(c.x) << " " << toInt(c.y) << " " << toInt(c.z) << " ";
    }
}
