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
Msg	*vistaPartilha;
/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DAS THREADS						 */
/* ----------------------------------------------------- */
DWORD WINAPI Escreve_Memoria(LPVOID param);

/* ----------------------------------------------------- */
/*  Função MAIN											 */
/* ----------------------------------------------------- */
int _tmain(int argc, LPTSTR argv[]) {
	Msg aux;
	DWORD tid;

	hMemoria = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NOME_MEM_GERAL);

	vistaPartilha = (Msg*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZEMENSAGEM);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemaforoMemoria = CreateSemaphore(NULL, 2, 2, SEM_MEM_GERAL);
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Escreve_Memoria, NULL, 0, &tid);
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	while (1) {
		WaitForSingleObject(hEventoMemoria, INFINITE);
		WaitForSingleObject(hSemaforoMemoria, INFINITE);

		_tcscpy_s(aux.username, SIZE_USERNAME, vistaPartilha->username);
		aux.codigoMsg = vistaPartilha->codigoMsg;
		
		ReleaseSemaphore(hSemaforoMemoria, 1, NULL);

		_tprintf(TEXT("Nome: %s \tInteiro:%d"), aux.username, aux.codigoMsg);
	}
	
	return 0;
}

/*----------------------------------------------------------------- */
/*  THREAD - Função que escreve mensagens na memoria partilhada 	*/
/* ---------------------------------------------------------------- */
DWORD WINAPI Escreve_Memoria(LPVOID param) {
	Msg aux;
	TCHAR buf[SIZE_USERNAME];
	_tprintf(TEXT("Nome: "));
	fflush(stdin);
	_fgetts(buf, SIZE_USERNAME, stdin);
	buf[_tcslen(buf) - 1] = '\0';
	_tcscpy_s(aux.username, SIZE_USERNAME,buf);
	aux.codigoMsg = CRIARJOGO;
	//CADA VEZ QUE ESCREVE NA MEM PARTILHADA TEM DE FAZER ISTO
	for (int i = 0; i < 2; i++) {
		WaitForSingleObject(hSemaforoMemoria, INFINITE);
	}
	vistaPartilha->codigoMsg = aux.codigoMsg;
	_tcscpy_s(vistaPartilha->username, SIZE_USERNAME, aux.username);
	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemaforoMemoria, 2, NULL);
	while (1) {
		_tprintf(TEXT("Inteiro: "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		aux.codigoMsg=_ttoi(buf);
		for (int i = 0; i < 2; i++) {
			WaitForSingleObject(hSemaforoMemoria, INFINITE);
		}
		vistaPartilha->codigoMsg = aux.codigoMsg;
		_tcscpy_s(vistaPartilha->username, SIZE_USERNAME, aux.username);
		SetEvent(hEventoMemoria);
		ResetEvent(hEventoMemoria);
		ReleaseSemaphore(hSemaforoMemoria, 2, NULL);

	}
}