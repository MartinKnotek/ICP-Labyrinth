/*
 * file: projekt.cpp
 * author: Martin Knotek
 * e-mail: xknote10@stud.fit.vutbr.cz
 * date: 16.4.2015
*/

/**
* @file projekt.cpp
*
* @author Martin Knotek
*
* Kontakt xknote10@stud.fit.vutbr.cz
*
* @date 16.4.2015
*
* @brief Hlavni soubor aplikace
*
* Tento soubor obsahuje projekt do ICP.
*
* @todo Dopsat komentář @brief v hlavnim popisu
*/


#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>
#include <regex>
//#include "main.h"

#define SOURADNICE_VOLNY_KAMEN 1,0
#define NATOCENI 1
#define POSUNUTI 2
#define POHYB 3
#define ULOZ 4
#define DALSI_HRAC 5
using namespace std;


/**
* @class Kamen
* @brief Hraci kamen na desce
*
* @todo Pridat popis ze zadani!!! //POZOR
* @author Martin Knotek
*/
class Kamen{
  friend class Deska; //trida Deska muze pristupovat k private tridy Kamen
  friend class Hra; //trida Hra muze pristupovat k private tridy Kamen
  char tvar{0}, predmet{' '};
  int natoceni{0};
public:
  static int count_I, count_L, count_T;


  /**
   * @brief Konstruktor
   * @param t je tvar kamene [I,L,T]
   * @param n je natoceni kamene <0,3>
   * @param p je predmet na kameni
   */
  Kamen(char t, int n, char p){
    tvar=t;
    predmet=p;
    natoceni=n;
    if (t=='I') count_I++;
    else if (t=='L') count_L++;
    else if (t=='T') count_T++;
    //Musi se zadat I,L nebo T, jinak se nic neprovede
  }


  /**
   * @brief Zmeni natoceni kamene
   * @param n je natoceni kamene <0,3>
   */
  void zmen_natoceni(int n){ //n musi byt v intervalu <0,3>
    natoceni=n;
  }


  /**
   * @brief Zmeni tvar kamene
   * @param t je tvar kamene [I,L,T]
   */
  void zmen_tvar(char t){ //t musi byt v I,L nebo T
    tvar=t;
  }

  /**
   * @brief Zmeni/prida predmet na kameni
   * @param p je predmet kamene
   */
  void zmen_predmet(char p){
    predmet=p;
  }


//  ///@brief
//  ///@param
//  Kamen op_s_predmet(char operace);
};
int Kamen::count_I=0;
int Kamen::count_L=0;
int Kamen::count_T=0;


/**
* @class Deska
* @brief Hraci deska
*
* @todo Pridat popis ze zadani!!! //POZOR
* @author Martin Knotek
*/
class Deska{
  friend class Hra; //trida Hra muze pristupovat k private tridy Deska
  int N;  //delka strany hraci desky
  vector<Kamen*> v;
  int nahodne;  //pro ulozeni hodnoty z random funkce
  int neg_I,neg_L,neg_T;  //viz metoda vytvor_nahodny_kamen()
  int pom;
  Kamen *aktual;  //pomocna promenna pro metodu pruchodny_smer()


  /**
   * @brief prepocte souradnice i,j na index vektoru
   * @param i radek
   * @param j sloupec
   * @return vraci prepocitane souradnice
   */
  int souradnice(int i,int j){
    return (i-1)*N+j;
  }


  /**
   * @brief vytvori volny kamen
   *
   * Tento kamen lezi mimo hraci desku a prvni hrac s nim zacina.
   */
  void vytvor_volny_kamen(){
    if (!(N%3)) { //kvuli algoritmu v metode vytvor_nahodny_kamen()
      pom++;
      vytvor_nahodny_kamen(SOURADNICE_VOLNY_KAMEN);
      pom--;
    }
    else
      vytvor_nahodny_kamen(SOURADNICE_VOLNY_KAMEN);
  }


