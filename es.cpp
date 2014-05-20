/*
 * algorithm: enumeration sort
 * author: Petr Kubat, xkubat11@stud.fit.vutbr.cz
 *
 */

 #include <mpi.h>
 #include <iostream>
 #include <fstream>

 using namespace std;

// tags
 #define TAG 0
 #define TAG2 1
 #define TAG3 2
 #define TAG4 3

 int main(int argc, char *argv[])
 {
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error

    // registry
    int regX = -1;
	int regY = -1;
	int regZ = -1;
	int regC = 1;

	// pro proc 0
	fstream fin;
	char input[]= "numbers";                          //jmeno souboru
    int number;                                     //hodnota pri nacitani souboru

    //MPI INIT
    MPI_Init(&argc,&argv);                          // inicializace MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // zjistíme, kolik procesů běží
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // zjistíme id svého procesu

    // otevreni souboru v masterovi
    if(myid == 0) {
        fin.open(input, ios::in);
    }

    /*
     * Prvni faze
     * 		nacteni a rozeslani cisel + shift a inkrementace registru C
     * */
	for(int i=1; i<numprocs; i++) {
		if(myid == 0){
			// nacteni cisel v masteru
			number= fin.get();
            cout<<number<<" ";
            number=number*100+i;
            MPI_Send(&number, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
            MPI_Send(&number, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
		}
			else {
				if(myid == 1) {
					MPI_Recv(&regY, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
					MPI_Send(&regY, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);
				}
				if(myid == i) {
					MPI_Recv(&regX, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
				}
				if (regX != -1 && regY != -1) {
					if (regX > regY) {
						regC++;
					}
				}
				if(myid != numprocs-1 && myid > 1) {
					MPI_Send(&regY, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);
					MPI_Recv(&regY, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
				}
				if(myid == numprocs-1) {
					MPI_Recv(&regY, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
				}
			}
	}
	if(myid == 0) {
		cout<<endl;
		fin.close();
	}

	// Zarazka pro inkrementaci registru Z
	if(myid == 1)
		regY = -1;

	/*
     * Druha faze
     * 		Shift registru y
     * */
	for(int i=1; i<numprocs; i++) {
		if(myid > 0) {
				if(myid == 1) {
					MPI_Send(&regY, 1, MPI_INT, myid+1, TAG3, MPI_COMM_WORLD);
				}
				if (regX != -1 && regY != -1) {
					if (regX > regY) {
						regC++;
					}
				}
				if(myid != numprocs-1 && myid > 1) {
					MPI_Send(&regY, 1, MPI_INT, myid+1, TAG3, MPI_COMM_WORLD);
					MPI_Recv(&regY, 1, MPI_INT, myid-1, TAG3, MPI_COMM_WORLD, &stat);
				}
				if(myid == numprocs-1) {
					MPI_Recv(&regY, 1, MPI_INT, myid-1, TAG3, MPI_COMM_WORLD, &stat);
				}

		}
	}
	/*
     * Treti faze
     * 		Rozesilani registru Z
     * */
	for(int i=1; i<numprocs; i++) {
		if(myid > 0) {
				if (regX != -1 && regY != -1) {
					if (regX > regY)
						regC++;
				}
				if (myid == i){
					MPI_Send(&regX, 1, MPI_INT, regC, TAG2, MPI_COMM_WORLD);
					MPI_Recv(&regZ, 1, MPI_INT, MPI_ANY_SOURCE, TAG2, MPI_COMM_WORLD, &stat);
					regZ=regZ/100;
				}
			}
	}
	/*
     * Ctvrta faze
     * 		Shift registru Z vpravo - rotate z posledniho procesoru do mastera
     * 		a nasledny vypis.
     * */
	for(int i=1; i<numprocs; i++) {
		if(myid == 0) {
			MPI_Recv(&regZ, 1, MPI_INT, numprocs-1, TAG4, MPI_COMM_WORLD, &stat);
			cout<<regZ<<endl;
		}
		if(myid == 1) {
			MPI_Send(&regZ, 1, MPI_INT, myid+1, TAG4, MPI_COMM_WORLD);
		}
		if(myid != numprocs-1 && myid > 1) {
			MPI_Send(&regZ, 1, MPI_INT, myid+1, TAG4, MPI_COMM_WORLD);
			MPI_Recv(&regZ, 1, MPI_INT, myid-1, TAG4, MPI_COMM_WORLD, &stat);
		}
		if(myid == numprocs-1) {
			MPI_Send(&regZ, 1, MPI_INT, 0, TAG4, MPI_COMM_WORLD);
			MPI_Recv(&regZ, 1, MPI_INT, myid-1, TAG4, MPI_COMM_WORLD, &stat);
		}
	}

    MPI_Finalize();
    return 0;

 }

