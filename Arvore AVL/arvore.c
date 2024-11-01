/**************************************************************/
/* Aluno: 
/* Número USP: 
/* Disciplina/Ano/EP: ACH2023/2021/EP2
/**************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#define true 1
#define false 0

typedef int bool;
typedef int TIPOCHAVE;

FILE* entrada;
FILE* saida;

int numOperacoes, x, operacao, qntElementos = 0, i = 0, inserir, excluir, aux;
char elementos[1000], elementosAux[12];

typedef struct aux {
	TIPOCHAVE chave;
	struct aux *esq;
	struct aux *dir;
	int bal;
} NO, *PONT;

/* cria um novo (aloca memoria e preenche valores) no com chave=ch e retorna seu endereco */
PONT criarNovoNo(TIPOCHAVE ch){
	PONT novoNo = (PONT)malloc(sizeof(NO));
	novoNo->esq = NULL;
	novoNo->dir = NULL;
	novoNo->chave = ch;
	novoNo->bal = 0;
	return novoNo;
}

void iniciarArvore(NO* *raiz)
{
  *raiz = NULL;
}

// Retorna o maior valor entre dois inteiros
int max(int a, int b){
	if (a>b) return a;
	return b;
}

// Retorna a altura de uma (sub-)arvore
int altura(PONT p){
	if (!p) return -1;
	else return 1 + max(altura(p->esq),altura(p->dir));
}

/* Exibe arvore Em Ordem (com parenteses para os filhos)    */
void exibirArvore(PONT raiz){
	if (raiz == NULL) return;
  else
    fprintf(saida,"(");
    fprintf(saida,"%i",raiz->chave);
    exibirArvore(raiz->esq);
    exibirArvore(raiz->dir);
    fprintf(saida,")");     
}

// Verifica se árvore é AVL
bool ehAVL(PONT p){
	int e,d;
	bool ok = true;
	if(p){
		ok = ehAVL(p->esq);
		if(ok) ok = ehAVL(p->dir);
		if(ok){
			e = altura(p->esq);
			d = altura(p->dir);
			if(e-d > 1 || e-d < -1) ok = false;  
			else ok = true;
		}
	}
	return(ok);
}

int atualizarBalanceamentoTotal(PONT raiz){
	if (!raiz) return 0;
	int d = atualizarBalanceamentoTotal(raiz->dir);
	int e = atualizarBalanceamentoTotal(raiz->esq);
	raiz->bal = d-e;
	return max(d,e)+1;
}

/* Rotações à direita (LL e LR) 
   Retornará o endereço do nó que será a nova raiz da subárvore originalmente 
   iniciada por p */
PONT rotacaoL(PONT p){
	PONT u, v;
	u = p->esq;
	if(u->bal == -1) {   // LL
		p->esq = u->dir;
		u->dir = p;
		p->bal = 0;
		u->bal = 0;
		return u;
	} else if (u->bal == 1) {  // LR
		v = u->dir;
		u->dir = v->esq;
		v->esq = u;
		p->esq = v->dir;
		v->dir = p;
		if(v->bal == -1) p->bal = 1;
		else p->bal = 0;
		if(v->bal == 1) u->bal = -1;
		else u->bal = 0;
		v->bal = 0;
		return v;
	}else{   // caso necessario apenas para a exclusao (u->bal == 0)
		p->esq = u->dir;
		u->dir = p;
		// p->bal = -1;    desnecessario pois o balanceamento de p nao chegou a ser mudado para -2
		u->bal = 1;
		return u;
	}
}

/* Rotações à esquerda (RR e RL) 
   Retornará o endereço do nó que será a nova raiz da subárvore originalmente 
   iniciada por p */
PONT rotacaoR(PONT p){
	PONT u, v;
	u = p->dir;
	if(u->bal == 1) {   // RR
		p->dir = u->esq;
		u->esq = p;
		p->bal = 0;
		u->bal = 0;
		return u;
	} else if (u->bal == -1){  // RL
		v = u->esq;
		u->esq = v->dir;
		v->dir = u;
		p->dir = v->esq;
		v->esq = p;
		if(v->bal == 1) p->bal = -1;
		else p->bal = 0;
		if(v->bal == -1) u->bal = 1;
		else u->bal = 0;
		v->bal = 0;
		return v;
	}else{   // caso necessario apenas para a exclusao (u->bal == 0)
		p->dir = u->esq;
		u->esq = p;
		u->bal = -1;
		// p->bal = 1;    desnecessario pois o balanceamento de p nao chegou a ser mudado para 2
		return u;	
	}
}