  /**
  * @brief Rozmisti kameny na hraci desku
  */
  void rozmisti_kameny(){
    srand(time(0)); // pouziti aktualniho casu jako 'seed' pro random generator
    for(int i=1;i<=N;i+=2)  //liche radky
    for(int j=1;j<=N;j+=2){ //liche sloupce
//      cout<<"pridavam na souradnice "<<i<<" "<<j<<endl; //SMAZ
      v[souradnice(i,j)]=new Kamen('T',rand()%4,' ');
    }
    //rohove kameny - tvar a natoceni
    v[souradnice(1,1)]->zmen_tvar('L');
    v[souradnice(1,N)]->zmen_tvar('L');
    v[souradnice(N,1)]->zmen_tvar('L');
    v[souradnice(N,N)]->zmen_tvar('L');
    v[souradnice(1,1)]->zmen_natoceni(1); //vlevo nahore
    v[souradnice(1,N)]->zmen_natoceni(2); //vpravo nahore
    v[souradnice(N,1)]->zmen_natoceni(0); //vpravo dole
    v[souradnice(N,N)]->zmen_natoceni(3); //vlevo dole
    Kamen::count_L+=4;
    Kamen::count_T-=4;

    //natoceni krajnich kamenu tvaru T na lichych indexech
    for(int i=3;i<N;i+=2){
      v[souradnice(1,i)]->zmen_natoceni(0); //horni strana
      v[souradnice(i,N)]->zmen_natoceni(1); //prava strana
      v[souradnice(N,i)]->zmen_natoceni(2); //spodni strana
      v[souradnice(i,1)]->zmen_natoceni(3); //leva strana
    }

//    cout<<"Pocet tvaru 1: "<<endl<<"I: "<<Kamen::count_I<<endl<<
//      "L: "<<Kamen::count_L<<endl<<"T: "<<Kamen::count_T<<endl<<endl; //SMAZ

    //pomocna promena pro vypocet neg_[I,L,T] v metode vytvor_nahodny_kamen
    pom=(N%3?(N*N/3)+1:N*N/3);
    //srand(time(0)); // pouziti aktualniho casu jako 'seed' pro random generator
    for(int i=1;i<=N;i+=2)  //liche radky
    for(int j=2;j<N;j+=2){  //sude sloupce
      vytvor_nahodny_kamen(i,j);
    }
//    cout<<"Pocet tvaru 2: "<<endl<<"I: "<<Kamen::count_I<<endl<<
//      "L: "<<Kamen::count_L<<endl<<"T: "<<Kamen::count_T<<endl<<endl; //SMAZ
    for(int i=2;i<N;i+=2) //sude radky
    for(int j=1;j<=N;j++){  //vsechny sloupce
      vytvor_nahodny_kamen(i,j);
    }
    vytvor_volny_kamen();  //vytvori volny kamen (kamen mimo hraci desku)
  }


  /**
   * @brief Vytvori nahodny kamen
   *
   * Vytvori kamenen v urcitem tvaru. Cim vice kamenu daneho tvaru je jiz
   * vytvorenych na hraci desce, tim mensi sance, ze tento tvar kamen dostane
   * @param i je souradnice radku
   * @param j je souradnice sloupce
   */
  void vytvor_nahodny_kamen(int i,int j){
    //nastaveni promennych pro random - cim vyssi cislo, tim vetsi sance
    neg_I=pom-Kamen::count_I; //0-n %
    neg_L=pom-Kamen::count_L; //n-m %
    neg_T=pom-Kamen::count_T; //m-100 %

//    cout<<"neg_I: "<<neg_I<<"   "<<rand()%4<<endl;  //SMAZ
//    cout<<"neg_L: "<<neg_L<<endl;  //SMAZ
//    cout<<"neg_T: "<<neg_T<<endl;  //SMAZ

    nahodne=rand()%(neg_I+neg_L+neg_T);  //0-100 procent

    if (nahodne<neg_I)
      v[souradnice(i,j)]=new Kamen('I',rand()%4,' ');
    else if (nahodne<neg_L+neg_I)
      v[souradnice(i,j)]=new Kamen('L',rand()%4,' ');
    else
      v[souradnice(i,j)]=new Kamen('T',rand()%4,' ');
  }


