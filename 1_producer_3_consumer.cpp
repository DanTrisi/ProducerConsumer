#include <omp.h>
#include <iostream>
#include <fstream>

using namespace std;

const int DIM = 10;

int main() {
    omp_set_num_threads(4); // 1 producer + 3 consumer

    fstream myin("ingresso.txt", ios::in);
    if (myin.fail()) {
        cerr << "Il file ingresso.txt non esiste\n";
        return 1;
    }

    char* buffer = new char[DIM];
    int i = 0, j = 0;
    bool termina = false;
    double start = omp_get_wtime();

    #pragma omp parallel sections shared(buffer, i, j, termina)
    {
        // Producer
        #pragma omp section
        {
            printf("Il producer è il numero [%d] \n", omp_get_thread_num());
                        
            while (true) {
                char c = myin.get();
                if (c == '0' || myin.eof()) {
                    //#pragma omp flush
                    termina = true;
                    #pragma omp flush(termina)
                    break;
                }
                int nexti = (i + 1) % DIM;
                while (nexti == j); 
                buffer[i] = c;
                #pragma omp flush(buffer)
                i = nexti;
                #pragma omp flush(i)
            }
        }

        // Consumer 1
        #pragma omp section
        {
            while (true) {
                #pragma omp flush(i, j, termina)
                if (termina && j == i) break;
                #pragma omp critical (read_section)
                {
                    if (j != i) {
                        printf("Consumer1 [%d] legge %c\n", omp_get_thread_num(), buffer[j]);
                        j = (j + 1) % DIM;
                        #pragma omp flush(j)
                    }
                }
            }
        }

        // Consumer 2
        #pragma omp section
        {
            while (true) {
                #pragma omp flush(i, j, termina)
                if (termina && j == i) break;

                #pragma omp critical (read_section)
                {
                    if (j != i) {
                        printf("Consumer2 [%d] legge %c\n", omp_get_thread_num(), buffer[j]);
                        j = (j + 1) % DIM;
                        #pragma omp flush(j)
                    }
                }
            }
        }
        // Consumer 3
        #pragma omp section
        {
            while (true) {
                #pragma omp flush(i, j, termina)
                if (termina && j == i) break;

                #pragma omp critical (read_section)
                {
                    if (j != i) {
                        printf("Consumer3 [%d] legge %c\n", omp_get_thread_num(), buffer[j]);
                        j = (j + 1) % DIM;
                        #pragma omp flush(j)
                    }
                }
            }
        }
    }

    double end = omp_get_wtime();
    cout << "\nTempo impiegato: " << end - start << " secondi" << endl;

    delete[] buffer;
    return 0;
}
