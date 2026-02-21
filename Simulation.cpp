#include "Simulation.h"

// Constructeur : initialise la simulation avec une ville et une saison par défaut (Hiver)
Simulation::Simulation(Ville* v) : maville(v), saisonCourante('H') {
        srand(time(0)); // Initialisation du générateur de nombres aléatoires pour les événements
    }

// Change la saison courante et affiche un message
void Simulation::changerSaison(char s) {
        saisonCourante = s;
        cout << "Saison actuelle : " << saisonCourante << endl;
        // le lancer de cycle n'est pas automatique pour plus de contrôle
    }

// Lance un cycle général pour tous les bâtiments de la ville
void Simulation::lancerCycleGeneral() {
        cout << "=== Lancement du cycle pour la saison " << saisonCourante << " ===" << endl;

        // Variables pour les totaux de consommation et production
        double totalEau = 0;
        double totalElec = 0;
        double totalSatisfaction = 0;
        double totalProduction =0;

        // Parcours tous les bâtiments pour appliquer leur cycle spécifique
        for (auto b : maville->getBatiments()) {
            b->lancerCycle(saisonCourante);  // Chaque bâtiment applique ses modifications saisonnières

            // Cumul des consommations et satisfaction
            totalEau += b->getConsommationEau();
            totalElec += b->getConsommationElectricite();
            totalSatisfaction += b->getEffetSatisfaction();

            // Si c'est une usine, cumuler la production
            Usine* u = dynamic_cast<Usine*>(b);
            if (u)
            {
                totalProduction += u->getProduction();
            }
        }

        // Mise à jour des ressources de la ville après consommation
        maville-> setEau(maville->geteauInit() - totalEau);
        maville->setElectricite(maville->getelecInit()- totalElec);

        // Calcul de la satisfaction moyenne de la ville
        if (!maville->getBatiments().empty())
            maville->calculerSatisfaction();

        // Mise à jour de la population (peut dépendre de la satisfaction)
        maville->majPopulation();
        maville->calculerSatisfaction();  // Recalcul après maj population

        // Normalisation des valeurs (entre 0 et 100)
        if (maville->getSatisfaction() < 0)   maville->setSatisfaction(0);
        if (maville->getSatisfaction() > 100) maville->setSatisfaction(100);

        // Normalisation de la satisfaction de chaque bâtiment
        for (auto b : maville->getBatiments()){
            if (b->getEffetSatisfaction()<0) b->setEffetSatisfaction(0);
            if(b->getEffetSatisfaction()>100) b->setEffetSatisfaction(100);
        }
    }

// Réinitialise les cycles de tous les bâtiments et de la ville
void Simulation:: resetCycleGeneral() {
        maville->resetCycleVille();  // Réinitialise les ressources de la ville
        cout << "Tous les cycles ont été réinitialisés." << endl;
    }

// Getter pour la saison courante
char Simulation::getSaison(){
    return saisonCourante;
}

