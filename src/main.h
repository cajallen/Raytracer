#pragma once

#include "imgui.h"
#include "PGA_3D.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <direct.h>

using std::string;
using std::vector;
using std::array;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::stringstream;

#include <glad/glad.h>          // Initialize with gladLoadGL()

#define X 0
#define Y 1


namespace P3 {

struct Geometry;
struct Sphere;
struct Material;

// This function is used as a helper to parse the keyed lines
// Pseudo: if prefix_matches ? string_without_prefix : "";
string rest_if_prefix(const string prefix, string content);


struct UIObject {
    int id;

    // Call with id to create new
    UIObject();
    // Call with id to replace
    UIObject(int old_id);

    virtual void ImGui() { }
    virtual string Encode() { return ""; }
    virtual void Decode(string s) { }

    string WithId(string s);
    bool operator==(UIObject rhs) { return id == rhs.id; }
};


struct UICamera : UIObject {
    float cp[3]{ 0.0f, 0.0f, 0.0f };
    float cf[3]{ 0.0f, 0.0f, 1.0f };
    float cu[3]{ 0.0f, 1.0f, 0.0f };
    float bc[3]{ 0.0f, 0.0f, 0.0f };
    float fov_ha = 45.0f;
    int window_res[2]{ 640, 480 };
    int max_depth = 5;

    using UIObject::UIObject;

    void ImGui();
    string Encode();
    void Decode(string s); // Camera decode takes in the whole line, instead of post key, as it is multiline.
};


struct UIMaterial : UIObject{
    float ambient[3]{ 0.0f, 0.0f, 0.0f };
    float diffuse[3]{ 0.0f, 0.0f, 0.0f };
    float specular[3]{ 0.0f, 0.0f, 0.0f };
    float transmissive[3]{ 0.0f, 0.0f, 0.0f };
    float phong = 0.0f;
    float ior = 0.0f;

    using UIObject::UIObject;

    void ImGui();
    string Encode();
    void Decode(string s);
};


// This only needs to exist in preperation for other Geometry primitives.
struct UIGeometry : UIObject {
    UIMaterial* mat;

    UIGeometry();
    UIGeometry(int old_id);

    vector<UIGeometry*>::iterator GetIter();
    void Delete();

    virtual Geometry* ToGeometry();

    void ImGui();
    string Encode();
    void Decode(string s);
};

struct UISphere : UIGeometry {
    float pos[3]{ 0.0f, 0.0f, 0.0f };
    float rad = 1.0f;

    using UIGeometry::UIGeometry;

    Geometry* ToGeometry();

    void ImGui();
    string Encode();
    void Decode(string s);
};


struct UILight : UIObject {
    float col[3] = { 0.0f, 0.0f, 0.0f };

    using UIObject::UIObject;

    vector<UILight*>::iterator GetIter();
    void Delete();

    void ImGui();
    string Encode();
    void Decode(string s);
};

struct UIAmbientLight : UILight {
    using UILight::UILight;

    void ImGui();
    string Encode();
    void Decode(string s);
};

struct UIPointLight : UILight {
    float pos[3] = { 0.0f, 0.0f, 0.0f };

    using UILight::UILight;

    void ImGui();
    string Encode();
    void Decode(string s);
};

struct UIDirectionalLight : UILight {
    float dir[3] = { 0.0f, 0.0f, 0.0f };

    using UILight::UILight;

    void ImGui();
    string Encode();
    void Decode(string s);
};

struct UISpotLight : UILight {
    float pos[3] = { 0.0f, 0.0f, 0.0f };
    float dir[3] = { 0.0f, 0.0f, 0.0f };
    float angle1 = 0.0f;
    float angle2 = 0.0f;

    using UILight::UILight;

    void ImGui();
    string Encode();
    void Decode(string s);
};


struct HitInformation {
    float dist;
    Point3D pos;
    Dir3D viewing;
    Dir3D normal;
};


struct Ray {
    Point3D pos;
    Dir3D dir;

    Ray(Point3D p, Dir3D d) : pos(p), dir(d.normalized()) { }
};


struct Camera {
    Point3D position;
    Dir3D forward, up, right;
    Color background_color;
    float half_vfov;
    array<int, 2> res;
    array<int, 2> mid_res;
    int max_depth = 5;

    Camera(UICamera* from);
};

struct Material {
    Color ambient = Color();
    Color diffuse = Color();
    Color specular = Color();
    Color transmissive = Color();
    float phong = 0.0f;
    float ior = 0.0f;

    Material() { }
    Material(UIMaterial* from);
};

struct Geometry {
    Material material; // We no longer care about pointers because at this point material is just constant values.

    Geometry(UIGeometry* from);

    virtual bool FindIntersection(Ray ray, HitInformation& intersection) { return false; }
};


struct Sphere : Geometry {
    Point3D position;
    float radius;

    Sphere(UISphere* from);

    bool FindIntersection(Ray ray, HitInformation* intersection);
};



void Reset();
void Load();
void Save();
void Render();

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

bool FindIntersection(vector<Geometry*> geometry, Ray ray, HitInformation* intersection);

}