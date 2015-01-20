#include<stdlib.h>

#ifndef LISTA_GENERICA
#define LISTA_GENERICA


/*
	Estrutura de uma lista generica.
	- Os conteudos armazenados por esta lista sao ponteiros void
	portanto pode-se armazenar qualquer tipo de dados, porem
	o usuario deve fazer o casting para utilizar o valor, ja
	que os valores de retorno tambem sao ponteiros void.
	- Tambem eh importante que se aloque uma posicao nova de memoria
	para o elemento que se quer inserir na lista antes de o inserir.
		Ex: Caso se insira na lista a variavel x e apos inserir x seja alterado
		fora da lista, x sera alterado dentro da lista, ja que a lista so
		armazena ponteiros.
		comentario alterado para teste
*/

/*
	ADICIONAR:
		-SORT
		-INSERE POR INDICE
		-LE POR INDICE
		-REMOVE POR INDICE
*/

#pragma region Estruturas

typedef struct Celula *PonteiroCelula;

typedef struct Celula
//estrutura de dados que armazena cada elemento da lista
{
	void *p;
	PonteiroCelula prox;
	PonteiroCelula ant;
	//Celula.p aponta para o conteudo da celula (item) [Ex: int]
	//Celula.tam armazena o tamanho (em bytes) do conteudo (item) [Ex: 4]
} Celula;

typedef struct
//estrutura de dados usado para a lista
{
	PonteiroCelula primeiro;
	PonteiroCelula ultimo;
	int tam;
	//Lista.primeiro aponta para a celula cabeça da lista
	//Lista.ultimo aponta para a celula rabo da lista
	//todos os elementos da lista ficaram entre essas 2 celulas
	//Lista.tam guarda o numero de elementos na lista
} Lista;

#pragma endregion Definicao das estruturas de dados basicas da lista

#pragma region Utilidades

int EstaVaziaLista(Lista l)
// l aponta para a lista a ser checada
// retorna true se a lista esta vazia, false se nao
{
	return !(l.tam);
}

void InicializaLista(Lista *l)
// l aponta para a lista a ser inicializada
// inicializa uma lista como estando vazia (necessario para evitar erros de segmentacao)
{
	l->primeiro = (Celula*)malloc(sizeof(Celula));
	l->ultimo = (Celula*)malloc(sizeof(Celula));
	//cria celula cabeca e celula rabo

	l->tam = 0;
	//marca a lista como vazia

	l->primeiro->prox = l->ultimo;
	l->primeiro->ant = NULL;
	l->ultimo->ant = l->primeiro;
	l->ultimo->prox = NULL;
	//linka as duas celulas base

	l->primeiro->p = NULL;
	l->ultimo->p = NULL;
	//coloca as duas celulas com conteudo vazio
}

int TamanhoLista(Lista l)
// l eh a lista a ter o tamanho checado
{
	return l.tam;
}

void* PeekLista(Lista l)
// l eh a lista a ter o elemento checado
// retorna um ponteiro para o primeiro elemento da lista, mantendo-o na lista
// caso não haja elementos retorna NULL
{
	if (l.tam > 0)
		return l.primeiro->prox->p;
	else
		return NULL;
}

#pragma endregion Funcoes variadas relacionadas a lista

#pragma region Insercao

void InsereFimLista(Lista *l, void *p)
// l aponta para a lista a ter o elemento inserido
// p aponta para o elemento a ser inserido na lista
// insere uma celula apontando para p ao final da lista
{
	Celula *aux, *q;
	aux = l->ultimo->ant;
	q = (Celula*)malloc(sizeof(Celula));
	q->p = p;
	//inicializa aux com o ultimo elemento da lista
	//e q com uma celula contendo p

	aux->prox = q;
	q->ant = aux;
	//insere q depois de aux

	l->ultimo->ant = q;
	q->prox = l->ultimo;
	//indica que q eh o novo ultimo elemento

	l->tam++;
}

