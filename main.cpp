#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include "stdc++.h"

using namespace std;

#define PLA 130.0
#define ABS 85.0 


static int ordem_de_servico = 0;
int op = -1;

fstream impressoes;
fstream laser;
fstream cncfresa;
fstream recibo;

float RoundCost(float value);

void load_file();
void save_file();

int MenuPrincipal();
int SelecionarServico();
void TextoDoMenu(int opcao);
void MostrarServico(int os);
/*
    filament_type =     1 ABS.  2 PLA.
    tipo_os =   1. 3D.    2. Laser.   3. CNC FRESA.
*/

class Impressao{
    protected:
        char objectName[100], cliente[100];
        float layer_height, infill, cost_time, cost_used, filament_used;
        int filament_type, minutes, os, tipo_os, impressora; //1 ABS.   2 PLA. // 1. Prusa    2. MakerBot
        char aux[10];
    public:
        void set_os(int ordem){ os = ordem; }
        void set_impressora(int printer){ impressora = printer; }
        void set_ClientName(char nome[]){ strcpy(cliente, nome); }
        void set_name(char nome[]){ strcpy(objectName, nome); }
        void set_layer_height(float height){ layer_height = height; }
        void set_infill(float fill){ infill = fill; }
        void set_filament_type(int type){ filament_type = type; }
        void set_filament_used(float used){ filament_used = used; }
        void set_time(int min){ minutes = min; }
        void set_cost_time(float cst){cost_time = cst; }
        void set_cost_used(float cst){cost_used = cst; }
        void set_tipo_os(int tipo){ tipo_os = tipo; }

        int get_os() const { return os; }
        char * get_client(){ return cliente;}
        char * get_objectName(){ return objectName; }
        float get_layer_height(){ return layer_height; }
        float get_infill(){ return infill; }
        float get_filament_used(){ return filament_used; }
        int get_filament_type(){ return filament_type; }
        int get_minutes(){ return minutes; }
        int get_tipo_os(){ return tipo_os; }
        float get_cost_used(){            
            if(filament_type == 1) cost_used = filament_used * (ABS/1000);
            else if(filament_type == 2) cost_used = filament_used * (PLA/1000);
            cost_used = RoundCost(cost_used);
            return cost_used;
        }
        float get_cost_time(){            
            cost_time = (1.6/60) * minutes;
            cost_time = RoundCost(cost_time);
            return cost_time;
        }
        char * get_filament_type_name(){
            if(filament_type == 1) strcpy(aux, "ABS");
            else if(filament_type == 2) strcpy(aux, "PLA"); 
            return aux;
        }
        char * get_printer(){
            if(impressora == 1) strcpy(aux, "Prusa i3 MK2");
            else if(impressora == 2) strcpy(aux, "MakerBot Replicator 2x");
            else strcpy(aux, "XYZ Printing");
            return aux; 
        }
        void viewPrint(){
            cout << "Ordem de servico: " << os << endl;
            cout << "Tipo de servico: " << tipo_os << endl;
            cout << "Nome do cliente: " << cliente << endl;
            cout << "Impressora: " << impressora << endl;
            cout << "Impressao: " << objectName << endl;
            cout << "Altura de camada: " << layer_height << "mm" << endl;
            cout << "Infill: " << infill << "%" << endl;
            cout << "Filamento: " << get_filament_type_name() << endl;
            cout << "Filamento usado: " << filament_used << " gramas" << endl;
            cout << "Tempo de impressao: " << minutes<< " minutos" << endl;
            cout << endl;
            cout << "Custo por tempo: R$" << get_cost_time() << endl;
            cout << "Custo por filamento usado: R$" << get_cost_used() << endl;
            cout << "Custo total da peca: R$" << get_cost_time() + get_cost_used() << endl << endl; 
        }
};
class CorteALaser{
    protected:
        char objectName[100], cliente[100];
        float cost_time, area_de_corte, perimetro;
        int  minutes, os, tipo_os;
        char aux[10];
    public:
        CorteALaser(){ tipo_os = 2;}
        void set_objectName(char name[]){ strcpy(objectName, name); }
        void set_cliente(char name[]){ strcpy(cliente, name); }
        void set_cost_time(float cost){ cost_time = cost; }
        void set_area(float area){ area_de_corte = area; }
        void set_perimetro(float Perimetro){ perimetro = Perimetro; }
        void set_minutes(int min){ minutes = min; }
        void set_os(int Os){ os = Os; }
        
