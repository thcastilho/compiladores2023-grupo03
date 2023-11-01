# myBC - my Basic Calculator
## 1º Trabalho da Disciplina de Compiladores, 6º semestre do curso de Ciências da Computação da Unesp de Rio Claro
## Grupo: João Mantilha, Pedro Bruce e Thiago César 

### Versões
O projeto myBC foi composto de várias versões construídas durante as aulas de laboratório.
- v0: versão inicial do projeto - fornecida pelo próprio professor;
- v1: alteração do **parser.c**, trocando as estruturas *if else* dos símbolos não terminais R, Q e F por estruturas *switch case*. O arquivo **main.c** foi alterado para permitir a leitura de um arquivo externo ao programa; 
- v2: o **lexer.c** identifica, além de identificadores e números inteiros, números octais, hexadecimais e ponto flutuante - adicionadas as funções *isOCT*, *isHEX*, *isEE*, *isFLT* e *isNUM*. Com isso, o  símbolo não terminal F, no **parser.c**, teve a estrutura *switch case* alterada;
- v3: a função *skipspaces* foi modificada para *skipSpacesAndComments*, agora com a possibilidade de pular comentários na sintaxe do Pascal (iniciando com '{'e finalizando com '}') e a gramática do **parser.c** foi passada para a forma EBNF. Além disso, a função *match* agora identifica um possível fim de arquivo prematuro e a linha que ele ocorre;
- v4: implementação inicial do vetor de caracteres *lexeme* que, conforme a varredura acontece no **lexer.c**, armazena os caracteres de entrada;
- v5: implementação do diagrama sintático incompleto no **parser.c**;
- v6: correção da função *isHEX* (na linha 141, o laço esperava um *digit* ao invés de um *xdigit*), implementação do diagrama sintático completo no **parser.c**;
- v7: correção do vetor de caracteres *lexeme* e implementação da notação posfixa no **parser.c** com a gramática LL(1) de expressões e ações semânticas com *printf()*;
- v8: implementação da notação posfixa no **parser.c** com a gramática EBNF de expressões e ações semânticas com *printf()*;
- v9: implementação da notação posfixa no **parser.c** com a gramática EBNF de expressões e diagrama sintático incompleto e ações semânticas com *printf()*;
- v10: implementação de operações de pilha no **parser.c** sendo representadas por ações semânticas com *printf()*, com a gramática LL(1) de expressões;
- v11: implementação de operações de pilha no **parser.c** sendo representadas por ações semânticas com *printf()*, com a gramática EBNF de expressões;
- v12: implementação de operações de pilha no **parser.c** sendo representadas por ações semânticas com *printf()*, com a gramática EBNF de expressões e diagrama sintático incompleto;
- v13: versão final do trabalho.
