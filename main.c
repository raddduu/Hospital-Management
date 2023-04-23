#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NR_DOCTORI 3
#define NR_PACIENTI 15
#define LIMITA_TIMP 10

pthread_mutex_t doctori[NR_DOCTORI];

int urm_doctor = 0;

void* thread_pacient(void* arg)
{
    int id = (int) arg;

    int asteptare = rand() % LIMITA_TIMP;

    sleep(asteptare);

    int doctor_id = -1;

    struct timespec moment_inceput;

    clock_gettime(CLOCK_MONOTONIC, &moment_inceput);

    while (doctor_id == -1)
    {
        for (int i = 0; i < NR_DOCTORI; i++)
	  {
            int curr_doctor = (urm_doctor + i) % NR_DOCTORI;

            if (pthread_mutex_trylock(&doctori[curr_doctor]) == 0)
		{
                doctor_id = curr_doctor;

                urm_doctor = (curr_doctor + 1) % NR_DOCTORI;

                break;
            }
        }

        if (doctor_id == -1)
        {
            sleep(rand() % LIMITA_TIMP);
        }
    }

    struct timespec moment_final;

    clock_gettime(CLOCK_MONOTONIC, &moment_final);

    long timp_asteptare = (moment_final.tv_sec * 1000 + moment_final.tv_nsec / 1000000) - (moment_inceput.tv_sec * 1000 + moment_inceput.tv_nsec / 1000000);

    sleep(rand() % LIMITA_TIMP);

    clock_gettime(CLOCK_MONOTONIC, &moment_final);

    long timp_consult = (moment_final.tv_sec * 1000 + moment_final.tv_nsec / 1000000) - (moment_inceput.tv_sec * 1000 + moment_inceput.tv_nsec / 1000000);

    printf("Patient %d waited for %ld ms, consulted with doctor %d for %ld ms.\n", id, timp_asteptare, doctor_id, timp_consult - timp_asteptare);

    pthread_mutex_unlock(&doctori[doctor_id]);

    return NULL;
}

int main()
{
    srand(time(NULL));

    for (int i = 0; i < NR_DOCTORI; i++)
    {
        pthread_mutex_init(&doctori[i], NULL);
    }

    pthread_t pacienti[NR_PACIENTI];

    for (int i = 0; i < NR_PACIENTI; i++)
    {
        pthread_create(&pacienti[i], NULL, thread_pacient, (void*) i);
    }

    for (int i = 0; i < NR_PACIENTI; i++)
    {
        pthread_join(pacienti[i], NULL);
    }

    for (int i = 0; i < NR_DOCTORI; i++)
    {
        pthread_mutex_destroy(&doctori[i]);
    }

    return 0;
}
