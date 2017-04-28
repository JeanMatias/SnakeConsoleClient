#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "../../SnakeServer/SnakeServer/TiposConstantes.h"

/* ----------------------------------------------------- */
/*  VARIÁVEIS GLOBAIS									 */
/* ----------------------------------------------------- */
HANDLE hEventoMemoria;
HANDLE hSemaforoMemoria;
HANDLE hMemoria;
HANDLE hThread;
MemGeral *vistaPartilha;
/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DAS THREADS						 */
/* ----------------------------------------------------- */
//void Escreve_Memoria(LPVOID param);
void Escreve_Memoria(MemGeral *param);

/* ----------------------------------------------------- */
/*  Função MAIN											 */
/* ----------------------------------------------------- */
int _tmain(int argc, LPTSTR argv[]) {
	MemGeral aux;
	DWORD tid;

	hMemoria = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NOME_MEM_GERAL);

	vistaPartilha = (Msg*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZEMENSAGEM);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemaforoMemoria = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, SEM_MEM_GERAL);

	if (hEventoMemoria == NULL || hSemaforoMemoria == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Escreve_Memoria, NULL, 0, &tid);
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	while (1) {
		WaitForSingleObject(hEventoMemoria, INFINITE);
		WaitForSingleObject(hSemaforoMemoria, INFINITE);

		aux.numClientes = vistaPartilha->numClientes;
		aux.estadoJogo = vistaPartilha->estadoJogo;

		ReleaseSemaphore(hSemaforoMemoria, 1, NULL);

		_tprintf(TEXT("NumClientes: %d \t Estado:%d"), aux.numClientes, aux.estadoJogo);
	}
	
	return 0;
}

/*----------------------------------------------------------------- */
/*  THREAD - Função que escreve mensagens na memoria partilhada 	*/
/* ---------------------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param) {
	MemGeral aux, aux2;
	TCHAR buf[SIZE_USERNAME];

	while (1) {
		_tprintf(TEXT("Inteiro: "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		aux.estadoJogo = _ttoi(buf);

		
	}
}

void Escreve_Memoria(MemGeral* param) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemaforoMemoria, INFINITE);
	}
	vistaPartilha->estadoJogo =  param->estadoJogo;
	vistaPartilha->numClientes = param->numClientes;
	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemaforoMemoria, MAXCLIENTES, NULL);
}