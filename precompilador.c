/*
 * precompilador.c
 * "Pre-compilador": junta, num arquivo so, o material do professor:
 *   - miniLex.h     -> PARTE 1 (definicoes)
 *   - miniLex.c     -> PARTE 2 (analisador lexico)
 *   - miniSintatico -> PARTE 3 (lookahead, consome, main)
 *   - ASDR3.c       -> PARTE 3 (gramatica expressao/termo/fator)
 *
 * Todo trecho que NAO e copia literal do professor esta marcado com
 *   // ALTERADO: ...   ou   // ADICIONADO: ...
 *
 * Gramatica (do ASDR3):
 *   <expressao> ::= <termo> {('+'|'-') <termo>}
 *   <termo>     ::= <fator> {('*'|'/') <fator>}
 *   <fator>     ::= numero | identificador | '(' <expressao> ')'
 *
 * Compilar: gcc -Wall -Wno-unused-result -g -Og precompilador.c -o precompilador
 */

/* ===================================================================
 * PARTE 1: miniLex.h
 * =================================================================== */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> // atof

// código interno atomo NUMERO
typedef enum{
    ERRO,          // sempre na posicao 0

    // palavras reservadas
    ALGORITMO,     // algoritmo
    CARACTERE,     // caractere
    DIV,           // div
    E,             // e
    ENQUANTO,      // enquanto
    ENTAO,         // entao
    ESCREVA,       // escreva
    FACA,          // faca
    FALSO,         // falso
    FIM,           // fim
    FUNCAO,        // funcao
    INICIO,        // inicio
    INTEIRO,       // inteiro
    LEIA,          // leia
    LOGICO,        // logico
    MOD,           // mod
    NAO,           // nao
    OU,            // ou
    PROCEDIMENTO,  // procedimento
    SE,            // se
    SENAO,         // senao
    VAR,           // var
    VERDADEIRO,    // verdadeiro

    // simbolos
    PONTO_VIRGULA, // ;
    VIRGULA,       // ,
    PONTO,         // .
    DOIS_PONTOS,   // :
    ATRIBUICAO,    // :=
    ABRE_PAR,      // (
    FECHA_PAR,     // )
    SOMA,          // +
    SUB,           // -
    MULT,          // *
    IGUAL,         // =
    DIFERENTE,     // <>
    MENOR,         // <
    MENOR_IGUAL,   // <=
    MAIOR,         // >
    MAIOR_IGUAL,   // >=

    // identificador, constantes e especiais
    IDENTIFICADOR, // ex.: num_1
    CONSTINT,      // ex.: 12, 12E2
    CONSTCHAR,     // ex.: 'a'
    COMENTARIO,    // {- ... -}
    EOS,           // fim do buffer

}TAtomo;

// Estrutura para comunicar com o analisador sintatico
typedef struct{
   TAtomo atomo;
   int linha;
    union{
        int numero;   // atributo do átomo constint
        char id[16];  // atributo identificador
        char ch;      // atributo do átomo constchar
    }atributo;

}TInfoAtomo;

// declaracao de funcao
void reconhece_constint(TInfoAtomo *info_atomo);
void reconhece_id(TInfoAtomo *info_atomo);
void reconhece_constchar(TInfoAtomo *info_atomo);
TInfoAtomo  obter_atomo(void);
void reconhece_comentario(TInfoAtomo *info_atomo);

/* ===================================================================
 * PARTE 2: miniLex.c (analisador lexico)
 * =================================================================== */

