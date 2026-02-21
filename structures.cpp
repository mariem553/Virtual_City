#include "structures.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <ctime>

// IMPLEMENTATION DE InputForm

// Démarrer le formulaire avec un type de bâtiment
void InputForm::start(int type) {
           active = true;
        buildingType = type;
        currentField = 0;
        values.clear();
        labels.clear();
        showCursor = false;
        errorMessage.clear();
        errorTimer = 0;
    // Définir les champs selon le type de bâtiment
    switch(type) {
        case 1: labels = {"Nom:", "Eau:", "Electricite:", "Capacite max:", "Habitants init:"}; break;  // Maison
        case 2: labels = {"Nom:", "Eau:", "Electricite:", "Production base:"}; break;  // Usine
        case 3: labels = {"Nom:", "Eau:", "Electricite:", "surface"}; break;  // Parc
        case 4: labels = {"Nom:", "Eau:", "Electricite:", "Capacite:", "Enfants:", "Enseignants:"}; break;  // École
        case 5: labels = {"Nom:", "Eau:", "Electricite:", "Capacite:"}; break;  // Stade
    }
    values.resize(labels.size(), "");  // Initialise chaque champ avec une chaîne vide
}

// Arrêter le formulaire
void InputForm::stop() {
    active = false;
    showCursor = false;
    errorMessage.clear();
}

// Vérifier si une chaîne est un nombre valide
bool InputForm::isValidNumber(const string &s) {
    if (s.empty()) return false;
    if (s == "." || s == "-") return false;

    bool hasPoint = false;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '.') {
            if (hasPoint) return false;  // Plus d'un point
            hasPoint = true;
        } else if (!isdigit(s[i])) {
            if (i == 0 && s[i] == '-') continue;  // Autorise le signe négatif en première position
            return false;
        }
    }

    try {
        double val = stod(s);
        return val > 0;  // On accepte uniquement les nombres positifs
    } catch (...) {
        return false;
    }
}

// Gérer les entrées clavier pour le formulaire
void InputForm::handleInput() {
    if (!active || !currentVille) return;

    // Navigation entre champs avec TAB
    if (IsKeyPressed(KEY_TAB)) {
        currentField = (currentField + 1) % labels.size();
        errorMessage.clear();
    }

    // Traitement des caractères saisis
    int key = GetCharPressed();
    while (key > 0) {
        // Vérifie si le champ actuel attend un nombre
        bool isNumberField = false;
        for (const auto& field : {"Eau", "Electricite", "Capacite", "Habitants", "Enfants", "Enseignants", "Production", "surface"}) {
            if (labels[currentField].find(field) != string::npos) {
                isNumberField = true;
                break;
            }
        }

        if (isNumberField) {
            // N'autorise que les chiffres, le point et le signe moins
            if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
                if (key == '.' && values[currentField].find('.') != string::npos) {
                    errorMessage = "Un seul point decimal!";
                    errorTimer = 120;
                } else {
                    values[currentField] += (char)key;
                }
            }
        } else {
            // Pour les champs texte, on accepte tous les caractères
            values[currentField] += (char)key;
        }
        key = GetCharPressed();
    }

    // Gestion de la touche retour arrière
    if (IsKeyPressed(KEY_BACKSPACE) && !values[currentField].empty()) {
        values[currentField].pop_back();
    }

    // Validation avec ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        // Vérification que le champ n'est pas vide
        if (values[currentField].empty()) {
            errorMessage = "Champ obligatoire!";
            errorTimer = 120;
            return;
        }

        // Vérifie si le champ attend un nombre
        bool isNumberField = false;
        for (const auto& field : {"Eau", "Electricite", "Capacite", "Habitants", "Enfants", "Enseignants", "Production", "surface"}) {
            if (labels[currentField].find(field) != string::npos) {
                isNumberField = true;
                break;
            }
        }

        if (isNumberField && !isValidNumber(values[currentField])) {
            errorMessage = "Nombre positif requis!";
            errorTimer = 120;
            return;
        }

        // Vérification des ressources disponibles (pour les champs eau et électricité)
        if ((labels[currentField].find("Eau") != string::npos ||
             labels[currentField].find("Electricite") != string::npos) &&
            currentField >= 1 && currentField <= 2) {

            try {
                double eauVal = 0, elecVal = 0;

                if (values[1] != "" && isValidNumber(values[1])) {
                    eauVal = stod(values[1]);
                }

                if (values[2] != "" && isValidNumber(values[2])) {
                    elecVal = stod(values[2]);
                }

                // Calcul des ressources disponibles dans la ville
                double eauConsommee = currentVille->calculerTotalEau();
                double elecConsommee = currentVille->calculerTotalElec();

                double eauDispo = currentVille->geteauInit() - eauConsommee;
                double elecDispo = currentVille->getelecInit() - elecConsommee;

                if (eauVal > eauDispo) {
                    errorMessage = "Eau insuffisante! Max: " + to_string((int)eauDispo);
                    errorTimer = 120;
                    return;
                }

                if (elecVal > elecDispo) {
                    errorMessage = "Electricite insuffisante! Max: " + to_string((int)elecDispo);
                    errorTimer = 120;
                    return;
                }
            } catch(...) {
                errorMessage = "Valeur numerique invalide!";
                errorTimer = 120;
                return;
            }
        }

        // Passer au champ suivant ou terminer
        if (currentField < labels.size() - 1) {
            currentField++;
            errorMessage.clear();
        } else {
            // Validation supplémentaire pour certains types de bâtiments
            bool valid = true;
            if (buildingType == 1) {  // Maison
                if (values[3] != "" && values[4] != "") {
                    int capacite = stoi(values[3]);
                    int habitants = stoi(values[4]);
                    if (habitants > capacite) {
                        errorMessage = "Habitants > Capacite!";
                        errorTimer = 120;
                        valid = false;
                    }
                }
            }
            else if (buildingType == 4) {  // École
                if (values[3] != "" && values[4] != "" && values[5] != "") {
                    int capacite = stoi(values[3]);
                    int enfants = stoi(values[4]);
                    int enseignants = stoi(values[5]);
                    (void)enseignants;
                    if (enfants > capacite) {
                        errorMessage = "Enfants > Capacite!";
                        errorTimer = 120;
                        valid = false;
                    }
                }
            }

            if (valid) {
                // Tous les champs sont valides, on passe au placement du bâtiment
                showCursor = true;  // Active le curseur de placement
                errorMessage.clear();
            }
        }
    }

    // Annulation avec CTRL gauche
    if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        stop();
    }

    // Décrémente le timer d'erreur
    if (errorTimer > 0) errorTimer--;
}

