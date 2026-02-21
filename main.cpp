#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iostream>

#include "Ville.h"
#include "Maison.h"
#include "Parc.h"
#include "Usine.h"
#include "Ecole.h"
#include "Stade.h"
#include "Simulation.h"
#include "structures.h"

using namespace std;

// DÉFINIR LES VARIABLES GLOBALES POUR LES SONS
Sound fireSound;
Sound sadSound;
bool playSadSound = false;
float previousSatisfaction = 100.0f;

// Prototype pour éviter le conflit avec PlaySound de Windows
#ifdef PlaySound
#undef PlaySound
#endif

int main() {
    // Initialisation de la fenêtre et des paramètres graphiques
    const int screenW = 1400, screenH = 900;
    InitWindow(screenW, screenH, "VirtualCity Simulation - Gestion Cycles/Saisons");

    // AJOUTER L'INITIALISATION AUDIO
    InitAudioDevice();

    // CRÉER LES SONS SYNTHÉTIQUES
    // ----------------------------

    //  Créer un son d'incendie synthétique
    Wave fireWave = { 0 };
    fireWave.frameCount = 44100;  // 1 seconde à 44100 Hz
    fireWave.sampleRate = 44100;
    fireWave.sampleSize = 16;
    fireWave.channels = 1;
    fireWave.data = malloc(fireWave.frameCount * sizeof(short));

    // Remplir avec un son "feu"
    for (unsigned int i = 0; i < fireWave.frameCount; i++) {
        float t = (float)i / 44100.0f;
        float freq = 800.0f * expf(-t * 3.0f);  // Fréquence qui descend
        float val = sinf(2.0f * 3.14159265358979323846f * freq * t) * (1.0f - t);
        val += ((rand() % 2000) - 1000) / 10000.0f;  // Ajouter du bruit
        ((short*)fireWave.data)[i] = (short)(val * 30000);
    }

    fireSound = LoadSoundFromWave(fireWave);
    UnloadWave(fireWave);

    // 2. Créer un son triste synthétique
    Wave sadWave = { 0 };
    sadWave.frameCount = 132300;  // 3 secondes
    sadWave.sampleRate = 44100;
    sadWave.sampleSize = 16;
    sadWave.channels = 1;
    sadWave.data = malloc(sadWave.frameCount * sizeof(short));

    for (unsigned int i = 0; i < sadWave.frameCount; i++) {
        float t = (float)i / 44100.0f;
        // Accord La mineur (A-C-E)
        float val = 0.3f * sinf(2.0f * 3.14159265358979323846f * 220.0f * t) +  // La
                    0.2f * sinf(2.0f * 3.14159265358979323846f * 261.6f * t) +  // Do
                    0.2f * sinf(2.0f * 3.14159265358979323846f * 329.6f * t);    // Mi
        val *= expf(-t * 0.3f);  // Fade out
        ((short*)sadWave.data)[i] = (short)(val * 25000);
    }

    sadSound = LoadSoundFromWave(sadWave);
    UnloadWave(sadWave);

    SetTargetFPS(60);
    srand(time(NULL));

    // Création des objets principaux de la simulation
    Ville* maVille = new Ville("VirtualCity");
    Simulation* sim = new Simulation(maVille);

    // Configuration de la caméra 3D
    Camera3D camera = { {25,20,25}, {0,0,0}, {0,1,0}, 45, CAMERA_PERSPECTIVE };

    // Déclaration des formulaires et données d'événements
    InputForm inputForm;
    StadeEventForm stadeEventForm;
    VisualEvent currentEvent;
    StadeEventData currentStadeEvent;

    // Vecteur pour stocker les données des bâtiments placés
    vector<BuildingData> buildings;

    // Variables pour la sélection et la gestion des saisons
    int selectedId = -1;
    char seasons[] = {'H','P','E','A'};  // Hiver, Printemps, Été, Automne
    int seasonIdx = 0;

    // Rectangles pour les boutons globaux (gestion habitants et événements de stade)
    Rectangle globalBtnAddHab = {0,0,0,0};
    Rectangle globalBtnRemHab = {0,0,0,0};
    Rectangle globalStadeBtns[3] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
    int globalSelectedId = -1;

    // Définition des boutons de l'interface
    vector<Rectangle> buttons = {
        {20,20,180,40},   // Maison
        {20,70,180,40},   // Usine
        {20,120,180,40},  // Parc
        {20,170,180,40},  // École
        {20,220,180,40},  // Stade
        {20,270,180,40}   // Supprimer
    };

    // Boucle principale de la simulation
    while (!WindowShouldClose()) {

        // Mise à jour de la caméra et des formulaires
        UpdateCamera(&camera, CAMERA_FREE);
        currentEvent.update();
        inputForm.currentVille = maVille;
        inputForm.handleInput();
        stadeEventForm.handleInput();

        // AJOUTER LA VÉRIFICATION DE SATISFACTION
        float currentSatisfaction = maVille->getSatisfaction();

        // Jouer le son de tristesse si la satisfaction passe sous 50%
        if (currentSatisfaction < 50.0f && previousSatisfaction >= 50.0f && !playSadSound) {
            playSadSound = true;
            PlayGameSound(sadSound);  // Utiliser :: pour spécifier la fonction de raylib
        }

        // Réinitialiser si la satisfaction remonte au-dessus de 50%
        if (currentSatisfaction >= 50.0f) {
            playSadSound = false;
        }

        previousSatisfaction = currentSatisfaction;

        // Gestion de l'événement de stade actif
        if (currentStadeEvent.active) {
            currentStadeEvent.timer--;
            if (currentStadeEvent.timer <= 0) {
                currentStadeEvent.active = false;
            }
        }

        // Suppression d'un bâtiment après un événement
        if (!currentEvent.active && currentEvent.buildingToRemoveId != -1) {
            int idToRemove = currentEvent.buildingToRemoveId;

            for (auto it = buildings.begin(); it != buildings.end(); ++it) {
                if (it->batiment && it->batiment->getId() == idToRemove) {
                    maVille->supprimerBatiment(idToRemove);
                    buildings.erase(it);
                    currentEvent.buildingToRemoveId = -1;
                    break;
                }
            }
        }

        // Calcul du rayon de la souris pour les interactions 3D
        Vector2 mouse = GetMousePosition();
        Ray mouseRay = GetMouseRay(mouse, camera);

        // Gestion du placement de bâtiment avec curseur
        if (inputForm.active && inputForm.showCursor) {
            Vector3 hit = GetRayPlaneCollision(mouseRay, {0,0,0}, {0,1,0});
            if (hit.x != 0 || hit.z != 0) {
                inputForm.placementPos = hit;
            }
        }

        // Gestion des clics de souris (interface et sélection)
        if (!inputForm.active && !stadeEventForm.active && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            bool buttonClicked = false;

            // Gestion du bouton d'ajout d'habitant
            if (CheckCollisionPointRec(mouse, globalBtnAddHab) && globalSelectedId != -1) {
                for (auto& bd : buildings) {
                    if (bd.batiment->getId() == globalSelectedId && bd.batiment->getType() == 'H') {
                        Maison* m = dynamic_cast<Maison*>(bd.batiment);
                        if (m) {
                            if (m->getHabitants() < m->getCapacite()) {
                                m->ajouterHabitants(1);
                                maVille->majPopulation();
                            }
                        }
                        buttonClicked = true;
                        break;
                    }
                }
            }

            // Gestion du bouton de retrait d'habitant
            if (CheckCollisionPointRec(mouse, globalBtnRemHab) && globalSelectedId != -1) {
                for (auto& bd : buildings) {
                    if (bd.batiment->getId() == globalSelectedId && bd.batiment->getType() == 'H') {
                        Maison* m = dynamic_cast<Maison*>(bd.batiment);
                        if (m) {
                            if (m->getHabitants() > 0) {
                                m->retirerHabitants(1);
                                maVille->majPopulation();
                            }
                        }
                        buttonClicked = true;
                        break;
                    }
                }
            }

            // Gestion des boutons d'événements de stade
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(mouse, globalStadeBtns[i]) && globalSelectedId != -1) {
                    cout << "DEBUG: Bouton stade " << (i+1) << " clique!" << endl;
                    for (auto& bd : buildings) {
                        if (bd.batiment->getId() == globalSelectedId && bd.batiment->getType() == 'S') {
                            Stade* s = dynamic_cast<Stade*>(bd.batiment);
                            if (s) {
                                stadeEventForm.start(globalSelectedId, i + 1, s->getCapacite());
                                buttonClicked = true;
                            }
                            break;
                        }
                    }
                    break;
                }
            }

            // Gestion des boutons principaux de l'interface
            if (!buttonClicked) {
                for (size_t i = 0; i < buttons.size(); i++) {
                    if (CheckCollisionPointRec(mouse, buttons[i])) {
                        buttonClicked = true;
                        switch(i) {
                            case 0: inputForm.start(1); break;   // Maison
                            case 1: inputForm.start(2); break;   // Usine
                            case 2: inputForm.start(3); break;   // Parc
                            case 3: inputForm.start(4); break;   // École
                            case 4: inputForm.start(5); break;   // Stade
                            case 5:  // Supprimer
                                if (globalSelectedId != -1) {
                                    for (auto it = buildings.begin(); it != buildings.end(); ++it) {
                                        if (it->batiment->getId() == globalSelectedId) {
                                            maVille->supprimerBatiment(globalSelectedId);
                                            it->batiment = nullptr;
                                            buildings.erase(it);
                                            globalSelectedId = -1;
                                            selectedId = -1;
                                            break;
                                        }
                                    }
                                }
                                break;
                        }
                        break;
                    }
                }
            }

            // Sélection d'un bâtiment par clic dans le monde 3D
            if (!buttonClicked) {
                Vector3 hit = GetRayPlaneCollision(mouseRay, {0,0,0}, {0,1,0});
                selectedId = -1;
                globalSelectedId = -1;
                float minDist = 3.0f;  // Distance maximale pour la sélection

                for (auto& bd : buildings) {
                    float dist = Vector3DistanceCustom(hit, bd.position);
                    if (dist < minDist) {
                        minDist = dist;
                        selectedId = bd.batiment->getId();
                        globalSelectedId = bd.batiment->getId();
                    }
                }
            }
        }

        // Traitement du formulaire d'événement de stade
        if (!stadeEventForm.active && stadeEventForm.participants != "") {
            int participants = stoi(stadeEventForm.participants);

            for (auto& bd : buildings) {
                if (bd.batiment->getId() == stadeEventForm.buildingId && bd.batiment->getType() == 'S') {
                    Stade* s = dynamic_cast<Stade*>(bd.batiment);
                    if (s) {
                        s->specialiteEvenement(*maVille, stadeEventForm.eventType, participants);

                        // Activation de l'affichage visuel de l'événement
                        currentStadeEvent.buildingId = stadeEventForm.buildingId;
                        currentStadeEvent.eventType = stadeEventForm.eventType;
                        currentStadeEvent.active = true;
                        currentStadeEvent.timer = 300;  // Durée d'affichage
                    }
                    break;
                }
            }

            stadeEventForm.stop();
        }

        // Placement d'un nouveau bâtiment
        if (inputForm.active && inputForm.showCursor && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector3 hit = GetRayPlaneCollision(mouseRay, {0,0,0}, {0,1,0});
            if (hit.x != 0 || hit.z != 0) {
                // Vérification de la distance avec les bâtiments existants
                bool valid = true;
                for (auto& bd : buildings) {
                    if (Vector3DistanceCustom(hit, bd.position) < 5.0f) {
                        valid = false;
                        break;
                    }
                }

                if (valid) {
                    Batiment* newBuilding = CreateBuildingFromForm(inputForm, hit);
                    if (newBuilding) {
                        // Vérification des ressources disponibles
                        double eauRequise = newBuilding->getConsommationEau();
                        double elecRequise = newBuilding->getConsommationElectricite();

                        double eauDispo = maVille->geteauInit() - maVille->calculerTotalEau();
                        double elecDispo = maVille->getelecInit() - maVille->calculerTotalElec();

                        if (eauRequise <= eauDispo && elecRequise <= elecDispo) {
                            maVille->ajouterBatiment(newBuilding);
                            buildings.push_back({newBuilding, hit});

                            // Mise à jour de la population si c'est une maison
                            if (newBuilding->getType() == 'H') {
                                maVille->majPopulation();
                            }

                            inputForm.stop();
                        } else {
                            delete newBuilding;
                            inputForm.errorMessage = "Ressources insuffisantes!";
                            inputForm.errorTimer = 120;  // Durée d'affichage de l'erreur
                        }
                    }
                }
            }
        }

        // Gestion des touches de contrôle
        if (IsKeyPressed(KEY_C)) sim->lancerCycleGeneral();  // Lancer un cycle
        if (IsKeyPressed(KEY_E)) triggerEvent(sim, maVille, currentEvent, buildings, selectedId, globalSelectedId);  // Déclencher un événement
        if (IsKeyPressed(KEY_S)) {  // Changer de saison et lancer un cycle
            sim->resetCycleGeneral();
            seasonIdx = (seasonIdx + 1) % 4;
            sim->changerSaison(seasons[seasonIdx]);
            sim->lancerCycleGeneral();
        }
        if (IsKeyPressed(KEY_R)) sim->resetCycleGeneral();  // Réinitialiser le cycle

        // Début du rendu graphique
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Mode 3D : dessin du monde
        BeginMode3D(camera);
        DrawPlane({0,0,0}, {50,50}, LIGHTGRAY);  // Sol
        DrawGrid(25, 2.0f);  // Grille

        // Dessin de tous les bâtiments
        for (auto& bd : buildings) {
            char type = bd.batiment->getType();
            Color col = GetBuildingColor(type);
            Vector3 size;
            Vector3 pos = bd.position;

            // Définition des tailles selon le type de bâtiment
            switch(type) {
                case 'H': size = {3,3,3}; pos.y = 1.5f; break;     // Maison
                case 'U': size = {4,5,4}; pos.y = 2.5f; break;     // Usine
                case 'P': size = {6,0.5f,6}; pos.y = 0.25f; break; // Parc
                case 'E': size = {5,4,5}; pos.y = 2.0f; break;     // École
                case 'S': size = {8,2,8}; pos.y = 1.0f; break;     // Stade
                default: size = {2,2,2}; pos.y = 1.0f;
            }

            DrawCube(pos, size.x, size.y, size.z, col);
            DrawCubeWires(pos, size.x, size.y, size.z, DARKGRAY);

            // Surbrillance du bâtiment sélectionné
            if (bd.batiment->getId() == selectedId) {
                DrawCubeWires(pos, size.x+0.2f, size.y+0.2f, size.z+0.2f, YELLOW);
            }
        }

        // Affichage du curseur de placement
        if (inputForm.active && inputForm.showCursor) {
            Vector3 size = {3,3,3};
            if (inputForm.buildingType == 2) size = (Vector3){4,5,4};
            else if (inputForm.buildingType == 3) size = (Vector3){6,0.5f,6};
            else if (inputForm.buildingType == 4) size = (Vector3){5,4,5};
            else if (inputForm.buildingType == 5) size = (Vector3){8,2,8};

            // Vérification de la validité de l'emplacement
            bool valid = true;
            for (auto& bd : buildings) {
                if (Vector3DistanceCustom(inputForm.placementPos, bd.position) < 5.0f) {
                    valid = false;
                    break;
                }
            }

            Color cursorColor = valid ? GREEN : RED;
            DrawCubeWires(inputForm.placementPos, size.x, size.y, size.z, cursorColor);
        }

        // Effets 3D des événements visuels
        currentEvent.draw3DEffects();
        EndMode3D();

        // Effet de blackout (panne d'électricité)
        if (currentEvent.active && currentEvent.type == 2 && currentEvent.blackoutIntensity > 0) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                         (Color){0, 0, 0, (unsigned char)(currentEvent.blackoutIntensity * 220)});
        }

        // Dessin des boutons principaux
        const char* btnTexts[] = {"Maison", "Usine", "Parc", "Ecole", "Stade", "Supprimer"};
        Color btnColors[] = {
            GetBuildingColor('H'), GetBuildingColor('U'), GetBuildingColor('P'),
            GetBuildingColor('E'), GetBuildingColor('S'), RED
        };

        for (size_t i = 0; i < buttons.size(); i++) {
            bool hover = CheckCollisionPointRec(mouse, buttons[i]);
            DrawRectangleRec(buttons[i], btnColors[i]);
            DrawRectangleLinesEx(buttons[i], 2, hover ? YELLOW : BLACK);
            int w = MeasureText(btnTexts[i], 18);
            DrawText(btnTexts[i], buttons[i].x + (buttons[i].width-w)/2, buttons[i].y+12, 18, BLACK);
        }

        // Affichage de la saison actuelle
        const char* seasonNames[] = {"HIVER", "PRINTEMPS", "ETE", "AUTOMNE"};
        DrawRectangle(20, 320, 180, 100, (Color){30,30,40,200});
        DrawRectangleLines(20, 320, 180, 100, GOLD);
        DrawText("SAISON ACTUELLE", 30, 330, 16, GOLD);
        DrawText(seasonNames[seasonIdx], 30, 360, 24, WHITE);
        DrawText(TextFormat("(%c)", seasons[seasonIdx]), 30, 400, 16, LIGHTGRAY);

        // Affichage des formulaires
        inputForm.draw();
        stadeEventForm.draw();
        currentEvent.draw();

        // Affichage de l'événement de stade actif
        if (currentStadeEvent.active) {
            const char* eventNames[] = {"", "Match football", "Concert", "Competition"};
            int yPos = 150;

            DrawRectangle(GetScreenWidth()/2 - 200, yPos, 400, 60, (Color){0,0,0,200});
            DrawRectangleLines(GetScreenWidth()/2 - 200, yPos, 400, 60, GOLD);

            string msg = "🎉 " + string(eventNames[currentStadeEvent.eventType]) + " au stade !";
            DrawText(msg.c_str(), GetScreenWidth()/2 - MeasureText(msg.c_str(), 20)/2, yPos + 10, 20, WHITE);

            // Barre de progression de l'événement
            float progress = currentStadeEvent.timer / 300.0f;
            DrawRectangle(GetScreenWidth()/2 - 198, yPos + 58, (int)(396 * progress), 2, GREEN);
        }

        // Panneau de statistiques
        int panelX = screenW - 350, panelY = 20, panelW = 330, panelH = 860;
        DrawRectangle(panelX, panelY, panelW, panelH, (Color){30,30,40,230});
        DrawRectangleLines(panelX, panelY, panelW, panelH, GOLD);

        DrawText("STATISTIQUES VILLE", panelX+20, panelY+20, 24, GOLD);
        int y = panelY + 60;

        // Affichage des statistiques de la ville
        DrawText(TextFormat("Ville: VirtualCity"), panelX+20, y, 18, WHITE); y+=30;
        DrawText(TextFormat("Budget: %.0f $", maVille->getBudget()), panelX+20, y, 18, GREEN); y+=30;
        DrawText(TextFormat("Population: %.0f", maVille->getPopulation()), panelX+20, y, 18, SKYBLUE); y+=30;
        double sat = maVille->getSatisfaction();
        Color satColor = sat >= 70 ? GREEN : (sat >= 40 ? ORANGE : RED);
        DrawText(TextFormat("Satisfaction: %.1f%%", sat), panelX+20, y, 18, satColor); y+=30;

        // Affichage des ressources
        double eauConsommee = maVille->calculerTotalEau();
        double eauRestante = maVille->getEau();
        double elecConsommee = maVille->calculerTotalElec();
        double elecRestante = maVille->getElectricite();

        DrawText(TextFormat("Eau consommee: %.0f", eauConsommee), panelX+20, y, 16, (Color){100,100,255,255}); y+=25;
        DrawText(TextFormat("Eau restante: %.0f", eauRestante), panelX+20, y, 16, BLUE); y+=25;
        DrawText(TextFormat("Electricite consommee: %.0f", elecConsommee), panelX+20, y, 16, (Color){255,255,100,255}); y+=25;
        DrawText(TextFormat("Electricite restante: %.0f", elecRestante), panelX+20, y, 16, YELLOW); y+=40;

        // Liste des bâtiments
        DrawText("BATIMENTS:", panelX+20, y, 20, GOLD); y+=30;
        for (auto& bd : buildings) {
            if (y > panelY+panelH-200) break;
            string typeStr;
            switch(bd.batiment->getType()) {
                case 'H': typeStr="Maison"; break;
                case 'U': typeStr="Usine"; break;
                case 'P': typeStr="Parc"; break;
                case 'E': typeStr="Ecole"; break;
                case 'S': typeStr="Stade"; break;
                default: typeStr="Inconnu";
            }
            DrawText(TextFormat("#%d: %s", bd.batiment->getId(), typeStr.c_str()),
                    panelX+20, y, 16, GetBuildingColor(bd.batiment->getType()));
            y+=25;
        }

        // Panneau de détails du bâtiment sélectionné
        if (selectedId != -1) {
            y = panelY+panelH-400;
            DrawRectangle(panelX, y-10, panelW, 390, (Color){40,40,50,240});
            DrawRectangleLines(panelX, y-10, panelW, 390, GOLD);
            DrawText("DETAILS BATIMENT", panelX+20, y, 22, GOLD); y+=40;

            for (auto& bd : buildings) {
                if (bd.batiment->getId() == selectedId) {
                    // Informations générales du bâtiment
                    DrawText(TextFormat("Nom: %s", bd.batiment->getNom().c_str()), panelX+20, y, 18, WHITE); y+=30;
                    DrawText(TextFormat("Type: %c", bd.batiment->getType()), panelX+20, y, 18, WHITE); y+=30;
                    DrawText(TextFormat("Satisfaction: %.1f", bd.batiment->getEffetSatisfaction()), panelX+20, y, 18, WHITE); y+=30;
                    DrawText(TextFormat("Eau: %.1f", bd.batiment->getConsommationEau()), panelX+20, y, 18, BLUE); y+=30;
                    DrawText(TextFormat("Electricite: %.1f", bd.batiment->getConsommationElectricite()), panelX+20, y, 18, YELLOW); y+=40;

                    // Détails spécifiques selon le type de bâtiment
                    if (bd.batiment->getType() == 'H') {
                        Maison* m = dynamic_cast<Maison*>(bd.batiment);
                        if (m) {
                            DrawText(TextFormat("Habitants: %d/%d", m->getHabitants(), m->getCapacite()),
                                    panelX+20, y, 18, SKYBLUE); y+=30;
                            DrawText(TextFormat("Impact ressources: %.1f", m->calculerImpactRessources()), panelX+20, y, 16, LIGHTGRAY); y+=40;

                            // Boutons de gestion des habitants
                            globalBtnAddHab = {(float)panelX+20, (float)y, 40, 40};
                            globalBtnRemHab = {(float)panelX+70, (float)y, 40, 40};
                            globalSelectedId = selectedId;

                            bool hoverAdd = CheckCollisionPointRec(mouse, globalBtnAddHab);
                            bool hoverRem = CheckCollisionPointRec(mouse, globalBtnRemHab);

                            DrawRectangleRec(globalBtnAddHab, hoverAdd ? LIME : GREEN);
                            DrawRectangleLinesEx(globalBtnAddHab, 2, hoverAdd ? YELLOW : BLACK);
                            DrawText("+", globalBtnAddHab.x+15, globalBtnAddHab.y+10, 25, BLACK);

                            DrawRectangleRec(globalBtnRemHab, hoverRem ? (Color){255,100,100,255} : RED);
                            DrawRectangleLinesEx(globalBtnRemHab, 2, hoverRem ? YELLOW : BLACK);
                            DrawText("-", globalBtnRemHab.x+17, globalBtnRemHab.y+10, 25, BLACK);

                            y+=50;
                            DrawText("Cliquez pour gerer habitants", panelX+20, y, 14, LIGHTGRAY); y+=30;
                        }
                    }
                    else if (bd.batiment->getType() == 'U') {
                        Usine* u = dynamic_cast<Usine*>(bd.batiment);
                        if (u) {
                            DrawText(TextFormat("Production: %.1f", u->getProduction()), panelX+20, y, 18, GREEN); y+=25;
                            DrawText(TextFormat("Pollution: %.1f", u->getPollution()), panelX+20, y, 18, RED); y+=25;
                            DrawText(TextFormat("Impact ressources: %.1f", u->calculerImpactRessources()), panelX+20, y, 16, LIGHTGRAY); y+=30;
                        }
                    }
                    else if (bd.batiment->getType() == 'P') {
                        Parc* p = dynamic_cast<Parc*>(bd.batiment);
                        if (p) {
                            DrawText(TextFormat("Bien-etre: +%.1f", p->getEffetBienEtre()), panelX+20, y, 18, GREEN); y+=25;
                            DrawText(TextFormat("Surface: %.1f km2", p->getSurface()), panelX+20, y, 16, LIGHTGRAY); y+=25;
                            DrawText(TextFormat("Impact ressources: %.1f", p->calculerImpactRessources()), panelX+20, y, 16, LIGHTGRAY); y+=30;
                        }
                    }
                    else if (bd.batiment->getType() == 'E') {
                        Ecole* e = dynamic_cast<Ecole*>(bd.batiment);
                        if (e) {
                            DrawText(TextFormat("Enfants: %d/%d", e->getEnfants(), e->getCapacite()), panelX+20, y, 18, WHITE); y+=25;
                            DrawText(TextFormat("Enseignants: %d", e->getEnseignants()), panelX+20, y, 18, WHITE); y+=25;
                            DrawText(TextFormat("Education: +%.1f", e->getEffetEducation()), panelX+20, y, 18, GREEN); y+=25;
                            DrawText(TextFormat("Impact ressources: %.1f", e->calculerImpactRessources()), panelX+20, y, 16, LIGHTGRAY); y+=30;
                        }
                    }
                    else if (bd.batiment->getType() == 'S') {
                        Stade* s = dynamic_cast<Stade*>(bd.batiment);
                        if (s) {
                            DrawText(TextFormat("Capacite: %d", s->getCapacite()), panelX+20, y, 18, WHITE); y+=25;
                            DrawText(TextFormat("Impact ressources: %.1f", s->calculerImpactRessources()), panelX+20, y, 16, LIGHTGRAY); y+=40;
                            DrawText("Organiser evenement:", panelX+20, y, 18, GOLD); y+=30;

                            // Boutons pour les événements de stade
                            const char* events[] = {"Match football", "Concert", "Competition"};
                            const char* eventDetails[] = {
                                "+5000$, +10% satisfaction",
                                "+8000$, +15% satisfaction",
                                "+6000$, +12% satisfaction"
                            };

                            for (int i = 0; i < 3; i++) {
                                globalStadeBtns[i] = {(float)panelX+20, (float)y, 280,35};
                                globalSelectedId = selectedId;

                                bool hover = CheckCollisionPointRec(mouse, globalStadeBtns[i]);

                                DrawRectangleRec(globalStadeBtns[i], hover ? GOLD : (Color){255,215,0,150});
                                DrawRectangleLinesEx(globalStadeBtns[i], 2, hover ? WHITE : BLACK);

                                int w = MeasureText(events[i], 16);
                                DrawText(events[i], globalStadeBtns[i].x + (globalStadeBtns[i].width-w)/2,
                                        globalStadeBtns[i].y+5, 16, BLACK);

                                DrawText(eventDetails[i], panelX+25, globalStadeBtns[i].y+25, 12, LIGHTGRAY);
                                y+=45;
                            }

                            // Affichage si un événement est en cours
                            if (currentStadeEvent.active && currentStadeEvent.buildingId == globalSelectedId) {
                                DrawText("Evenement en cours...", panelX+20, y, 16, GREEN);
                            }
                        }
                    }
                    break;
                }
            }
        } else {
            // Réinitialisation des boutons globaux si aucun bâtiment n'est sélectionné
            globalBtnAddHab = {0,0,0,0};
            globalBtnRemHab = {0,0,0,0};
            for (int i = 0; i < 3; i++) {
                globalStadeBtns[i] = {0,0,0,0};
            }
            globalSelectedId = -1;
        }

        // Affichage des contrôles
        DrawText("CONTROLES: C=Cycle E=Evenement S=Saison+Cycle R=Reset", 20, screenH-40, 18, DARKGRAY);
        DrawText("Cliquez sur un batiment pour details et gestion", 20, screenH-20, 18, DARKGRAY);

        // Message d'erreur pour le placement de bâtiment
        if (inputForm.active && inputForm.showCursor) {
            bool valid = true;
            for (auto& bd : buildings) {
                if (Vector3DistanceCustom(inputForm.placementPos, bd.position) < 5.0f) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                DrawText("Trop proche d'un autre batiment!", 10, screenH - 120, 18, RED);
            }
        }

        EndDrawing();
    }

    // Nettoyage et fermeture
    UnloadSound(fireSound);
    UnloadSound(sadSound);
    CloseAudioDevice();

    CloseWindow();
    delete sim;
    delete maVille;

    return 0;
}