/* Inserção AVL: p é inicializado com o endereco do nó raiz e *alterou com false*/
void inserirAVL(PONT* pp, TIPOCHAVE ch, bool* alterou){
	PONT p = *pp;
	if(!p){
		*pp = criarNovoNo(ch);
		*alterou = true;
	} else {
if(ch == p->chave) *alterou = false;
		else if(ch <= p->chave) {
			inserirAVL(&(p->esq), ch, alterou);
			if(*alterou)
				switch (p->bal) {
					case 1 : p->bal = 0;
					*alterou = false;
					break;
					case 0 : p->bal = -1;
					break;
					case -1: *pp = rotacaoL(p);
					*alterou = false;
					break;
				}
		} else {
			inserirAVL(&(p->dir), ch, alterou);
			if(*alterou)
				switch (p->bal) {
					case -1: p->bal = 0;
					*alterou = false;
					break;
					case 0 : p->bal = 1;
					break;
					case 1 : *pp = rotacaoR(p);
					*alterou = false;
					break;
				}
		}
	}
}

/* retorna o endereco do NO que contem chave=ch ou NULL caso a chave nao seja encontrada. Utiliza busca binaria recursiva*/
PONT buscaBinaria(TIPOCHAVE ch, PONT raiz){
	if (raiz == NULL) return NULL;
	if (raiz->chave == ch) return raiz;
	if (raiz->chave<ch) 
		return buscaBinaria(ch,raiz->dir);
	return buscaBinaria(ch,raiz->esq);
}  

/* Auxiliar da funcao excluirChave, procura a maior chave menor que a chave que 
   serah excluida            */
PONT maiorAEsquerda(PONT p, PONT *ant){
  *ant = p;
  p = p->esq;
  while (p->dir) {
    *ant = p;
    p = p->dir;
  }
  return(p);
}

/* exclui a chave com valor igual a ch   */
bool excluirAVL(PONT* raiz, TIPOCHAVE ch, bool* alterou){
  PONT atual = *raiz;
  if (!atual) {
    *alterou = false;
    return false;
  }
  if (atual->chave == ch){
    PONT substituto, pai_substituto;
    if(!atual->esq || !atual->dir) { // tem zero ou um filho
      if(atual->esq) substituto = atual->esq;
      else substituto = atual->dir;
      *raiz = substituto;
      free(atual);
      *alterou = true;
      return true;
    }
    else {   // tem dois filhos
      substituto = maiorAEsquerda(atual,&pai_substituto);
      atual->chave = substituto->chave;
      ch = substituto->chave; // continua o codigo excluindo o substituto
    }
  }
  bool res;
  if (ch > atual->chave) {
    res = excluirAVL(&(atual->dir), ch, alterou); 
    if (*alterou){
       switch (atual->bal){
          case 1: atual->bal = 0;
          return true;
          case 0: atual->bal = -1;
          *alterou = false;
          return true;
          case -1: *raiz = rotacaoL(atual);
          if (atual->bal != 0) *alterou = false;
          return true;
       }
    }
  }else{
    res = excluirAVL(&(atual->esq), ch, alterou);
    if (*alterou){
       switch (atual->bal){
          case -1: atual->bal = 0;
          return true;
          case 0: atual->bal = 1;
          *alterou = false;
          return true;
          case 1: *raiz = rotacaoR(atual);
          if (atual->bal != 0) *alterou = false;
          return true;
       }
    }
  }
  return res;
}

/* funcao auxiliar na destruicao (liberacao da memoria) de uma arvore */
void destruirAux(PONT subRaiz){
	if (subRaiz){
		destruirAux(subRaiz->esq);
		destruirAux(subRaiz->dir);
		free(subRaiz);
	}
}

/* libera toda memoria de uma arvore e coloca NULL no valor da raiz*/
void destruirArvore(PONT * raiz){
	destruirAux(*raiz);
	*raiz = NULL;
}