// Dessiner le formulaire
void InputForm::draw() {
    if (!active) return;

    int width = 600, height = 500;
    int x = (GetScreenWidth() - width) / 2;
    int y = (GetScreenHeight() - height) / 2;

    // Fond du formulaire
    DrawRectangle(x, y, width, height, (Color){60, 70, 80, 240});
    DrawRectangleLines(x, y, width, height, WHITE);

    // Titre selon le type de bâtiment
    const char* titles[] = {"", "MAISON", "USINE", "PARC", "ECOLE", "STADE"};
    DrawText(titles[buildingType], x + 20, y + 20, 28, GOLD);

    // Affichage des ressources disponibles
    if (currentVille) {
        double eauConsommee = currentVille->calculerTotalEau();
        double elecConsommee = currentVille->calculerTotalElec();

        double eauDispo = currentVille->geteauInit() - eauConsommee;
        double elecDispo = currentVille->getelecInit() - elecConsommee;

        string eauText = "Eau disponible: " + to_string((int)eauDispo) + " (consommee: " + to_string((int)eauConsommee) + ")";
        string elecText = "Electricite disponible: " + to_string((int)elecDispo) + " (consommee: " + to_string((int)elecConsommee) + ")";

        DrawText(eauText.c_str(), x + 20, y + 50, 16, BLUE);
        DrawText(elecText.c_str(), x + 20, y + 70, 16, YELLOW);
    }

    // Dessin des champs de saisie
    int yOffset = 100;
    for (size_t i = 0; i < labels.size(); i++) {
        int posY = y + yOffset + (int)i * 55;
        DrawText(labels[i].c_str(), x + 30, posY, 20, WHITE);

        Rectangle field = {(float)(x + 250), (float)posY, 300, 35};
        bool current = (currentField == i);
        DrawRectangleRec(field, current ? LIGHTGRAY : GRAY);
        DrawRectangleLinesEx(field, 2, current ? YELLOW : WHITE);

        DrawText(values[i].c_str(), field.x + 10, field.y + 8, 20, BLACK);

        // Curseur clignotant pour le champ actif
        if (current && ((int)(GetTime() * 2) % 2) == 0) {
            int textW = MeasureText(values[i].c_str(), 20);
            DrawText("|", field.x + 10 + textW, field.y + 8, 20, RED);
        }
    }

    // Instructions
    if (!showCursor) {
        DrawText("TAB=Naviguer ENTER=Valider LEFT_CTRL=Annuler", x + 20, y + height - 40, 16, YELLOW);
    } else {
        DrawText("Cliquez pour placer le batiment", x + 20, y + height - 40, 20, GREEN);
    }

    // Affichage des erreurs
    if (!errorMessage.empty() && errorTimer > 0) {
        DrawText(errorMessage.c_str(), x + 20, y + height - 70, 18, RED);
    }
}

// IMPLEMENTATION DE StadeEventForm

// Démarrer le formulaire d'événement de stade
void StadeEventForm::start(int id, int type, int capacity) {
    active = true;
    buildingId = id;        // ID du stade concerné
    eventType = type;       // Type d'événement (1: football, 2: concert, 3: compétition)
    participants = "";      // Chaîne pour le nombre de participants
    errorMessage = "";
    errorTimer = 0;
    maxCapacity = capacity; // Capacité maximale du stade
}

// Arrêter le formulaire
void StadeEventForm::stop() {
    active = false;
    buildingId = -1;
    eventType = 0;
    participants = "";
    errorMessage = "";
    errorTimer = 0;
    maxCapacity = 0;
}

// Gérer les entrées pour le formulaire d'événement
void StadeEventForm::handleInput() {
    if (!active) return;

    // Accepter uniquement les chiffres
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= '0' && key <= '9') {
            participants += (char)key;
        }
        key = GetCharPressed();
    }

    // Retour arrière
    if (IsKeyPressed(KEY_BACKSPACE) && !participants.empty()) {
        participants.pop_back();
    }

    // Validation avec ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        if (participants.empty()) {
            errorMessage = "Champ obligatoire!";
            errorTimer = 120;
            return;
        }

        int nbParticipants = 0;
        try {
            nbParticipants = stoi(participants);
        } catch(...) {
            errorMessage = "Nombre invalide!";
            errorTimer = 120;
            return;
        }

        if (nbParticipants <= 0) {
            errorMessage = "Nombre positif requis!";
            errorTimer = 120;
            return;
        }

        // Vérification de la capacité
        if (nbParticipants > maxCapacity) {
            errorMessage = "Capacite depassee! Max: " + to_string(maxCapacity);
            errorTimer = 120;
            return;
        }

        // Ferme le formulaire si tout est valide
        active = false;
    }

    // Annulation avec CTRL gauche
    if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        stop();
    }

    if (errorTimer > 0) errorTimer--;
}