  /**
   * @brief Zjisti zda ma kamen pruchodnou stranu v danem smeru
   * @param smer W - sever, S -jih, A - zapad, D - vychod
   * @param i souradnice - radek
   * @param j souradnice - sloupec
   * @return vraci true pokud je kamen v danem smeru pruchodny, jinak false
   */
  bool pruchodny_smer(char smer,int i,int j){
    aktual=v[souradnice(i,j)];
    switch (smer){
    case 'W':
      if(((aktual->tvar=='I')&&((aktual->natoceni==1)||(aktual->natoceni==3)))||
        ((aktual->tvar=='L')&&((aktual->natoceni==1)||(aktual->natoceni==2)))||
        ((aktual->tvar=='T')&&(aktual->natoceni==0)))
        return false;
      else return true;
      break;
    case 'S':
      if(((aktual->tvar=='I')&&((aktual->natoceni==1)||(aktual->natoceni==3)))||
        ((aktual->tvar=='L')&&((aktual->natoceni==0)||(aktual->natoceni==3)))||
        ((aktual->tvar=='T')&&(aktual->natoceni==2)))
        return false;
      else return true;
      break;
    case 'A':
      if(((aktual->tvar=='I')&&((aktual->natoceni==0)||(aktual->natoceni==2)))||
        ((aktual->tvar=='L')&&((aktual->natoceni==0)||(aktual->natoceni==1)))||
        ((aktual->tvar=='T')&&(aktual->natoceni==3)))
        return false;
      else return true;
      break;
    case 'D':
      if(((aktual->tvar=='I')&&((aktual->natoceni==0)||(aktual->natoceni==2)))||
        ((aktual->tvar=='L')&&((aktual->natoceni==2)||(aktual->natoceni==3)))||
        ((aktual->tvar=='T')&&(aktual->natoceni==1)))
        return false;
      else return true;
      break;
    }
    cerr<<"Chyba - spatny parametr metody pruchodny_smer(char smer,int i,int j)";
    return false;
  }

public:
  /**
   * @brief Konstruktor
   *
   * urci velikost vektoru v a zavola metodu rozmisti_kameny()
   * @param rozmer je rozmer strany desky - liche cislo (5-11)
   */
  Deska(int rozmer){
    N=rozmer;
    v.resize(N*N+1);
    rozmisti_kameny();
  }


  /**
   * @brief Vrati rozmer hraci desky
   * @return vraci rozmer hraci desky
   */
  int rozmer(){
    return N;
  }

  /**
   * @brief Posune radek doprava nebo doleva
   * @param c_radku je cislo radku
   * @param doprava true - doprava, false - doleva
   */
  void posun_radek(int c_radku, bool doprava){
    Kamen *vymen=v[0];
    if(doprava){
      v[0]=v[souradnice(c_radku,N)];
      for (int i=N;i>1;i--){
        v[souradnice(c_radku,i)]=v[souradnice(c_radku,i-1)];
      }
      v[souradnice(c_radku,1)]=vymen;
    }
    else{
      v[0]=v[souradnice(c_radku,1)];
      for (int i=1;i<N;i++){
        v[souradnice(c_radku,i)]=v[souradnice(c_radku,i+1)];
      }
      v[souradnice(c_radku,N)]=vymen;
    }
  }


  /**
   * @brief Posune sloupec dolu nebo nahoru
   * @param c_sloupce je cislo sloupce
   * @param dolu true - dolu, false - nahoru
   */
  void posun_sloupec(int c_sloupce, bool dolu){
    Kamen *vymen=v[0];
    if(dolu){
      v[0]=v[souradnice(N,c_sloupce)];
      for (int i=N;i>1;i--){
        v[souradnice(i,c_sloupce)]=v[souradnice(i-1,c_sloupce)];
      }
      v[souradnice(1,c_sloupce)]=vymen;
    }
    else{
      v[0]=v[souradnice(1,c_sloupce)];
      for (int i=1;i<N;i++){
        v[souradnice(i,c_sloupce)]=v[souradnice(i+1,c_sloupce)];
      }
      v[souradnice(N,c_sloupce)]=vymen;
    }
  }


//  void vykresli_bod(Kamen *k){
//    for
//    if (k!=nullptr)
//      cout<<
//  }
};



