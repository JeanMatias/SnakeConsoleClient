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
MemGeral *vistaPartilhaGeral;

/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DAS THREADS						 */
/* ----------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param);


/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES PARA A DLL						 */
/* ----------------------------------------------------- */
void Escreve_Memoria(MemGeral param);
int Cria_Jogo(MemGeral param, int numJogadores);
void chamaCriaJogo(void);
void getMapa(MemGeral *param);
void imprimeMapa(MemGeral *param);
void leMemoriaPartilhada(MemGeral* param);

/* ----------------------------------------------------- */
/*  VARIAVEIS GLOBAIS PARA A DLL						 */
/* ----------------------------------------------------- */
int numJogadores = 1;  //Num Jogadores a jogar nesta maquina
int indiceCobras;		//Indice na memoria Dinamica em que se encontram a primeira cobra desta maquina.
TCHAR username1[SIZE_USERNAME];	//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];	//Nome do Jogador 2 desta Maquina


/* ----------------------------------------------------- */
/*  Função MAIN											 */
/* ----------------------------------------------------- */
int _tmain(int argc, LPTSTR argv[]) {
	MemGeral aux;
	DWORD tid;
	TCHAR buffer[TAM_BUFFER];
	int var_inicio=0;

	/* ---- Definição Memória Partilhada ---- */
	hMemoria = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NOME_MEM_GERAL);

	vistaPartilhaGeral = (MemGeral*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_MEM_GERAL);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemaforoMemoria = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, SEM_MEM_GERAL);

	if (hEventoMemoria == NULL || hSemaforoMemoria == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	
	/* ---- Definição Pipes ---- */

	

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	/* ---- Entrada inicial do cliente ---- */
		_tprintf(TEXT("\n\n\t 1 - Ligar servidor local. \n\n\t 2 - Ligar servidor remoto. \n\n\t > "));
		fflush(stdin);
		_fgetts(buffer, SIZE_USERNAME, stdin);
		buffer[_tcslen(buffer) - 1] = '\0';
		var_inicio = _ttoi(buffer);

	/* ---- Entrada em servidor local - Memória partilhada ---- */
	if (var_inicio == 1) {

		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Interage_Cliente, NULL, 0, &tid);
			while (1) {
				WaitForSingleObject(hEventoMemoria, INFINITE);
				WaitForSingleObject(hSemaforoMemoria, INFINITE);

				aux.mensagem.codigoMsg = vistaPartilhaGeral->mensagem.codigoMsg;
				_tcscpy_s(aux.mensagem.username, SIZE_USERNAME, vistaPartilhaGeral->mensagem.username);
				aux.estadoJogo = vistaPartilhaGeral->estadoJogo;

				ReleaseSemaphore(hSemaforoMemoria, 1, NULL);

				_tprintf(TEXT("CLIENTE: %s \t Codigo:%d\tEstado:%d"), aux.mensagem.username, aux.mensagem.codigoMsg, aux.estadoJogo);
			}
	}
	/* ---- Entrada em servidor remoto - Pipes ---- */
	else if(var_inicio == 2){
	
				_tprintf(TEXT("Lá Lá Lá ....   em manutenção....."));
				_gettch();
	}
		
	return 0;
}

