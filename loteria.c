#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PRO 12
#define NUM_TICKETS 100

int dar_tickets(int* tickets);

int main(){
    srand(time(NULL));
    int i;
    int quantum = 1;
    int tickets[NUM_TICKETS];       //vetor com os tickets para a loteria
    int used_ticket[NUM_TICKETS];   //vetor que armazena se um ticket foi usado. 0 = não foi usado, 1 = foi usado 
    int processos_finalizados = 0;  //variavel de controle dos processos finalizados
    
    int troca_contextos = 0;    
    float t_m_espera = 0;
    float t_m_vida = 0;

    int ts_chegada[NUM_PRO];        //vetor que armazena os tempos de chegada dos procesos
    int ts_burst[NUM_PRO];          //vetor que armazena a duração dos processos
    int ts_finalizacao[NUM_PRO];    //vetor que armazena os tempos de finalização dos processos

    //loop para marcar todos os tickes como não usados
    for(i=0;i<NUM_TICKETS;i++){
        used_ticket[i]=0;
    }
    
    //Bloco de gerenciamento do arquivo de entrada
    FILE *entrada;
    entrada = fopen("entrada.txt","r");
    // Verifica existência do arquivo de entrada
    if (entrada == NULL){
        printf("\nArquivo de entrada não encontrado\n");
        exit(EXIT_FAILURE);
    }

    //Criação do vetor dos processos
    int processos[NUM_PRO][4];  //[0] = Tempo de chegada [1] = Duração (Burst)  [2] = prioridade (não implementado)  [3] = finalizado(1) ou não(0)
    
    
    //Lendo arquivo de entrada e armazenando tempo de chegada e Duração (Burst) na matriz de processos
    for(i=0;i<NUM_PRO;i++){
        fscanf(entrada,"%d %d %d", &processos[i][0], &processos[i][1], &processos[i][2]);
        processos[i][3] = 0;                 //marca que os processos ainda não foram concluídos
        ts_chegada[i] = processos[i][0];     
        ts_burst[i] = processos[i][1];       //Armazenando os tempos de chegada e duração
    }

    fclose(entrada);
    //Fim do bloco do arquivo de entrada

    //Dando tickets para os processos
    dar_tickets(tickets);

    //Geração do arquivo de saída
    FILE *saida;
    saida = fopen("saida.txt","w+");

    //Impressão do cabeçalho do arquivo de saída
    fprintf(saida,"Tempo  ");
    for (i=0; i<10;i++){
        fprintf(saida,"  P%d",i+1);
    }
    for (i=10; i<NUM_PRO; i++){
        fprintf(saida," P%d",i+1);
    }
    fprintf(saida,"\n");

    int tempo = 0;      
    int s;                  //varíavel do sorteio
    int aux_pro = 0;        //Varíavel auxiliar para contar as trocas de contexto
    int posicao_impressao;  //Variavel para auxiliar com a impressao dos tempos no arquivo de saida
    int pro;                //Variavel para marcar o processo atual
    int validacao;          //Varaivel para auxiliar se um processo vai ser executado

    while(processos_finalizados<NUM_PRO){
        do{
            //Sorteio do ticket
            s = rand()%NUM_TICKETS;
            validacao = 0;
            //Verifica se um icket foi usado
            if(used_ticket[s]==0){
                validacao++;
                pro = tickets[s];
            }
            //Verifica se o processo ainda não acabou
            if(processos[pro][3] == 0){
                validacao++;
            }
            pro = tickets[s];

        }while(validacao !=2); //Ticket válido e e Processo não finalizado
        
        processos[pro][1] = processos[pro][1] - quantum;
        used_ticket[pro] = 1;
        
        //Verifica se o processo atual acabou e incrementa processos_finalizados
        if(processos[pro][1] <= 0){ 
            processos[pro][3] = 1;
            processos_finalizados++;
            ts_finalizacao[pro] = tempo;
        }
        tempo++;    
        
        //Bloco de impressão no arquivo de saída
        if(tempo<9){
            fprintf(saida,"%d - %d    ",tempo,tempo+1);
        }else{
            if(tempo<10){
                printf(saida,"%d - %d   ",tempo,tempo+1);
            }else{
                fprintf(saida,"%d - %d  ",tempo,tempo+1);
            }
        }
        for(posicao_impressao=0;posicao_impressao<pro;posicao_impressao++){
            fprintf(saida,"--  ");       
         }
        fprintf(saida,"##  ");
        for(posicao_impressao=pro;posicao_impressao<NUM_PRO-1;posicao_impressao++){
            fprintf(saida,"--  ");
        }
            
        fprintf(saida,"\n");
        //Fim do bloco de impressão
        
        //Verifica se houve troca de contexto
        if(pro != aux_pro){
            troca_contextos++;
        }
        aux_pro = pro;    
    }
    //Contagem de tempo médio de vida e tempo médio de espera
    for(i=0;i<NUM_PRO;i++){
        t_m_vida += ts_finalizacao[i] - ts_chegada[i];
        t_m_espera +=  ts_finalizacao[i] - ts_chegada[i] - ts_burst[i];
    }

    t_m_vida = t_m_vida/NUM_PRO;
    t_m_espera = t_m_espera/NUM_PRO;
    
    fprintf(saida,"\n");
    fprintf(saida,"\n");

    fprintf(saida,"Trocas de contexto: %d\n", troca_contextos);
    fprintf(saida,"Tempo medio de vida: %.2f\n", t_m_vida);
    fprintf(saida,"tempo medio de espera: %.2f\n", t_m_espera);
    fclose(saida);

    return 0;
    
}

//Função que associa os tickets aos processos
int dar_tickets(int *tickets){
    int i = 0;
    int p; //número do processo
    for(i=0;i<NUM_TICKETS;i++){
        p = rand()%NUM_PRO;
        tickets[i] = p;
    }
}
