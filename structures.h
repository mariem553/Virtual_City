#ifndef STRUCTURES_H
#define STRUCTURES_H
#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <ctime>
#include "Ville.h"
#include "Maison.h"
#include "Parc.h"
#include "Usine.h"
#include "Ecole.h"
#include "Stade.h"
#include "Simulation.h"

using namespace std;
extern Sound fireSound;
extern Sound sadSound;
extern bool playSadSound;
extern float previousSatisfaction;;
// STRUCTURES
#ifdef PlaySound
#undef PlaySound
#endif
// Stocke les données d'un bâtiment placé dans le monde 3D
struct BuildingData {
    Batiment* batiment;
    Vector3 position;
};


// Gère le formulaire de création de bâtiments
struct InputForm {
    bool active = false;
    int buildingType = 0;
    vector<string> labels;
    vector<string> values;
    size_t currentField = 0;
    Vector3 placementPos = {0, 0, 0};
    bool showCursor = false;
    string errorMessage = "";
    int errorTimer = 0;
    Ville* currentVille = nullptr;  // Pointeur vers la ville active (pour vérifier les ressources)

    // Méthodes (définies dans structures.cpp)
    void start(int type);
    void stop();
    bool isValidNumber(const string &s);
    void handleInput();
    void draw();
};


// Gère le formulaire d'organisation d'événements dans un stade
struct StadeEventForm {
    bool active = false;
    int buildingId = -1;
    int eventType = 0; // 1: Match, 2: Concert, 3: Competition
    string participants = "";
    string errorMessage = "";
    int errorTimer = 0;
    int maxCapacity = 0;

    void start(int id, int type, int capacity);
    void stop();
    void handleInput();
    void draw();
};

struct StadeEventData {
    int buildingId = -1;
    int eventType = 0;
    bool active = false;
    float timer = 0;
};

struct VisualEvent {
    bool active = false;
    int type = 0;
    int timer = 0;
    string message = "";
    vector<Vector3> particles;
    vector<Color> particleColors;
    vector<Vector3> buildingParticles;
    Vector3 targetBuildingPos;
    float eventCenterX = 0.0f;
    float eventCenterZ = 0.0f;
    bool blackoutEffect = false;
    float blackoutIntensity = 0.0f;
    int buildingToRemoveId = -1;

    void trigger(int eventType, const string& msg, Vector3 buildingPos = {0,0,0});
    void update();
    void draw();
    void draw3DEffects();
};

// FONCTIONS UTILITAIRES
// Fonction personnalisée pour jouer du son
void PlayGameSound(Sound sound);
Vector3 Vector3SubtractCustom(Vector3 a, Vector3 b);
float Vector3DotProductCustom(Vector3 a, Vector3 b);
Vector3 Vector3ScaleCustom(Vector3 v, float s);
Vector3 Vector3AddCustom(Vector3 a, Vector3 b);
float Vector3DistanceCustom(Vector3 a, Vector3 b);

Vector3 GetRayPlaneCollision(Ray ray, Vector3 planePoint, Vector3 planeNormal);
Color GetBuildingColor(char type);
Batiment* CreateBuildingFromForm(InputForm& form, Vector3 pos);
void triggerEvent(Simulation* sim, Ville* ville, VisualEvent& visualEvent, vector<BuildingData>& buildings, int& selectedId, int& globalSelectedId);

#endif