// Nome de cada atomo na saida. MESMA ORDEM do enum TAtomo
char *strAtomo[] = {
    "erro",          // ERRO

    // palavras reservadas
    "algoritmo",     // ALGORITMO
    "caractere",     // CARACTERE
    "div",           // DIV
    "e",             // E
    "enquanto",      // ENQUANTO
    "entao",         // ENTAO
    "escreva",       // ESCREVA
    "faca",          // FACA
    "falso",         // FALSO
    "fim",           // FIM
    "funcao",        // FUNCAO
    "inicio",        // INICIO
    "inteiro",       // INTEIRO
    "leia",          // LEIA
    "logico",        // LOGICO
    "mod",           // MOD
    "nao",           // NAO
    "ou",            // OU
    "procedimento",  // PROCEDIMENTO
    "se",            // SE
    "senao",         // SENAO
    "var",           // VAR
    "verdadeiro",    // VERDADEIRO

    // simbolos
    "ponto_virgula", // PONTO_VIRGULA
    "virgula",       // VIRGULA
    "ponto",         // PONTO
    "dois_pontos",   // DOIS_PONTOS
    "atribuicao",    // ATRIBUICAO
    "abre_par",      // ABRE_PAR
    "fecha_par",     // FECHA_PAR
    "mais",          // SOMA
    "menos",         // SUB
    "vezes",         // MULT
    "igual",         // IGUAL
    "diferente",     // DIFERENTE
    "menor",         // MENOR
    "menor_igual",   // MENOR_IGUAL
    "maior",         // MAIOR
    "maior_igual",   // MAIOR_IGUAL

    // identificador, constantes e especiais
    "identificador", // IDENTIFICADOR
    "constint",      // CONSTINT
    "constchar",     // CONSTCHAR
    "comentario",    // COMENTARIO
    "fim_de_arquivo",// EOS

};

// NOVO: tabela de palavras reservadas
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
    {"se",           SE},
    {"senao",        SENAO},
    {"var",          VAR},
    {"verdadeiro",   VERDADEIRO},
};

// variavel global
// ALTERADO: buffer com uma expressao no formato do ASDR3
// (numeros com ponto e ids minusculos, que e o que o miniLex reconhece)
char *buffer ="a + 1.5 *\n(b - 3.0)";
char lexema[20];
int contaLinha = 1;