/**
* @class Hrac
* @brief Jeden hrac
*
* @todo Pridat popis ze zadani!!! //POZOR
* @author Martin Knotek
*/
class Hrac{
  int x,y;  //pozice na hraci desce
  Deska *deska;
  char hledany_predmet{'_'};
public:
  /**
   * @brief Konstruktor
   *
   * Nastavi pocatecni pozici hrace
   * @param deska je ukazatel na hraci desku, na ktere bude hrac hrat
   * @param x je cislo radku, na kterem bude hrac zacinat
   * @param y je cislo sloupce, na kterem bude hrac zacinat
   */
  Hrac(Deska *deska,int x, int y){
    Hrac::x=x;
    Hrac::y=y;
    Hrac::deska=deska;
  }

  /**
   * @brief Cislo radku, na kterem se hrac nachazi
   * @return vrati cislo radku, na kterem se dany hrac nachazi
   */
  int radek(){
    return x;
  }

  /**
   * @brief Cislo sloupce, na kterem se hrac nachazi
   * @return vrati cislo sloupce, na kterem se dany hrac nachazi
   */
  int sloupec(){
    return y;
  }

  void posun(char smer){
    cout<<"posun hrace"<<endl;
    x++;y++;
  }
};



/**
 * @class Hra
 * @brief Trida Hra reprezentuje jednu hru
 *
 * @todo Pridat popis ze zadani!!! //POZOR
 * @author Martin Knotek
 */
class Hra{
  Deska *hraci_plocha;
  vector<Hrac*> hrac{NULL,NULL,NULL,NULL};
  vector<char> balicek_karet;
  int poc_karet{0},poc_hracu;

  /**
   * @brief Ziska od lidra rozmer hraci desky
   *
   * Zkontroluje zda je ziskany rozmer v mezich a pote pomoci tohoto
   * rozmeru vytvori hraci desku.
   * @return vraci true, pokud byl ziskany rozmer v mezich, jinak
   * vraci false
   */
  bool nastaveni_rozmeru(){
    int rozmer;
    cout<<"Zadejte rozmer hraci desky (licha cisla od 5 do 11):";
    cin>>rozmer;

    if (!((rozmer>=5)&&(rozmer<=11)&&(rozmer%2))){
      cerr<<"Spatny rozmer pole - pouze licha cisla 5-11"<<endl;

      if (cin.fail()){  //nebylo zadano cislo
        cin.clear();
        cin.ignore(256,'\n');
      }
      return false;
    }
    hraci_plocha=new Deska(rozmer);
    return true;
  }

  /**
   * @brief Ziska od lidra pocet hracu
   *
   * Zkontroluje zda je ziskany pocet hracu v mezich a pote pomoci tohoto
   * poctu vytvori dany pocet hracu
   * @return vraci true, pokud byl ziskany pocet hracu v mezich, jinak false
   */
  bool nastaveni_poctu_hracu(){
    int rozmer=hraci_plocha->rozmer();
    cout<<"Zadejte pocet hracu (2-4):";
    cin>>poc_hracu;
//    hrac.resize(poc_hracu);
    switch (poc_hracu) {
      case 4:
        hrac[3]=new Hrac(hraci_plocha,rozmer,rozmer);
      case 3:
        hrac[2]=new Hrac(hraci_plocha,rozmer,1);
      case 2:
        hrac[1]=new Hrac(hraci_plocha,1,rozmer);
        hrac[0]=new Hrac(hraci_plocha,1,1);
        return true;
        break;
      default:
        cerr<<"Mohou hrat pouze  dva, tri nebo ctyri hraci"<<endl;
        if (cin.fail()){  //nebylo zadano cislo
          cin.clear();
          cin.ignore(256,'\n');
        }
        return false;
        break;
    }
  }

