#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include "..\..\DLL\DLL\DLL.h"

/* ----------------------------------------------------- */
/*  VARI�VEIS GLOBAIS									 */
/* ----------------------------------------------------- */
HANDLE hThread;

/* ----------------------------------------------------- */
/*  PROTOTIPOS FUN��ES DAS THREADS						 */
/* ----------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param);
DWORD WINAPI interageJogo(LPVOID param);


/* ----------------------------------------------------- */
/*  PROTOTIPOS FUN��ES									 */
/* ----------------------------------------------------- */
void chamaCriaJogo(void);
void imprimeMapa(MemGeral *param);
void chamaAssociaJogo(void);


/* ----------------------------------------------------- */
/*  VARIAVEIS GLOBAIS PARA A DLL						 */
/* ----------------------------------------------------- */
int numJogadores = 1;  //Num Jogadores a jogar nesta maquina
int indiceCobras;		//Indice na memoria Dinamica em que se encontram a primeira cobra desta maquina.
TCHAR username1[SIZE_USERNAME];	//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];	//Nome do Jogador 2 desta Maquina


/* ----------------------------------------------------- */
/*  Fun��o MAIN											 */
/* ----------------------------------------------------- */
int _tmain(int argc, LPTSTR argv[]) {
	MemGeral aux;
	DWORD tid;
	TCHAR buffer[TAM_BUFFER];
	int var_inicio=0;

	/* ---- Defini��o Mem�ria Partilhada ---- */
	/*
	hMemoria = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NOME_MEM_GERAL);

	vistaPartilhaGeral = (MemGeral*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_MEM_GERAL);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemMemoria = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, SEM_MEM_GERAL);
	*/
	preparaMemoriaPartilhada();
	/*
	if (hEventoMemoria == NULL || hSemMemoria == NULL) {
		_tprintf(TEXT("[Erro] Cria��o de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	*/
	
	/* ---- Defini��o Pipes ---- */

	

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

	/* ---- Entrada em servidor local - Mem�ria partilhada ---- */
	if (var_inicio == 1) {

		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Interage_Cliente, NULL, 0, &tid);
			while (1) {
				esperaPorActualizacao();
				leMemoriaPartilhada(&aux);

				//_tprintf(TEXT("CLIENTE: %s \t Codigo:%d\tEstado:%d"), aux.mensagem.username, aux.mensagem.codigoMsg, aux.estadoJogo);
			}
	}
	/* ---- Entrada em servidor remoto - Pipes ---- */
	else if(var_inicio == 2){
	
				_tprintf(TEXT("L� L� L� ....   em manuten��o....."));
				_gettch();
	}
		
	return 0;
}

/*----------------------------------------------------------------- */
/*  THREAD - Fun��o que escreve mensagens na memoria partilhada 	*/
/* ---------------------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param) {
	
	TCHAR buf[SIZE_USERNAME];
	int var_inicio;
	MemGeral aux;
	DWORD tid;

	_tprintf(TEXT("Nome: "));
	fflush(stdin);
	_fgetts(username1, SIZE_USERNAME, stdin);
	username1[_tcslen(username1) - 1] = '\0';

	while (1) {
		system("cls");
		_tprintf(TEXT("\n\n\t 1 - Criar Jogo. \n\n\t 2 - Associar a Jogo. \n\n\t 8 - Iniciar Jogo. \n\n\t> "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		var_inicio = _ttoi(buf);

		switch (var_inicio)
		{
		case CRIACAOJOGO:chamaCriaJogo();
			break;
		case ASSOCIACAOJOGO:chamaAssociaJogo();
			break;
		case INICIARJOGO:
			IniciaJogo(username1);
			system("cls");
			leMemoriaPartilhada(&aux);
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)interageJogo, NULL, 0, &tid);
			while (1) {
				esperaPorActualizacao();
				system("cls");
				getMapa(&aux);
				imprimeMapa(&aux);
			}
			
		default:
			break;
		}
		
	}
}

DWORD WINAPI interageJogo(LPVOID param) {
	TCHAR tecla;
	BOOLEAN continua = TRUE;

	while (continua) {
		tecla = _gettch();
		switch (tecla)
		{
		case 'W':
		case 'w':mudaDirecao(CIMA);
			break;
		case 'S':
		case 's':mudaDirecao(BAIXO);
			break;
		case 'A':
		case 'a':mudaDirecao(ESQUERDA);
			break;
		case 'D':
		case 'd':mudaDirecao(DIREITA);
			break;
		case 'P':
		case 'p':continua = FALSE;
			break;
		default:
			break;
		}
	}
	
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
		_tprintf(TEXT("N�O CRIADO "));
	}
}

void chamaAssociaJogo(void) {
	AssociaJogo(1, username1, NULL);
}

void imprimeMapa(MemGeral *param) {
	for (int i = 0; i < param->config.L; i++) {
		for (int j = 0; j < param->config.C; j++) {
			_tprintf(TEXT("%c"), param->mapa[i][j]);
		}
		_tprintf(TEXT("\n"));
	}
}