TInfoAtomo  obter_atomo(void){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;
    // elimina espacos, faz a contagem de linhas 
    while( *buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r' ){
        if( *buffer == '\n' ){
            contaLinha++;
        }
        buffer++;
    }

    //fica aqui para contar uma linha msm com comentario
    info_atomo.linha = contaLinha;

    if(*buffer == 0) // final de buffer
        info_atomo.atomo = EOS;
    else if( isdigit(*buffer)) // reconhece numero
        reconhece_constint(&info_atomo);
    else if(isalpha(*buffer)) // reconhece id
        reconhece_id(&info_atomo);
    else if(*buffer == '*'){
        info_atomo.atomo = MULT;
        buffer++;
    }
    else if(*buffer == '+'){
        info_atomo.atomo = SOMA;
        buffer++;
    }
    else if(*buffer == '-'){ // ADICIONADO: copia do bloco do '+'
        info_atomo.atomo = SUB;
        buffer++;
    }
    else if(*buffer == '('){ // ADICIONADO: copia do bloco do '+'
        info_atomo.atomo = ABRE_PAR;
        buffer++;
    }
    else if(*buffer == ')'){ // ADICIONADO: copia do bloco do '+'
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

    else if( *buffer == ':' ){        // ← novo: autômato do : e :=
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

void reconhece_comentario(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;

    // q0: tem que ser '{'
    if( *buffer == '{' ){
        buffer++;
        goto q1;
    }
    return;

q1: // depois do '{' tem que vir '-'
    if( *buffer == '-' ){
        buffer++;
        goto q2;
    }
    return;          // '{' sozinho: ERRO

q2:
    if( *buffer == '\0' ){
        return;              // acabou o texto sem fechar: ERRO
    }
    if( *buffer == '-' ){
        buffer++;
        goto q3;             // pode ser o começo do '-}'
    }
    if( *buffer == '\n' ){
        contaLinha++;        // conta a linha aqui dentro
    }
    buffer++;                // qualquer outro caractere: ignora
    goto q2;

q3:
    if( *buffer == '}' ){
        buffer++;
        info_atomo->atomo = COMENTARIO;   // fechou: deu certo
        return;
    }
    goto q2;                 // não fechou: volta a procurar
}
    
/*
funcao implementa o automato do constchar
constchar -> ' qualquer_caractere '
q0 --'--> q1 --qualquer (menos \0)--> q2 --'--> q3 (final)
*/
void reconhece_constchar(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;

    // q0: apostrofo de abertura
    if( *buffer == '\'' ){
        buffer++;
        goto q1;
    }
    return;

q1: // caractere do meio: qualquer um, menos o fim do texto
    if( *buffer != '\0' ){
        info_atomo->atributo.ch = *buffer;
        buffer++;
        goto q2;
    }
    return;

q2: // apostrofo de fechamento
    if( *buffer == '\'' ){
        buffer++;
        info_atomo->atomo = CONSTCHAR;
        return;
    }
    return;
}

/*
funcao implementa o automato para a expressao regular
DIGITO -> 0|1|...|9 
NUMERO -> DIGITO+.DIGITO+
*/
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
        return;          // 12E2x: letra colada → ERRO
    goto final;          // 12E2; → aceita

    // recorta lexema
    final:
    if( buffer - ini_lexema > 19 ){
      return;      // número grande demais: ERRO 
    }

    strncpy(lexema,ini_lexema,buffer-ini_lexema);
    lexema[buffer-ini_lexema] = '\0';
    info_atomo->atomo = CONSTINT;
    info_atomo->atributo.numero = atof(lexema); // o que essa linha faz plmds explixa direito seu fdp para de explicar como sae eu soubbesse as coisas

    return ;
}
// IDENTIFICADOR -> LETRA_MINUSCULA(LETRA_MINUSCULA|DIGITO)
// LETRA_MINUSCULA -> a|b|...|z
// LETRA_MAIUSCULA -> A|B|...|Z
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
    // recorta lexema 
    if( buffer - ini_lexema > 15 ){
        return;
       }   // identificador grande demais: ERRO
    strncpy(info_atomo->atributo.id,ini_lexema,buffer-ini_lexema);
    info_atomo->atributo.id[buffer-ini_lexema] = '\0';
    info_atomo->atomo = IDENTIFICADOR;

    // NOVO: se o lexema estiver na tabela, troca o átomo
    for (int i = 0; i < NUM_RESERVADAS; i++) {
        if (strcasecmp(info_atomo->atributo.id, reservadas[i].lexema) == 0) {
            info_atomo->atomo = reservadas[i].atomo;
            break;
        }
    }

    return;
}

/* ===================================================================
 * PARTE 3: miniSintatico.c + ASDR3.c (analisador sintatico)
 * =================================================================== */

// variavel global do analisador sintatico
TAtomo lookahead;       // ALTERADO: no ASDR3 era char, aqui e TAtomo
TInfoAtomo info_atomo;

//<expressao>::= <termo> {('+'|'-') <termo>}
//<termo>::=<fator> {('*'|'/') <fator>}
//<fator>::='a'|'b'|'c'|...|'1'|'2'|'3'|...|'('<expressão>')'
// prototipacao de funcao
void expressao(); 
void termo();
void fator();
void consome( TAtomo atomo );

int main(){
    // primeira chamada para inicializar o lookahead
    info_atomo = obter_atomo();
    lookahead = info_atomo.atomo;
    
    expressao(); // chama o simbolo inicial da gramatica // ALTERADO: era E()

    consome(EOS);

    printf("Compilador\nAnalise sintatica concluida sem erros\n");

    return 0;
}
void consome( TAtomo atomo ){
    if( lookahead == atomo ){
        info_atomo = obter_atomo();
        lookahead = info_atomo.atomo;
    }
    else{
        // tratador de erros
        printf("Erro sintatico: esperado [%s] encontrado [%s]\n",strAtomo[atomo],strAtomo[lookahead]);
        exit(1);
    }
}

//<expressao>::= <termo> {('+'|'-') <termo>}
void expressao(){
    termo();
    while(lookahead == SOMA || lookahead == SUB){ // ALTERADO: '+' e '-' viraram SOMA e SUB
        consome(lookahead);
        termo();
    }
}
//<termo>::=<fator> {('*'|'/') <fator>}
void termo(){
    fator();
    while(lookahead == MULT || lookahead == BARRA){ // ALTERADO: '*' e '/' viraram MULT e BARRA
        consome(lookahead);
        fator();
    }
}
//<fator>::='a'|'b'|'c'|...|'1'|'2'|'3'|...|'('<expressão>')'
void fator(){
    if(lookahead == NUMERO){               // ALTERADO: era isdigit(lookahead)
        consome(lookahead);
    }
    else if(lookahead == IDENTIFICADOR){   // ALTERADO: era isalpha(lookahead)
        consome(lookahead);
    }
    else{
        consome(ABRE_PAR);                 // ALTERADO: era consome('(')
        expressao();
        consome(FECHA_PAR);                // ALTERADO: era consome(')')
    }
}