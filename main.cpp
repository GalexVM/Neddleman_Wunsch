#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>

struct casilla{
    casilla* left = nullptr, *top=nullptr, *diagonal = nullptr;
    std::string valor = "__";
    char l,t;
};

using tablero = std::vector<casilla>;
using alignment = std::vector<std::vector<std::string>>;
using std::string;
using std::vector;
using std::cout;
using std::pair;
using std::max;
using std::stoi;

int match = 1, indel = -2, mismatch = -1;
void iniciarlizarTablero(tablero& tab, unsigned int fsize, unsigned int csize, string c1, string c2);
void imprimirTablero(tablero& tab, unsigned int fsize, unsigned int csize);
void NeddlemanWunsch(tablero& tab, unsigned int fsize, unsigned int csize);
std::vector<alignment> findAllAlingments(tablero& tab, unsigned int fsize, unsigned int csize, casilla* temp= nullptr);
alignment findBestAlignment(tablero& tab, unsigned  int fsize, unsigned  int csize);
void printPathsToFile(vector<alignment>& recorridos);
void printAlignmentsToFile(vector<alignment>& recorridos, unsigned int fsize, unsigned int csize);
void printPathsToFile(alignment& recorrido);
void printAlignmentsToFile(alignment& recorrido, unsigned int fsize, unsigned int csize);
void leerInputs(string& st1, string& st2, string file);

int main() {
    string cadena1;
    string cadena2;
    leerInputs(cadena1,cadena2,"input.txt");
    auto fsize = cadena2.size()+2;
    auto csize = cadena1.size()+2;
    tablero tab1((cadena1.size()+2)*(cadena2.size()+2));
    iniciarlizarTablero(tab1, fsize, csize,cadena1,cadena2);
    //imprimirTablero(tab1, fsize, csize);
    NeddlemanWunsch(tab1, fsize, csize);
    //imprimirTablero(tab1, fsize, csize);
    //auto rs = findAllAlingments(tab1,fsize,csize);
    //printPathsToFile(rs);
    //printAlignmentsToFile(rs,fsize,csize);
    auto best = findBestAlignment(tab1,fsize,csize);
    printPathsToFile(best);
    printAlignmentsToFile(best,fsize,csize);
}

