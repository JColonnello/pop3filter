#pragma once

enum ConnectionResult
{
	CONNECTION_ONGOING,
	CONNECTION_FAILED,
	CONNECTION_COMPLETED
};

// Crea una tarea de conexión, devuelve numero de la tarea
int startTCPConnection(const char *hostname, const char *port);
// Le paso el numero de tarea, devuelve el estado de la tarea
// Cuando falla o completa libera los recursos de la tarea
// Si completa devuelve fd
enum ConnectionResult connectToServer(int task, int *fd);
int signalfd_setup();