/*----------------------------------------------------------------- */
/*  THREAD - Função que escreve mensagens na memoria partilhada 	*/
/* ---------------------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param) {
	
	TCHAR buf[SIZE_USERNAME];
	int var_inicio;
	MemGeral aux;

	_tprintf(TEXT("Nome: "));
	fflush(stdin);
	_fgetts(username1, SIZE_USERNAME, stdin);
	username1[_tcslen(username1) - 1] = '\0';

	while (1) {

		_tprintf(TEXT("\n\n\t 1 -Criar Jogo. \n\n\t 2 - Associar a Jogo. \n\n\t > "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		var_inicio = _ttoi(buf);

		switch (var_inicio)
		{
		case CRIACAOJOGO:chamaCriaJogo();
			//Sleep(200);
			_tprintf(TEXT("\n"));
			getMapa(&aux);
			leMemoriaPartilhada(&aux);
			imprimeMapa(&aux);
			break;
		case ASSOCIACAOJOGO:_tprintf(TEXT("\n\n\t A fazer "));
			break;
		default:
			break;
		}
		
	}
}

void Escreve_Memoria(MemGeral param) {
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemaforoMemoria, INFINITE);
	}
	vistaPartilhaGeral->estadoJogo =  param.estadoJogo;
	
	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemaforoMemoria, MAXCLIENTES, NULL);
}

void chamaCriaJogo(void) {
	MemGeral aux;
	_tcscpy_s(aux.mensagem.username, SIZE_USERNAME, username1);
	aux.mensagem.codigoMsg = CRIARJOGO;
	_tcscpy_s(aux.criador, SIZE_USERNAME, aux.mensagem.username);

	aux.config.A = NUMAUTOSNAKE;
	aux.config.C = COLUNAS;
	aux.config.L = LINHAS;
	aux.config.N = 1;
	aux.config.O = NUMOBJETOS;
	aux.config.T = TAMANHOSNAKE;

	if (Cria_Jogo(aux, 1)) {
		_tprintf(TEXT("CRIADO "));
	}
	else {
		_tprintf(TEXT("NÂO CRIADO "));
	}
}

void getMapa(MemGeral *param) {
	WaitForSingleObject(hSemaforoMemoria, INFINITE);
	for (int i = 0; i < vistaPartilhaGeral->config.L;i++) {
		for (int j = 0; j < vistaPartilhaGeral->config.C;j++) {
			param->mapa[i][j] = vistaPartilhaGeral->mapa[i][j];
		}
	}
	ReleaseSemaphore(hSemaforoMemoria, 1, NULL);
}

void leMemoriaPartilhada(MemGeral* param) {

	WaitForSingleObject(hSemaforoMemoria, INFINITE);

	param->estadoJogo = vistaPartilhaGeral->estadoJogo;
	param->mensagem.codigoMsg = vistaPartilhaGeral->mensagem.codigoMsg;
	_tcscpy_s(param->mensagem.username, SIZE_USERNAME, vistaPartilhaGeral->mensagem.username);
	param->config.C = vistaPartilhaGeral->config.C;
	param->config.L = vistaPartilhaGeral->config.L;

	ReleaseSemaphore(hSemaforoMemoria, 1, NULL);
}

void imprimeMapa(MemGeral *param) {
	for (int i = 0; i < param->config.L; i++) {
		for (int j = 0; j < param->config.C; j++) {
			_tprintf(TEXT("%c "), param->mapa[i][j]);
		}
		_tprintf(TEXT("\n"));
	}
}

int Cria_Jogo(MemGeral param, int numJogadores) {
	WaitForSingleObject(hSemaforoMemoria, INFINITE);
	if (!(vistaPartilhaGeral->estadoJogo == CRIACAOJOGO)) {
		return 0;
	}
	for (int i = 0; i < MAXCLIENTES-1; i++) {
		WaitForSingleObject(hSemaforoMemoria, INFINITE);
	}

	vistaPartilhaGeral->estadoJogo = ASSOCIACAOJOGO;
	vistaPartilhaGeral->config = param.config;
	vistaPartilhaGeral->mensagem.codigoMsg = param.mensagem.codigoMsg;
	_tcscpy_s(vistaPartilhaGeral->mensagem.username, SIZE_USERNAME, param.mensagem.username);
	_tcscpy_s(vistaPartilhaGeral->criador, SIZE_USERNAME, param.criador);
	vistaPartilhaGeral->vagasJogadores = param.config.N - numJogadores;

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemaforoMemoria, MAXCLIENTES, NULL);
	return 1;
}