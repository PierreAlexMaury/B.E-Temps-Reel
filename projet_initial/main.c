#include "includes.h"
#include "global.h"
#include "fonctions.h"

/**
 * \fn void initStruct(void)
 * \brief Initialisation des structures de l'application (tâches, mutex, 
 * semaphore, etc.)
 */
void initStruct(void);

void reinitStruct(void);

/**
 * \fn void startTasks(void)
 * \brief Démarrage des tâches
 */
void startTasks(void);

/**
 * \fn void deleteTasks(void)
 * \brief Arrêt des tâches
 */
void deleteTasks(void);

int main(int argc, char**argv) {
    printf("#################################\n");
    printf("#      DE STIJL PROJECT         #\n");
    printf("#################################\n");
    
    //signal(SIGTERM, catch_signal);
    //signal(SIGINT, catch_signal);

    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT | MCL_FUTURE);
    /* For printing, please use rt_print_auto_init() and rt_printf () in rtdk.h
     * (The Real-Time printing library). rt_printf() is the same as printf()
     * except that it does not leave the primary mode, meaning that it is a
     * cheaper, faster version of printf() that avoids the use of system calls
     * and locks, instead using a local ring buffer per real-time thread along
     * with a process-based non-RT thread that periodically forwards the
     * contents to the output stream. main() must call rt_print_auto_init(1)
     * before any calls to rt_printf(). If you forget this part, you won't see
     * anything printed.
     */
    rt_print_auto_init(1);
    initStruct();
    startTasks();
    pause();
    deleteTasks();

    return 0;
}    

void initStruct(void) {
    int err;
    /* Creation des mutex */
    if (err = rt_mutex_create(&mutexEtat, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_mutex_create(&mutexMove, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexRobot, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexVerifConnexion, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexServeur, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexArena, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_mutex_create(&mutexCamera, NULL)) {
        rt_printf("Error mutex create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    /* Creation du semaphore */
    if (err = rt_sem_create(&semConnecterRobot, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_sem_create(&semDeplacerRobot, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_sem_create(&semBatterie, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    /*if (err = rt_sem_create(&semWatchdog, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }*/
    
    if (err = rt_sem_create(&semCamera, NULL, 0, S_FIFO)) {
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_sem_create(&semBarriere, NULL, 0, S_FIFO)) { //n-1
        rt_printf("Error semaphore create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    /* Creation des taches */
    if (err = rt_task_create(&tServeur, NULL, 0, PRIORITY_TSERVEUR, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tconnect, NULL, 0, PRIORITY_TCONNECT, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tmove, NULL, 0, PRIORITY_TMOVE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tenvoyer, NULL, 0, PRIORITY_TENVOYER, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tbatterie, NULL, 0, PRIORITY_TBATTERIE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    /*if (err = rt_task_create(&twatchdog, NULL, 0, PRIORITY_TWATCHDOG, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);    d_arena_free(arena);
    }*/
    
    if (err = rt_task_create(&tcamera, NULL, 0, PRIORITY_TCAMERA, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    if (err = rt_task_create(&tclean, NULL, 0, PRIORITY_TCLEAN, 0)) {
                        rt_printf("Error task create: %s\n", strerror(-err));
                        exit(EXIT_FAILURE);
    }

    /* Creation des files de messages */
    if (err = rt_queue_create(&queueMsgGUI, "toto", MSG_QUEUE_SIZE*sizeof(DMessage), MSG_QUEUE_SIZE, Q_FIFO)){
        rt_printf("Error msg queue create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }

    /* Creation des structures globales du projet */
    robot = d_new_robot();
    move = d_new_movement();
    serveur = d_new_server();
    batterie = d_new_battery();
    arena = NULL;
    camera = d_new_camera();

}

void reinitStruct(void) {
    int err;
    
    /* Creation des taches */
    if (err = rt_task_create(&tServeur, NULL, 0, PRIORITY_TSERVEUR, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tconnect, NULL, 0, PRIORITY_TCONNECT, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tmove, NULL, 0, PRIORITY_TMOVE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tenvoyer, NULL, 0, PRIORITY_TENVOYER, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_create(&tbatterie, NULL, 0, PRIORITY_TBATTERIE, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    /*if (err = rt_task_create(&twatchdog, NULL, 0, PRIORITY_TWATCHDOG, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);    d_arena_free(arena);
    }*/
    
    if (err = rt_task_create(&tclean, NULL, 0, PRIORITY_TCLEAN, 0)) {
                        rt_printf("Error task create: %s\n", strerror(-err));
                        exit(EXIT_FAILURE);
    }
    
    if (err = rt_task_create(&tcamera, NULL, 0, PRIORITY_TCAMERA, 0)) {
        rt_printf("Error task create: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    
    /* Creation des structures globales du projet */
    robot = d_new_robot();
    move = d_new_movement();
    serveur = d_new_server();
    batterie = d_new_battery();
    arena = NULL;
    camera = d_new_camera();

}

void startTasks() {
    int err;
    if (err = rt_task_start(&tconnect, &connecter, NULL)) {
        rt_printf("Error task start connecter: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tServeur, &communiquer, NULL)) {
        rt_printf("Error task start serveur: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tmove, &deplacer, NULL)) {
        rt_printf("Error task start move: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tenvoyer, &envoyer, NULL)) {
        rt_printf("Error task start envoyer: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tbatterie, &niveaubatterie, NULL)) {
        rt_printf("Error task start batterie: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    if (err = rt_task_start(&tcamera, &t_camera, NULL)) {
        rt_printf("Error task start camera: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }
    /*if (err = rt_task_start(&twatchdog, &watchdog, NULL)) {
        rt_printf("Error task start: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    }*/
    

}

void deleteTasks() {
    rt_task_delete(&tServeur);
    rt_task_delete(&tconnect);
    rt_task_delete(&tmove);
    rt_task_delete(&tenvoyer);
    rt_task_delete(&tbatterie);
    rt_task_delete(&tcamera);
    /*rt_task_delete(&twatchdog);*/
}
