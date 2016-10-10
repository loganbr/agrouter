/*
        agrouter - protocol optimizing network

        Copyright (C) 2004 agrouter authors

        This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*******Parametros Configuraveis****/
#define qtd_nos_rede 16
#define lin_tab_rotas 62
#define indiv 150
#define tx_mutacao 0.25
#define qtd_selecionados 15

/********Parametros Fixos************/
#define num_ger  500
#define no_alelo 4
#define alelo 48
#define gene 12

void cruza();
void myrand();
void gerapop();
void selecao();
void mutacao();
void inicializa();
void convdecimal();
void monta_rotas();
void aptidao(int , int);
void finaliza(int);
int modal();
int nos_ligados(int, int);


int rota_sel[gene+2], melhor_rota[gene+2], bin[indiv][alelo];
int no[indiv][gene], custo_total[indiv], score[indiv], geracao, pulos=0;
int ponto_corte1, ponto_corte2;
int reproducao[qtd_selecionados];

int rotas[62][3] = {{0,1,2}, {0,2,2}, {0,3,1}, {1,0,2}, {1,4,2}, {1,5,4},
		    {2,0,2}, {2,3,3}, {2,6,5}, {2,10,7}, {3,0,1}, {3,2,3},
		    {3,4,8}, {3,6,3}, {4,1,2}, {4,3,8}, {4,5,4}, {4,7,1},
		    {5,1,4}, {5,4,4}, {5,7,2}, {5,13,10}, {6,2,5}, {6,3,3},
		    {6,7,4}, {6,8,2}, {7,4,1}, {7,5,2}, {7,6,4}, {7,8,2},
		    {7,9,3}, {8,6,2}, {8,7,2}, {8,9,2}, {8,10,2}, {8,11,3},
                    {9,7,3}, {9,8,2}, {9,12,1}, {9,13,7}, {10,2,7}, {10,8,2},
                    {10,11,2}, {10,14,6}, {11,8,3}, {11,10,2}, {11,12,6}, {11,14,1},
                    {12,9,1}, {12,11,6}, {12,13,2}, {12,15,3}, {13,5,10}, {13,9,7},
		    {13,12,2}, {13,15,9}, {14,10,6}, {14,11,1}, {14,15,5}, {15,12,3},
		    {15,13,9}, {15,14,5}};

/*************** Funcao de Inicializacao da Populacao ***************/

void myrand()
{
  time_t agora;
  agora=(unsigned) time(&agora);
  srand(agora);
}

void gerapop()
{
 int i=0, j=0;

 myrand();
 for(i=0; i<indiv; i++)
    for(j=0; j<alelo; j++)
    bin[i][j] = rand() % 2;
 
convdecimal();
}

/*************** Funcao de Conversao para Decimal ***************/

void convdecimal()
{
 int i=0, j=0, k=0, l=0, m=0, num=0, decimal=0;

 num = 1;
 m = no_alelo - 1;

 for(i=0; i<indiv; i++)
    for(j=0,k=0; j<gene; j++,k+=no_alelo)
    {
       for(l=0; l<no_alelo; l++)
       {
	  decimal += (bin[i][k+m] * num);

	  num<<=1;
	  m--;
       }
       no[i][j] = decimal;
       decimal = 0;
       num = 1;
       m = no_alelo - 1;
    }
}

/*************** Funcao Monta Rotas ***************/

void monta_rotas()
{
 int i=0, j=0, no1=0, no2=0, origem=0, custo=0;
 origem = 0;
 for(i=0; i<indiv; i++)
 {
    score[i] = 0;
    custo_total[i] = 0;
    no1 = origem;
    for(j=0; j<gene; j++)
    {
       no2 = no[i][j];
       custo=nos_ligados(no1, no2);
       if(custo != 0)
       {
	 score[i] += 1;
	 custo_total[i] += custo;
	 no1 = no2;
       }
       else
	 break;
    }
 }
}

/******************** Funcao de Nos Ligados ************************/

int nos_ligados(int no1, int no2)
{
 int i;
 for(i=0; i<lin_tab_rotas; i++)
    if((no1==rotas[i][0]) && (no2==rotas[i][1]))
      return rotas[i][2];
 return 0;
}


/******************** Funcao de Modal ************************/

int modal()
{
 int i=0, temp[indiv], maior=0, corte=0;
 for(i=0; i<indiv; i++)
      temp[i] = 0;
 for(i=0; i<indiv; i++)
    if(score[i] != 0)
      temp[score[i]] = temp[score[i]]+1;
 for(i=1; i<indiv; i++)
    if(temp[i] > maior)
    {
      maior = temp[i];
      corte = i;
    }
 return corte;
}

/*************** Funcao de Aptidao ***************/

void aptidao(int origem, int destino)
{
 int i=0, j=0, k=0, no1=0,no2=0, custo=0, custo_tot;
 pulos=0;

 for(i=0; i<indiv; i++)
 {
    no1 = origem;
    rota_sel[0]=origem;
    if(custo_total[i] != 0)
    {
      custo = custo_tot = pulos = 0;
      for(j=1; j<gene; j++)
      {
	 no2=no[i][j];
	 custo = nos_ligados(no1,no2);
	 if(custo ^ 0)
	 {
	   rota_sel[j]=no2;
	   custo_tot += custo;
	   pulos++;
	 }
	 else
	   break;

	 if(no2 == destino)
	 {
	   if(custo_tot < melhor_rota[gene])
	   {
	      for(k=0; k<gene; k++)
		 melhor_rota[k] = rota_sel[k];
	      melhor_rota[gene] = custo_tot;
	      melhor_rota[gene+1] = pulos;
	   }
	 }
	 no1 = no2;
      }
    }
 }
 if(geracao == num_ger)
   finaliza(destino);
}