  /**
   * @brief Ziska od lidra pocet ukolovych karet
   *
   * Zkontroluje zda je ziskany pocet karet v mezich a pote pomoci tohoto
   * poctu vytvori dany pocet karet
   * @todo vytvorit ukolove karty
   * @return vraci true, pokud byl ziskany pocet karet v mezich, jinak false
   */
  bool nastaveni_poctu_karet(){
    cout<<"Zadejte pocet ukolovych karet (12 nebo 24):";
    cin>>poc_karet;
    if (!((poc_karet==12)||(poc_karet==24))){
      cerr<<"Nepovoleny pocet ukolovych karet"<<endl;

      if (cin.fail()){  //nebylo zadano cislo
        cin.clear();
        cin.ignore(256,'\n');
      }
      return false;
    }

    vytvor_balicek(balicek_karet,poc_karet);
    rozmisti_predmety();
//    //vygeneruje balicek karet
//    int poradi;
//    balicek_karet.assign(poc_karet,' ');

//    for(int i=0;i<poc_karet;i++){
//      poradi=rand()%(poc_karet-i);
//      for(int j=0;j<poc_karet;j++){
//        if(balicek_karet[j]==' '){
//          if(!poradi){
//            balicek_karet[j]='A'+i;
//            break;
//          }
//          poradi--;
//        }
//      }
//    }
    return true;
  }


  /**
   * @brief Vytvori balicek karet
   * @param balicek_karet je reference na balicek, ktery se ma vygenerovat
   * @param poc_karet je pocet generovanych karet
   */
  void vytvor_balicek(vector<char> &balicek_karet,int poc_karet){
    //vygeneruje balicek karet
    int poradi;
    balicek_karet.assign(poc_karet,' ');

    for(int i=0;i<poc_karet;i++){
      poradi=rand()%(poc_karet-i);
      for(int j=0;j<poc_karet;j++){
        if(balicek_karet[j]==' '){
          if(!poradi){
            balicek_karet[j]='a'+i;   //od ktereho znaku se bude generovat
            break;
          }
          poradi--;
        }
      }
    }
  }


  void rozmisti_predmety(){
    vector<char> karty;
    vytvor_balicek(karty,poc_karet);
    unsigned int p_st; //pravdepodobnost
    int poc_kamenu=hraci_plocha->rozmer()*hraci_plocha->rozmer();
    for(int i=1;i<=poc_kamenu;i++){
      p_st=rand()%(poc_kamenu-i+1);
      if(p_st<karty.size()){
        hraci_plocha->v[i]->zmen_predmet(karty.back());
        karty.pop_back();
//        if(!karty.size())
//          break;
      }
    }

//    for(int i=1;i<=hraci_plocha->rozmer();i++){
//      for(int j=1;j<=hraci_plocha->rozmer();j++){
//        p_st=rand()%(hraci_plocha->rozmer()*hraci_plocha->rozmer()-
//                     hraci_plocha->souradnice(i,j)-1);

//        p_st=karty.size()/(hraci_plocha->rozmer()*hraci_plocha->rozmer()-
//                        hraci_plocha->souradnice(i,j)-1);
//>=
//      }
//    }

//    while(karty.size()){

//      cout<<karty.back()<<" /// ";


//      karty.pop_back();
//    }
  }

public:

  /**
   * @brief Konstruktor
   *
   * Zjisti a nastavi rozmer hraci plochy, pocet hracu a pocet ukolovych karet
   */
  Hra(){
    while(!nastaveni_rozmeru()){}
    while(!nastaveni_poctu_hracu()){}
    while(!nastaveni_poctu_karet()){}
  }


  /**
   * @brief Posune hrace po hraci plose
   * @param c_hrace je oznaceni hrace <1;4>
   * @param smer udava smer pohybu (W,A,S,D)
   */
  void posun_hrace(int c_hrace,char smer){
    hrac[c_hrace-1]->posun(smer); //hraci jsou cislovany od nuly
  }