// Dessiner le formulaire d'événement
void StadeEventForm::draw() {
    if (!active) return;

    int width = 500, height = 300;
    int x = (GetScreenWidth() - width) / 2;
    int y = (GetScreenHeight() - height) / 2;

    DrawRectangle(x, y, width, height, (Color){60, 70, 80, 240});
    DrawRectangleLines(x, y, width, height, GOLD);

    // Titre selon le type d'événement
    const char* eventNames[] = {"", "Match de football", "Concert", "Competition"};
    DrawText(eventNames[eventType], x + 20, y + 20, 24, GOLD);

    // Capacité maximale
    DrawText(TextFormat("Capacite maximale: %d", maxCapacity), x + 20, y + 50, 18, YELLOW);

    DrawText("Nombre de participants:", x + 20, y + 90, 20, WHITE);

    // Champ de saisie
    Rectangle field = {(float)(x + 20), (float)(y + 130), 460, 35};
    DrawRectangleRec(field, LIGHTGRAY);
    DrawRectangleLinesEx(field, 2, YELLOW);

    DrawText(participants.c_str(), field.x + 10, field.y + 8, 20, BLACK);

    // Curseur clignotant
    if (((int)(GetTime() * 2) % 2) == 0) {
        int textW = MeasureText(participants.c_str(), 20);
        DrawText("|", field.x + 10 + textW, field.y + 8, 20, RED);
    }

    // Instructions
    DrawText("ENTREE = Valider LEFT_CTRL = Annuler", x + 20, y + height - 40, 16, YELLOW);

    // Affichage des erreurs
    if (!errorMessage.empty() && errorTimer > 0) {
        DrawText(errorMessage.c_str(), x + 20, y + height - 70, 18, RED);
    }
}

// IMPLEMENTATION DE VisualEvent


// Déclencher un événement visuel
void VisualEvent::trigger(int eventType, const string& msg, Vector3 buildingPos) {
    active = true;
    type = eventType;
    timer = 480;  // Durée de l'événement (en frames, ~8 secondes à 60 FPS)
    message = msg;
    particles.clear();
    particleColors.clear();
    buildingParticles.clear();
    targetBuildingPos = buildingPos;
    blackoutEffect = (eventType == 2);  // Panne de courant
    blackoutIntensity = 0.0f;
    buildingToRemoveId = -1;  // Utilisé pour la suppression d'un bâtiment (incendie)

    eventCenterX = 0.0f;
    eventCenterZ = 0.0f;

    // Initialiser les particules selon le type d'événement
    switch(eventType) {
        case 1: // Pigeons
            for (int i = 0; i < 20; i++) {
                particles.push_back({(float)GetRandomValue(-30,30), (float)GetRandomValue(5,20), (float)GetRandomValue(-30,30)});
                particleColors.push_back((Color){120,120,120,255});  // Gris pour les pigeons
            }
            break;
        case 2: // Panne de courant
            for (int i = 0; i < 15; i++) {
                particles.push_back({
                    (float)GetRandomValue(-25, 25),
                    (float)GetRandomValue(3, 15),
                    (float)GetRandomValue(-25, 25)
                });
                int randomBlue = GetRandomValue(50, 200);
                particleColors.push_back((Color){255, 255, (unsigned char)randomBlue, 255});  // Éclairs bleutés
            }
            break;
        case 3: // Grève
            for (int i = 0; i < 5; i++) {
                particles.push_back({
                    -15.0f + i * 7.5f,  // Espacement régulier pour les pancartes
                    1.0f,
                    0.0f
                });
                particleColors.push_back(RED);  // Pancartes rouges
            }
            break;
        case 4: // Embouteillage
            for (int i = 0; i < 12; i++) {
                particles.push_back({-15.0f + i * 3.0f, 0.5f, (float)GetRandomValue(-10,10)});
                particleColors.push_back(i % 3 == 0 ? RED : (i % 3 == 1 ? BLUE : GREEN));  // Voitures colorées
            }
            break;
        case 5: // Forage d'eau (panneaux solaires)
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 4; col++) {
                    particles.push_back({
                        -15.0f + col * 10.0f,
                        3.0f + row * 2.0f,
                        -5.0f + row * 5.0f
                    });
                    particleColors.push_back((Color){30, 60, 120, 255});  // Panneaux bleus
                }
            }

            // Éléments du puits de forage
            buildingParticles.push_back({0, 0.5f, 0});
            particleColors.push_back((Color){101, 67, 33, 255});  // Terre

            buildingParticles.push_back({0, 3.0f, 0});
            particleColors.push_back((Color){150, 150, 150, 255});  // Métal

            buildingParticles.push_back({0, 5.0f, 0});
            particleColors.push_back((Color){200, 200, 200, 255});  // Structure

            // Gouttes d'eau autour
            for (int i = 0; i < 6; i++) {
                float angle = i * 60.0f * DEG2RAD;
                buildingParticles.push_back({
                    cos(angle) * 3.0f,
                    1.0f + (i % 2) * 0.5f,
                    sin(angle) * 3.0f
                });
                particleColors.push_back((Color){0, 100, 255, 200});  // Eau
            }
            break;
        case 6: // Sortie scolaire
            {
                float centerX = 10.0f;
                float centerZ = 0.0f;

                // Enfants
                for (int i = 0; i < 8; i++) {
                    particles.push_back({
                        centerX - 10.0f + i * 3.0f,
                        1.0f,
                        centerZ
                    });

                    Color childColors[] = {
                        (Color){255, 100, 100, 255},
                        (Color){100, 100, 255, 255},
                        (Color){100, 255, 100, 255},
                        (Color){255, 255, 100, 255},
                        (Color){255, 100, 255, 255},
                        (Color){100, 255, 255, 255},
                        (Color){255, 150, 50, 255},
                        (Color){150, 100, 255, 255}
                    };
                    particleColors.push_back(childColors[i % 8]);

                    // Ballons
                    buildingParticles.push_back({
                        centerX - 10.0f + i * 3.0f,
                        4.0f,
                        centerZ
                    });
                    Color balloonColors[] = {RED, BLUE, GREEN, YELLOW, PURPLE, ORANGE, PINK, SKYBLUE};
                    particleColors.push_back(balloonColors[i % 8]);

                    // Sacs à dos
                    buildingParticles.push_back({
                        centerX - 10.0f + i * 3.0f,
                        0.2f,
                        centerZ + 1.5f
                    });
                    particleColors.push_back((Color){139, 69, 19, 255});  // Marron pour les sacs
                }

                // Enseignants
                buildingParticles.push_back({centerX - 13.0f, 1.5f, centerZ});
                particleColors.push_back((Color){50, 50, 50, 255});  // Enseignant 1
                buildingParticles.push_back({centerX + 11.0f, 1.5f, centerZ});
                particleColors.push_back((Color){70, 70, 70, 255});  // Enseignant 2
                break;
            }
        case 7: // Incendie d'usine
            for (int i = 0; i < 15; i++) {
                particles.push_back({(float)GetRandomValue(-20,20), (float)GetRandomValue(5,15), (float)GetRandomValue(-20,20)});
                particleColors.push_back((Color){255,50,0,200});  // Feu orange/rouge
            }
            for (int i = 0; i < 25; i++) {
                buildingParticles.push_back({
                    targetBuildingPos.x + (float)GetRandomValue(-2,2),
                    targetBuildingPos.y + 2.0f + (float)GetRandomValue(0,6),
                    targetBuildingPos.z + (float)GetRandomValue(-2,2)
                });
                particleColors.push_back((Color){255,100,0,200});  // Feu moins intense
            }
            break;
        case 8: // Fête des arbres
            for (int i = 0; i < 40; i++) {
                particles.push_back({(float)GetRandomValue(-30,30), (float)GetRandomValue(2,20), (float)GetRandomValue(-30,30)});
                Color colors[] = {RED, GREEN, BLUE, YELLOW, PURPLE, ORANGE};  // Confettis colorés
                particleColors.push_back(colors[i % 6]);
            }
            break;
    }
}

