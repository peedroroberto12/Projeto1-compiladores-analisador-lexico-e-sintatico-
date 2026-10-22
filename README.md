# Projeto 1 - Compilador: Análise Léxica e Sintática

[![C](https://img.shields.io/badge/Linguagem-C-00599C?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Status](https://img.shields.io/badge/Status-Conclu%C3%ADdo-success?style=for-the-badge)](https://github.com/peedroroberto12/Projeto1-compiladores-analisador-lexico-e-sintatico-)

Este projeto consiste na implementação de um compilador em linguagem C, com foco na fase inicial da compilação: análise léxica e análise sintática. O objetivo foi desenvolver um sistema capaz de ler um arquivo de entrada, tokenizar o código e validar sua estrutura sintática, demonstrando os conceitos fundamentais de compiladores.

## Visão Geral

A aplicação realiza a leitura de um arquivo de entrada, identifica os tokens do código fonte e verifica se a sequência de símbolos é válida de acordo com a gramática definida. O projeto foi desenvolvido com base em estruturas e implementações inspiradas em exemplos clássicos de compiladores, incluindo materiais e referências da disciplina.

## Funcionalidades

- Análise léxica
- Reconhecimento de tokens
- Análise sintática
- Validação da estrutura do código de entrada
- Tratamento de erros básicos de compilação
- Geração de saída textual do processamento

## Tecnologias e Linguagem

- Linguagem: C
- Ambiente de desenvolvimento: GCC
- Estrutura de compilação: console/terminal

## Estrutura do Projeto

- `compilador.c` — implementação principal do compilador
- `README.md` — documentação do projeto

## Como Compilar

No diretório do projeto, execute:

```bash
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