  /**
   * @brief Posune radek/sloupce
   *
   * vlozi volny kamen a tim vytlaci jiny, ktery se stane volnym kamenem
   * @param r_s_smer je retezec, ktery udava ktery radek/sloupec se ma
   * posunout kterym smerem (cislo nasledovane smerem).
   *
   * format retezce: cs (c je sude cislo, s je smer pohybu [W,A,S,D])
   * @return vraci true, pokud retezec vyhovuje formatu, jinak false
   */
  bool posun_rad_sl(string r_s_smer){
    int rad_sl;
    char predmet;
    if(r_s_smer.length()==2){
      rad_sl=r_s_smer[0]-'0';
      predmet=r_s_smer[1];
    }
    else if(r_s_smer.length()==3){
      rad_sl=10*(r_s_smer[0]-'0')+r_s_smer[1]-'0';
      predmet=r_s_smer[2];
    }
    else{
      return false;
    }
//cout<<"///"<<rad_sl<<","<<predmet<<"///"<<endl;
    if(!(rad_sl%2)&&(rad_sl>0)&&(rad_sl<hraci_plocha->rozmer())){
      switch (predmet) {
        case 'W':
//          cout<<"Posun "<<rad_sl<<". sloupec nahoru."<<endl;
          hraci_plocha->posun_sloupec(rad_sl,false);
          break;

        case 'A':
          hraci_plocha->posun_radek(rad_sl,false);
//          cout<<"Posun "<<rad_sl<<". radek doleva."<<endl;
          break;

        case 'S':
          hraci_plocha->posun_sloupec(rad_sl,true);
//          cout<<"Posun "<<rad_sl<<". sloupec dolu."<<endl;
          break;

        case 'D':
          hraci_plocha->posun_radek(rad_sl,true);
//          cout<<"Posun "<<rad_sl<<". radek doprava."<<endl;
          break;

        default:
          return false;
          break;
      }
      return true;
    }
    else {
      return false;
    }
//      r_s_smer>>
//      cout<<r_s_smer[1];
//      return true;

//    for (unsigned i=0; i<sm.size(); ++i) {
//        std::cout << "[" << sm[i] << "] ";
//    }

    //cout<<r_s_smer.length()<<endl;


//    if((rad_sl%2)&&(rad_sl>0)&&(rad_sl<hra->))
//      if (cin.fail()){  //nebylo zadano cislo
//        cin.clear();
//        cin.ignore(256,'\n');
    return false;
  }


  /**
   * @brief Natoci volny kamen
   * @param n udava natoceni n*90° n€<0;3>
   */
  void natoceni_volneho_kamene(int n){
    hraci_plocha->v[0]->zmen_natoceni(n);
  }


  /**
   * @brief Vraci pocet hracu ve hre
   * @return vrati pocet hracu
   */
  int pocet_hracu(){
    return poc_hracu;
  }


  /**
   * @brief Ulozi hru do souboru
   *
   * @todo dodelat ukladani hry - zatim jen vypisuje hlasku
   */
  void uloz_hru(){
    cout<<"Ukladam hru"<<endl;
  }



  /**
   * @brief Vypise obsah balicku ukolovych karet
   */
  void vypis_balicek(){
//    for(int i=poc_karet-1;i>=0;i--){  //vypisuje vektor (balicek) odzadu
//      cout<<balicek_karet[i];
//    }
    for(int i=balicek_karet.size()-1;i>=0;i--){  //vypisuje (balicek) odzadu
      cout<<balicek_karet[i];
    }
    cout<<endl;
  }

  /**
   * @brief Vykresli hraci desku pomoci textove grafiky
   *
   * Vykresluje do terminalu - pouziva se pouze textova grafika
   */
  void vykreslit_terminal(){
    int N=hraci_plocha->rozmer();
    int radek{1};
    cout<<"   ";
    for(int i=1;i<N+1;i++)
      cout<<"|"<<setfill('0')<<setw(3)<<i<<"|";
    cout<<endl<<"___";
    //oddelovaci radek
    for(int i=0;i<N;i++)
      cout<<":===:";
    cout<<endl;
    for(int i=1;i<=N;i++){
      //prvni radek
      cout<<setfill('0')<<setw(3)<<radek++;
      for(int j=1;j<=N;j++){
        vykresli_kamen_1(i,j);
      }
      //druhy radek
      cout<<endl<<"   ";
      for(int j=1;j<=N;j++){
        vykresli_kamen_2(i,j);
      }
      //treti radek
      cout<<endl<<"   ";
      for(int j=1;j<=N;j++){
        vykresli_kamen_3(i,j);
      }
      cout<<endl<<"___";
      //oddelovaci radek
      for(int i=0;i<N;i++)
        cout<<":===:";
      cout<<endl;
    }

//    for(int i=1;i<=N;i++){
//      for(int j=1;j<=N;j++){
//        cout<<v[souradnice(i,j)]->tvar<<" ";
//      }
//
//      cout<<endl;
//    }
    cout<<"volny kamen: "<<endl;
    vykresli_kamen_1(SOURADNICE_VOLNY_KAMEN);
    cout<<endl;
    vykresli_kamen_2(SOURADNICE_VOLNY_KAMEN);
    cout<<endl;
    vykresli_kamen_3(SOURADNICE_VOLNY_KAMEN);
    cout<<endl;

    cout<<"Balicek karet:";
    vypis_balicek();
  }


