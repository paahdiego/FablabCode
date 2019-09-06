/*
    Autor: Patrick Diego - Engenharia Elétrica UFPB.
    Código: FABLABCODE.

    Descrição: 
        Código utilizado para cadastro e calculo de custo dos serviços realizados pelo FABLAB UFPB.

    @Paahdiego.
*/

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include "stdc++.h"
#include <iomanip>
#include <sstream>

using namespace std;

#define PLA 130.0
#define ABS 85.0 


static int ordem_de_servico = 0;
int op = -1;
char tipo_do_servico[100];
string separador =  "______________________________________\n\n";

fstream impressoes;
fstream laser;
fstream cncfresa;
fstream recibo;

/*
    Algumas Variaveis e seus usos.
    
    filament_type =     
        1 ABS.  2 PLA.
    tipo_os =   
        1. 3D.    2. Laser.   3. CNC FRESA.
    impressora = 
        1. Prusa    2. MakerBot
*/

//Funcoes que Carregam e Salvam os dados em seus respectivos .csv
void load_file();
void save_file();

//Funcoes do Programa
int MenuPrincipal(); // Auto-indicativo
int SelecionarServico(); // Selecao de tipo de Servico (tipo_os) usada em diversas partes do programa
float RoundCost(float value); // Corrige o Valor float para um formato Currency
void TextoDoMenu(int opcao); //Gambiarra
void MostrarServico(int os); //Mostra os dados de uma OS 
void CriarOrdemDeServico(int opcao = 0); // Auto-indicativo
void CriarImpressao(int opcao = 0); // Cria uma ordem do tipo Impressao
void CriarLaser(int opcao = 0); //Leia acima 
void CriarCNC(int opcao = 0); //Leia acima depois de acima
char * tipo_de_servico(int tipo);  // Retorna o texto especificando o tipo de servico
bool validacao_int(int init, int final, int value, int text_menu); // validacao de inteiros usada em diversas partes do programa
int validacao_os(int os, int menu); // busca uma OS nas listas
void RelatorioDeUso();
void CalculoValorTotalOS(); //Recebe e valida uma OS para Calculo de valor total

void CalculoReciboImpressao(int os); //Calculo por OS e Recibo
void CalculoReciboLaser(int os); //Calculo por OS e Recibo
void CalculoReciboCNC(int os); //Calculo por OS e Recibo

string SeparadorHTML(); // HTML p/ separar objetos no recibo
string HeaderAndStyle(int tipo, int os = -1, string cliente = ""); // Header HTML com estilo para o recibo
string GerarRelatorio(int tipo, int os = -1);
template <class type> string precision_to_string(type value, int precision); // Precisao de ponto flutuante em string

//Classes usadas no programa
class Impressao{
    protected:
        char objectName[100], cliente[100];
        float layer_height, infill, cost_time, cost_used, filament_used;
        int filament_type, minutes, os, tipo_os, impressora;
        char aux[100];
    public:
        Impressao(){ tipo_os = 1; }
        void set_os(int ordem){ os = ordem; }
        void set_impressora(int printer){ impressora = printer; }
        void set_cliente(char nome[]){ strcpy(cliente, nome); }
        void set_objectName(char nome[]){ strcpy(objectName, nome); }
        void set_layer_height(float height){ layer_height = height; }
        void set_infill(float fill){ infill = fill; }
        void set_filament_type(int type){ filament_type = type; }
        void set_filament_used(float used){ filament_used = used; }
        void set_time(int min){ minutes = min; }
        void set_cost_time(float cst){cost_time = cst; }
        void set_cost_used(float cst){cost_used = cst; }
        void set_tipo_os(int tipo){ tipo_os = tipo; }

        int get_os() const { return os; }
        char * get_cliente(){ return cliente;}
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
        int get_printer_int(){ return impressora; }
        char * get_printer(){
            if(impressora == 1) strcpy(aux, "Prusa i3 MK2");
            else if(impressora == 2) strcpy(aux, "MakerBot Replicator 2x");
            else strcpy(aux, "XYZ Printing");
            return aux; 
        }
        void viewPrint(int option = 0){
            cout << "Ordem de servico: " << os << endl;
            cout << "Nome do cliente: " << cliente << endl;
            cout << "Tipo de servico: " << tipo_de_servico(tipo_os) << endl;
            cout << "Impressora: " << get_printer() << endl;
            cout << "Impressao: " << objectName << endl;
            
            if(option == 1){
                cout << "Altura de camada: " << layer_height << "mm" << endl;
                cout << "Infill: " << infill << "%" << endl;
                cout << "Filamento: " << get_filament_type_name() << endl;
                cout << "Filamento usado: " << filament_used << " gramas" << endl;
                cout << "Tempo de impressao: " << minutes<< " minutos" << endl;
                cout << endl;
                cout << "Custo por tempo: R$" << get_cost_time() << endl;
                cout << "Custo por filamento usado: R$" << get_cost_used() << endl;
                cout << "Custo total da peca: R$" << get_cost_time() + get_cost_used() << endl; 
            }
            cout << separador;
        }
};
class CorteALaser{
    protected:
        char objectName[100], cliente[100], material[100];
        float cost_time, area_de_corte, perimetro;
        int  minutes, os, tipo_os;
        char aux[100];
    public:
        CorteALaser(){ tipo_os = 2;}
        void set_objectName(char name[]){ strcpy(objectName, name); }
        void set_material(char name[]){ strcpy(material, name); }
        void set_cliente(char name[]){ strcpy(cliente, name); }
        void set_cost_time(float cost){ cost_time = cost; }
        void set_area(float area){ area_de_corte = area; }
        void set_perimetro(float Perimetro){ perimetro = Perimetro; }
        void set_minutes(int min){ minutes = min; }
        void set_os(int Os){ os = Os; }
        void set_tipo_os(int tipo){ tipo_os = tipo; } 
        