// Mettre à jour l'événement visuel
void VisualEvent::update() {
    if (timer > 0) timer--;
    if (timer == 0) {
        active = false;
        blackoutEffect = false;
        blackoutIntensity = 0.0f;
    }

    // Gérer l'intensité du blackout pour les pannes de courant
    if (blackoutEffect) {
        if (timer > 420) blackoutIntensity += 0.01f;      // Montée progressive
        else if (timer < 60) blackoutIntensity -= 0.02f;  // Descente progressive
        blackoutIntensity = max(0.0f, min(0.8f, blackoutIntensity));  // Clamping entre 0 et 0.8
    }

    // Animer les particules
    float time = (float)GetTime();
    for (size_t i = 0; i < particles.size(); i++) {
        switch(type) {
            case 1:  case 8: // Pigeons et fête
                particles[i].y -= 0.05f;  // Descente légère
                particles[i].x += sin(time + i) * 0.01f;  // Mouvement sinusoïdal
                if (particles[i].y < 0) particles[i].y = GetRandomValue(10,25);  // Réapparition en haut
                break;
            case 2: // Panne
                particles[i].y -= 0.1f;  // Descente plus rapide
                if (particles[i].y < 0) particles[i].y = GetRandomValue(10,20);
                break;
            case 3: // Grève
                particles[i].y = 1.0f + sin(time * 2.0f + i) * 0.15f;  // Léger mouvement vertical
                break;
            case 4: // Embouteillage
                particles[i].x += 0.02f;  // Déplacement vers la droite
                if (particles[i].x > 20) particles[i].x = -20;  // Réapparition à gauche
                break;
            case 5: // Forage
                particles[i].y = 3.0f + sin(time * 0.3f + i) * 0.5f;  // Oscillation verticale
                particles[i].x += sin(time * 0.5f + i) * 0.01f;  // Léger mouvement horizontal
                break;
            case 6: // Sortie scolaire
                if (i < 8) {
                    particles[i].y = 1.0f + sin(time * 2 + i) * 0.1f;  // Sautillement des enfants
                    particles[i].x += sin(time * 1 + i) * 0.01f;       // Déplacement léger
                }

                // Animation des ballons
                if (i >= 0 && i < 8) {
                    int ballonIndex = i;
                    buildingParticles[ballonIndex].x = particles[i].x + sin(time * 3 + i) * 0.2f;
                    buildingParticles[ballonIndex].z = particles[i].z + cos(time * 3 + i) * 0.2f;
                    buildingParticles[ballonIndex].y = 3.5f + sin(time * 2 + i) * 0.3f;
                }

                // Animation des enseignants
                if (i >= 16 && i < 18) {
                    int teacherIndex = i - 16;
                    buildingParticles[i].y = 1.5f + sin(time * 1.5f + teacherIndex) * 0.05f;
                }
                break;
            case 7: // Incendie
                particles[i].y += sin(time * 8 + i) * 0.1f;  // Montée sinusoïdale
                particles[i].x += sin(time * 3 + i) * 0.02f;  // Écart horizontal
                if (particles[i].y > 15) particles[i].y = 5;  // Réinitialisation en bas
                if (i < buildingParticles.size()) {
                    buildingParticles[i].y += sin(time * 10 + i) * 0.2f;
                    buildingParticles[i].x += sin(time * 4 + i) * 0.05f;
                    if (buildingParticles[i].y > targetBuildingPos.y + 8) buildingParticles[i].y = targetBuildingPos.y + 2;
                }
                break;
        }
    }
}