// Déclenche un événement spécifique selon le code fourni
void Simulation:: declencherEvenement(int c) {
    // Structure de contrôle pour les différents types d'événements
    switch(c) {
        case 0:  // Événement : invasion de pigeons
            cout << "Une nuee de pigeons geants envahit votre ville !\n";
            cout << "Trop de dechets dans les rues ! Satisfaction -5%\n";
            maville->setSatisfaction(maville->getSatisfaction() - 7);
            maville->CalculerSatisfactionBat(-7);  // Applique à tous les bâtiments
            cout << "Le service de nettoyage remet de l'ordre. Satisfaction +10%\n";
            maville->setSatisfaction(maville->getSatisfaction() + 4);
            maville->CalculerSatisfactionBat(4);
            break;

        case 1:  // Événement : panne de courant
            cout << "Panne de courant !\n";
            cout << "la Satisfaction de la ville diminue de 5% ,\n Tous les batiments perdent 5% de leur effet satisfaction\n";
            cout<<"\nType Usine : pollution , production , conommation diminuent !";
            cout<<"\nconsommation d'electricite de la ville et des batimets diminue";

            maville->setElectricite(maville->getElectricite() * 0.5);  // Réduction de 50%

            cout << "Satisfaction -5%\n";
            maville->setSatisfaction(maville->getSatisfaction() - 5);
            maville->CalculerSatisfactionBat(-5);

            // Applique l'effet à chaque bâtiment
            for (auto b : maville->getBatiments()){
                b->setConsommationElectricite(-b->getConsommationElectricite()*0.5);
                Usine* u = dynamic_cast<Usine*>(b);
                if (u)  // Effets spécifiques aux usines
                {
                    u->setProduction(u->getProduction() * 0.7);  // -30% production
                    u->setPollution(u->getProduction()*0.05);    // Pollution proportionnelle
                }
            }
            break;

        case 2:  // Événement : grève des jardiniers
            cout << "Greve des jardiniers !\n";
            cout << "Satisfaction -10%\n";
            maville->setSatisfaction(maville->getSatisfaction() - 10);
            maville->CalculerSatisfactionBat(-10);

            // Affecte spécifiquement les parcs
            for (auto b : maville->getBatiments()) {
                if (b->getType() == 'P') {
                    Parc* p = dynamic_cast<Parc*>(b);
                    if (p) p->ameliorerBienEtre(-5);  // Réduction du bien-être
                }
            }
            break;

        case 3:  // Événement : embouteillage
            cout << "Embouteillage geant ! Satisfaction -7%\n";
            maville->setSatisfaction(maville->getSatisfaction() - 7);
            maville->CalculerSatisfactionBat(-7);
            break;

        case 4:  // Événement : investissement dans les énergies renouvelables
        {
            cout<< "la ville investit dans un sondage citoyen et des panneaux solaires Urbains dans les batiments publics et les usines"<< endl;
            maville->setSatisfaction(maville->getSatisfaction()+30);
            maville->CalculerSatisfactionBat(30);

            // Réduction de la consommation électrique de tous les bâtiments
            for (auto b : maville->getBatiments()){
                b->setConsommationElectricite(-b->getConsommationElectricite()*0.5);
                Usine* u = dynamic_cast<Usine*>(b);
                if (u)  // Réduction supplémentaire de pollution pour les usines
                    u->setPollution(u->getPollution()-2);
            }
            maville->setBudget(maville->getBudget() - 1500);  // Coût de l'investissement
            break;
        }

        case 5:  // Événement : sortie scolaire
            cout << " Sortie scolaire au parc !\n";
            cout << "Les eleves profitent du plein air et apprennent dans la nature.\n";

            // Effet général sur la ville
            cout << "Satisfaction +5%\n";
            maville->setSatisfaction(maville->getSatisfaction() + 5);
            maville->CalculerSatisfactionBat(+5);

            // Effet sur les écoles : amélioration éducative
            for (auto b : maville->getBatiments()) {
                if (b->getType() == 'E') {
                    Ecole* e = dynamic_cast<Ecole*>(b);
                    if (e) {
                        cout << " -> L'école " << e->getNom() << " améliore son effet éducatif.\n";
                        e->SetEducation(3.0);  // +3 effet éducation
                    }
                }
            }

            // Effet sur les parcs : augmentation du bien-être
            for (auto b : maville->getBatiments()) {
                if (b->getType() == 'P') {
                    Parc* p = dynamic_cast<Parc*>(b);
                    if (p) {
                        cout << " -> Le parc " << p->getNom() << " gagne en popularité.\n";
                        p->ameliorerBienEtre(1.0);  // +1 effet bien-être
                    }
                }
            }

            // Coût financier de l'événement
            cout << "Cout du transport : -100 Budget\n";
            maville->setBudget(maville->getBudget() - 100);
            break;

        case 6:  // Événement : incendie d'usine
        {
            vector<Usine*> usinesDispo;

            // Récupération de toutes les usines disponibles
            for (auto b : maville->getBatiments()) {
                Usine* u = dynamic_cast<Usine*>(b);
                if (u) usinesDispo.push_back(u);
            }

            // Vérification qu'il y a au moins une usine
            if (usinesDispo.empty()) {
                cout << "\n Aucune usine disponible dans la ville !\n";
                break;
            }

            cout << "\n=== DECLENCHEMENT D'UN INCENDIE DANS UNE USINE ===\n";

            // Sélection aléatoire d'une usine
            int index = rand() % usinesDispo.size();
            Usine* u = usinesDispo[index];

            cout << "L'usine " << u->getNom() << " #"
                 << u->getId() << " a pris feu !\n";

            maville->supprimerBatiment(u->getId());  // Suppression de l'usine

            maville->setSatisfaction(maville->getSatisfaction() - 10);
            maville->CalculerSatisfactionBat(-10);

            break;
        }

        case 7:  // Événement : fête des arbres
        {
            cout << "\n===  FÊTE DES ARBRES  ===\n";
            cout << "La ville organise une grande célébration dédiée à la nature.\n";
            cout << "Les habitants plantent des arbres et profitent des espaces verts.\n";

            // Bonus de satisfaction générale
            cout << "Satisfaction +12%\n";
            maville->setSatisfaction(maville->getSatisfaction() + 12);
            maville->CalculerSatisfactionBat(+12);
            maville->setBudget(maville->getBudget()-100);  // Coût de l'organisation

            // Boost des parcs : augmentation du bien-être
            for (auto b : maville->getBatiments()) {
                if (b->getType() == 'P') {
                    Parc* p = dynamic_cast<Parc*>(b);
                    if (p) {
                        p->ameliorerBienEtre(+3);  // +3 bien-être
                    }
                }
            }

        }
    }

    // Normalisation finale des valeurs après l'événement
    if (maville->getSatisfaction() < 0)   maville->setSatisfaction(0);
    if (maville->getSatisfaction() > 100) maville->setSatisfaction(100);
    for (auto b : maville->getBatiments()){
        if (b->getEffetSatisfaction()<0) b->setEffetSatisfaction(0);
        if(b->getEffetSatisfaction()>100) b->setEffetSatisfaction(100);
    }
}