        int get_os() const { return os; }
        int get_tipo_os(){ return tipo_os; }
        char * get_material(){ return material; }
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
        void viewLaser(int option = 0){
            cout << "Ordem de servico: " << os << endl;
            cout << "Nome do cliente: " << cliente << endl;
            cout << "Tipo de servico: " << tipo_de_servico(tipo_os) << endl;
            cout << "Nome do corte: " << objectName << endl;
            
            if(option == 1){
                cout << "Material: " << material << endl;
                cout << "Area de corte: " << area_de_corte << endl;
                cout << "Perimetro de corte: " << perimetro << endl;
                cout << "Tempo de corte: " << minutes<< " minutos" << endl;
                cout << "Custo por tempo: R$" << get_cost_time() << endl;
            }
            cout << separador;   
        }   
};
class CorteCNC{
    protected:
        char objectName[100], cliente[100], material[100];
        float cost_time, area_de_corte, perimetro;
        int  minutes, os, tipo_os;
        char aux[10];
    public:
        CorteCNC(){ tipo_os = 3;}
        void set_objectName(char name[]){ strcpy(objectName, name); }
        void set_cliente(char name[]){ strcpy(cliente, name); }
        void set_material(char name[]){ strcpy(material, name); }
        void set_cost_time(float cost){ cost_time = cost; }
        void set_area(float area){ area_de_corte = area; }
        void set_perimetro(float Perimetro){ perimetro = Perimetro; }
        void set_minutes(int min){ minutes = min; }
        void set_os(int Os){ os = Os; }
        void set_tipo_os(int tipo){ tipo_os = tipo; }
        
        int get_os() const { return os; }
        int get_tipo_os(){ return tipo_os; }
        char * get_cliente(){ return cliente; }
        char * get_objectName(){ return objectName; }
        char * get_material(){ return material; }
        float get_area(){ return area_de_corte; }
        float get_perimetro(){ return perimetro; }
        int get_minutes(){ return minutes; }
        float get_cost_time(){             
            cost_time = 1.2 * minutes;           
            cost_time = RoundCost(cost_time);
            return cost_time;
        }
        void viewCNC(int option = 0){
            cout << "Ordem de servico: " << os << endl;
            cout << "Nome do cliente: " << cliente << endl;
            cout << "Tipo de servico: " << tipo_de_servico(tipo_os) << endl;
            cout << "Nome do corte: " << objectName << endl;
            
            if(option == 1){
                cout << "Material: " << material << endl;
                cout << "Area de corte: " << area_de_corte << endl;
                cout << "Perimetro de corte: " << perimetro << endl;
                cout << "Tempo de corte: " << minutes << " minutos" << endl;
                cout << "Custo por tempo: R$" << get_cost_time() << endl;
            }
            cout << separador;   
        }
};

vector <Impressao> lista_impressao;
vector <CorteALaser> lista_laser;
vector <CorteCNC> lista_cnc;

bool prioridade_print(const Impressao &a, const Impressao &b) {
    return ( a.get_os() < b.get_os() );
}
bool prioridade_laser(const CorteALaser &a, const CorteALaser &b) {
    return ( a.get_os() < b.get_os() );
}
bool prioridade_cnc(const CorteCNC &a, const CorteCNC &b) {
    return ( a.get_os() < b.get_os() );
}