        int get_os(){ return os; }
        int get_tipo_os(){ return tipo_os; }
        char * get_cliente(){ return cliente; }
        char * get_objectName(){ return objectName; }
        float get_area(){ return area_de_corte; }
        float get_perimetro(){ return perimetro; }
        int get_minutes(){ return minutes; }
        float get_cost_time(){             
            cost_time = 0.6 * minutes;           
            cost_time = RoundCost(cost_time);
            return cost_time;
        }
        void viewLaser(){
            cout << "Ordem de servico: " << os << endl;
            cout << "Tipo de servico: " << tipo_os << endl;
            cout << "Nome do cliente: " << cliente << endl;
            cout << "Nome do corte: " << objectName << endl;
            cout << "Tempo de corte: " << minutes<< " minutos" << endl;
            cout << endl;
            
        }
            
};
class CorteCNC{
};

vector <Impressao> lista_impressao;
vector <CorteALaser> lista_laser;
vector <CorteCNC> lista_cnc;

int main(){
    
    load_file();
    cout << "ordem de servico: " << ordem_de_servico << endl; 
    system("read -p \"Enter para continuar\"");

    do{ 

        op = MenuPrincipal();
        
        switch(op){
                case 1:
                                
                    break;
                case 2:
                    break;
                case 3:
                    TextoDoMenu(3);
                    if(ordem_de_servico >= 0){ 
                        for(int i = 0; i <= ordem_de_servico; i++){
                            MostrarServico(i);
                        }
                    }
                    else{
                        cout << "lista zerada." << endl << endl;
                        system("read -p \"Enter para continuar\"");
                    }
                    break;
                case 4:
                    system("clear");
                    //if(lista_impressao.size() != 0) totalOS();
                    break;
                case 5:
                    system("clear");
                    //totalUSO();
                    break;    
                default:
                    system("clear");
                    
                    cout << "opcao invalida digitada." << endl;
            }
            if(op != 0) op = -1;
    }while(op != 0);
    save_file();

    return 0;
}
void TextoDoMenu(int opcao){
    system("clear");
    switch(opcao){
        case -1: cout << "\t\tMenu Principal.\n\n\n"; break;
        case 1: cout << "\t\tCriar Ordem de Servico.\n\n\n"; break;
        case 2: cout << "\t\tInserir Item A Ordem de Servico.\n\n\n"; break;
        case 3: cout << "\t\tLista de Ordens de Servico.\n\n\n"; break;
        case 4: cout << "\t\tCalculo de Custo de Ordem de Servico.\n\n\n"; break;
        case 5: cout << "\t\tGerar Recibo de Ordem de Servico.\n\n\n"; break;
        case 9: cout << "\t\tSelecao de Tipo de Servico.\n\n\n";
    }
}
int MenuPrincipal(){
    int opcao_menu;
    TextoDoMenu(op);

    cout << "1. Criar ordem de servico." << endl;
    cout << "2. Inserir item a ordem de servico." << endl;
    cout << "3. Lista com todos servicos." << endl;
    cout << "4. Custo total por OS." << endl;
    cout << "5. Uso total de cada maquina." << endl;
    cout << endl;
    cout << "0. Sair do Programa." << endl << endl;

    cout << "opcao: ";
    cin >> opcao_menu;
    cin.ignore();

    return opcao_menu;    
}
void MostrarServico(int os){
    for(int i = 0; i < lista_impressao.size(); i++){
        if(os == lista_impressao[i].get_os()) lista_impressao[i].viewPrint(); 
    }
    for(int i = 0; i < lista_laser.size(); i++){
        if(os == lista_laser[i].get_os()) lista_laser[i].viewLaser(); 
    }
    /*
    for(int i = 0; i < lista_impressao.size(); i++){
        if(os == lista_cnc[i].get_os()) lista_cnc[i].viewCNC(); 
    }
    */
   system("read -p \"Enter para continuar\"");
}
int SelecionarServico(){
    int servico = 0;

    do{
        system("clear");
        TextoDoMenu(9);
        cout << "1. Impressão 3D.\n2. Corte a Laser.\n3. Corte CNC Fresa\n\nDigite o tipo de servico:\t";
        cin >> servico;
        cin.ignore();
    }while(servico < 1 && servico > 3);

    return servico;
}
float RoundCost(float value){
    float Faux = 0;
    int Iaux = 0;
    
    Iaux = int(value);
    Faux = value - (float)Iaux;           
    value = float(Iaux);
    Faux = Faux * 100;
    Iaux = int(Faux);
    Faux = float(Iaux)/100;
    value += Faux;

    return value;
}
void load_file(){
    int bigger = 0;
	char auxC[100];

    Impressao *auximpressoes = new Impressao;
	string aux[12];
	
	bool check = false;

	impressoes.open("impressao.csv", ios::in); // leitura;
	if(!impressoes.is_open()) cout << "arquivo impressoes.csv nao carregado" << endl;

	while(impressoes.good()){
		getline(impressoes, aux[0], ';');
		if(aux[0] != ""){
			getline(impressoes, aux[1], ';');
			getline(impressoes, aux[2], ';');
			getline(impressoes, aux[3], ';');
			getline(impressoes, aux[4], ';');
			getline(impressoes, aux[5], ';');
            getline(impressoes, aux[6], ';');
            getline(impressoes, aux[7], ';');
            getline(impressoes, aux[8], ';');
            getline(impressoes, aux[9], ';');
            getline(impressoes, aux[10], ';');
			getline(impressoes, aux[11], '\n');
            check = true;
		}
		
		if(check){
            auximpressoes->set_os(atoi(aux[0].c_str()));
            strcpy(auxC, aux[1].c_str());
			auximpressoes->set_ClientName(auxC);
            strcpy(auxC, aux[2].c_str());
			auximpressoes->set_name(auxC);
			auximpressoes->set_layer_height( atof(aux[3].c_str()));
			auximpressoes->set_infill( atof(aux[4].c_str()));
			auximpressoes->set_cost_used( atof(aux[5].c_str()));
            auximpressoes->set_cost_time( atof(aux[6].c_str()));
			auximpressoes->set_filament_used(atof(aux[7].c_str()));
            auximpressoes->set_filament_type(atoi(aux[8].c_str()));
            auximpressoes->set_time(atoi(aux[9].c_str()));
            auximpressoes->set_impressora(atoi(aux[10].c_str()));
            auximpressoes->set_tipo_os(atoi(aux[11].c_str()));

            if(auximpressoes->get_os() > bigger) bigger = auximpressoes->get_os();

			lista_impressao.push_back(*auximpressoes);
		}
	}
    impressoes.close();

    ordem_de_servico = bigger + 1;
}
void save_file(){
	impressoes.open("impressao.csv", ios::out); // escrever no arquivo	
	
	for(int i = 0; i < lista_impressao.size(); i++){
        impressoes << lista_impressao[i].get_os() << ";";
        impressoes << lista_impressao[i].get_client() << ";";    
		impressoes << lista_impressao[i].get_objectName() << ";";
		impressoes << lista_impressao[i].get_layer_height() << ";";
		impressoes << lista_impressao[i].get_infill() << ";";
		impressoes << lista_impressao[i].get_cost_used() << ";";
        impressoes << lista_impressao[i].get_cost_time() << ";";
		impressoes << lista_impressao[i].get_filament_used() << ";";
		impressoes << lista_impressao[i].get_filament_type() << ";";
        impressoes << lista_impressao[i].get_minutes() << "\n";
        impressoes << lista_impressao[i].get_printer() << "\n";

		if(i < lista_impressao.size() - 1){
			impressoes << lista_impressao[i].get_tipo_os() << "\n";
		}
		else{
			impressoes << lista_impressao[i].get_tipo_os();
		}
	}	
	impressoes.close();
}