/*ABB*/
bool inserirABB(PONT* raiz, TIPOCHAVE ch){
  if (*raiz == NULL){
    *raiz = criarNovoNo(ch);
    return true;
  }else{
	if ((*raiz)->chave >= ch) return inserirABB(&(*raiz)->esq, ch);
    else return inserirABB(&(*raiz)->dir, ch);
  }
}

int main(int argc, char *argv[]) {

entrada = fopen(argv[1],"r");
if(entrada == NULL)
{
    printf("Arquivo de entrada não foi aberto"); 
    return 0;
}
saida = fopen(argv[2],"a");

/**Armazenando o n° de operações (primeiro valor esperado do arquivo de entrada)**/
fscanf(entrada, "%i", &numOperacoes);
if(numOperacoes < 1)
{
    printf("Erro ao tentar ler a quantidade de operacoes\n");
    return 0;
}


//Imprimindo a quantidade de operacoes no arquivo de saida
fprintf(saida, "%i\n", numOperacoes);

//Loop para passar por todas as operações. A quantidade de repeticoes deve ser igual ao total de operacoes indicado no arquivo de entrada
for(int k=1; k<=numOperacoes; k++)
{
  //Imprimindo no arquivo de saida o codigo da operacao executada nessa iteracao
  fscanf(entrada, "%i", &operacao);
  fprintf(saida, "%i\n", operacao);

  //Operacao 1
  if(operacao == 1)
  {
    NO* raiz;
    bool alterou = false;
    iniciarArvore(&raiz);
    int j = 0;
  
    //O formato dado de entrada nao ha nenhuma indicacao de quantos elementos terao na lista. Essa foi a unica forma que encontrei de fazer a leitura de uma linha inteira e parar, considerando que, caso haja mais numeros no arquivo de entrada apos essa linha, seriam referentes a outras operacoes
    while(j<2)
    {
      fgets(elementos,100,entrada);  
      j++;
    }
    /*for(j=0;j<100;j++)
    {
      if(elementos[j] == ' ')
      qntElementos++;
    }
    qntElementos++;*/

    //Como o while acima le os elementos como string e registra inclusive os espacos em branco, o trecho seguinte e para resolver esse problema
    //Usa uma string auxiliar para armazenar cada numero por vez. Depois converte para int e insere na arvore
    int l = 0, m=0;
    for(j=0;j<=1000;j++)
    {

      if(elementos[j] == '0' || elementos[j] == '1' || elementos[j] =='2' || elementos[j] =='3' || elementos[j] =='4' || elementos[j] =='5' || elementos[j] =='6' || elementos[j] =='7' || elementos[j] =='8' || elementos[j] =='9')
      {
        elementosAux[l] = elementos[j];
        l++;
      }
      else
      {
        x = atoi(elementosAux);
        if(x!=0)
          inserirAVL(&raiz,x,&alterou);
        for(l=0;l<=12;l++)
        {
         elementosAux[l] = ' ';
        }
        l=0;
      }
    }
    //Loop para limpar essa variavel. As vezes quando a entrada tinha mais de uma operacao, sem esse loop, alguns elementos indevidos eram inseridos nas arvores
    for(j=0;j<=100;j++)
    {
      elementos[j] = ' ';
    }
    //A funcao seguinte ja imprime a arvore no arquivo de saida
    exibirArvore(raiz);
    fprintf(saida,"\n");
    destruirArvore(&raiz);
  }

  //Nas seguintes eu repito a forma de ler e inserir os elementos. Mudam algumas funcoes apos a leitura/insercao, conforme o que se espera da operacao

  //Operacao 2
  if(operacao==2)
  {
    fscanf(entrada, "%i", &inserir);

    int j = 0;
    while(j<2)
    {
      fgets(elementos,100,entrada);  
      j++;
    }

    NO* raiz;
    bool alterou = false;
    iniciarArvore(&raiz);

    int l = 0, m=0;
    for(j=0;j<=1000;j++)
    {

      if(elementos[j] == '0' || elementos[j] == '1' || elementos[j] =='2' || elementos[j] =='3' || elementos[j] =='4' || elementos[j] =='5' || elementos[j] =='6' || elementos[j] =='7' || elementos[j] =='8' || elementos[j] =='9')
      {
        elementosAux[l] = elementos[j];
        l++;
      }
      else
      {
        x = atoi(elementosAux);
        if(x!=0)
          inserirAVL(&raiz,x,&alterou);
        for(l=0;l<=12;l++)
        {
         elementosAux[l] = ' ';
        }
        l=0;
      }
    }
    for(j=0;j<=100;j++)
    {
      elementos[j] = ' ';
    }
    inserirAVL(&raiz,inserir,&alterou);
    exibirArvore(raiz);
    fprintf(saida,"\n");
    destruirArvore(&raiz);    
  }

  //Operacao 3
  if(operacao==3)
  {
    fscanf(entrada, "%i", &excluir);

    int excluir2 = excluir;
    int j = 0;
    while(j<2)
    {
      fgets(elementos,100,entrada);  
      j++;
    }

    NO* raiz;
    bool alterou = false;
    iniciarArvore(&raiz);

    int l = 0, m=0;
    for(j=0;j<=1000;j++)
    {

      if(elementos[j] == '0' || elementos[j] == '1' || elementos[j] =='2' || elementos[j] =='3' || elementos[j] =='4' || elementos[j] =='5' || elementos[j] =='6' || elementos[j] =='7' || elementos[j] =='8' || elementos[j] =='9')
      {
        elementosAux[l] = elementos[j];
        l++;
      }
      else
      {
        x = atoi(elementosAux);
        if(x!=0)
          inserirAVL(&raiz,x,&alterou);
        for(l=0;l<=12;l++)
        {
         elementosAux[l] = ' ';
        }
        l=0;
      }
    }
    for(j=0;j<=100;j++)
    {
      elementos[j] = ' ';
    }
    excluirAVL(&raiz,excluir2,&alterou);
    exibirArvore(raiz);
    fprintf(saida,"\n");
    destruirArvore(&raiz); 
  }

  if(operacao==4)
  {
    int chave;
    fscanf(entrada,"%i",&chave);
    int j = 0;
    while(j<2)
    {
      fgets(elementos,100,entrada);  
      j++;
    }

    NO* raiz;
    bool alterou = false;
    iniciarArvore(&raiz);

    int l = 0;
    for(j=0;j<=100;j++)
    {

      if(elementos[j] == '0' || elementos[j] == '1' || elementos[j] =='2' || elementos[j] =='3' || elementos[j] =='4' || elementos[j] =='5' || elementos[j] =='6' || elementos[j] =='7' || elementos[j] =='8' || elementos[j] =='9')
      {
        elementosAux[l] = elementos[j];
        l++;
      }
      else
      {
        x = atoi(elementosAux);
        if(x!=0)
          inserirABB(&raiz,x);
        for(l=0;l<=12;l++)
        {
         elementosAux[l] = ' ';
        }
        l=0;
      }
      
    }
    for(j=0;j<=100;j++)
    {
      elementos[j] = ' ';
    }
    atualizarBalanceamentoTotal(raiz);
    NO* aux = buscaBinaria(chave, raiz);
    fprintf(saida,"%i\n",aux->bal);
    destruirArvore(&raiz);
  }

  if(operacao==5)
  {
    int j = 0;
    while(j<2)
    {
      fgets(elementos,100,entrada);  
      j++;
    }

    NO* raiz;
    bool alterou = false;
    iniciarArvore(&raiz);

    int l = 0, m=0;
    for(j=0;j<=1000;j++)
    {

      if(elementos[j] == '0' || elementos[j] == '1' || elementos[j] =='2' || elementos[j] =='3' || elementos[j] =='4' || elementos[j] =='5' || elementos[j] =='6' || elementos[j] =='7' || elementos[j] =='8' || elementos[j] =='9')
      {
        elementosAux[l] = elementos[j];
        l++;
      }
      else
      {
        x = atoi(elementosAux);
        if(x!=0)
          inserirABB(&raiz,x);
        for(l=0;l<=10;l++)
        {
         elementosAux[l] = ' ';
        }
        l=0;
      }
    }
    for(j=0;j<=100;j++)
    {
      elementos[j] = ' ';
    }
    bool avl = ehAVL(raiz);
    if(avl == true)
    {
      fprintf(saida,"T\n");
    }
    if(avl == false)
    {
      fprintf(saida,"F\n");
    }
    destruirArvore(&raiz);
  }
}



  return 0;
}
