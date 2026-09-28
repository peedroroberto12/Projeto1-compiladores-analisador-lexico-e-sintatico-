// Projeto compiladores: Compilador fase 1: Análise léxica e sintática
// Pedro Roberto Fernandes Noronha, RA:10443434

// Implementado com base no PDF do projeto e nos códigos disponibilizados
// no GitHub da disciplina, como minilex, minisintatico e ASDR3.

// compilacao: gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
// Executar: ./compilador <arquivo de entrada>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

// =====================================================================
// ANALISADOR LEXICO
// =====================================================================

typedef enum{
    ERRO,

    // palavras reservadas
    ALGORITMO,
    CARACTERE,
    DIV,
    E,
    ENQUANTO,
    ENTAO,
    ESCREVA,
    FACA,
    FALSO,
    FIM,
    FUNCAO,
    INICIO,
    INTEIRO,
    LEIA,
    LOGICO,
    MOD,
    NAO,
    OU,
    PROCEDIMENTO,
    SE,
    SENAO,
    VAR,
    VERDADEIRO,

    // simbolos
    PONTO_VIRGULA,
    VIRGULA,
    PONTO,
    DOIS_PONTOS,
    ATRIBUICAO,
    ABRE_PAR,
    FECHA_PAR,
    SOMA,
    SUB,
    MULT,
    IGUAL,
    DIFERENTE,
    MENOR,
    MENOR_IGUAL,
    MAIOR,
    MAIOR_IGUAL,

    // identificador, constantes e especiais
    IDENTIFICADOR,
    CONSTINT,
    CONSTCHAR,
    COMENTARIO,
    EOS,

}TAtomo;

// Informacoes adicionais do atomo
typedef struct{
   TAtomo atomo;
   int linha;
    union{
        int numero;
        char id[16];
        char ch;
    }atributo;

}TInfoAtomo;

void reconhece_constint(TInfoAtomo *info_atomo);
void reconhece_id(TInfoAtomo *info_atomo);
void reconhece_constchar(TInfoAtomo *info_atomo);
TInfoAtomo  obter_atomo(void);
void reconhece_comentario(TInfoAtomo *info_atomo);

// Nome de cada atomo
char *strAtomo[] = {
    "erro",

    "algoritmo",
    "caractere",
    "div",
    "e",
    "enquanto",
    "entao",
    "escreva",
    "faca",
    "falso",
    "fim",
    "funcao",
    "inicio",
    "inteiro",
    "leia",
    "logico",
    "mod",
    "nao",
    "ou",
    "procedimento",
    "se",
    "senao",
    "var",
    "verdadeiro",

    "ponto_virgula",
    "virgula",
    "ponto",
    "dois_pontos",
    "atribuicao",
    "abre_par",
    "fecha_par",
    "mais",
    "menos",
    "vezes",
    "igual",
    "diferente",
    "menor",
    "menor_igual",
    "maior",
    "maior_igual",

    "identificador",
    "constint",
    "constchar",
    "comentario",
    "fim_de_arquivo",

};

// Tabela de palavras reservadas
typedef struct {
    char *lexema;
    TAtomo atomo;
} TPalavraReservada;

#define NUM_RESERVADAS 23

TPalavraReservada reservadas[NUM_RESERVADAS] = {
    {"algoritmo",    ALGORITMO},
    {"caractere",    CARACTERE},
    {"div",          DIV},
    {"e",            E},
    {"enquanto",     ENQUANTO},
    {"entao",        ENTAO},
    {"escreva",      ESCREVA},
    {"faca",         FACA},
    {"falso",        FALSO},
    {"fim",          FIM},
    {"funcao",       FUNCAO},
    {"inicio",       INICIO},
    {"inteiro",      INTEIRO},
    {"leia",         LEIA},
    {"logico",       LOGICO},
    {"mod",          MOD},
    {"nao",          NAO},
    {"ou",           OU},
    {"procedimento", PROCEDIMENTO},
    {"se",            SE},
    {"senao",        SENAO},
    {"var",          VAR},
    {"verdadeiro",   VERDADEIRO},
};

char *buffer;
char *inicio_buffer;
char lexema[20];
int contaLinha = 1;

