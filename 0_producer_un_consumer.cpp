#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <fstream>

using namespace std; 

const int DIMENSIONE_DI_PARTENZA = 20;

int main(){
    omp_set_num_threads(2);
    //============================ creato stream ingresso ===========================
    fstream myin; 
    myin.open("ingresso.txt",ios::in);
        if (myin.fail()) {
            cerr << "Il file " << "ingresso.txt" << " non esiste\n";
            exit(1);
        }
    //============================ variabili ===========================
    char * buffer = new char[DIMENSIONE_DI_PARTENZA];
    char c;
    int dim=DIMENSIONE_DI_PARTENZA,i=0,j=0,nexti,locali,localj;
    bool flag, termina=false,localtermina; 
    double start, end; 
    start=omp_get_wtime();
    #pragma omp parallel sections
    {
        //########################## producer #############################################
        #pragma omp section
        { //producer                                        
            while(1){
                nexti=(i+1)%dim;
                //leggo j aggiornata dal consumer
                #pragma omp atomic read
                localj=j;
                if(nexti==localj){
                    continue;
                }
                c=myin.get(); 
                if(c=='0') {
                    //scrivo la flag per terminare
                    #pragma omp atomic write
                    termina=true;
                    break;}
                buffer[i]=c;
                //flush della variabile buffer
                #pragma omp flush (buffer)
                //scrivo l'aggiornamento di i
                #pragma omp atomic write
                i=nexti;
            }
        }
        //########################## consumer #############################################
        #pragma omp section
        { 
            while(1){
                do{
                    //leggo i aggiornato da producer
                    #pragma omp atomic read
                    locali=i; 
                    //leggo termina aggiornato da producer
                    #pragma omp atomic read
                    localtermina=termina;
                    if(termina==1&&locali==j) 
                        break;
                }while(locali==j);
                if(localtermina && locali == j)
                    break;
                printf("Thread %d read %c\n",omp_get_thread_num(),buffer[j]);
                //scrivo l'aggiornamento di j
                #pragma omp atomic write
                j=(j+1)%dim; 
            }
        }
    }
    end=omp_get_wtime();
    cout<<endl<<endl<<endl<<"tempo impiegato  :   "<<end-start<<endl; 
    return 0; 
}