void InsereInicioLista(Lista *l, void *p)
// l aponta para a lista a ter o elemento inserido
// p aponta para o elemento a ser inserido na lista
// insere uma celula apontando para p no inicio da lista
{
	Celula *aux, *q;
	aux = l->primeiro->prox;
	q = (Celula*)malloc(sizeof(Celula));
	q->p = p;
	//inicializa aux com o primeiro elemento da lista
	//e q com uma celula contendo p

	aux->ant = q;
	q->prox = aux;
	//insere q antes de aux

	l->primeiro->prox = q;
	q->ant = l->primeiro;
	//indica que q eh o novo primeiro elemento

	l->tam++;
}

void InsereOrdenadoLista(Lista *l, void *p, int(*f)(void*, void*))
// l aponta para a lista a ter o elemento inserido
// p aponta para o elemento a ser inserido na lista
// f aponta para uma funcao que retorna true caso o primeiro parametro deva ficar antes do segundo
{
	Celula *a, *b, *c;

	b = (Celula*)malloc(sizeof(Celula));
	b->p = p;
	//cria b, a celula que ira conter p

	a = l->primeiro;
	c = a->prox;
	//inicia c com o primeiro elemento

	while (c != l->ultimo && f(c->p, p))
	{
		a = c;
		c = c->prox;
	}
	//enquanto f for verdadeira "caminha" com a posicao em que p sera inserido
	//ira parar quando c for o elemento a ficar depois de p (logo, a ficara antes)

	a->prox = b;
	b->ant = a;
	b->prox = c;
	c->ant = b;
	//insere b entre a e c

	l->tam++;
}

#pragma endregion Funcoes que inserem na lista

#pragma region Remocao

void* RetiraFimLista(Lista *l)
// l aponta para a lista a ter o elemento retirado
// retorna um ponteiro para o item "guardado" pela celula ao fim da lista
// retorna NULL caso a lista esteja vazia
{
	if (l->tam == 0)
		return NULL;
		//caso a lista esteja vazia retorna NULL

	Celula *p, *q;
	q = l->ultimo->ant;
	// q recebe o ultimo elemento
	p = q->ant;
	// p recebe o penultimo elemento

	p->prox = l->ultimo;
	l->ultimo->ant = p;
	//indica que p eh o novo ultimo elemento

	void *aux;
	aux = q->p;
	// armazena o conteudo de q em aux

	free(q);

	l->tam--;

	return aux;
}

void* RetiraInicioLista(Lista *l)
// l aponta para a lista a ter o elemento retirado
// retorna um ponteiro para o item "guardado" pela celula no inicio da lista
// retorna NULL caso a lista esteja vazia
{
	if (l->tam == 0)
		return NULL;
	//caso a lista esteja vazia retorna NULL

	Celula *p, *q;
	q = l->primeiro->prox;
	// q recebe o primeiro elemento
	p = q->prox;
	// p recebe o segundo elemento

	p->ant = l->primeiro;
	l->primeiro->prox = p;
	//indica que p eh o novo primeiro elemento

	void *aux;
	int *teste;
	aux = q->p;
	// armazena o conteudo de q em aux

	free(q);

	l->tam--;

	teste = (int*)aux;

	return aux;
}

void* RetiraElementoLista(Lista *l, int(*f)(void*, void*), void* x)
// l aponta para a lista a ter o elemento retirado
// f aponta para a funcao que retorna true caso o elemento deva ser retirado
// f recebe 2 parametros, decidindo se o 1o deve ser retirado baseado em uma comparacao com o 2o
// x eh o segundo parametro com o qual f decide se o elemento deve ser retirado
// retorna um ponteiro para o primeiro elemento em que f retorne true
// retorna NULL caso nenhum elemento 
{
	if (l->tam == 0)
		return NULL;
	//caso a lista esteja vazia retorna NULL
	
	Celula *p;
	p = l->primeiro->prox;
	//inicia p com o primeiro elemento da lista

	while (p != l->ultimo && !f(p->p, x))
	{
		p = p->prox;
	}
	//enquanto f for falso "caminha" com o ponteiro p
	//ira parar quando p conter o elemento a ser retirado ou chegar ao fim da lista

	void *aux;
	aux = p->p;
	//aux recebe o elemento a ser retornado

	free(p);

	return aux;
}

#pragma endregion Funcoes que removem da lista

#endif