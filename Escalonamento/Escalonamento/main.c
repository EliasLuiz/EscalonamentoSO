#include<stdlib.h>
#include<stdio.h>
#include "lista.h"


#pragma region Declaracoes Iniciais
typedef struct
//Tipo de dados Processo, que serve como base para a simulacao//comentario alterado para teste
{
	int id;
	int tempoCpu;
	int tempoChegada;
	int prioridade;
} Processo;

int ins_tempoCriacao(void *a, void *b);
int ins_tempoCPU(void *a, void *b);
int ins_prioridade(void *a, void *b);

void Enfileira(Lista *l, Processo p, int algoritmo);
#pragma endregion Prototipos de funcoes e declaracao de tipos da dados usados na simulacao


int main()
{
	//----------------------------------------------------
	// PARAMETROS DA SIMULACAO
	//----------------------------------------------------
	#pragma region Parametros da simulacao

	int algoritmo = 0, nProcessos, tempoTroca = 0, fatiaTempo = 0;
	Lista filaPronto, filaCriacao;
	//fila = fila dos processos em pronto
	//filaaux = fila dos processos a serem criados
	InicializaLista(&filaPronto);
	InicializaLista(&filaCriacao);

	printf("1 - FIFO: ");
	printf("\n2 - SJF: ");
	printf("\n3 - Circular: ");
	printf("\n4 - Prioridade: ");
	printf("\nAlgoritmo: ");
	while (algoritmo < 1 || algoritmo > 4) //cerca entrada invalida do usuario
		scanf("%d", &algoritmo);
	printf("Numero de processos: ");
	scanf("%d", &nProcessos);
	printf("Tempo de troca de contexto: ");
	scanf("%d", &tempoTroca);
	if (algoritmo == 3)
	{
		printf("Fatia de tempo: ");
		scanf("%d", &fatiaTempo);
	}
	//le parametros da simulacao

	#pragma endregion


	//----------------------------------------------------
	// CRIACAO DOS PROCESSOS
	//----------------------------------------------------
	#pragma region Criacao dos Processos

	int *vInitProcessos;
	vInitProcessos = (int*)malloc(nProcessos*sizeof(int));
	//salva o tempo em que cada processo vai iniciar

	int i;
	printf("\n\n");
	for (i = 0; i<nProcessos; i++)
	{
		Processo *p;
		p = (Processo*)malloc(sizeof(Processo));
		// lista requer uma alocacao de memoria por elemento
		p->id = i;
		printf("Processo %d: ", i);
		printf("\nTempo de Chegada: ");
		scanf("%d", &p->tempoChegada);
		printf("Tempo de CPU: ");
		scanf("%d", &p->tempoCpu);
		if (algoritmo == 4)
		{
			printf("Prioridade: ");
			scanf("%d", &p->prioridade);
		}
		InsereOrdenadoLista(&filaCriacao, p, ins_tempoCriacao);

		vInitProcessos[i] = p->tempoChegada;
	}
	//leitura dos dados dos processos

	#pragma endregion

	
	//----------------------------------------------------
	// EXECUCAO DA SIMULACAO
	//----------------------------------------------------
	#pragma region Execucao da Simulacao

	int tempoTotal = 0;
	//tempo da cpu
	//valor final = tempo total de simulacao
	int tempoRestante = -1;
	//tempo restante do total alocado para o processo
	Processo emExec;
	emExec.id = 0; //processo especial que "executa" no 1o ciclo 
				   //			(depois eh sobrescrito pelo id=0)
	emExec.tempoCpu = 0;
	emExec.prioridade = -1;
	emExec.tempoChegada = 0;
	//variavel que armazena qual processo esta em execucao
	int *vFimProcessos;
	vFimProcessos = (int*)malloc(nProcessos*sizeof(int));
	//salva o tempo em que cada processo termina
	int *vProcessos;
	vProcessos = NULL;
	//salva qual processo esta executando
	//Ex: vProcessos[10] = 3 
	//		=> no tempo 10 o processo em execucao eh o numero 3
	//-1 = Ocioso
	//-2 = Troca de contexto

	while (!EstaVaziaLista(filaCriacao) || !EstaVaziaLista(filaPronto) || tempoRestante >= 0)
	{
		//ALGORITMO DE SIMULACAO:
		//cria processos
		//olha se vai ter preempcao por prioridade
		//olha se vai continuar executando
			//se for executa
			//se nao, olha se acabou
				//se terminou descarta
				//se nao terminou volta pra fila

		Processo *proximo;
		proximo = (Processo*)PeekLista(filaCriacao);
		while( proximo != NULL && proximo->tempoChegada <= tempoTotal )
		//Enquanto o primeiro processo da fila for criado nesse momento
		{
			Processo *p;
			p = RetiraInicioLista(&filaCriacao);
			Enfileira(&filaPronto, *p, algoritmo);
			free(p);

			proximo = (Processo*)PeekLista(filaCriacao);
		} //Retira o processo da fila de criacao e insere na fila de pronto

		proximo = (Processo*)PeekLista(filaPronto);
		if (algoritmo == 4 && proximo != NULL && proximo->prioridade > emExec.prioridade)
		//Se deve ocorrer uma preempcao por prioridade
		{
			tempoRestante = 0;
		} //Zera o tempo alocado para o processo atual


		if (emExec.tempoCpu == 1)
		{
			tempoRestante = 0;
		} //Olha se processo terminou (escalonamento circular)

		if (tempoRestante > 0)
		{
			tempoRestante--;
			emExec.tempoCpu--;
		} //Olha se vai continuar executando
		else
		{
			if (emExec.tempoCpu > 1)
			{
				emExec.tempoCpu--;
				Enfileira(&filaPronto, emExec, algoritmo);
			} //Se o processo nao terminou de executar volta para a fila de pronto
			else if (emExec.id != -1)
			{
				vFimProcessos[emExec.id] = tempoTotal;
			} //Se o processo terminou armazena o tempo de termino

			proximo = ((Processo*)RetiraInicioLista(&filaPronto));
			if (proximo == NULL)
			{
				emExec.id = -1; //Processo nulo: indica cpu ociosa
				tempoRestante = -1;
			}
			else
			{
				emExec = *proximo;

				if (algoritmo == 3)
				{
					tempoRestante = fatiaTempo - 1;
				} // se for circular aloca uma fatia de tempo para o processo
				else
				{
					tempoRestante = emExec.tempoCpu - 1;
				} // se nao for circular, aloca o tempo necessario

				tempoTotal += tempoTroca;
				vProcessos = (int*)realloc(vProcessos, tempoTotal * sizeof(int));
				for (i = tempoTotal - tempoTroca; i < tempoTotal; i++)
				{
					vProcessos[i] = -2;
				}//faz a troca de contexto
			}
			//Escalona o proximo processo

		}//Caso precise escalonar um processo novo

		tempoTotal++;

		vProcessos = (int*)realloc(vProcessos, tempoTotal * sizeof(int));
		vProcessos[tempoTotal - 1] = emExec.id;
		//Armazena qual processo esta em execucao naquele momento
	}

	#pragma endregion


	//----------------------------------------------------
	// ANALISE DE RESULTADOS / CRIACAO DA TABELA
	//----------------------------------------------------
	#pragma region Analise de Resultados / Criacao da Tabela

	char** matriz;
	//tabela onde sera armazenado os resultados
	//O = Ocioso
	//P = Pronto
	//E = Execucao
	//I = Indisponivel
	//F = Finalizado
	//T = Troca de contexto

	matriz = (char**)malloc(tempoTotal * sizeof(char*));
	for (i = 0; i<tempoTotal; i++)
	{
		matriz[i] = (char*)malloc(nProcessos * sizeof(char));
	}
	//char matriz[nExec][nProcessos]

	int j;
	for (i = 0; i<tempoTotal; i++)
	{	// i marca tempo de cpu
		for (j = 0; j<nProcessos; j++)
		{	// j marca processo
			if (vInitProcessos[j] > i)
			{
				matriz[i][j] = 'I';
			}//Caso o processo ainda nao tenha sido criado
			else if (i >= vFimProcessos[j])
			{
				matriz[i][j] = 'F';
			}//Caso ja tenha terminado
			else if(vProcessos[i]==-1)	 
			{							  
				matriz[i][j] = 'O';
			}//Caso a cpu esteja ociosa
			else if (vProcessos[i] == -2)
			{
				matriz[i][j] = 'T';
			}//Caso a cpu esteja trocando o contexto
			else if (vProcessos[i] == j)
			{
				matriz[i][j] = 'E';
			}//Caso seja o processo executando
			else
			{
				matriz[i][j] = 'P';
			}//Caso esteja na fila de pronto
		}
	}
	//cria a tabela que salva os estados dos processos em cada tempo

	#pragma endregion


	//----------------------------------------------------
	// ARMAZENAMENTO EM DISCO DOS RESULTADOS
	//----------------------------------------------------
	#pragma region Armazenamento em Disco dos Resultados

	FILE *fp;
	fp = fopen("resultado.txt", "w");
	//arquivo onde será armazenado a tabela de resultado


	fprintf(fp, "Processo\t");
	for (i = 0; i<tempoTotal; i++)
		fprintf(fp, "%d\t", i);
	fprintf(fp, "Turnaround");

	for (j = 0; j<nProcessos; j++)
	{	//j marca processos
		fprintf(fp, "\n%d\t\t", j);
		for (i = 0; i<tempoTotal; i++)
		{	//i marca tempo
			fprintf(fp, "%c\t", matriz[i][j]);
		}
		fprintf(fp, "%d", vFimProcessos[j] - vInitProcessos[j]);
	}
	//salva resultado no arquivo

	/*
	Exemplo de resultado:
	Processo	0	1	2	3	4	.	.	.	Turnaround
	0			I	I	P	E	F				4
	1			T	E	F	F	F				2
	2			T	P	E	F	F				3
	.			.					.			.
	.			.						.		.
	.			.							.	.
	*/

	fclose(fp);

	system("resultado.txt");

	return EXIT_SUCCESS;

	#pragma endregion

}


