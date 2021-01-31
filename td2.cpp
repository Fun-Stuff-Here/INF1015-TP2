#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include "cppitertools/range.hpp"
#include "gsl/span"
using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ajouterFilm(ListeFilms& listeFilms, Film* filmPtr)
{

	if (listeFilms.nElements >= listeFilms.capacite )
	{
		//doubler
		int nouvelleCapacite = listeFilms.capacite*2;
			
		//allocation memoire
		ListeFilms nouvelleListeFilms;
		nouvelleListeFilms.elements = new Film* [nouvelleCapacite];
		for(int i; i<nouvelleCapacite;i++)
		{
			nouvelleListeFilms.elements[i] = new Film;
			//nouvelleListeFilms[i]->acteurs. = new ListeActeurs; 
		}


			
			//copier
		for(int i = 0; i < listeFilms.nElements; i++)
		{
			nouvelleListeFilms.elements[i] = listeFilms.elements[i];
		}

		//delete
		delete[] listeFilms.elements;//maybe delete chaque pointeur dedans


		//update listeFilms
		listeFilms.capacite = nouvelleCapacite;
		listeFilms.elements = nouvelleListeFilms.elements;
	}
	//add film
	listeFilms.elements[listeFilms.nElements] = filmPtr;
		
	//update listeFilms
	listeFilms.nElements++;


}


Acteur* trouverActeur(string nomRechercher, ListeActeurs& listeActeurs)
{	
		
	for (int i = 0; i < listeActeurs.nElements; i++)
	{
		if (listeActeurs.elements[i]->nom == nomRechercher)
			return listeActeurs.elements[i];
	}
	return nullptr;
}
//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film;
//la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void enleverFilm(ListeFilms& listeFilms, Film* filmPtr)
{
	for (int i = 0; i < listeFilms.nElements; i)
	{
		if (filmPtr== listeFilms.elements[i])
		{
			listeFilms.elements[i] = nullptr;
			listeFilms.nElements--;
			i--;
		}
		else
		{
			listeFilms.elements[i] = listeFilms.elements[i];
		}
		
	}
}

//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.





//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
Acteur* lireActeur(istream& fichier, ListeActeurs& listeActeurs)
{
	Acteur* actPtr; 
	actPtr = new Acteur;
	actPtr	->nom = lireString(fichier);
	actPtr -> anneeNaissance = lireUint16 (fichier);
	actPtr -> sexe = lireUint8(fichier);
	Acteur* acteurExistant = trouverActeur(actPtr->nom, listeActeurs);
	//check si existant
	if (acteurExistant != nullptr)
	{
		delete actPtr;
		actPtr = nullptr;
		return acteurExistant;
	}
	else
	{	//Debug line
		cout << "L'acteur n'existe pas" << endl;
		cout<< actPtr->nom<<endl;
		//allocation memoire
		actPtr->joueDans.elements = new Film * [1];
		actPtr->joueDans.nElements =0;
		actPtr->joueDans.capacite =1; 
		return  actPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
	}
}


Film* lireFilm(istream& fichier)
{
	Film* filmPtr;
	filmPtr = new Film;
	filmPtr->titre = lireString(fichier);
	filmPtr->realisateur = lireString(fichier);
	filmPtr->anneeSortie = lireUint16(fichier);
	filmPtr->recette = lireUint16(fichier);

	filmPtr->acteurs.nElements = lireUint8 (fichier); 
	if (filmPtr->acteurs.nElements == 0)
		filmPtr->acteurs.capacite = 1;
	else
		filmPtr->acteurs.capacite = filmPtr->acteurs.nElements;

	//Allocation de la liste d'acteur
	filmPtr ->acteurs.elements = new Acteur*[filmPtr -> acteurs.capacite];
	for (int i = 0; i < filmPtr ->acteurs.nElements;i++)
	{
		//ListeFilms l;
		filmPtr->acteurs.elements[i] = new Acteur;
		//film.acteurs.elements[i]->joueDans = l;
		//film.acteurs.elements[i]->joueDans.elements = new Film*[1];
		//film.acteurs.elements[i]->joueDans.nElements = 0;
		//film.acteurs.elements[i]->joueDans.capacite = 1;
	}



	 //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	for (int i = 0; i < filmPtr->acteurs.nElements; i++) {

		filmPtr -> acteurs.elements[i] = lireActeur(fichier, filmPtr->acteurs); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		ajouterFilm(filmPtr->acteurs.elements[i]->joueDans, filmPtr);
	}
	return filmPtr; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms creerListe(string nomFichier)
{

	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	int nElements = lireUint16(fichier);

	//TODO: Créer une liste de films vide.
	ListeFilms listeFilms = {1, 0, new Film* [1]};
	for (int i = 0; i < nElements; i++) {
		listeFilms.elements[i] = lireFilm(fichier); //TODO: Ajouter le film à la liste.
	}


	return listeFilms; //TODO: Retourner la liste de films.
}

//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.

//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).

void afficherListeFilms(const ListeFilms& listeFilms)
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = {};
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	for (int i = 0; i < listeFilms.nElements; i++) {
		//TODO: Afficher le film.
		cout << ligneDeSeparation;
	}
}

void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = nullptr;
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms(acteur->joueDans);
}

int main()
{
	#ifdef VERIFICATION_ALLOCATION_INCLUS
	bibliotheque_cours::VerifierFuitesAllocations verifierFuitesAllocations;
	#endif
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Detected memory leak" de "4 bytes" affiché dans la "Sortie", qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms = creerListe("films.bin");
	
	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	afficherListeFilms(listeFilms);
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	
	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	
	//TODO: Détruire tout avant de terminer le programme.  L'objet verifierFuitesAllocations devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
}
