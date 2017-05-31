#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include "..\..\SnakeDLL\SnakeDLL\SnakeDLL.h"

/* ----------------------------------------------------- */
/*  VARIÁVEIS GLOBAIS									 */
/* ----------------------------------------------------- */
HANDLE hThread;

/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES DAS THREADS						 */
/* ----------------------------------------------------- */
DWORD WINAPI Interage_Cliente(LPVOID param);
DWORD WINAPI interageJogo(LPVOID param);


/* ----------------------------------------------------- */
/*  PROTOTIPOS FUNÇÕES									 */
/* ----------------------------------------------------- */
void chamaCriaJogo(void);
void imprimeMapa();
void chamaAssociaJogo(TCHAR username[SIZE_USERNAME], int codigo);


/* ----------------------------------------------------- */
/*  VARIAVEIS GLOBAIS PARA A DLL						 */
/* ----------------------------------------------------- */
int numJogadores = 0;				//Num Jogadores a jogar nesta maquina
int indiceCobras = 0;				//Indice na memoria Dinamica em que se encontram a primeira cobra desta maquina.
TCHAR username1[SIZE_USERNAME];		//Nome do Jogador 1 desta Maquina
TCHAR username2[SIZE_USERNAME];		//Nome do Jogador 2 desta Maquina
int pId;							//Process Id deste cliente
int linhas;
int colunas;
int mapa[MAX_LINHAS][MAX_COLUNAS];


/* ----------------------------------------------------- */
/*  Função MAIN											 */
/* ----------------------------------------------------- */
int _tmain(int argc, LPTSTR argv[]) {
	DWORD tid;
	TCHAR buffer[TAM_BUFFER];
	int var_inicio=0;

	pId = GetCurrentProcessId();

	preparaMemoriaPartilhada();
	
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

		WaitForSingleObject(hThread, INFINITE);
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
	DWORD tid;

	_tprintf(TEXT("Nome 1: "));
	fflush(stdin);
	_fgetts(username1, SIZE_USERNAME, stdin);
	username1[_tcslen(username1) - 1] = '\0';

	_tprintf(TEXT("Nome 2: "));
	fflush(stdin);
	_fgetts(username2, SIZE_USERNAME, stdin);
	username1[_tcslen(username2) - 1] = '\0';

	while (1) {
		system("cls");
		_tprintf(TEXT("\n\n\t 1 - Criar Jogo. \n\n\t 2 - Associar a Jogo. \n\n\t 3 - Iniciar Jogo. \n\n\t> "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		var_inicio = _ttoi(buf);

		switch (var_inicio)
		{
		case CRIACAOJOGO:chamaCriaJogo();
			Sleep(500);
			chamaAssociaJogo(username1,ASSOCIAR_JOGADOR1);
			break;
		case ASSOCIACAOJOGO:
			if (numJogadores == 1) {
				chamaAssociaJogo(username2, ASSOCIAR_JOGADOR2);
			}
			
			break;
		case 3:
			pede_IniciaJogo(pId);
			esperaPorActualizacaoMapa();
			getLimitesMapa(&linhas, &colunas);
			system("cls");
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)interageJogo, NULL, 0, &tid);
			while (1) {
				esperaPorActualizacaoMapa();
				getMapa(mapa);
				system("cls");
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
		case 'w':mudaDirecao(CIMA,pId, JOGADOR1);
			break;
		case 'S':
		case 's':mudaDirecao(BAIXO, pId, JOGADOR1);
			break;
		case 'A':
		case 'a':mudaDirecao(ESQUERDA, pId, JOGADOR1);
			break;
		case 'D':
		case 'd':mudaDirecao(DIREITA, pId, JOGADOR1);
			break;
		case 'I':
		case 'i':mudaDirecao(CIMA, pId, JOGADOR2);
			break;
		case 'K':
		case 'k':mudaDirecao(BAIXO, pId, JOGADOR2);
			break;
		case 'J':
		case 'j':mudaDirecao(ESQUERDA, pId, JOGADOR2);
			break;
		case 'L':
		case 'l':mudaDirecao(DIREITA, pId, JOGADOR2);
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
	ConfigInicial aux;
	
	aux.A = NUMAUTOSNAKE;
	aux.C = COLUNAS;
	aux.L = LINHAS;
	aux.N = 2;
	aux.O = NUMOBJETOS;
	aux.T = TAMANHOSNAKE;

	pede_CriaJogo(aux, pId);
}

void chamaAssociaJogo(TCHAR username[SIZE_USERNAME],int codigo) {
	pede_AssociaJogo(pId, username, codigo);
	numJogadores++;
}

void imprimeMapa() {
	for (int i = 0; i < linhas; i++) {
		for (int j = 0; j < colunas; j++) {
			switch (mapa[i][j])
			{
			case PAREDE:_tprintf(TEXT("#"));
				break;
			case ESPACOVAZIO:_tprintf(TEXT(" "));
				break;
			default:_tprintf(TEXT("%d"),mapa[i][j]/100-1);
				break;
			}
		}
		_tprintf(TEXT("\n"));
	}
}