int main(){
    
    load_file();
    
    /*
        //Area de Testes:

        cout << "ordem de servico: " << ordem_de_servico << endl; 
        system("read -p \"Enter para continuar\"");

        for(int i = 0; i < lista_impressao.size(); i++) {
            lista_impressao[i].viewPrint(1);
            cout << endl;
        }
    
        int value = 0;
        bool check = false;
        do{
                TextoDoMenu(13);
                cout << "1. ABS\n2. PLA\n\nTipo de filamento: ";
                cin >> value;
                check = validacao_int(1, 2, value, 13);
        }while(!check);
    */
    do{ 
        sort(lista_impressao.begin(), lista_impressao.end(), prioridade_print);
        sort(lista_laser.begin(), lista_laser.end(), prioridade_laser);
        sort(lista_cnc.begin(), lista_cnc.end(), prioridade_cnc);

        op = MenuPrincipal();
        
        switch(op){
                case 0: break;
                case 1: 
                    CriarOrdemDeServico();       
                    break;
                case 2:
                    CriarOrdemDeServico(1);
                    break;
                case 3:
                    do{
                        TextoDoMenu(3);
                        cout << "1. Tipo de Servico.\n2. Por OS.\n\nDeseja mostrar todas ordens de servico por: ";
                        cin >> op;
                        cin.ignore();
                    }while(op != 1 && op !=2); 
                    
                    if(op == 1){
                        do{
                            TextoDoMenu(3);
                            //cout << "1. Impressoes 3D.\n2. Corte a Laser\n3. Corte CNC\n\nDeseja mostrar todas ordens de qual servico: ";
                            //cin >> op;
                            //cin.ignore();
                            op = SelecionarServico();
                            
                        }while(op != 1 && op !=2 && op != 3);
                        
                        if(op == 1){
                            TextoDoMenu(10);
                            for(int i = 0; i < lista_impressao.size(); i++){
                                lista_impressao[i].viewPrint();
                            } 
                        }
                        else if(op ==2){
                            TextoDoMenu(11);
                            for(int i = 0; i < lista_laser.size(); i++){
                                lista_laser[i].viewLaser();
                            }
                        }
                        else{   
                            TextoDoMenu(12);
                            for(int i = 0; i < lista_cnc.size(); i++){
                                lista_cnc[i].viewCNC();
                            }
                        }
                    }
                    else if(op == 2){
                        TextoDoMenu(3);
                        for(int i = 0; i <= ordem_de_servico; i++){
                            MostrarServico(i);
                        }
                    }
                    system("read -p \"Enter para continuar\"");
                    break;
                case 4:
                    CalculoValorTotalOS();
                    //if(lista_impressao.size() != 0) bool checkListas(lista vazia) do something
                    break;
                case 5:
                    RelatorioDeUso(); 
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
        case 1: cout << "\t\tCriar Ordem de Servico\n\n\n"; break;
        case 2: cout << "\t\tInserir Item A Ordem de Servico\n\n\n"; break;
        case 3: cout << "\t\tLista de Ordens de Servico\n\n\n"; break;
        case 4: cout << "\t\tCalculo de Custo de Ordem de Servico\n\n\n"; break;
        case 5: cout << "\t\tGerar Recibo de Ordem de Servico\n\n\n"; break;

        case 9: cout << "\t\tSelecao de Tipo de Servico\n\n\n"; break;
        case 10: cout << "\t\tLista de Impressoes 3D\n\n\n"; break;
        case 11: cout << "\t\tLista de Cortes a Laser\n\n\n"; break;
        case 12: cout << "\t\tLista de Cortes CNC\n\n\n"; break;
        case 13: cout << "\t\tCadastrar Impressao 3D\n\n\n"; break;
        case 14: cout << "\t\tCadastrar Corte a Laser\n\n\n"; break;
        case 15: cout << "\t\tCadastrar Corte CNC\n\n\n"; break;
        case 16: cout << "\t\tCalculo OS - Impressao 3D\n\n\n"; break;
        case 17: cout << "\t\tCalculo OS - Corte a Laser\n\n\n"; break;
        case 18: cout << "\t\tCalculo OS - Corte CNC\n\n\n"; break;
    }
}
char * tipo_de_servico(int tipo){
    
    switch (tipo){
        case 1: strcpy(tipo_do_servico, "Impressao 3D"); break;
        case 2: strcpy(tipo_do_servico, "Corte a Laser"); break;
        case 3: strcpy(tipo_do_servico, "Corte CNC"); break;   
    }
    return tipo_do_servico;
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
    for(int i = 0; i < lista_cnc.size(); i++){
        if(os == lista_cnc[i].get_os()) lista_cnc[i].viewCNC(); 
    }
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
void CriarOrdemDeServico(int opcao){
    TextoDoMenu(1);

    int tipo_servico = SelecionarServico();
    switch (tipo_servico){
        case 1: 
            CriarImpressao(opcao);
            break;
        case 2:
            CriarLaser(opcao);
            break;
        case 3:
            CriarCNC(opcao);
            break;
    }
}
void CriarImpressao(int opcao){
    char objectName[100], cliente[100];
    int filament_type, minutes, os_search, printer;
    float layer_height, filament_used, infill; 
    
    bool check = false;
    
    Impressao aux;

    if(opcao == 0){    
        TextoDoMenu(1);
        cout << "Digite o nome do cliente: ";
        cin.getline(cliente, 100);
        aux.set_cliente(cliente);
    }
    else if(opcao == 1){
        do{
            TextoDoMenu(2);
            cout << "digite o numero da OS: ";
            cin >> os_search;
            cin.ignore();

            for(int i = 0; i < lista_impressao.size(); i++){
                if(os_search == lista_impressao[i].get_os()) check = true; 
                if(check){ 
                    aux.set_cliente( lista_impressao[i].get_cliente() );
                    aux.set_os( lista_impressao[i].get_os() );
                    break;
                }
            }
            if(!check){
                TextoDoMenu(2);
                cout << "ordem de servico nao cadastrada.\n\nDigite novamente.";
                system("read -p \"Enter para continuar\"");
            } 
        }while(!check);
    }
    
    do{
        TextoDoMenu(13);
        cout << "Digite o titulo da impressao: ";
        cin.getline(objectName, 100);
        
        check = false;
        do{
            TextoDoMenu(13);        
            cout << "1. Prusa i3 MK2\n2. MakerBot Replicator\n\nDigite a impressora 3D: ";
            cin >> printer;
            system("clear");
            check = validacao_int(1, 2, printer, 13);
        }while(!check);
        
        do{
            TextoDoMenu(13);
            cout << "1. ABS\n2. PLA\n\nTipo de filamento: ";
            cin >> filament_type;
            check = validacao_int(1, 2, filament_type, 13);
        }while(!check);

        TextoDoMenu(13);
        cout << "Altura de camada: ";
        cin >> layer_height;

        TextoDoMenu(13);
        cout << "Infill (%): ";
        cin >> infill;

        TextoDoMenu(13);
        cout << "quantidade de filamento (gramas): ";
        cin >> filament_used;

        TextoDoMenu(13);        
        cout << "tempo de impressao (minutos): ";
        cin >> minutes;        

        if(opcao == 0) aux.set_os(ordem_de_servico);
        
        aux.set_objectName(objectName);
        aux.set_layer_height(layer_height);
        aux.set_infill(infill);
        aux.set_filament_type(filament_type);
        aux.set_filament_used(filament_used);
        aux.set_time(minutes);
        aux.set_impressora(printer);
        lista_impressao.push_back(aux);

        TextoDoMenu(13);
        cout << "Deseja inserir outra peca? \n\n1.SIM\n2.NAO \n\nopcao: ";
        cin >> op;
        cin.ignore();
    }while(op == 1);
    
    if(opcao == 0) ordem_de_servico++;
}
void CriarLaser(int opcao){
    char objectName[100], cliente[100], material[50];
    int minutes, os_search;
    float area, perimetro;
    
    bool check = false;
    
    CorteALaser aux;

    if(opcao == 0){    
        TextoDoMenu(1);
        cout << "Digite o nome do cliente: ";
        cin.getline(cliente, 100);
        aux.set_cliente(cliente);
    }
    else if(opcao == 1){
        do{
            TextoDoMenu(2);
            cout << "digite o numero da OS: ";
            cin >> os_search;
            cin.ignore();

            for(int i = 0; i < lista_laser.size(); i++){
                if(os_search == lista_laser[i].get_os()) check = true; 
                if(check) { 
                    aux.set_cliente( lista_laser[i].get_cliente() );
                    aux.set_os( lista_laser[i].get_os() );
                    break;
                }
            }
            if(!check){
                TextoDoMenu(2);
                cout << "ordem de servico nao cadastrada.\n\nDigite novamente.";
                system("read -p \"Enter para continuar\"");
            } 
        }while(!check);
    }
    
    do{
        if(opcao == 0) aux.set_os(ordem_de_servico);

        TextoDoMenu(14);
        cout << "digite o nome do corte a laser: ";
        cin.getline(objectName, 100);

        TextoDoMenu(14);
        cout << "digite o material: ";
        cin.getline(material, 50);

        TextoDoMenu(14);        
        cout << "area de corte: ";
        cin >> area;

        TextoDoMenu(14);        
        cout << "perimetro: ";
        cin >> perimetro;

        TextoDoMenu(14);        
        cout << "tempo de corte (minutos): ";
        cin >> minutes;        

        aux.set_objectName(objectName);
        aux.set_material(material);
        aux.set_area(area);
        aux.set_perimetro(perimetro);
        aux.set_minutes(minutes);

        lista_laser.push_back(aux);

        TextoDoMenu(14);
        cout << "deseja inserir outra peca? \n\n1.SIM\n2.NAO \n\nopcao: ";
        cin >> op;
        cin.ignore();
    }while(op == 1);
    
    if(opcao == 0) ordem_de_servico++;
}
void CriarCNC(int opcao){
    char objectName[100], cliente[100], material[50];
    int minutes, os_search;
    float area, perimetro;
    
    bool check = false;
    
    CorteCNC aux;

    if(opcao == 0){    
        TextoDoMenu(1);
        cout << "Digite o nome do cliente: ";
        cin.getline(cliente, 100);
        aux.set_cliente(cliente);
    }
    else if(opcao == 1){
        do{
            TextoDoMenu(2);
            cout << "digite o numero da OS: ";
            cin >> os_search;
            cin.ignore();

            for(int i = 0; i < lista_cnc.size(); i++){
                if(os_search == lista_cnc[i].get_os()) check = true; 
                if(check) { 
                    aux.set_cliente( lista_cnc[i].get_cliente() );
                    aux.set_os( lista_cnc[i].get_os() );
                    break;
                }
            }
            if(!check){
                TextoDoMenu(2);
                cout << "ordem de servico nao cadastrada.\n\nDigite novamente.";
                system("read -p \"Enter para continuar\"");
            } 
        }while(!check);
    }
    
    do{
        if(opcao == 0) aux.set_os(ordem_de_servico);

        TextoDoMenu(15);
        cout << "digite o nome do corte: ";
        cin.getline(objectName, 100);

        TextoDoMenu(15);
        cout << "digite o material: ";
        cin.getline(material, 50);

        TextoDoMenu(15);        
        cout << "area de corte: ";
        cin >> area;

        TextoDoMenu(15);        
        cout << "perimetro: ";
        cin >> perimetro;

        TextoDoMenu(15);        
        cout << "tempo de corte (minutos): ";
        cin >> minutes;        

        aux.set_objectName(objectName);
        aux.set_material(material);
        aux.set_area(area);
        aux.set_perimetro(perimetro);
        aux.set_minutes(minutes);

        lista_cnc.push_back(aux);

        TextoDoMenu(15);
        cout << "deseja inserir outra peca? \n\n1.SIM\n2.NAO \n\nopcao: ";
        cin >> op;
        cin.ignore();
    }while(op == 1);
    
    if(opcao == 0) ordem_de_servico++;
}
void CalculoValorTotalOS(){
    int  tipo_os, os, opcao;
    bool check = false;

    do{
        TextoDoMenu(4);
        cout << "Digite a Ordem de Servico: "; 
        cin >> os;
        tipo_os = validacao_os(os, 4);
        
        if(tipo_os != -1){
            do{
                TextoDoMenu(4);
                MostrarServico(os);
                cout << "\n\nOS Desejada selecionada?\n1. SIM\n2. NAO\n\nopcao: ";
                cin >> opcao;
                check = validacao_int(1, 2, opcao, 4);
            }while(!check);
        }
        if(opcao == 2) tipo_os = -1;
    }while(tipo_os == -1);    
    
    switch(tipo_os){
        case 1 : 
            CalculoReciboImpressao(os); 
            break;
        case 2 : 
            CalculoReciboLaser(os);
        break;
        case 3 :
            CalculoReciboCNC(os);
         break;
    }
}
void CalculoReciboImpressao(int os){
    float total_tempo = 0, total_material = 0;
    int qt_pecas = 0, tempo_total = 0, opcao = 0, tipo_os = -1, indice = 0;
    bool check = false;
    
    do{
        TextoDoMenu(16);
        qt_pecas = 0; tempo_total = 0; total_material = 0; total_tempo = 0;

        for(int i = 0; i < lista_impressao.size(); i++){
            if(os == lista_impressao[i].get_os()){
                total_tempo += lista_impressao[i].get_cost_time();
                total_material += lista_impressao[i].get_cost_used();
                tempo_total += lista_impressao[i].get_minutes();
                lista_impressao[i].viewPrint(1);
                tipo_os = lista_impressao[i].get_tipo_os();
                indice = i;
                qt_pecas++;
            } 
        }
        cout << "Tempo total: " << tempo_total << " minutos" << endl;    
        cout << "Quantidade de pecas: " << qt_pecas << " un" << endl;
        cout << "Custo total por material: R$" << RoundCost(total_material) << endl;
        cout << "Custo total por tempo: R$" << RoundCost(total_tempo) << endl;
        cout << "Custo total: R$" << RoundCost(total_material + total_tempo) << endl << endl;

        cout << "\nDeseja gerar o recibo?\n1. SIM\n2. NAO\n\nopcao: ";
        cin >> opcao; 

        check = validacao_int(1,2,opcao, 16);
    }while(!check);

    if(opcao == 1){
       string file_name = "recibo";

       file_name = file_name + " " + string(lista_impressao[indice].get_cliente()) + " OS" + to_string(os) + ".html";

       recibo.open(file_name.c_str(), ios::out);

       recibo << HeaderAndStyle(tipo_os, os, string(lista_impressao[indice].get_cliente()));

       recibo << GerarRelatorio(tipo_os, os);

       recibo << "<p id=\"borda\"></p>";
       recibo << "<p>Quantidade de pecas: " << qt_pecas << " unidades </p>";
       recibo << "<p>Tempo total de impressao: " << tempo_total << " minutos</p>";
       recibo << "<p>Custo total por tempo: R$ " << RoundCost(total_tempo) << "</p>";
       recibo << "<p>Custo total por material: R$ "  << RoundCost(total_material) << "</p>";
       recibo << "<h3>Total: R$" << RoundCost(total_tempo + total_material) << "</h3>";
       recibo << "</div></body></html>";
       recibo.close();

        file_name = "open \"" + file_name + '\"';
        system(file_name.c_str());
    }
    
    system("read -p \"Enter para continuar\"");
}
void CalculoReciboLaser(int os){
    float total_tempo = 0;
    int qt_pecas = 0, tempo_total = 0, opcao = 0, tipo_os = -1, indice = 0;
    bool check = false;
    
    do{
        TextoDoMenu(17);
        qt_pecas = 0; tempo_total = 0; total_tempo = 0;

        for(int i = 0; i < lista_laser.size(); i++){
            if(os == lista_laser[i].get_os()){
                total_tempo += lista_laser[i].get_cost_time();
                tempo_total += lista_laser[i].get_minutes();
                lista_laser[i].viewLaser(1);
                tipo_os = lista_laser[i].get_tipo_os();
                indice = i;
                qt_pecas++;
            } 
        }
        cout << "Tempo total: " << tempo_total << " minutos" << endl;    
        cout << "Quantidade de pecas: " << qt_pecas << " un" << endl;
        cout << "Custo total por tempo: R$" << RoundCost(total_tempo) << endl;

        cout << "\nDeseja gerar o recibo?\n1. SIM\n2. NAO\n\nopcao: ";
        cin >> opcao; 

        check = validacao_int(1,2,opcao, 17);
    }while(!check);

    if(opcao == 1){
       string file_name = "recibo";

       file_name = file_name + " " + string(lista_laser[indice].get_cliente()) + " OS" + to_string(os) + ".html";

       recibo.open(file_name.c_str(), ios::out);

       recibo << HeaderAndStyle(tipo_os, os, string(lista_laser[indice].get_cliente()));

       recibo << GerarRelatorio(tipo_os, os);

       recibo << "<p id=\"borda\"></p>";
       recibo << "<p>Quantidade de pecas: " << qt_pecas << " unidades </p>";
       recibo << "<p>Tempo total de corte: " << tempo_total << " minutos</p>";
       recibo << "<h3>Custo total por tempo: R$" << RoundCost(total_tempo) << "</h3>";
       recibo << "</div></body></html>";
       recibo.close();

        file_name = "open \"" + file_name + '\"';
        system(file_name.c_str());
    }
    
    system("read -p \"Enter para continuar\"");
}
void CalculoReciboCNC(int os){
    float total_tempo = 0;
    int qt_pecas = 0, tempo_total = 0, opcao = 0, tipo_os = -1, indice = 0;
    bool check = false;
    
    do{
        TextoDoMenu(18);
        qt_pecas = 0; tempo_total = 0; total_tempo = 0;

        for(int i = 0; i < lista_cnc.size(); i++){
            if(os == lista_cnc[i].get_os()){
                total_tempo += lista_cnc[i].get_cost_time();
                tempo_total += lista_cnc[i].get_minutes();
                lista_cnc[i].viewCNC(1);
                tipo_os = lista_cnc[i].get_tipo_os();
                indice = i;
                qt_pecas++;
            } 
        }
        cout << "Tempo total: " << tempo_total << " minutos" << endl;    
        cout << "Quantidade de pecas: " << qt_pecas << " un" << endl;
        cout << "Custo total por tempo: R$" << RoundCost(total_tempo) << endl;

        cout << "\nDeseja gerar o recibo?\n1. SIM\n2. NAO\n\nopcao: ";
        cin >> opcao; 

        check = validacao_int(1,2,opcao, 18);
    }while(!check);

    if(opcao == 1){
       string file_name = "recibo";

       file_name = file_name + " " + string(lista_cnc[indice].get_cliente()) + " OS" + to_string(os) + ".html";

       recibo.open(file_name.c_str(), ios::out);

       recibo << HeaderAndStyle(tipo_os, os, string(lista_cnc[indice].get_cliente()));

       recibo << GerarRelatorio(tipo_os, os);

       recibo << "<p id=\"borda\"></p>";
       recibo << "<p>Quantidade de pecas: " << qt_pecas << " unidades </p>";
       recibo << "<p>Tempo total de corte: " << tempo_total << " minutos</p>";
       recibo << "<h3>Custo total por tempo: R$" << RoundCost(total_tempo) << "</h3>";
       recibo << "</div></body></html>";
       recibo.close();

        file_name = "open \"" + file_name + '\"';
        system(file_name.c_str());
    }
    
    system("read -p \"Enter para continuar\"");
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

    if(value < 0) value *= -1; //if de fresco
    return value;
}
void load_file(){
    int bigger = -1;
	char auxC[100];

    Impressao *auximpressoes = new Impressao;
    CorteALaser *auxlaser = new CorteALaser;
    CorteCNC *auxcnc = new CorteCNC;
	string aux[12];
    string auxLaserCNC[9];
	
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
			auximpressoes->set_cliente(auxC);
            strcpy(auxC, aux[2].c_str());
			auximpressoes->set_objectName(auxC);
			auximpressoes->set_layer_height( atof(aux[3].c_str()));
			auximpressoes->set_infill( atof(aux[4].c_str()));
			auximpressoes->set_cost_used( atof(aux[5].c_str()));
            auximpressoes->set_cost_time( atof(aux[6].c_str()));
			auximpressoes->set_filament_used(atof(aux[7].c_str()));
            auximpressoes->set_filament_type(atoi(aux[8].c_str()));
            auximpressoes->set_time(atoi(aux[9].c_str()));
            auximpressoes->set_tipo_os(atoi(aux[10].c_str()));
            auximpressoes->set_impressora(atoi(aux[11].c_str()));

            if(auximpressoes->get_os() > bigger) bigger = auximpressoes->get_os();

			lista_impressao.push_back(*auximpressoes);
		}
	}
    impressoes.close();

    check = false;
    laser.open("laser.csv", ios::in);
	if(!laser.is_open()) cout << "arquivo laser.csv nao carregado" << endl;

	while(laser.good()){
		getline(laser, auxLaserCNC[0], ';');
		if(auxLaserCNC[0] != ""){
			getline(laser, auxLaserCNC[1], ';');
            getline(laser, auxLaserCNC[2], ';');
            getline(laser, auxLaserCNC[3], ';');
            getline(laser, auxLaserCNC[4], ';');
            getline(laser, auxLaserCNC[5], ';');
            getline(laser, auxLaserCNC[6], ';');
            getline(laser, auxLaserCNC[7], ';');
			getline(laser, auxLaserCNC[8], '\n');
            check = true; // marca como verdadeiro se o arquivo tiver informacoes
		}
		
		if(check){
            auxlaser->set_os(atoi(auxLaserCNC[0].c_str()));
            strcpy(auxC, auxLaserCNC[1].c_str());
			auxlaser->set_cliente(auxC);
            strcpy(auxC, auxLaserCNC[2].c_str());
			auxlaser->set_objectName(auxC);
            auxlaser->set_area(atof(auxLaserCNC[3].c_str()));
            auxlaser->set_perimetro(atof(auxLaserCNC[4].c_str()));
            strcpy(auxC, auxLaserCNC[5].c_str());
            auxlaser->set_material(auxC);
            auxlaser->set_tipo_os(atoi(auxLaserCNC[6].c_str()));
            auxlaser->set_minutes(atoi(auxLaserCNC[7].c_str()));
            auxlaser->set_cost_time(atof(auxLaserCNC[8].c_str()));

            if(auxlaser->get_os() > bigger) bigger = auxlaser->get_os();

			lista_laser.push_back(*auxlaser);
		}
	}
    laser.close();

    check = false;
    cncfresa.open("cncfresa.csv", ios::in);
	if(!cncfresa.is_open()) cout << "arquivo laser.csv nao carregado" << endl;

	while(cncfresa.good()){
		getline(cncfresa, auxLaserCNC[0], ';');
		if(auxLaserCNC[0] != ""){
			getline(cncfresa, auxLaserCNC[1], ';');
            getline(cncfresa, auxLaserCNC[2], ';');
            getline(cncfresa, auxLaserCNC[3], ';');
            getline(cncfresa, auxLaserCNC[4], ';');
            getline(cncfresa, auxLaserCNC[5], ';');
            getline(cncfresa, auxLaserCNC[6], ';');
            getline(cncfresa, auxLaserCNC[7], ';');
			getline(cncfresa, auxLaserCNC[8], '\n');
            check = true;
		}
		
		if(check){
            auxcnc->set_os(atoi(auxLaserCNC[0].c_str()));
            strcpy(auxC, auxLaserCNC[1].c_str());
			auxcnc->set_cliente(auxC);
            strcpy(auxC, auxLaserCNC[2].c_str());
			auxcnc->set_objectName(auxC);
            auxcnc->set_area(atof(auxLaserCNC[3].c_str()));
            auxcnc->set_perimetro(atof(auxLaserCNC[4].c_str()));
            strcpy(auxC, auxLaserCNC[5].c_str());
            auxcnc->set_material(auxC);
            auxcnc->set_tipo_os(atoi(auxLaserCNC[6].c_str()));
            auxcnc->set_minutes(atoi(auxLaserCNC[7].c_str()));
            auxcnc->set_cost_time(atof(auxLaserCNC[8].c_str()));

            if(auxcnc->get_os() > bigger) bigger = auxcnc->get_os();

			lista_cnc.push_back(*auxcnc);
		}
	}
    cncfresa.close();

    ordem_de_servico = bigger + 1;
}
void save_file(){
	impressoes.open("impressao.csv", ios::out); // escrever no arquivo	
	
	for(int i = 0; i < lista_impressao.size(); i++){
        impressoes << lista_impressao[i].get_os() << ";";
        impressoes << lista_impressao[i].get_cliente() << ";";    
		impressoes << lista_impressao[i].get_objectName() << ";";
		impressoes << lista_impressao[i].get_layer_height() << ";";
		impressoes << lista_impressao[i].get_infill() << ";";
		impressoes << lista_impressao[i].get_cost_used() << ";";
        impressoes << lista_impressao[i].get_cost_time() << ";";
		impressoes << lista_impressao[i].get_filament_used() << ";";
		impressoes << lista_impressao[i].get_filament_type() << ";";
        impressoes << lista_impressao[i].get_minutes() << ";";
        impressoes << lista_impressao[i].get_tipo_os() << ";";

		if(i < lista_impressao.size() - 1) impressoes << lista_impressao[i].get_printer_int() << "\n";
		else impressoes << lista_impressao[i].get_printer_int();
	}	
	impressoes.close();

    laser.open("laser.csv", ios::out);
    for(int i = 0; i < lista_laser.size(); i++){
        laser << lista_laser[i].get_os() << ";";
        laser << lista_laser[i].get_cliente() << ";";
        laser << lista_laser[i].get_objectName() << ";";
        laser << lista_laser[i].get_area() << ";";
        laser << lista_laser[i].get_perimetro() << ";";
        laser << lista_laser[i].get_material() << ";";
        laser << lista_laser[i].get_tipo_os() << ";";
        laser << lista_laser[i].get_minutes() << ";";
        
        if(i < lista_laser.size() - 1) laser << lista_laser[i].get_cost_time() << "\n";
        else laser << lista_laser[i].get_cost_time();
    }
    laser.close();

    cncfresa.open("cncfresa.csv", ios::out);
    for(int i = 0; i < lista_cnc.size(); i++){
        cncfresa << lista_cnc[i].get_os() << ";";
        cncfresa << lista_cnc[i].get_cliente() << ";";
        cncfresa << lista_cnc[i].get_objectName() << ";";
        cncfresa << lista_cnc[i].get_area() << ";";
        cncfresa << lista_cnc[i].get_perimetro() << ";";
        cncfresa << lista_cnc[i].get_material() << ";";
        cncfresa << lista_cnc[i].get_tipo_os() << ";";
        cncfresa << lista_cnc[i].get_minutes() << ";";
        
        if(i < lista_cnc.size() - 1) cncfresa << lista_cnc[i].get_cost_time() << "\n";
        else cncfresa << lista_cnc[i].get_cost_time();
    }
    cncfresa.close();
}
bool validacao_int(int init, int final, int value, int text_menu){
    bool check = false;
    
    for(int i = init; i <= final; i++) {
        if(value == i){ 
            check = true;
            break;
        }
    }
    if(!check){
        TextoDoMenu(text_menu);
        cout << "valor invalido digite novamente" << endl;
        system("read -p \"Enter para continuar\"");    
    }
    return check;
}
int validacao_os(int os, int menu){
    bool check = false;
    
    for(int i = 0; i < lista_impressao.size(); i++){
        if(os == lista_impressao[i].get_os()) {
            check = true;
            return 1;
        }
    }
    for(int i = 0; i < lista_laser.size(); i++){
        if(os == lista_laser[i].get_os()) {
            check = true;
            return 2;
        }
    }
    for(int i = 0; i < lista_cnc.size(); i++){
        if(os == lista_cnc[i].get_os()) {
            check = true;
            return 3;
        }
    }

    if(!check){
        TextoDoMenu(menu);
        cout << "Ordem de servico nao encontrada. Digite novamente\n"; 
        system("read -p \"Enter para continuar\"");    
        
        return -1;
    }
    return -1;            
}
string SeparadorHTML(){
    string sep = "<tr><td></td><td></td></tr><tr><td></td><td></td></tr><tr class=\"borda\"><td></td><td></td></tr><tr><td></td><td></td></tr><tr><td></td><td></td></tr>";
    return sep;
}
string HeaderAndStyle(int tipo, int os, string cliente){
    string aux = "Recibo";
    if(tipo == 5) aux = " Relatorio de uso";
    string head = "<!DOCTYPE html><html><head><title>" + aux + "</title><style>#box{width: 700px;background-color:palegoldenrod;padding-left: 1%;padding-top: 0.1%;padding-bottom: 0.5%;padding-right: 1%;}table{width: 100%;}.borda {background-color: black;}#borda{border: solid black 1px;}</style></head>";
    head += "<body><div id=\"box\"><h2>" + aux + "</h2>";
    if(tipo != 5){
        head += "<h4>Ordem de Servico: " + to_string(os) + "</h4>";
        head += "<h4>Tipo de Servico: " + string(tipo_de_servico(tipo)) + "</h4>";
        head += "<h4>Cliente: " + cliente + "</h4>";
    }
    return head;
}
string GerarRelatorio(int tipo, int os){
    string html;
    switch(tipo){
        case 1:
            html = "<table>"; 
            for(int i = 0; i < lista_impressao.size(); i++){
                if(os == lista_impressao[i].get_os()){
                    html += SeparadorHTML();
                    html += "<tr><td>Impressora: </td><td>" + string(lista_impressao[i].get_printer()) + "</td></tr>";
                    html += "<tr><td>Impressao: </td><td>" + string(lista_impressao[i].get_objectName()) + "</td></tr>";
                    html += "<tr><td>Altura de camada: </td><td>" + precision_to_string(lista_impressao[i].get_layer_height(), 2) + " mm</td></tr>";
                    html += "<tr><td>Infill: </td><td>" + precision_to_string(lista_impressao[i].get_infill(), 0) + "%</td></tr>";
                    html += "<tr><td>Filamento: </td><td>" + string(lista_impressao[i].get_filament_type_name()) + "</td></tr>";
                    html += "<tr><td>Filamento usado: </td><td>" + precision_to_string(lista_impressao[i].get_filament_used(), 2) + "g</td></tr>";
                    html += "<tr><td>Tempo de impressao: </td><td>" + precision_to_string(lista_impressao[i].get_minutes(), 2) + " minutos</td></tr>";
                    html += "<tr><td>Custo por tempo: </td><td>R$" + precision_to_string(lista_impressao[i].get_cost_time(), 2) + "</td></tr>";
                    html += "<tr><td>Custo por material: </td><td>R$" + precision_to_string(lista_impressao[i].get_cost_used(), 2) + "</td></tr>";
                    html += "<tr><td>Custo total da peca: </td><td>R$" + precision_to_string(lista_impressao[i].get_cost_time() + lista_impressao[i].get_cost_used(), 2)  + "</td></tr>";
                    html += "<tr><td></td><td></td><tr>";
                    html += "<tr><td></td><td></td><tr>";
                }
            }
            html += "</table>";  
            break;
        case 2:
            html = "<table>"; 
            for(int i = 0; i < lista_laser.size(); i++){
                if(os == lista_laser[i].get_os()){
                    html += SeparadorHTML();
                    html += "<tr><td>Corte: </td><td>" + string(lista_laser[i].get_objectName()) + "</td></tr>";
                    html += "<tr><td>Area de corte: </td><td>" + precision_to_string(lista_laser[i].get_area(), 2) + " mm^2</td></tr>";
                    html += "<tr><td>Perimetro: </td><td>" + precision_to_string(lista_laser[i].get_perimetro(), 2) + " mm</td></tr>";
                    html += "<tr><td>Material: </td><td>" + string(lista_laser[i].get_material()) + "</td></tr>";
                    html += "<tr><td>Tempo de corte: </td><td>" + precision_to_string(lista_laser[i].get_minutes(), 0) + " minutos</td></tr>";
                    html += "<tr><td>Custo por tempo: </td><td>R$" + precision_to_string(lista_laser[i].get_cost_time(), 2) + "</td></tr>";
                    html += "<tr><td></td><td></td><tr>";
                    html += "<tr><td></td><td></td><tr>";
                }
            }
            html += "</table>";
            break;
        case 3:
            html = "<table>"; 
            for(int i = 0; i < lista_cnc.size(); i++){
                if(os == lista_cnc[i].get_os()){
                    html += SeparadorHTML();
                    html += "<tr><td>Corte: </td><td>" + string(lista_cnc[i].get_objectName()) + "</td></tr>";
                    html += "<tr><td>Area de corte: </td><td>" + precision_to_string(lista_cnc[i].get_area(), 2) + " mm^2</td></tr>";
                    html += "<tr><td>Perimetro: </td><td>" + precision_to_string(lista_cnc[i].get_perimetro(), 2) + " mm</td></tr>";
                    html += "<tr><td>Material: </td><td>" + string(lista_cnc[i].get_material()) + "</td></tr>";
                    html += "<tr><td>Tempo de corte: </td><td>" + precision_to_string(lista_cnc[i].get_minutes(), 0) + " minutos</td></tr>";
                    html += "<tr><td>Custo por tempo: </td><td>R$" + precision_to_string(lista_cnc[i].get_cost_time(), 2) + "</td></tr>";
                    html += "<tr><td></td><td></td><tr>";
                    html += "<tr><td></td><td></td><tr>";
                }
            }
            html += "</table>";
            break;
        case 5:
            int media_tempo_por_os = 0, qt_pecas = 0, media_tempo = 0, qt_os = 0;
            
            html = "<table>";
        
            for(int i = 0; i < lista_impressao.size(); i++){
                if(!i) os = lista_impressao[i].get_os();
                
                if(lista_impressao[i].get_os() == os){
                    media_tempo_por_os += lista_impressao[i].get_minutes();
                    qt_pecas++;
                }
                else if(lista_impressao[i].get_os() > os){
                    os = lista_impressao[i].get_os();
                    media_tempo += (media_tempo_por_os / qt_pecas) + 1;
                    media_tempo_por_os = 0;
                    qt_pecas = 0;

                }
            }
            html += "</table>";
          break;
    }
    return html;
}
template <class type> string precision_to_string(type value, int precision){
    stringstream stream;
    stream << fixed << setprecision(precision) << value;    
    string s = stream.str();
    return s;
}
void RelatorioDeUso(){
    string file_name = "Relatorio de Uso.html";

       recibo.open(file_name.c_str(), ios::out);

       recibo << HeaderAndStyle(5);
       recibo << GerarRelatorio(5);
       recibo << "</div></body></html>";
       recibo.close();

        file_name = "open \"" + file_name + '\"';
        system(file_name.c_str());
}