  /**
   * @brief Vykresli prvni cast kamene leziciho na souradnicich x,y
   *
   * pomocna funkce k funkci vykresli_desku_term()
   * @param i je cislo radku
   * @param j je cislo sloupce
   */
  void vykresli_kamen_1(int i, int j){
    cout<<"|";

//    if(hrac[2]){//treti hrac
//      if((hrac[2]->radek()==i)&&(hrac[2]->sloupec()==j)){
//        cout<<"3";
//      }
//      else {
//        cout<<"#";
//      }
//    }
    //prvni hrac?
    cout<<((hrac[0]&&(hrac[0]->radek()==i)&&(hrac[0]->sloupec()==j))?"1":"#");
    //cesta na sever
    cout<<(hraci_plocha->pruchodny_smer('W',i,j)?' ':'#');
    //druhy hrac?
    cout<<((hrac[1]&&(hrac[1]->radek()==i)&&(hrac[1]->sloupec()==j))?"2":"#");
    cout<<"|";
  }
  /**
   * @brief Vykresli druhou cast kamene leziciho na souradnicich x,y
   *
   * pomocna funkce k funkci vykresli_desku_term()
   * @param i je cislo radku
   * @param j je cislo sloupce
   */
  void vykresli_kamen_2(int i, int j){
    cout<<"|";
    //cesta na zapad
    cout<<(hraci_plocha->pruchodny_smer('A',i,j)?' ':'#');
    //predmet
    cout<<hraci_plocha->v[hraci_plocha->souradnice(i,j)]->predmet;
    //cesta na vychod
    cout<<(hraci_plocha->pruchodny_smer('D',i,j)?' ':'#');
    cout<<"|";
  }
  /**
   * @brief Vykresli treti cast kamene leziciho na souradnicich x,y
   *
   * pomocna funkce k funkci vykresli_desku_term()
   * @param i je cislo radku
   * @param j je cislo sloupce
   */
  void vykresli_kamen_3(int i, int j){
    cout<<"|";
    //treti hrac
    cout<<((hrac[2]&&(hrac[2]->radek()==i)&&(hrac[2]->sloupec()==j))?"3":"#");
    //cesta na jih
    cout<<(hraci_plocha->pruchodny_smer('S',i,j)?' ':'#');
    //ctvrty hrac
    cout<<((hrac[3]&&(hrac[3]->radek()==i)&&(hrac[3]->sloupec()==j))?"4":"#");
    cout<<"|";
  }

};


/**
 * @brief Prevede male znaky anglicke abecedy na velke
 * @param retezec je reference na string, ktery se prevest
 */
void velka_pismena(string &retezec){
  for(unsigned i=0;i<retezec.length();i++){
    if((retezec[i]>='a')&&(retezec[i]<='z'))
      retezec[i]+='A'-'a';
  }
}


/**
 * @brief Posune obrazovku o 5OO radku
 */
void smaz_obrazovku(){
  for(int i=0;i<10;i++)
    cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
         <<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
          <<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
           <<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
            <<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
}


/**
 * @brief Rozbehne program
 *
 * @return vraci nulu pokud vse probehlo v poradku
 */
