#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum
{
    TK_REVERSED, //記号
    TK_EOF,      //整数トークン
    TK_NUM,      //入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

//トークン型
struct Token
{
    TokenKind kind; //トークンの型
    Token *next;    //次の入力トークン
    int val;        //kindがTK_NUMの場合、その数値
    char *str;      //トークン文字列
};
//現在注目しているトークン
Token *token;

//エラーを報告するための関数
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
//次のトークンが期待している記号の時には、トークンを一つ読み進めて
//真を返す。それ以外の時には偽を返す
bool consume(char op)
{
    if (token->kind != TK_REVERSED || token->str[0] != op)
    {
        error("'%c'ではありません", op);
    }
    token = token->next;
}

//次のトークンが数値の時、トークンを一つ読み進めてその数値を返す
//それ以外の場合にはエラーを報告する
int expect_number()
{
    if (token->kind != TK_NUM)
    {
        error("数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

//新しいトークンを作成してCURにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

//入力文字列pをトーク内ずしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        //空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }
        if (*p == '+' || *p == '-')
        {
            cur = new_token(TK_REVERSED, cur, p++);
            continue;
        }
        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        printf("トークナイズできません\n");
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません");
        return 1;
    }
    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof())
    {
        if (consume('+'))
        {
            printf("    add rax, %d\n", expect_number());
            continue;
        }
        if (consume('-'))
        {
            printf("    sub rax, %d\n", expect_number());
            continue;
        }
    }
    printf("    ret\n");
    return 0;
}