// Retorna o proximo atomo
TInfoAtomo  obter_atomo(void){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // Ignora espacos e conta as linhas
    while( *buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r' ){
        if( *buffer == '\n' ){
            contaLinha++;
        }
        buffer++;
    }

    info_atomo.linha = contaLinha;

    if(*buffer == 0)
        info_atomo.atomo = EOS;
    else if( isdigit(*buffer))
        reconhece_constint(&info_atomo);
    else if(isalpha(*buffer))
        reconhece_id(&info_atomo);
    else if(*buffer == '*'){
        info_atomo.atomo = MULT;
        buffer++;
    }
    else if(*buffer == '+'){
        info_atomo.atomo = SOMA;
        buffer++;
    }
    else if(*buffer == '-'){
        info_atomo.atomo = SUB;
        buffer++;
    }
    else if(*buffer == '('){
        info_atomo.atomo = ABRE_PAR;
        buffer++;
    }
    else if(*buffer == ')'){
        info_atomo.atomo = FECHA_PAR;
        buffer++;
    }

    else if(*buffer == ';'){
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
    }
    else if(*buffer == ','){
        info_atomo.atomo = VIRGULA;
        buffer++;
    }
    else if(*buffer == '.'){
        info_atomo.atomo = PONTO;
        buffer++;
    }

    else if(*buffer == '='){
        info_atomo.atomo = IGUAL;
        buffer++;
    }

    // Operadores de dois caracteres
    else if( *buffer == ':' ){
        buffer++;
        if( *buffer == '=' ){
            buffer++;
            info_atomo.atomo = ATRIBUICAO;
        }
        else{
            info_atomo.atomo = DOIS_PONTOS;
        }
    }

    else if( *buffer == '\'' ){
        reconhece_constchar(&info_atomo);
    }

    else if( *buffer == '{' ){
        reconhece_comentario(&info_atomo);
    }

    else if( *buffer == '>' ){
        buffer++;
        if( *buffer == '=' ){
            buffer++;
            info_atomo.atomo = MAIOR_IGUAL;
        }
        else{
            info_atomo.atomo = MAIOR;
        }
    }

    else if( *buffer == '<' ){
        buffer++;
        if( *buffer == '=' ){
            buffer++;
            info_atomo.atomo = MENOR_IGUAL;
        }
        else if( *buffer == '>' ){
            buffer++;
            info_atomo.atomo = DIFERENTE;
        }
        else{
            info_atomo.atomo = MENOR;
        }
    }

    return info_atomo;
}

// Reconhece comentarios {- ... -}
void reconhece_comentario(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;

    if( *buffer == '{' ){
        buffer++;
        goto q1;
    }
    return;

q1:
    if( *buffer == '-' ){
        buffer++;
        goto q2;
    }
    return;

q2:
    if( *buffer == '\0' ){
        return;
    }
    if( *buffer == '-' ){
        buffer++;
        goto q3;
    }
    if( *buffer == '\n' ){
        contaLinha++;
    }
    buffer++;
    goto q2;

q3:
    if( *buffer == '}' ){
        buffer++;
        info_atomo->atomo = COMENTARIO;
        return;
    }
    goto q2;
}

// Reconhece constantes do tipo char
void reconhece_constchar(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;

    if( *buffer == '\'' ){
        buffer++;
        goto q1;
    }
    return;

q1:
    if( *buffer != '\0' ){
        info_atomo->atributo.ch = *buffer;
        buffer++;
        goto q2;
    }
    return;

q2:
    if( *buffer == '\'' ){
        buffer++;
        info_atomo->atomo = CONSTCHAR;
        return;
    }
    return;
}

// Reconhece constantes inteiras
void reconhece_constint(TInfoAtomo *info_atomo){
    char *ini_lexema = buffer;
    info_atomo->atomo = ERRO;

    if(isdigit(*buffer)){
        buffer++;
        goto q1;
    }
    return;

q1:
    if( isdigit(*buffer) ){
        buffer++;
        goto q1;
    }
    if( *buffer == 'E' ){
        buffer++;
        goto q2;
    }

    if( isalpha(*buffer) ){
        return;
    }
    goto final;

q2:
    if( isdigit(*buffer) ){
        buffer++;
        goto q4;
    }

    if( *buffer == '+'){
        buffer++;
        goto q3;
    }
    return;

q3:
    if( isdigit(*buffer) ){
        buffer++;
        goto q4;
    }
    return;

q4:
    if( isdigit(*buffer) ){
        buffer++;
        goto q4;
    }
    if( isalpha(*buffer) )
        return;
    goto final;

final:
    if( buffer - ini_lexema > 19 ){
      return;
    }

    strncpy(lexema,ini_lexema,buffer-ini_lexema);
    lexema[buffer-ini_lexema] = '\0';
    info_atomo->atomo = CONSTINT;
    info_atomo->atributo.numero = atof(lexema);

    return ;
}