int main() {
  Hra *hra=new Hra;
  string zprava{""};
  //int rad_sl; //cislo radku/sloupce

//  cout<<"Pocet tvaru: "<<endl<<"I: "<<Kamen::count_I<<endl<<
//      "L: "<<Kamen::count_L<<endl<<"T: "<<Kamen::count_T<<endl<<endl; //SMAZ

  string vstup{""};
  int stav{DALSI_HRAC},hrac{0};
  int poc_hracu{hra->pocet_hracu()};

//  cout<<"Natoc volny kamen (W,A,S,D-natocit, N-preskocit):";
//  cin.clear();
//  cin>>vstup;
  while(true) {
    smaz_obrazovku();
    hra->vykreslit_terminal();
    cout<<zprava;
    zprava="";
    cin.clear();
//    cin.ignore(256,'\n');

    if(vstup=="N"){
      if(stav==NATOCENI)
        stav=POSUNUTI;
      else if (stav==POHYB) {
        stav=DALSI_HRAC;
      }
    }
    if(vstup=="Q"){
      stav=ULOZ;
    }

    switch (stav) {
      case DALSI_HRAC:
//        cout << "\033[2J" << flush;
//        cout.clear();
        hrac=(hrac%poc_hracu)+1;  //dalsi hrac
        smaz_obrazovku();
        cout<<"Na tahu je hrac "<<hrac<<endl<<
              "pokracujte zmacknutim klavesy Enter"<<endl;
        getchar();  //stisk klavesy
        cin.ignore(256,'\n'); //uklid smeti - kdyby bylo zadano vice znaku

        stav=NATOCENI;
        break;

      case NATOCENI:
        cout<<"Natoc volny kamen (W,A,S,D-natoceni, N-preskocit):";
        cin>>vstup;
        velka_pismena(vstup);
        if(vstup=="N")
          break;
        else if(vstup=="W")
          hra->natoceni_volneho_kamene(0);
        else if(vstup=="A")
          hra->natoceni_volneho_kamene(3);
        else if(vstup=="S")
          hra->natoceni_volneho_kamene(2);
        else if(vstup=="D")
          hra->natoceni_volneho_kamene(1);
        else
          zprava="!!!Spatny format vstupu!!!\n";
//        if(vstup=="Q")
//          hra->uloz_hru();

        break;

      case POSUNUTI:
        cout<<"Posun radek/sloupec ([cislo radku/sloupce][smer]):";
//        cout<<"Cislo radku/sloupce k posunuti (pouze suda cisla):";
//        cin>>rad_sl;
//        cout<<"Smer posunuti (W-nahoru, A-vlevo, S-dolu, D-vpravo):";
//        cin>>rad_sl;

//        do {
        cin.clear();
        cin>>vstup; //TODO upper case - všechny mala pismena prevest na velka
        velka_pismena(vstup);
        if(vstup=="Q"){
          stav=ULOZ;
          break;
        }
//        }while(!hra->posun_rad_sl(vstup));
        if(hra->posun_rad_sl(vstup))
          stav=POHYB;
        else
          zprava="Spatny format. Napr.: 4D posune 4.radek doprava\n"
                 "format: cs (c-sude cislo;s-smer pohybu WASD)\n";

        break;
//        break;

      case POHYB:
        cout<<"Pohybujte figurkou (W,A,S,D - pohyb, N - ukonceni tahu):";

        cin>>vstup;
        velka_pismena(vstup);
        for(unsigned i=0;i<vstup.length();i++){
          if((vstup[i]=='W')||(vstup[i]=='A')||
             (vstup[i]=='S')||(vstup[i]=='D')){
            hra->posun_hrace(hrac,vstup[i]);  //TODO - vytvorit funkci posun_hrace
          }
          else if(vstup[i]=='N')
            break;
          else
            zprava="Zadan jiny znak nez W,A,S,D nebo N\n";
        }
//        zprava="vykonan pohyb\n"; //SMAZAT
        //stav=DALSI_HRAC;
        break;

      case ULOZ:
        hra->uloz_hru();
        exit(0);
        break;

      default:
        break;
    }
//    cin.clear();
//    cin>>vstup;
  }

  //ulozeni hry
//  hra->uloz_hru();


  return 0;
}


/*** konec souboru projekt.cpp ***/