// Dessiner la notification de l'événement (2D)
void VisualEvent::draw() {
    if (!active) return;

    Color colors[] = {BLACK, GRAY, YELLOW, GREEN, ORANGE, LIME, BLUE, RED, (Color){154,205,50,255}};

    int yPos = 50 + (type - 1) * 45;  // Position verticale selon le type d'événement

    if (type == 2) {
        string text = "[PANNE] " + message;
        DrawRectangle(GetScreenWidth()/2 - 250, yPos, 500, 40, (Color){0,0,0,150});
        DrawRectangleLines(GetScreenWidth()/2 - 250, yPos, 500, 40, YELLOW);
        DrawText(text.c_str(), GetScreenWidth()/2 - MeasureText(text.c_str(), 22)/2, yPos + 10, 22, WHITE);
        DrawText("PANNE DE COURANT - VILLE DANS LE NOIR!",
                GetScreenWidth()/2 - MeasureText("PANNE DE COURANT - VILLE DANS LE NOIR!", 18)/2,
                yPos + 50, 18, YELLOW);
    } else {
        DrawRectangle(GetScreenWidth()/2 - 250, yPos, 500, 40, (Color){0,0,0,200});
        DrawRectangleLines(GetScreenWidth()/2 - 250, yPos, 500, 40, colors[type]);
        string text = "[" + to_string(type) + "] " + message;
        DrawText(text.c_str(), GetScreenWidth()/2 - MeasureText(text.c_str(), 22)/2, yPos + 10, 22, WHITE);
    }
}