// Reconhece identificadores
void reconhece_id(TInfoAtomo *info_atomo){
    char *ini_lexema = buffer;
    info_atomo->atomo = ERRO;

    if( isalpha(*buffer)){
        buffer++;
        goto q1;
    }
    return;

q1:
    if( isalpha(*buffer)|| isdigit(*buffer) || (*buffer == '_') ){
        buffer++;
        goto q1;
    }
    goto final2;

final2:

    if( buffer - ini_lexema > 15 ){
        return;
       }

    strncpy(info_atomo->atributo.id,ini_lexema,buffer-ini_lexema);
    info_atomo->atributo.id[buffer-ini_lexema] = '\0';
    info_atomo->atomo = IDENTIFICADOR;

    // Verifica se o identificador e uma palavra reservada
    for (int i = 0; i < NUM_RESERVADAS; i++) {
        if (strcasecmp(info_atomo->atributo.id, reservadas[i].lexema) == 0) {
            info_atomo->atomo = reservadas[i].atomo;
            break;
        }
    }

    return;
}

// =====================================================================
// ANALISADOR SINTATICO
// =====================================================================

TAtomo lookahead;
TInfoAtomo info_atomo;

char *le_arquivo( char *nome );
void termina( int codigo );
void imprime_atomo( TInfoAtomo info );
void proximo_atomo();
void consome( TAtomo atomo );
void programa();
void bloco();
void declaracao_variaveis();
void lista_variaveis();
void tipo();
void declaracao_de_rotinas();
void declaracao_de_funcao();
void declaracao_de_procedimento();
void parametros_formais();
void parametro_formal();
void comando_composto();
void comando();
void atribuicao_ou_chamada();
void comando_entrada();
void comando_saida();
void comando_condicional();
void comando_repeticao();
void expressao();
void expressao_simples();
void termo();
void fator();
void lista_expressao();

int main( int argc, char *argv[] ){
    if( argc < 2 ){
        printf("Uso: %s <arquivo de entrada>\n", argv[0]);
        return 1;
    }

    inicio_buffer = le_arquivo(argv[1]);
    buffer = inicio_buffer;

    proximo_atomo();

    programa();

    consome(EOS);

    int linhas = contaLinha;
    if( buffer > inicio_buffer && *(buffer - 1) == '\n' ){
        linhas--;
    }

    printf("%d linhas analisadas, programa sintaticamente correto\n", linhas);

    termina(0);
    return 0;
}

// Le o arquivo inteiro para a memoria
char *le_arquivo( char *nome ){
    FILE *arq = fopen(nome, "rb");

    if( arq == NULL ){
        printf("Erro: nao foi possivel abrir o arquivo %s\n", nome);
        exit(1);
    }

    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    fseek(arq, 0, SEEK_SET);

    char *texto = malloc(tamanho + 1);

    if( texto == NULL ){
        printf("Erro: memoria insuficiente\n");
        fclose(arq);
        exit(1);
    }

    fread(texto, 1, tamanho, arq);
    texto[tamanho] = '\0';

    fclose(arq);
    return texto;
}

// Libera a memoria utilizada
void termina( int codigo ){
    free(inicio_buffer);
    exit(codigo);
}

// Imprime o atomo reconhecido
void imprime_atomo( TInfoAtomo info ){
    if( info.atomo == IDENTIFICADOR ){
        printf("# %d:%s: %s\n", info.linha, strAtomo[info.atomo], info.atributo.id);
    }
    else if( info.atomo == CONSTINT ){
        printf("# %d:%s: %d\n", info.linha, strAtomo[info.atomo], info.atributo.numero);
    }
    else if( info.atomo == CONSTCHAR ){
        printf("# %d:%s: %c\n", info.linha, strAtomo[info.atomo], info.atributo.ch);
    }
    else{
        printf("# %d:%s\n", info.linha, strAtomo[info.atomo]);
    }
}

// Atualiza o lookahead
void proximo_atomo(){
    info_atomo = obter_atomo();

    while( info_atomo.atomo == COMENTARIO ){
        imprime_atomo(info_atomo);
        info_atomo = obter_atomo();
    }

    if( info_atomo.atomo == ERRO ){
        printf("# %d:erro lexico\n", info_atomo.linha);
        termina(0);
    }

    lookahead = info_atomo.atomo;
}

// Consome o atomo esperado
void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        if( atomo != EOS ){
            imprime_atomo(info_atomo);
        }
        proximo_atomo();
    }
    else{
        printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
               info_atomo.linha, strAtomo[atomo], strAtomo[lookahead]);
        termina(0);
    }
}

