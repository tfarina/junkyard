#ifndef TOKEN_H_
#define TOKEN_H_

typedef enum token_kind_e {
  TOK_EOF = 0,

  TOK_LBRACKET = '[',
  TOK_RBRACKET = ']',
  TOK_LBRACE   = '{',
  TOK_RBRACE   = '}',

  TOK_END
} token_kind_t;

#endif  /* TOKEN_H_ */