// Dessiner les effets 3D de l'événement
void VisualEvent::draw3DEffects() {
    if (!active) return;

    float time = (float)GetTime();

    // Effet de blackout pour les pannes
    if (blackoutEffect && blackoutIntensity > 0) {
        DrawCube((Vector3){0,0,0}, 200,200,200, (Color){0,0,0,(unsigned char)(blackoutIntensity*255)});

        // Étincelles aléatoires pendant le blackout
        if ((int)(time * 8) % 10 < 3) {
            for (int i = 0; i < 8; i++) {
                float x = GetRandomValue(-20, 20);
                float y = GetRandomValue(3, 15);
                float z = GetRandomValue(-20, 20);
                DrawSphere((Vector3){x, y, z}, 0.3f, YELLOW);
            }
        }
    }

    // Dessiner les particules selon le type d'événement
    for (size_t i = 0; i < particles.size(); i++) {
        switch(type) {
            case 1: DrawSphere(particles[i], 0.4f, particleColors[i]); break;  // Pigeons sphériques
            case 2:
                if (!blackoutEffect || (int)(time * 10) % 5 < 3) {
                    DrawCube(particles[i], 0.2f, 0.5f, 0.2f, particleColors[i]);  // Éclairs rectangulaires
                }
                break;
            case 3: // Grève avec pancartes
                if (i < 5) {
                    Vector3 signPos = particles[i];
                    // Poteau de la pancarte
                    DrawCube((Vector3){signPos.x, signPos.y - 0.5f, signPos.z},
                             0.1f, 1.0f, 0.1f, (Color){100, 100, 100, 255});
                    // Pancarte
                    DrawCube((Vector3){signPos.x, signPos.y + 0.3f, signPos.z},
                             1.5f, 1.5f, 0.05f, RED);
                    // Barre de texte
                    DrawCube((Vector3){signPos.x, signPos.y + 0.3f, signPos.z + 0.03f},
                             2.0f, 0.2f, 0.02f, WHITE);
                }

                // Texte "GRÈVE" en 3D
                if (i == 0) {
                    // LETTRE G
                    DrawCube((Vector3){-3.5f, 3.0f, 0}, 0.8f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){-4.0f, 3.5f, 0}, 0.2f, 0.8f, 0.1f, RED);
                    DrawCube((Vector3){-3.0f, 3.5f, 0}, 0.2f, 0.8f, 0.1f, RED);
                    DrawCube((Vector3){-3.5f, 4.0f, 0}, 0.8f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){-3.0f, 3.8f, 0}, 0.2f, 0.4f, 0.1f, RED);

                    // LETTRE R
                    DrawCube((Vector3){-1.5f, 3.0f, 0}, 0.2f, 1.0f, 0.1f, RED);
                    DrawCube((Vector3){-1.0f, 3.5f, 0}, 0.6f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){-0.5f, 3.8f, 0}, 0.2f, 0.4f, 0.1f, RED);
                    DrawCube((Vector3){-1.0f, 4.0f, 0}, 0.6f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){-0.5f, 3.2f, 0}, 0.2f, 0.4f, 0.1f, RED);

                    // LETTRE È (E accent)
                    DrawCube((Vector3){0.5f, 3.0f, 0}, 0.2f, 1.0f, 0.1f, RED);
                    DrawCube((Vector3){1.0f, 3.0f, 0}, 0.6f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){1.0f, 3.5f, 0}, 0.4f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){1.0f, 4.0f, 0}, 0.6f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){1.3f, 4.2f, 0}, 0.3f, 0.1f, 0.1f, RED);

                    // LETTRE V
                    DrawCube((Vector3){2.5f, 3.0f, 0}, 0.2f, 0.8f, 0.1f, RED);
                    DrawCube((Vector3){3.0f, 4.0f, 0}, 0.2f, 0.8f, 0.1f, RED);
                    DrawCube((Vector3){2.5f, 3.6f, 0}, 0.6f, 0.2f, 0.1f, RED);

                    // LETTRE E
                    DrawCube((Vector3){4.0f, 3.0f, 0}, 0.2f, 1.0f, 0.1f, RED);
                    DrawCube((Vector3){4.5f, 3.0f, 0}, 0.6f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){4.5f, 3.5f, 0}, 0.4f, 0.2f, 0.1f, RED);
                    DrawCube((Vector3){4.5f, 4.0f, 0}, 0.6f, 0.2f, 0.1f, RED);

                    // Points de suspension
                    for (int dot = 0; dot < 3; dot++) {
                        DrawCube((Vector3){5.5f, 3.2f + dot * 0.4f, 0},
                                 0.2f, 0.2f, 0.1f, RED);
                    }

                    DrawCube((Vector3){0, 3.5f, -0.1f}, 12.0f, 2.0f, 0.05f, (Color){0, 0, 0, 150});
                }
                break;
            case 4: DrawCube(particles[i], 1.5f, 0.8f, 2.5f, particleColors[i]); break;  // Voitures
            case 5: // Forage d'eau
                if (i < 12) {
                    Vector3 panelPos = particles[i];
                    // Support du panneau solaire
                    DrawCube((Vector3){panelPos.x, panelPos.y - 1.0f, panelPos.z},
                             0.2f, 2.0f, 0.2f, (Color){100, 100, 100, 255});
                    // Panneau solaire
                    DrawCube((Vector3){panelPos.x, panelPos.y + 0.5f, panelPos.z},
                             2.0f, 0.05f, 1.5f, (Color){30, 60, 120, 255});

                    // Cellules photovoltaïques
                    for (int cellY = 0; cellY < 3; cellY++) {
                        for (int cellX = 0; cellX < 4; cellX++) {
                            float cellPosX = panelPos.x - 0.75f + cellX * 0.5f;
                            float cellPosZ = panelPos.z - 0.6f + cellY * 0.5f;
                            DrawCube((Vector3){cellPosX, panelPos.y + 0.06f, cellPosZ},
                                     0.4f, 0.01f, 0.4f, (Color){0, 0, 0, 255});

                            // Effet de brillance aléatoire
                            if ((cellX + cellY + (int)time) % 3 == 0) {
                                DrawCube((Vector3){cellPosX, panelPos.y + 0.07f, cellPosZ},
                                         0.35f, 0.005f, 0.35f, (Color){255, 255, 200, 100});
                            }
                        }
                    }

                    DrawCubeWires((Vector3){panelPos.x, panelPos.y + 0.5f, panelPos.z},
                                  2.02f, 0.07f, 1.52f, (Color){50, 50, 50, 255});
                }

                // Puits de forage
                if (i == 0) {
                    // Base du puits
                    DrawCylinder((Vector3){0, 0.5f, 0}, 2.0f, 2.0f, 1.0f, 16, (Color){101, 67, 33, 255});
                    // Colonne centrale
                    DrawCylinder((Vector3){0, 1.5f, 0}, 0.5f, 0.5f, 3.0f, 8, (Color){150, 150, 150, 255});
                    // Plateforme
                    DrawCube((Vector3){0, 4.0f, 0}, 1.5f, 0.3f, 1.5f, (Color){200, 200, 200, 255});
                    DrawCube((Vector3){0, 4.3f, 0}, 0.8f, 0.4f, 0.8f, (Color){180, 180, 180, 255});
                    // Bras de forage
                    DrawCube((Vector3){1.0f, 3.5f, 0}, 2.0f, 0.1f, 0.1f, (Color){120, 120, 120, 255});
                    DrawLine3D({0, 5.0f, 0}, {0, 1.0f, 0}, (Color){80, 80, 80, 255});
                    // Réservoir d'eau
                    DrawCube((Vector3){-4.0f, 1.0f, 0}, 3.0f, 2.0f, 3.0f, (Color){0, 100, 200, 150});
                    DrawCubeWires((Vector3){-4.0f, 1.0f, 0}, 3.0f, 2.0f, 3.0f, BLUE);
                    float waterLevel = 0.5f + sin(time * 0.5f) * 0.2f;  // Niveau d'eau oscillant
                    DrawCube((Vector3){-4.0f, 0.5f + waterLevel, 0},
                             2.8f, waterLevel * 1.8f, 2.8f, (Color){0, 150, 255, 200});
                    // Pompe
                    DrawCylinder((Vector3){-2.0f, 0.5f, 0}, 0.2f, 0.2f, 4.0f, 8, (Color){100, 100, 100, 255});
                    DrawCylinder((Vector3){-2.0f, 0.5f, 0}, 0.25f, 0.25f, 0.3f, 8, (Color){150, 150, 150, 255});
                }

                // Gouttes d'eau
                if (i >= 15 && i < 21) {
                    int dropIndex = i - 15;
                    (void)dropIndex;
                    Vector3 dropPos = buildingParticles[i];
                    DrawSphere(dropPos, 0.3f, (Color){0, 100, 255, 180});
                    DrawCylinder((Vector3){dropPos.x, dropPos.y - 0.4f, dropPos.z},
                                 0.1f, 0.05f, 0.2f, 6, (Color){0, 150, 255, 100});
                }

                // Panneau indicateur
                if (i == 0) {
                    DrawCube((Vector3){5.0f, 3.0f, 0}, 2.0f, 0.1f, 1.5f, (Color){255, 200, 50, 255});
                    DrawCube((Vector3){5.0f, 3.5f, 0}, 1.8f, 0.1f, 1.3f, (Color){255, 255, 255, 255});
                    DrawCylinder((Vector3){5.0f, 1.5f, 0}, 0.1f, 0.1f, 3.0f, 6, (Color){100, 100, 100, 255});
                    DrawCube((Vector3){5.0f, 3.5f, 0}, 0.8f, 0.05f, 0.8f, (Color){0, 100, 255, 255});
                    DrawSphere((Vector3){5.0f, 3.6f, 0}, 0.3f, (Color){0, 150, 255, 255});
                }
                break;
            case 6: // Sortie scolaire
                if (i < 8) {
                    // Corps des enfants
                    DrawCube(particles[i], 0.5f, 1.0f, 0.5f, particleColors[i]);
                    // Tête
                    DrawSphere((Vector3){particles[i].x, particles[i].y+0.8f, particles[i].z},
                               0.3f, (Color){255, 220, 180, 255});
                    // Yeux
                    DrawCube((Vector3){particles[i].x-0.1f, particles[i].y+0.85f, particles[i].z+0.2f},
                             0.08f, 0.08f, 0.1f, BLACK);
                    DrawCube((Vector3){particles[i].x+0.1f, particles[i].y+0.85f, particles[i].z+0.2f},
                             0.08f, 0.08f, 0.1f, BLACK);
                }

                // Ballons
                if (i < 8) {
                    Vector3 balloonPos = buildingParticles[i];
                    // Corde du ballon
                    DrawLine3D(balloonPos,
                              (Vector3){particles[i].x, particles[i].y+0.5f, particles[i].z},
                              (Color){200,200,200,200});
                    // Ballon
                    DrawSphere(balloonPos, 0.4f, particleColors[8 + i]);
                }

                // Sacs à dos
                if (i >= 8 && i < 16) {
                    int sacIndex = i - 8;
                    Vector3 backpackPos = buildingParticles[i];
                    DrawCube(backpackPos, 0.3f, 0.2f, 0.15f, particleColors[16 + sacIndex]);
                    // Bretelle
                    DrawCube((Vector3){backpackPos.x+0.15f, backpackPos.y+0.05f, backpackPos.z},
                             0.4f, 0.03f, 0.03f, (Color){80,80,80,255});
                }

                // Enseignants
                if (i >= 16 && i < 18) {
                    int teacherIndex = i - 16;
                    Vector3 teacherPos = buildingParticles[i];
                    DrawCube(teacherPos, 0.6f, 1.8f, 0.6f, particleColors[24 + teacherIndex]);
                    // Tête
                    DrawSphere((Vector3){teacherPos.x, teacherPos.y+1.1f, teacherPos.z},
                               0.35f, (Color){255, 200, 150, 255});
                    // Chapeau
                    DrawCube((Vector3){teacherPos.x, teacherPos.y+1.3f, teacherPos.z},
                             0.4f, 0.1f, 0.4f, teacherIndex == 0 ? BROWN : BLACK);
                }
                break;
            case 7: // Incendie
                if (i < buildingParticles.size()) {
                    DrawSphere(buildingParticles[i], 0.6f + sin(time*10+i)*0.2f, (Color){255,100,0,200});
                    // Fumée
                    DrawSphere((Vector3){buildingParticles[i].x, buildingParticles[i].y+1.5f, buildingParticles[i].z},
                              0.8f + sin(time*8+i)*0.3f, (Color){50,50,50,120});
                }
                DrawSphere(particles[i], 0.4f, (Color){255,50,0,150});
                break;
            case 8: // Fête
                DrawCube(particles[i], 0.15f, 0.15f, 0.15f, particleColors[i]);  // Confettis cubiques
                break;
        }
    }

    // Arbres décorés pour la fête des arbres
    if (type == 8) {
        for (int tree = 0; tree < 5; tree++) {
            float angle = tree * 72.0f * DEG2RAD;
            float x = cos(angle) * 8.0f;
            float z = sin(angle) * 8.0f;

            // Herbe autour de l'arbre
            DrawCylinder((Vector3){x, 0.05f, z}, 2.0f, 2.0f, 0.1f, 12, (Color){100, 180, 100, 255});
            // Tronc
            DrawCylinder((Vector3){x, 0.1f, z}, 0.4f, 0.3f, 1.5f, 10, (Color){101, 67, 33, 255});
            DrawCylinder((Vector3){x, 1.6f, z}, 0.3f, 0.2f, 1.0f, 8, (Color){120, 80, 40, 255});
            // Feuillage (3 sphères pour un effet plus naturel)
            DrawSphere((Vector3){x, 2.8f, z}, 1.3f, (Color){0, 140, 0, 220});
            DrawSphere((Vector3){x, 3.3f, z}, 1.1f, (Color){0, 160, 0, 200});
            DrawSphere((Vector3){x, 3.8f, z}, 0.9f, (Color){0, 180, 0, 180});

            // Guirlandes
            for (int garland = 0; garland < 8; garland++) {
                float garlandAngle = garland * 45.0f * DEG2RAD + time * 0.5f;
                float garlandHeight = 1.8f + garland * 0.2f;
                float garlandX = x + cos(garlandAngle) * 1.2f;
                float garlandZ = z + sin(garlandAngle) * 1.2f;

                Color garlandColors[] = {
                    (Color){255, 50, 50, 255},
                    (Color){50, 150, 255, 255},
                    (Color){255, 255, 50, 255},
                    (Color){50, 255, 50, 255},
                    (Color){255, 100, 255, 255},
                    (Color){255, 150, 50, 255}
                };

                DrawSphere((Vector3){garlandX, garlandHeight, garlandZ},
                           0.15f + sin(time * 3 + garland) * 0.02f,
                           garlandColors[garland % 6]);
            }

            // Boules de Noël
            for (int ball = 0; ball < 3; ball++) {
                float ballAngle = ball * 120.0f * DEG2RAD + time * 0.3f;
                float ballX = x + cos(ballAngle) * 0.8f;
                float ballZ = z + sin(ballAngle) * 0.8f;
                float ballY = 2.5f + ball * 0.4f;

                Color ballColors[] = {
                    (Color){255, 40, 40, 255},
                    (Color){40, 40, 255, 255},
                    (Color){255, 215, 0, 255},
                    (Color){255, 255, 255, 255},
                    (Color){40, 255, 40, 255},
                    (Color){255, 100, 180, 255}
                };

                DrawSphere((Vector3){ballX, ballY, ballZ},
                           0.25f + sin(time * 2 + ball) * 0.02f,
                           ballColors[(tree * 2 + ball) % 6]);

                // Reflet sur la boule
                DrawSphere((Vector3){ballX - 0.05f, ballY + 0.05f, ballZ + 0.05f},
                           0.05f, (Color){255, 255, 255, 150});
            }

            // Étoile au sommet
            DrawCube((Vector3){x, 4.2f, z}, 0.4f, 0.4f, 0.05f, (Color){255, 215, 0, 255});
            DrawCube((Vector3){x, 4.2f, z}, 0.05f, 0.4f, 0.4f, (Color){255, 215, 0, 255});
        }
    }
}