// algoritmo nome; ... .
void programa(){
    consome(ALGORITMO);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// Declaracoes e comandos do programa
void bloco(){
    declaracao_variaveis();
    declaracao_de_rotinas();
    comando_composto();
}

// Declaracao de variaveis
void declaracao_variaveis(){
    if( lookahead == VAR ){
        consome(VAR);
        lista_variaveis();
        consome(PONTO_VIRGULA);

        while( lookahead == IDENTIFICADOR ){
            lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
}

// Lista de variaveis
void lista_variaveis(){
    consome(IDENTIFICADOR);

    while( lookahead == VIRGULA ){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }

    consome(DOIS_PONTOS);
    tipo();
}

// Tipos disponiveis
void tipo(){
    if( lookahead == CARACTERE ){
        consome(CARACTERE);
    }
    else if( lookahead == INTEIRO ){
        consome(INTEIRO);
    }
    else{
        consome(LOGICO);
    }
}

// Declaracao de funcoes e procedimentos
void declaracao_de_rotinas(){
    while( lookahead == FUNCAO || lookahead == PROCEDIMENTO ){
        if( lookahead == FUNCAO ){
            declaracao_de_funcao();
        }
        else{
            declaracao_de_procedimento();
        }
    }
}

// Declaracao de funcao
void declaracao_de_funcao(){
    consome(FUNCAO);
    tipo();
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// Declaracao de procedimento
void declaracao_de_procedimento(){
    consome(PROCEDIMENTO);
    consome(IDENTIFICADOR);
    
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// Parametros formais
void parametros_formais(){
    consome(ABRE_PAR);

    if( lookahead == VAR || lookahead == IDENTIFICADOR ){
        parametro_formal();

        while( lookahead == PONTO_VIRGULA ){
            consome(PONTO_VIRGULA);
            parametro_formal();
        }
    }

    consome(FECHA_PAR);
}

// Parametro formal
void parametro_formal(){
    if( lookahead == VAR ){
        consome(VAR);
    }

    lista_variaveis();
}

// Comando composto
void comando_composto(){
    consome(INICIO);
    comando();

    while( lookahead == PONTO_VIRGULA ){
        consome(PONTO_VIRGULA);
        comando();
    }

    consome(FIM);
}

// Identifica o tipo de comando
void comando(){
    if( lookahead == IDENTIFICADOR ){
        atribuicao_ou_chamada();
    }
    else if( lookahead == LEIA ){
        comando_entrada();
    }
    else if( lookahead == ESCREVA ){
        comando_saida();
    }
    else if( lookahead == SE ){
        comando_condicional();
    }
    else if( lookahead == ENQUANTO ){
        comando_repeticao();
    }
    else{
        comando_composto();
    }
}

// Atribuicao ou chamada de procedimento
void atribuicao_ou_chamada(){
    consome(IDENTIFICADOR);

    if( lookahead == ATRIBUICAO ){
        consome(ATRIBUICAO);
        expressao();
    }
    else if( lookahead == ABRE_PAR ){
        consome(ABRE_PAR);
        lista_expressao();
        consome(FECHA_PAR);
    }
}

// Comando de entrada
void comando_entrada(){
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);

    while( lookahead == VIRGULA ){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }

    consome(FECHA_PAR);
}

// Comando de saida
void comando_saida(){
    consome(ESCREVA);
    consome(ABRE_PAR);

    lista_expressao();
    consome(FECHA_PAR);
}

// Comando condicional
void comando_condicional(){
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();

    if( lookahead == SENAO ){
        consome(SENAO);
        comando();
    }
}

// Comando de repeticao
void comando_repeticao(){
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

// Expressao com operador relacional
void expressao(){
    expressao_simples();

    if( lookahead == DIFERENTE || lookahead == MENOR || lookahead == MENOR_IGUAL ||
        lookahead == MAIOR_IGUAL || lookahead == MAIOR || lookahead == IGUAL ){

        consome(lookahead);
        expressao_simples();
    }
}

// Operacoes de soma
void expressao_simples(){
    termo();

    while( lookahead == SOMA || lookahead == SUB || lookahead == MOD || lookahead == OU ){
        consome(lookahead);
        termo();
    }
}

// Operacoes de multiplicacao
void termo(){
    fator();

    while( lookahead == MULT || lookahead == DIV || lookahead == E ){
        consome(lookahead);
        fator();
    }
}

// Fator de uma expressao
void fator(){
    if( lookahead == IDENTIFICADOR ){
        consome(IDENTIFICADOR);

        if( lookahead == ABRE_PAR ){
            consome(ABRE_PAR);
            lista_expressao();
            consome(FECHA_PAR);
        }
    }
    else if( lookahead == CONSTINT ){
        consome(CONSTINT);
    }
    else if( lookahead == CONSTCHAR ){
        consome(CONSTCHAR);
    }
    else if( lookahead == VERDADEIRO ){
        consome(VERDADEIRO);
    }
    else if( lookahead == FALSO ){
        consome(FALSO);
    }
    else if( lookahead == SOMA || lookahead == SUB || lookahead == NAO ){
        consome(lookahead);
        fator();
    }
    else{
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
}

// Lista de expressoes
void lista_expressao(){
    expressao();

    while( lookahead == VIRGULA ){
        consome(VIRGULA);
        expressao();
    }
}
