
#include "ListeFilms.hpp"

typedef uint8_t UInt8;
typedef uint16_t UInt16;

using namespace std;
using namespace gsl;
using namespace iter;


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


 ListeFilms::ListeFilms()
{
	nElements_ = 0;
	capacite_ = 1;
	elements_ = new Film * [capacite_];
}


 ListeFilms::ListeFilms(std::string nomFichier)
{

	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	nElements_ = 0;
	capacite_ = 1;
	int nFilms = lireUint16(fichier);
	elements_ = new Film * [capacite_];

	//TODO: Créer une liste de films vide.
	for (int i = 0; i < nFilms; i++) {
		Film* ptrFilm = lireFilm(fichier);
		ajouterFilm(ptrFilm);//TODO: Ajouter le film à la liste.
	}
}


ListeFilms::~ListeFilms()
{
	//vu à la semaine 4, utiliser la méthode detruireListeFilms à la place
}

void ListeFilms::detruireListeFilms()
{
	while (nElements_ != 0)
	{
	detruireFilm(elements_[0]);
	}
	delete[] elements_;
	elements_ = nullptr;
}


Film* ListeFilms::lireFilm(istream& fichier)
{
	Film* filmPtr = new Film;
	filmPtr->titre = lireString(fichier);
	filmPtr->realisateur = lireString(fichier);
	filmPtr->anneeSortie = lireUint16(fichier);
	filmPtr->recette = lireUint16(fichier);

	filmPtr->acteurs.nElements = lireUint8(fichier);
	filmPtr->acteurs.capacite = filmPtr->acteurs.nElements;

	//Allocation de la liste d'acteur
	filmPtr->acteurs.elements = new Acteur * [filmPtr->acteurs.capacite];
	//NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	for (int i = 0; i < filmPtr->acteurs.nElements; i++) {
		filmPtr->acteurs.elements[i] = lireActeur(fichier); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		filmPtr->acteurs.elements[i]->joueDans.ajouterFilm(filmPtr);
	}
	return filmPtr; //TODO: Retourner le pointeur vers le nouveau film.
}


Acteur* ListeFilms::lireActeur(istream& fichier)
{
	Acteur* actPtr = new Acteur;
	actPtr->nom = lireString(fichier);
	actPtr->anneeNaissance = lireUint16(fichier);
	actPtr->sexe = lireUint8(fichier);

	Acteur* acteurExistant = trouverActeur(actPtr->nom);
	//check si existant
	if (acteurExistant != nullptr)
	{
		actPtr->joueDans.detruireListeFilms();
		delete actPtr;
		actPtr = nullptr;
		return acteurExistant;
	}
	else
	{	//Debug line
		cout << actPtr->nom << endl;
		return  actPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
	}
}

Acteur* ListeFilms::trouverActeur(string nom) const
{
	Acteur* ptrActeur = nullptr;
	for (int i = 0; i < nElements_; i++)
	{
		ptrActeur = trouverActeurListeActeurs(nom, elements_[i]->acteurs);
		if (ptrActeur != nullptr)
			break;
	}
	return ptrActeur;
}

Acteur* ListeFilms::trouverActeurListeActeurs(string nomRechercher, ListeActeurs& listeActeurs) const
{

	for (int i = 0; i < listeActeurs.nElements; i++)
	{
		if (listeActeurs.elements[i]->nom == nomRechercher)
			return listeActeurs.elements[i];
	}
	return nullptr;
}




void ListeFilms::detruireFilm(Film* ptrFilm)
{
	for (int i = 0; i < ptrFilm->acteurs.nElements; i++)
	{
		Acteur* ptrActeur = ptrFilm->acteurs.elements[i];
		ptrActeur->joueDans.enleverFilm(ptrFilm);
		if (!(ptrActeur->joueDans.nElements_))
		{
			//Debug
			cout << "Destruction de l'acteur : " << ptrActeur->nom << endl;
			ptrActeur->joueDans.detruireListeFilms();
			delete ptrActeur; 
			ptrActeur = nullptr;
		}

	}
	enleverFilm(ptrFilm);
	//relacher la memoire du film
	delete[] ptrFilm->acteurs.elements;
	ptrFilm->acteurs.elements = nullptr;
	delete ptrFilm; 
	ptrFilm = nullptr;
}


void ListeFilms::enleverFilm(Film* ptrFilm)
{
	for (int i = 0; i < nElements_; i++)
	{
		if (ptrFilm == elements_[i])
		{
			elements_[i] = elements_[--nElements_];
			elements_[nElements_] = nullptr;
			break;
		}

	}
}

void ListeFilms::ajouterFilm(Film* ptrFilm)
{
	if (nElements_>=capacite_)
		doublerCapacite();
	//add film
	elements_[nElements_]= ptrFilm;

	//update listeFilms
	nElements_++;

}


void ListeFilms::doublerCapacite()
{
	//doubler
	capacite_ *= 2;
	Film** oldElements = elements_;
	elements_ = new Film * [capacite_];
	//copier
	for (int i = 0; i < nElements_; i++)
	{
		elements_[i] =oldElements[i];
	}
	//delete
	delete[] oldElements;
	oldElements = nullptr;
}



void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35mфффффффффффффффффффффффффффффффффффффффф\033[0m\n";
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> lFilms{elements_, nElements_};
	for (int i = 0; i < lFilms.size(); i++) {
		//TODO: Afficher le film.
		afficherFilm(*lFilms[i]);
		cout << ligneDeSeparation;
	}
}


void ListeFilms::afficherFilm(const Film& film)const
{
	cout << " " << film.titre << ", " << film.realisateur << ", " << film.anneeSortie << ", " << film.recette << endl;
	for (int i = 0; i < film.acteurs.nElements; i++)
	{
		afficherActeur(*film.acteurs.elements[i]);
	}
}

void ListeFilms::afficherActeur(const Acteur& acteur) const
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}


Film* ListeFilms::operator[] (std::size_t const index) const
{
	return elements_[index];
}


void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = listeFilms.trouverActeur(nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		acteur->joueDans.afficherListeFilms();
}