void leerInputs(string& st1, string& st2, string file){
    std::ifstream input(file);
    string temp;
   // std::getline(input,temp);
    std::getline(input,st1);
    std::getline(input,st2);
    input.close();
}
void printAlignmentsToFile(vector<alignment>& recorridos, unsigned int fsize, unsigned int csize){
    //int size = std::max((int)fsize,(int)csize);
    auto size = recorridos[0].size();
    std::ofstream file("alineaciones.txt");
    if(!file.is_open()){
        cout<<"Error abriendo file de alineaciones\n";
    }
    else{
        for(auto& r : recorridos){
            string t(size,' ');
            string t2(size,' ');
            auto indice = size-1;
            for(auto& c : r){
                if(c[3] == "diagonal"){
                    t[indice] = c[0][0];
                    t2[indice] = c[1][0];
                }else if(c[3] == "top"){
                    t[indice] = c[0][0];
                    t2[indice] = '-';
                }else if(c[3] == "left"){
                    t[indice] = '-';
                    t2[indice] = c[1][0];
                }else
                    cout<<"Error. información de casilla indefinida\n";
                indice--;
            }
            file << t << "\n";
            file << t2 << "\n" << "\n";
        }
    }
    file.close();
}
void printPathsToFile(vector<alignment>& recorridos) {
    std::ofstream file("recorridos.txt");
    if(!file.is_open()){
        cout<<"Error abriendo file de recorridos\n";
    }
    else{
        for(auto& r : recorridos){
            for(auto& c : r){
                file << "{";
                for(auto& str : c){
                    file << str << ", ";
                }
                file <<"}, ";
            }
            file << "\n";
        }

    }
    file.close();
}
void printPathsToFile(alignment& recorrido){
    vector<alignment> vec;
    vec.push_back(recorrido);
    printPathsToFile(vec);
}
void printAlignmentsToFile(alignment& recorrido, unsigned int fsize, unsigned int csize){
    vector<alignment> vec;
    vec.push_back(recorrido);
    printAlignmentsToFile(vec,fsize,csize);
}
alignment findBestAlignment(tablero& tab, unsigned  int fsize, unsigned  int csize){
    alignment  bestAlignment;
    auto temp = &(tab[csize*(fsize-1)+csize-1]); //inicio (inf. der.)
    int score = 0;
    while(!(temp->left == nullptr && temp->top == nullptr && temp->diagonal == nullptr)){
        if(temp->left != nullptr && temp->top == nullptr && temp->diagonal == nullptr){
            bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //x,y,v,dir
            temp = temp->left; score+= stoi(temp->left->valor);
        }
        else if(temp->left == nullptr && temp->top != nullptr && temp->diagonal == nullptr){
            bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"}); //x,y,v,dir
            temp = temp->top; score+= stoi(temp->top->valor);
        }
        else if(temp->left == nullptr && temp->top == nullptr && temp->diagonal != nullptr){
            bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //x,y,v,dir
            temp = temp->diagonal; score+= stoi(temp->diagonal->valor);
        }
        else if(temp->left != nullptr && temp->top != nullptr && temp->diagonal == nullptr){
            if(std::stoi(temp->left->valor) > std::stoi(temp->top->valor)){
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //x,y,v,dir
                temp = temp->left; score+= stoi(temp->left->valor);
            }
            else{
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"}); //x,y,v,dir
                temp = temp->top; score+= stoi(temp->top->valor);
            }
        }
        else if(temp->left == nullptr && temp->top != nullptr && temp->diagonal != nullptr){
            if(std::stoi(temp->diagonal->valor) > std::stoi(temp->top->valor)){
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //x,y,v,dir
                temp = temp->diagonal; score+= stoi(temp->diagonal->valor);
            }
            else{
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"}); //x,y,v,dir
                temp = temp->top; score+= stoi(temp->top->valor);
            }
        }
        else if(temp->left != nullptr && temp->top == nullptr && temp->diagonal != nullptr){
            if(std::stoi(temp->left->valor) > std::stoi(temp->diagonal->valor)){
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //x,y,v,dir
                temp = temp->left; score+= stoi(temp->left->valor);
            }
            else{
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //x,y,v,dir
                temp = temp->diagonal; score+= stoi(temp->diagonal->valor);
            }
        }
        else if(temp->left != nullptr && temp->top != nullptr && temp->diagonal != nullptr){
            auto l = stoi(temp->left->valor), t = stoi(temp->top->valor), d = stoi(temp->diagonal->valor);
            if(max(max(l,t),d) == l){
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //x,y,v,dir
                temp = temp->left; score+= l;
            }
            else if(max(max(l,t),d) == t){
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"}); //x,y,v,dir
                temp = temp->top; score+= t;
            }
            else{
                bestAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //x,y,v,dir
                temp = temp->diagonal; score+= d;
            }
        }
    }
    return bestAlignment;

}
std::vector<alignment> findAllAlingments(tablero& tab, unsigned int fsize, unsigned int csize, casilla* temp){
    std::vector<alignment> alignments;
    alignment firstAlignment;
    temp = (!temp) ? &(tab[csize*(fsize-1)+csize-1]) : temp;//start point

    while(!(temp->left == nullptr && temp->top == nullptr && temp->diagonal == nullptr)){
        if(temp->left != nullptr && temp->top == nullptr && temp->diagonal == nullptr){
            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //x,y,v,dir
            temp = temp->left;
        }
        else if(temp->left == nullptr && temp->top != nullptr && temp->diagonal == nullptr){
            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"}); //x,y,v,dir
            temp = temp->top;
        }
        else if(temp->left == nullptr && temp->top == nullptr && temp->diagonal != nullptr){
            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //x,y,v,dir
            temp = temp->diagonal;
        }
        else if(temp->left != nullptr && temp->top != nullptr && temp->diagonal == nullptr){


            alignment newAlignment(firstAlignment);//Enviar camino alterno a función recursiva
            newAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"});
            auto extraAlignments = findAllAlingments(tab,fsize,csize,temp->top);
            for(auto& i : extraAlignments)
                i.insert(i.begin(),newAlignment.begin(),newAlignment.end());
            alignments.insert(alignments.end(),extraAlignments.begin(),extraAlignments.end()); //Concatenar vectores

            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //Camino original
            temp = temp->left;
        }
        else if(temp->left == nullptr && temp->top != nullptr && temp->diagonal != nullptr){
            alignment newAlignment(firstAlignment);//Enviar camino alterno a función recursiva
            newAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"});

            auto extraAlignments = findAllAlingments(tab,fsize,csize,temp->top);
            for(auto& i : extraAlignments)
                i.insert(i.begin(),newAlignment.begin(),newAlignment.end());
            alignments.insert(alignments.end(),extraAlignments.begin(),extraAlignments.end()); //Concatenar vectores

            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"}); //Camino original
            temp = temp->diagonal;
        }
        else if(temp->left != nullptr && temp->top == nullptr && temp->diagonal != nullptr){
            alignment newAlignment(firstAlignment);//Enviar camino alterno a función recursiva
            newAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"});

            auto extraAlignments = findAllAlingments(tab,fsize,csize,temp->diagonal);
            for(auto& i : extraAlignments)
                i.insert(i.begin(),newAlignment.begin(),newAlignment.end());
            alignments.insert(alignments.end(),extraAlignments.begin(),extraAlignments.end()); //Concatenar vectores

            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //Camino original
            temp = temp->left;
        }
        else if(temp->left != nullptr && temp->top != nullptr && temp->diagonal != nullptr){
            alignment newAlignment(firstAlignment);
            newAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "top"});
            auto extraAlignments = findAllAlingments(tab,fsize,csize,temp->top);
            for(auto& i : extraAlignments)
                i.insert(i.begin(),newAlignment.begin(),newAlignment.end());
            alignments.insert(alignments.end(),extraAlignments.begin(),extraAlignments.end()); //Concatenar vectores

            alignment newAlignment2(firstAlignment);
            newAlignment2.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "diagonal"});
            auto extraAlignments2 = findAllAlingments(tab,fsize,csize,temp->diagonal);
            for(auto& i : extraAlignments)
                i.insert(i.begin(),newAlignment.begin(),newAlignment.end());
            alignments.insert(alignments.end(),extraAlignments.begin(),extraAlignments.end()); //Concatenar vectores

            firstAlignment.push_back({string(1,temp->l),string(1,temp->t),temp->valor, "left"}); //camino original
            temp = temp->left;
        }

    }
    alignments.push_back(firstAlignment);
    return alignments;



}
void NeddlemanWunsch(tablero& tab, unsigned int fsize, unsigned int csize){
    for(int i = 2; i < fsize; i++){
        for(int j = 2; j < csize; j++){
            auto columnLetter = tab[j].valor;
            auto rowLetter = tab[csize*i].valor;
            tab[csize*i+j].l = rowLetter[0];
            tab[csize*i+j].t = columnLetter[0];
            auto m = (columnLetter == rowLetter) ? match : mismatch;
            int op1 = std::stoi(tab[csize*i+j-1].valor); //left
            int op2 = std::stoi(tab[csize*(i-1)+j].valor); //up
            int op3 = std::stoi(tab[csize*(i-1)+(j-1)].valor); //diagonal

            auto mayor =  std::max(std::max(op1+indel,op2+indel),op3+m);
            tab[csize*i+j].valor = std::to_string(mayor); //valor elegido
            if(mayor == op1+indel) tab[csize*i+j].left = &(tab[csize*i+j-1]); //puntero al mayor
            if(mayor == op2+indel) tab[csize*i+j].top = &(tab[csize*(i-1)+j]);
            if(mayor == op3+m) tab[csize*i+j].diagonal = &(tab[csize*(i-1)+j-1]);
        }

    }
}
void iniciarlizarTablero(tablero& tab, unsigned int fsize, unsigned int csize, string c1, string c2){
    int valorInicial = 0;
    tab[csize*1 + 1].valor =  std::to_string(valorInicial);
    for(int i = 2; i < csize || i < fsize; i++) //Inicializar letras
    {
        valorInicial+=indel;
        if(i<csize){
            tab[i].valor =  string(1,c1[i-2]);
            tab[csize*1 + i].valor = std::to_string(valorInicial); //Valores iniciales de las columnas
            tab[csize*1 + i].left = &(tab[csize*1 + (i-1)]); //Puntero a la izq
            tab[csize*1 + i].t = tab[i].valor[0];
            tab[csize*1 + i].l = '_';
        }
        if(i<fsize){
            tab[csize*i].valor = string(1,c2[i-2]);
            tab[csize*i + 1].valor = std::to_string(valorInicial); //Valores iniciales de las filas
            tab[csize*i + 1].top = &(tab[csize*(i-1) + 1]); //Puntero arriba
            tab[csize*i + 1].l = tab[csize*i].valor[0];
            tab[csize*i + 1].t = '_';
        }
    }
}
void imprimirTablero(tablero& tab, unsigned int fsize, unsigned int csize){
    for(int i = 0; i < fsize; i++){
        for(int j = 0; j < csize; j++){
            std::cout << tab[csize*i + j].valor << "\t";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}
