// MPI-server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <mpi.h>

#define MAX_DATA 256

int main(int argc, char** argv)
{
    MPI_Comm client;
    MPI_Status status;
    char port_name[MPI_MAX_PORT_NAME] = {0};
    double buf[MAX_DATA];
    char file_name[] = "mpiportname.txt";
    FILE* port_file = NULL;
    int    size, again;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 1) { printf("Server too big"); fflush(stdout); goto fail; }
    MPI_Open_port(MPI_INFO_NULL, port_name);
    //printf("server available at %s\n", port_name); fflush(stdout);

    port_file = fopen(file_name, "wt");
    if (port_file)
    {
        // open port
        fprintf(port_file, "%s", port_name); fflush(stdout);
        fclose(port_file);
    }
    else
    {
        printf("Failed to open file %s", file_name); fflush(stdout);
        goto fail;
    }

    while (1)
    {
        printf("Waiting for client ...\n"); fflush(stdout);
        MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &client);
        printf("Accepted client\n"); fflush(stdout);

        again = 1;
        while (again) {
            MPI_Recv(buf, MAX_DATA, MPI_DOUBLE,
                MPI_ANY_SOURCE, MPI_ANY_TAG, client, &status);
            switch (status.MPI_TAG)
            {
            case 0:
                printf("Received terminate message ...\n"); fflush(stdout);
                //printf("Freeing comm ...\n"); fflush(stdout);
                //MPI_Comm_free(&client);
                printf("Disconnecting ...\n"); fflush(stdout);
                MPI_Comm_disconnect(&client);
                printf("Closing port ...\n"); fflush(stdout);
                MPI_Close_port(port_name);
                printf("Finalizing MPI ...\n"); fflush(stdout);
                MPI_Finalize();
                printf("Finalized MPI\n"); fflush(stdout);
                return 0;
            case 1:
                printf("Received disconnect message ...\n"); fflush(stdout);
                printf("Disconnecting ...\n"); fflush(stdout);
                MPI_Comm_disconnect(&client);
                printf("Disconnected\n"); fflush(stdout);
                again = 0;
                break;
            case 2:
                /* do something */
                printf("value 0: %lf\n", buf[0]); fflush(stdout);
                break;
            default:
                /* Unexpected message type */
                printf("Abort\n"); fflush(stdout);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }

fail:
    int flag = 0;
    MPI_Finalized(&flag);
    if (!flag)
    {
        printf("Closing port ...\n"); fflush(stdout);
        MPI_Close_port(port_name);
        printf("Finalizing MPI ...\n"); fflush(stdout);
        MPI_Finalize();
        printf("Finalized MPI\n"); fflush(stdout);
    }
    return 0;
}