#pragma region Implementacoes

void Enfileira(Lista *l, Processo p, int algoritmo)
// Funcao que escolhe o procedimento de insercao adequado ao algoritmo
{
	Processo *aux;
	aux = (Processo*)malloc(sizeof(Processo));
	*aux = p;
	//a lista precisa que uma posicao de memoria nova seja alocada a cada insercao

	switch (algoritmo)
	{
		case 1: //FIFO
		case 3: //Circular
			InsereFimLista(l, aux);
			break;
		case 2: //SJF
			InsereOrdenadoLista(l, aux, ins_tempoCPU);
			break;
		case 4: //Prioridade
			InsereOrdenadoLista(l, aux, ins_prioridade);
			break;
		default: //Erro
			exit(EXIT_FAILURE);
	}
}

#pragma region Funcoes Auxiliares de Ordenacao

int ins_tempoCriacao(void *a, void *b)
//Retorna true caso o processo a seja criado antes de b, false caso contrário
{
	Processo *x, *y;
	x = a;
	y = b;
	return x->tempoChegada <= y->tempoChegada;
} 

int ins_tempoCPU(void *a, void *b)
//Retorna true caso o processo a tenha um tempo de cpu menor que b, false caso contrário
{
	Processo *x, *y;
	x = a;
	y = b;
	return x->tempoCpu < y->tempoCpu;
}

int ins_prioridade(void *a, void *b)
//Retorna true caso o processo a tenha uma prioridade maior que b, false caso contrário
{
	Processo *x, *y;
	x = a;
	y = b;
	return x->prioridade > y->prioridade;
}

#pragma endregion Funcoes usadas para inserir ordenado na lista, cada uma segundo um parametro

#pragma endregion Implementacao das funcoes usadas na simulacao