// FONCTIONS UTILITAIRES

void PlayGameSound(Sound sound) {
    // Utiliser l'API bas niveau de Raylib
    if (sound.stream.buffer != NULL) {
        PlayAudioStream(sound.stream);
    }
}
// Soustraction de deux Vector3
Vector3 Vector3SubtractCustom(Vector3 a, Vector3 b) { return (Vector3){a.x-b.x, a.y-b.y, a.z-b.z}; }

// Produit scalaire de deux Vector3
float Vector3DotProductCustom(Vector3 a, Vector3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

// Multiplication d'un Vector3 par un scalaire
Vector3 Vector3ScaleCustom(Vector3 v, float s) { return (Vector3){v.x*s, v.y*s, v.z*s}; }

// Addition de deux Vector3
Vector3 Vector3AddCustom(Vector3 a, Vector3 b) { return (Vector3){a.x+b.x, a.y+b.y, a.z+b.z}; }

// Distance entre deux Vector3
float Vector3DistanceCustom(Vector3 a, Vector3 b) {
    float dx = a.x-b.x, dy = a.y-b.y, dz = a.z-b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

// Calculer la collision d'un rayon avec un plan
Vector3 GetRayPlaneCollision(Ray ray, Vector3 planePoint, Vector3 planeNormal) {
    float denom = Vector3DotProductCustom(planeNormal, ray.direction);
    if (fabs(denom) > 1e-6) {
        Vector3 diff = Vector3SubtractCustom(planePoint, ray.position);
        float t = Vector3DotProductCustom(diff, planeNormal) / denom;
        if (t >= 0) return Vector3AddCustom(ray.position, Vector3ScaleCustom(ray.direction, t));
    }
    return (Vector3){0,0,0};  // Pas de collision
}

// Obtenir la couleur d'un bâtiment selon son type
Color GetBuildingColor(char type) {
    switch(type) {
        case 'H': return (Color){135,206,250,255};  // Maison: bleu clair
        case 'U': return DARKGRAY;                  // Usine: gris foncé
        case 'P': return (Color){144,238,144,255};  // Parc: vert clair
        case 'E': return (Color){255,182,193,255};  // École: rose
        case 'S': return GOLD;                      // Stade: or
        default: return GRAY;
    }
}

// Créer un bâtiment à partir des données d'un formulaire
Batiment* CreateBuildingFromForm(InputForm& form, Vector3 pos) {
    try {
        switch(form.buildingType) {
            case 1: // Maison
                return new Maison(form.values[0], stod(form.values[1]), stod(form.values[2]),
                                 stoi(form.values[3]), stoi(form.values[4]));
            case 2: // Usine
                return new Usine(form.values[0], stod(form.values[1]), stod(form.values[2]),
                                stod(form.values[3]));
            case 3: // Parc
                return new Parc(form.values[0], stod(form.values[1]), stod(form.values[2]),
                               stod(form.values[3]));
            case 4: // École
                return new Ecole(form.values[0], stod(form.values[1]), stod(form.values[2]),
                                stoi(form.values[3]), stoi(form.values[4]), stoi(form.values[5]));
            case 5: // Stade
                return new Stade(form.values[0], stod(form.values[1]), stod(form.values[2]),
                                stoi(form.values[3]));
        }
    } catch(...) {
        return nullptr;  // En cas d'erreur de conversion
    }
    return nullptr;
}

// Déclencher un événement aléatoire dans la simulation
void triggerEvent(Simulation* sim, Ville* ville, VisualEvent& visualEvent,
                  vector<BuildingData>& buildings, int& selectedId, int& globalSelectedId) {
    bool hasUsine = false;
    bool hasParc = false;
    bool hasEcole = false;
    Batiment* usineTarget = nullptr;
    Vector3 usinePos = {0,0,0};

    // Vérifier quels bâtiments sont présents
    for (auto bd : buildings) {
        if (bd.batiment->getType() == 'U') {
            hasUsine = true;
            usineTarget = bd.batiment;
            usinePos = bd.position;
        }
        if (bd.batiment->getType() == 'P') hasParc = true;
        if (bd.batiment->getType() == 'E') hasEcole = true;
    }

    int eventType;
    vector<int> possibleEvents;

    // Déterminer les événements possibles selon les bâtiments présents
    for (int i = 0; i <= 7; i++) {
        if (i == 5 && (!hasParc || !hasEcole)) continue;  // Sortie scolaire nécessite parc et école
        if (i == 6 && !hasUsine) continue;                // Incendie nécessite une usine
        possibleEvents.push_back(i);
    }

    if (possibleEvents.empty()) return;  // Aucun événement possible

    // Choisir un événement aléatoire parmi les possibles
    eventType = possibleEvents[GetRandomValue(0, possibleEvents.size()-1)];

    string messages[] = {"NUEES DE PIGEONS!", "PANNE COURANT!", "GREVE!", "EMBOUTEILLAGE!",
                        "PROJET_SOLAIRE_PARTICIPATIF", "SORTIE SCOLAIRE!", "INCENDIE USINE!", "FETE ARBRES!"};

    // Gestion spéciale pour l'incendie d'usine (qui détruit un bâtiment)
    if (eventType == 6 && usineTarget) {
              PlayGameSound(fireSound);
        visualEvent.trigger(eventType + 1, messages[eventType], usinePos);
        sim->declencherEvenement(eventType);

        visualEvent.buildingToRemoveId = usineTarget->getId();

        // Désélectionner l'usine si elle était sélectionnée
        if (selectedId == usineTarget->getId()) {
            selectedId = -1;
            globalSelectedId = -1;
        }
    } else {
        visualEvent.trigger(eventType + 1, messages[eventType]);
        sim->declencherEvenement(eventType);
    }
}
