
#include "ListeFilms.hpp"
//#include "structures.hpp"


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

ListeFilms::ListeFilms(int capacite)
{
	nElements_ = 0;
	capacite_ = capacite;
	elements_ = new Film*[capacite_];
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
	//while (nElements_ != 0)
	//{
	//	detruireFilm(elements_[0]);
	//}
	delete[] elements_;
}


Film* ListeFilms::lireFilm(istream& fichier)
{
	Film* filmPtr;
	filmPtr = new Film;
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
	Acteur* actPtr;
	actPtr = new Acteur;
	actPtr->nom = lireString(fichier);
	actPtr->anneeNaissance = lireUint16(fichier);
	actPtr->sexe = lireUint8(fichier);

	Acteur* acteurExistant = trouverActeur(actPtr->nom);
	//check si existant
	if (acteurExistant != nullptr)
	{
		delete actPtr;
		actPtr = nullptr;
		return acteurExistant;
	}
	else
	{	//Debug line
		cout << actPtr->nom << endl;
		//allocation memoire
		actPtr->joueDans = *new ListeFilms;

		return  actPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
	}
}

Acteur* ListeFilms::trouverActeur(string nom) const
{
	Acteur* ptrActeur = nullptr;
	for (int i = 0; i < size(); i++)
	{
		ptrActeur = trouverActeurListeActeurs(nom, getFilms()[i]->acteurs);
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

Film** ListeFilms::getFilms() const
{
	return elements_;
}

int ListeFilms::size() const
{

	return nElements_;
}

void ListeFilms::detruireFilm(Film* ptrFilm)
{
	//joueDans rien
	for (int i = 0; i < ptrFilm->acteurs.nElements; i++)
	{
		Acteur* ptrActeur = ptrFilm->acteurs.elements[i];
		ptrActeur->joueDans.enleverFilm(ptrFilm);
		if (!(ptrActeur->joueDans.size()))
		{
			//Debug
			cout << "Destruction de l'acteur : " << ptrActeur->nom << endl;
			ptrActeur->joueDans.~ListeFilms();
			delete ptrActeur;
			ptrActeur = nullptr;
		}

	}
	enleverFilm(ptrFilm);
	//relacher la memoire du film
	//delete[] ptrFilm->acteurs.elements;
	//ptrFilm->acteurs.elements = nullptr;
	//delete ptrFilm;
	//ptrFilm = nullptr;
}


void ListeFilms::enleverFilm(Film* ptrFilm)
{
	for (int i = 0; i < size(); i++)
	{
		if (ptrFilm == getFilms()[i])
		{
			elements_[i] = elements_[--nElements_];
			elements_[nElements_] = nullptr;
			break;
		}

	}
}

void ListeFilms::ajouterFilm(Film* ptrFilm)
{
	if (size()>=capacite_)
		doublerCapacite();
	//add film
	setFilm(size(), ptrFilm);

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
	for (int i = 0; i < size(); i++)
	{
		setFilm(i,oldElements[i]);
	}
	//delete
	delete[] oldElements;
	oldElements = nullptr;
}



ListeFilms ListeFilms::copy(int capacite)
{


	ListeFilms l = ListeFilms{capacite};
	for (unsigned i = 0; i < size(); i++)
	{
		l.setFilm(i, getFilm(i));
	}
	l.capacite_ = capacite_;
	l.nElements_ = size();
	return l;
}

Film* ListeFilms::getFilm(unsigned index) const
{
	if (index >= capacite_)
		throw std::out_of_range("Indice trop grand");
	return elements_[index];

}

void ListeFilms::setFilm(unsigned index, Film* ptrFilm)
{
	if (index >= capacite_)
		throw std::out_of_range("Indice trop grand");
	elements_[index] = ptrFilm;
}



void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35mфффффффффффффффффффффффффффффффффффффффф\033[0m\n";
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	span<Film*> lFilms{getFilms(), size() };
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
