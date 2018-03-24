/*
 * *****************************************************************************
 *
 * Copyright 2018 Gavin D. Howard
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * *****************************************************************************
 *
 * The lexer.
 *
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <status.h>
#include <lex.h>
#include <bc.h>

BcStatus bc_lex_whitespace(BcLex *lex, BcLexToken *token) {

  char c;

  token->type = BC_LEX_WHITESPACE;
  c = lex->buffer[lex->idx];

  while ((isspace(c) && c != '\n') || c == '\\') {
    ++lex->idx;
    c = lex->buffer[lex->idx];
  }

  return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_string(BcLex *lex, BcLexToken *token) {

  const char *start;
  size_t newlines, len, i, j;
  char c;

  newlines = 0;
  token->type = BC_LEX_STRING;
  i = lex->idx;
  c = lex->buffer[i];

  while (c != '"' && c != '\0') {
    if (c == '\n') ++newlines;
    c = lex->buffer[++i];
  }

  if (c == '\0') {
    lex->idx = i;
    return BC_STATUS_LEX_NO_STRING_END;
  }

  len = i - lex->idx;

  token->string = malloc(len + 1);
  if (!token->string) return BC_STATUS_MALLOC_FAIL;

  start = lex->buffer + lex->idx;

  for (j = 0; j < len; ++j) token->string[j] = start[j];

  token->string[len] = '\0';

  lex->idx = i + 1;
  lex->line += newlines;

  return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_comment(BcLex *lex, BcLexToken *token) {

  size_t newlines, i;
  const char *buffer;
  char c;
  bool end;

  newlines = 0;
  token->type = BC_LEX_WHITESPACE;
  end = false;

  ++lex->idx;
  i = lex->idx;
  buffer = lex->buffer;

  while (!end) {

    c = buffer[i];

    while (c != '*' && c != '\0') {
      if (c == '\n') ++newlines;
      c = buffer[++i];
    }

    if (c == '\0' || buffer[i + 1] == '\0') {
      lex->idx = i;
      return BC_STATUS_LEX_NO_COMMENT_END;
    }

    end = buffer[i + 1] == '/';
    i += end ? 0 : 1;
  }

  lex->idx = i + 2;
  lex->line += newlines;

  return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_number(BcLex *lex, BcLexToken *token, char start) {

  const char *buffer, *buf;
  size_t backslashes, len, hits, i, j;
  char c;
  bool point;

  token->type = BC_LEX_NUMBER;
  point = start == '.';
  buffer = lex->buffer + lex->idx;
  backslashes = 0;
  i = 0;
  c = buffer[i];

  while (c && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
               (c == '.' && !point) || (c == '\\' && buffer[i + 1] == '\n')))
  {
    if (c == '\\') {
      ++i;
      backslashes += 1;
    }

    c = buffer[++i];
  }

  len = i + 1;

  token->string = malloc(len - backslashes * 2 + 1);
  if (!token->string) return BC_STATUS_MALLOC_FAIL;

  token->string[0] = start;

  buf = buffer - 1;
  hits = 0;

  for (j = 1; j < len; ++j) {

    c = buf[j];

    // If we have hit a backslash, skip it.
    // We don't have to check for a newline
    // because it's guaranteed.
    if (hits < backslashes && c == '\\') {
      ++hits;
      ++j;
      continue;
    }

    token->string[j - (hits * 2)] = c;
  }

  token->string[j - (hits * 2)] = '\0';
  lex->idx += i;

  return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_name(BcLex *lex, BcLexToken *token) {

  BcStatus status;
  const char *buffer;
  size_t i;
  char c;

  buffer = lex->buffer + lex->idx - 1;

  for (i = 0; i < sizeof(bc_lex_keywords) / sizeof(bc_lex_keywords[0]); ++i) {

    if (!strncmp(buffer, bc_lex_keywords[i].name, bc_lex_keywords[i].len)) {

      token->type = BC_LEX_KEY_AUTO + i;

      if (!bc_lex_keywords[i].posix &&
          (status = bc_posix_error(BC_STATUS_POSIX_BAD_KEYWORD,
                                   lex->file, lex->line,
                                   bc_lex_keywords[i].name)))
      {
        return status;
      }

      // We need to minus one because the
      // index has already been incremented.
      lex->idx += bc_lex_keywords[i].len - 1;

      return BC_STATUS_SUCCESS;
    }
  }

  token->type = BC_LEX_NAME;

  i = 0;
  c = buffer[i];

  while ((c >= 'a' && c<= 'z') || (c >= '0' && c <= '9') || c == '_')
    c = buffer[++i];

  if (i > 1 && (status = bc_posix_error(BC_STATUS_POSIX_NAME_LEN,
                                        lex->file, lex->line, buffer)))
  {
    return status;
  }

  token->string = malloc(i + 1);
  if (!token->string) return BC_STATUS_MALLOC_FAIL;

  strncpy(token->string, buffer, i);
  token->string[i] = '\0';

  // Increment the index. It is minus one
  // because it has already been incremented.
  lex->idx += i - 1;

  return BC_STATUS_SUCCESS;
}

BcStatus bc_lex_token(BcLex *lex, BcLexToken *token) {

  BcStatus status;
  char c, c2;

  status = BC_STATUS_SUCCESS;

  c = lex->buffer[lex->idx++];

  // This is the workhorse of the lexer.
  switch (c) {

    case '\0':
    {
      token->type = BC_LEX_EOF;
      break;
    }

    case '\t':
    {
      status = bc_lex_whitespace(lex, token);
      break;
    }

    case '\n':
    {
      lex->newline = true;
      token->type = BC_LEX_NEWLINE;
      break;
    }

    case '\v':
    case '\f':
    case '\r':
    case ' ':
    {
      status = bc_lex_whitespace(lex, token);
      break;
    }

    case '!':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_REL_NOT_EQ;
      }
      else {

        if ((status = bc_posix_error(BC_STATUS_POSIX_BOOL_OPS,
                                     lex->file, lex->line, "!")))
        {
          return status;
        }

        token->type = BC_LEX_OP_BOOL_NOT;
      }

      break;
    }

    case '"':
    {
      status = bc_lex_string(lex, token);
      break;
    }

    case '#':
    {
      if ((status = bc_posix_error(BC_STATUS_POSIX_SCRIPT_COMMENT,
                                  lex->file, lex->line, NULL)))
      {
        return status;
      }

      token->type = BC_LEX_WHITESPACE;

      ++lex->idx;
      while (lex->idx < lex->len && lex->buffer[lex->idx] != '\n') ++lex->idx;

      break;
    }

    case '%':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_MODULUS;
      }
      else token->type = BC_LEX_OP_MODULUS;

      break;
    }

    case '&':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '&') {

        if ((status = bc_posix_error(BC_STATUS_POSIX_BOOL_OPS,
                                     lex->file, lex->line, "&&")))
        {
          return status;
        }

        ++lex->idx;
        token->type = BC_LEX_OP_BOOL_AND;
      }
      else {
        token->type = BC_LEX_INVALID;
        status = BC_STATUS_LEX_BAD_CHARACTER;
      }

      break;
    }

    case '(':
    {
      token->type = BC_LEX_LEFT_PAREN;
      break;
    }

    case ')':
    {
      token->type = BC_LEX_RIGHT_PAREN;
      break;
    }

    case '*':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_MULTIPLY;
      }
      else token->type = BC_LEX_OP_MULTIPLY;

      break;
    }

    case '+':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_PLUS;
      }
      else if (c2 == '+') {
        ++lex->idx;
        token->type = BC_LEX_OP_INC;
      }
      else token->type = BC_LEX_OP_PLUS;

      break;
    }

    case ',':
    {
      token->type = BC_LEX_COMMA;
      break;
    }

    case '-':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_MINUS;
      }
      else if (c2 == '-') {
        ++lex->idx;
        token->type = BC_LEX_OP_DEC;
      }
      else token->type = BC_LEX_OP_MINUS;

      break;
    }

    case '.':
    {
      c2 = lex->buffer[lex->idx];

      if (isdigit(c2)) {
        status = bc_lex_number(lex, token, c);
      }
      else {

        status = bc_posix_error(BC_STATUS_POSIX_DOT_LAST,
                                lex->file, lex->line, NULL);

        token->type = BC_LEX_KEY_LAST;
      }

      break;
    }

    case '/':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_DIVIDE;
      }
      else if (c2 == '*') status = bc_lex_comment(lex, token);
      else token->type = BC_LEX_OP_DIVIDE;

      break;
    }

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      status = bc_lex_number(lex, token, c);
      break;
    }

    case ';':
    {
      token->type = BC_LEX_SEMICOLON;
      break;
    }

    case '<':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_REL_LESS_EQ;
      }
      else token->type = BC_LEX_OP_REL_LESS;

      break;
    }

    case '=':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_REL_EQUAL;
      }
      else token->type = BC_LEX_OP_ASSIGN;

      break;
    }

    case '>':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_REL_GREATER_EQ;
      }
      else token->type = BC_LEX_OP_REL_GREATER;

      break;
    }

    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    {
      status = bc_lex_number(lex, token, c);
      break;
    }

    case '[':
    {
      token->type = BC_LEX_LEFT_BRACKET;
      break;
    }

    case '\\':
    {
      status = bc_lex_whitespace(lex, token);
      break;
    }

    case ']':
    {
      token->type = BC_LEX_RIGHT_BRACKET;
      break;
    }

    case '^':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '=') {
        ++lex->idx;
        token->type = BC_LEX_OP_ASSIGN_POWER;
      }
      else token->type = BC_LEX_OP_POWER;

      break;
    }

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    {
      status = bc_lex_name(lex, token);
      break;
    }

    case '{':
    {
      token->type = BC_LEX_LEFT_BRACE;
      break;
    }

    case '|':
    {
      c2 = lex->buffer[lex->idx];

      if (c2 == '|') {

        if ((status = bc_posix_error(BC_STATUS_POSIX_BOOL_OPS,
                                     lex->file, lex->line, "||")))
        {
          return status;
        }

        ++lex->idx;
        token->type = BC_LEX_OP_BOOL_OR;
      }
      else {
        token->type = BC_LEX_INVALID;
        status = BC_STATUS_LEX_BAD_CHARACTER;
      }

      break;
    }

    case '}':
    {
      token->type = BC_LEX_RIGHT_BRACE;
      break;
    }

    default:
    {
      token->type = BC_LEX_INVALID;
      status = BC_STATUS_LEX_BAD_CHARACTER;
      break;
    }
  }

  return status;
}

void bc_lex_init(BcLex *lex, const char *file) {
  assert(lex && file);
  lex->line = 1;
  lex->newline = false;
  lex->file = file;
}

BcStatus bc_lex_next(BcLex *lex, BcLexToken *token) {

  BcStatus status;

  assert(lex && token);

  if (lex->idx == lex->len) {
    token->type = BC_LEX_EOF;
    return BC_STATUS_LEX_EOF;
  }

  if (lex->newline) {
    ++lex->line;
    lex->newline = false;
  }

  // Loop until failure or we don't have whitespace. This
  // is so the parser doesn't get inundated with whitespace.
  do {
    token->string = NULL;
    status = bc_lex_token(lex, token);
  } while (!status && token->type == BC_LEX_WHITESPACE);

  return status;
}

BcStatus bc_lex_text(BcLex *lex, const char *text, BcLexToken *token) {

  assert(lex && text && token);

  lex->buffer = text;
  lex->idx = 0;
  lex->len = strlen(text);

  return bc_lex_next(lex, token);
}