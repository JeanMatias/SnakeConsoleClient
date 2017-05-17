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
void imprimeMapa(MemGeral *param);
void chamaAssociaJogo(void);


/* ----------------------------------------------------- */
/*  VARIAVEIS GLOBAIS PARA A DLL						 */
/* ----------------------------------------------------- */
int numJogadores = 1;  //Num Jogadores a jogar nesta maquina
int indiceCobras = 0;		//Indice na memoria Dinamica em que se encontram a primeira cobra desta maquina.
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
			while (1) {
				esperaPorActualizacao();
				leMemoriaPartilhada(&aux);

				//_tprintf(TEXT("CLIENTE: %s \t Codigo:%d\tEstado:%d"), aux.mensagem.username, aux.mensagem.codigoMsg, aux.estadoJogo);
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
		_tprintf(TEXT("\n\n\t 1 - Criar Jogo. \n\n\t 2 - Associar a Jogo. \n\n\t 8 - Iniciar Jogo. \n\n\t> "));
		fflush(stdin);
		_fgetts(buf, SIZE_USERNAME, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		var_inicio = _ttoi(buf);

		switch (var_inicio)
		{
		case CRIACAOJOGO:chamaCriaJogo();
			Sleep(500);
			chamaAssociaJogo();
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
		case 'w':mudaDirecao(CIMA,indiceCobras);
			break;
		case 'S':
		case 's':mudaDirecao(BAIXO, indiceCobras);
			break;
		case 'A':
		case 'a':mudaDirecao(ESQUERDA, indiceCobras);
			break;
		case 'D':
		case 'd':mudaDirecao(DIREITA, indiceCobras);
			break;
		case 'I':
		case 'i':mudaDirecao(CIMA, indiceCobras+1);
			break;
		case 'K':
		case 'k':mudaDirecao(BAIXO, indiceCobras+1);
			break;
		case 'J':
		case 'j':mudaDirecao(ESQUERDA, indiceCobras+1);
			break;
		case 'L':
		case 'l':mudaDirecao(DIREITA, indiceCobras+1);
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

	if (Cria_Jogo(aux,username1)) {
		_tprintf(TEXT("CRIADO "));
	}
	else {
		_tprintf(TEXT("NÂO CRIADO "));
	}
}

void chamaAssociaJogo(void) {
	AssociaJogo(2, username1, username2,&indiceCobras);
}

void imprimeMapa(MemGeral *param) {
	for (int i = 0; i < param->config.L; i++) {
		for (int j = 0; j < param->config.C; j++) {
			_tprintf(TEXT("%c"), param->mapa[i][j]);
		}
		_tprintf(TEXT("\n"));
	}
}