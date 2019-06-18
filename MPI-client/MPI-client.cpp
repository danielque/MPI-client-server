// MPI-client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_DATA 256

int main(int argc, char** argv)
{
    MPI_Comm server = MPI_COMM_NULL;
    MPI_Request request = MPI_REQUEST_NULL;
    double buf[MAX_DATA] = {0.};
    char port_name[MPI_MAX_PORT_NAME] = {0};
    char file_name[] = "mpiportname.txt";
    FILE* port_file = NULL;
    char done = 0;
    int tag;

    MPI_Init(&argc, &argv);
    port_file = fopen(file_name, "rt");
    if (port_file)
    {
        // open port
        fgets(port_name, MPI_MAX_PORT_NAME - 1, port_file);
        fclose(port_file);
    }
    else
    {
        printf("Failed to open file %s", file_name); fflush(stdout);
        goto fail;
    }

    printf("Connecting to server ...\n"); fflush(stdout);
    MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &server);
    printf("Connected\n"); fflush(stdout);

    while (!done) {
        tag = 2; /* Action to perform */
        buf[0] = 17.6;
        MPI_Ssend(buf, 64, MPI_DOUBLE, 0, tag, server);
        /* etc */
        done = 1;
    }

    //printf("Sending disconnect message ...\n"); fflush(stdout);
    //MPI_Ssend(buf, 0, MPI_DOUBLE, 0, 1, server);

    printf("Sending terminate message ...\n"); fflush(stdout);
    //MPI_Ssend(buf, 0, MPI_DOUBLE, 0, 0, server);
    MPI_Isend(buf, 0, MPI_DOUBLE, 0, 0, server, &request);
    printf("Waiting for request ...\n"); fflush(stdout);
    MPI_Wait(&request, MPI_STATUSES_IGNORE);

    printf("Disconnecting ...\n"); fflush(stdout);
    MPI_Comm_disconnect(&server);
    printf("Disconnected\n"); fflush(stdout);

fail:
    printf("Finalizing MPI ...\n"); fflush(stdout);
    MPI_Finalize();
    printf("Finalized MPI\n"); fflush(stdout);

    return 0;
}