/*************** Funcao de Selecao ***************/

void selecao()
{
 int i, j, k, bin_aux, custo_aux, score_aux;
 float melhori=0.0, melhorj=0.0;
 convdecimal();
 monta_rotas();
 for(i=0; i<indiv-1; i++)
 {
    for(j=i+1; j<indiv; j++)
    {
       melhorj=score[j]+1.0-(float)(custo_total[j]/10.0);
       melhori=score[i]+1.0-(float)(custo_total[i]/10.0);
       if(melhorj>melhori)
       {
	 for(k=0; k<alelo; k++)
	 {
	    bin_aux = bin[i][k];
	    bin[i][k] = bin[j][k];
	    bin[j][k] = bin_aux;
	 }
	 custo_aux = custo_total[j];
	 custo_total[j] = custo_total[i];
	 custo_total[i] = custo_aux;

	 score_aux = score[j];
	 score[j] = score[i];
	 score[i] = score_aux;
       }
    }
 }
convdecimal();
monta_rotas();
}


/*************** Funcao de Cruzamento (crossover) ***************/

void cruzamento()
{
 int i, j, k, cont=0, qtd_filhos, cru[indiv][alelo], tot_filhos=0;
 float perc;
 /* Calcula a qtd. de filhos p/ cada selecionado */
 for(i=0,perc=.3;i<3;i++,perc-=.1)
 {
    reproducao[i]=perc*indiv;
    tot_filhos+=reproducao[i];
 }
 qtd_filhos=(indiv-tot_filhos)/(qtd_selecionados-3);

 for(i=3;i<qtd_selecionados;i++)
    reproducao[i]=qtd_filhos;
  reproducao[3]+=indiv-(tot_filhos+(qtd_filhos*(qtd_selecionados-3)));

 /* Calcula os pontos de corte */
 ponto_corte1 = modal() * no_alelo;

 if(ponto_corte1 <= alelo-(no_alelo*2))
    ponto_corte2 =  ponto_corte1+((alelo-ponto_corte1)/2);
 else
    ponto_corte2 = alelo;

 do{
    for(i=0; i<qtd_selecionados; i++)
    {
       qtd_filhos=reproducao[i];
       for(j=0; j<qtd_filhos; j++)
       {
	  for(k=0; k<ponto_corte1; k++)
	     cru[cont][k] = bin[i][k];
	  for(k=ponto_corte1;k<ponto_corte2; k++)
	     cru[cont][k] = bin[j][k];
	  for(k=ponto_corte2;k<alelo; k++)
	     cru[cont][k] = bin[j+1][k];
	  cont++;
       }
    }
 }while(cont < indiv);

 for(i=0; i<indiv; i++)
    for(j=0; j<alelo; j++)
    {
      bin[i][j] = cru[i][j];
    }
}

/*************** Funcao de Mutacao ***************/

void mutacao()
{
 int i=0, j=0, k=0, taxa=0;

 taxa = tx_mutacao * (alelo * indiv);
 myrand();
 for(k=0; k<taxa; k++)
 {
    i = rand() % indiv;
    j = rand() % alelo;

    if(bin[i][j] == 1)
      bin[i][j] = 0;
    else
      bin[i][j] = 1;
 }
}

/*************** Funcao Finaliza ***************/

void finaliza(int destino)
{
  int i=0;
  if(melhor_rota[gene] != 32767)
  {
    printf("\n\nObjetivo Alcancado !!\n\n");
    printf("Custo = %d\n",melhor_rota[gene]);
    printf("Pulos = %d\n",melhor_rota[gene+1]);
    printf("rota.: ");
    for(i=0; i < gene+1; i++)
    {
       printf("%d ", melhor_rota[i]);
       if(melhor_rota[i] == destino)
	 break;
    }
  }
  else
    printf("\n\nA rota nao foi encontrada !!!\n");
}

/************* Funcao para Inicializar Vetores *************/

void inicializa()
{
 int i;
 for(i=0; i<gene; i++)
   melhor_rota[i] = -1;
 melhor_rota[gene] = 32767;
 melhor_rota[gene+1] = 32767;

}

/*************** Funcao Principal (main) ***************/

int main()
{
 int O, D;
 time_t first, second;
 system("clear");
	 
 printf("ORIGEM.: ");scanf("%d",&O);
 printf("DESTINO.: ");scanf("%d",&D);
 first=time(NULL);

 inicializa();
 gerapop();

 geracao = 1;
 while(geracao<=num_ger)
 {
   selecao();
   cruzamento();
   aptidao(O,D);
   mutacao();
   geracao++;
 }
 second=time(NULL);
 printf("\nTempo Gasto em Segundos: %.2f\n", difftime(second,first));
getch();

return 0;
}
