#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

typedef long long integer_t;

typedef struct Error {
  enum ErrorType {
    ERROR_NONE = 0,
    ERROR_ARGUMENTS,
    ERROR_TYPE,
    ERROR_GENERIC,
    ERROR_SYNTAX,
    ERROR_TODO,
    ERROR_MAX,
  } type;
  char* msg;
} Error;

typedef struct Node {
  enum NodeType {
    NODE_TYPE_NONE,
    NODE_TYPE_INTEGER,
    NODE_TYPE_MAX,
  } type;
  union NodeValue {
    integer_t integer;
  } value;

  struct Node* children[3];
} Node;

// TODO: API to create new Binding. API to add Binding to environment.
typedef struct Binding {
  char* id;
  Node* value;
  struct Binding* next;
} Binding;

// TODO: API to create new Environment.
typedef struct Environment {
  struct Environment* parent;
  Binding* bind;
} Environment;

Error ok = { ERROR_NONE, NULL };
const char* whitespace = " \r\n";
const char* delimiters = " \r\n,():";

#define ERROR_CREATE(n, t, msg)     \
   Error (n) = { (t), (msg) }
#define ERROR_PREP(n, t, message)   \
  (n).type = (t);                   \
  (n).msg = (message);              \

long file_size(FILE* file) {
  if (!file) { return 0; }
  fpos_t original = {0};
  if (fgetpos(file, &original) != 0) {
    printf("fgetpos() failed: %i\n", errno);
  }
  fseek(file, 0, SEEK_END);
  long out = ftell(file);
  if (fsetpos(file, &original) != 0) {
    printf("fsetpos() failed: %i\n", errno);
  }
  return out;
}

char* file_contents(char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
    printf("Could no open file at %s\n", path);
    return NULL;
  }
  long size = file_size(file);
  char* contents = malloc(size + 1);
  char* write_it = contents;
  size_t bytes_read = 0;
  while (bytes_read < size) {
    size_t bytes_read_this_iteration = fread(write_it, 1, size - bytes_read, file);
    if (ferror(file)) {
      printf("Error while reading: %i\n", errno);
      free(contents);
      return NULL;
    }
    
    bytes_read += bytes_read_this_iteration;
    write_it += bytes_read_this_iteration;

    if (feof(file)) { break; }
  }
  contents[bytes_read] = '\0';
  return contents;
}

void print_error(Error err) {
  if (err.type == ERROR_NONE) { return; }
  printf("ERROR: ");
  assert(ERROR_MAX == 6);
  switch (err.type) {
  default:
    printf("Unkown error type...");
  case ERROR_TODO:
    printf("TODO (not implemented)");
    break;
  case ERROR_SYNTAX:
    printf("Invalid syntax");
    break;
  case ERROR_TYPE:
    printf("Mismatched types");
    break;
  case ERROR_ARGUMENTS:
    printf("Invalid arguments");
    break;
  case ERROR_GENERIC:
    break;
  case ERROR_NONE:
    break;
  }
  putchar('\n');
  if (err.msg) {
    printf(": %s\n", err.msg);
  }
}

/// Lex the next token for SOURCE, and point to it with DEG and END.
Error lex(char* source, char** beg, char** end) {
  Error err = ok;
  if (!source || !beg || !end) {
    ERROR_PREP(err, ERROR_ARGUMENTS, "Can not lex empty source.");
    return err;
  }
  
  *beg = source;
  *beg += strspn(*beg, whitespace);
  *end = *beg;
  if (**end == '\0') { return err; }
  *end += strcspn(*beg, delimiters);
  if (*end == *beg) {
    *end += 1;
  }
  return err;
}

void environment_set() {
  
}

Error parse_expr(char* source) {
  char* beg = source;
  char* end = source;
  Error err = ok;
  while ((&err == lex(end, &beg, &end).type == ERROR_NONE)) {
    if (end - beg == 0) { break; }
    //printf("lexed: %.*s\n", end - beg, beg);
  }
  return err;
}

void print_usage(char** argv) {
  printf("USAGE: %s <path_to_file_to_compile>\n", argv[0]);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    print_usage(argv);
    exit(0);
  }

  char* path = argv[1];
  char* contents = file_contents(path);
  if (contents) {
    // printf("Contents of %s:\n---\n\"%s\"\n---\n", path, contents);
    
    Error err = parse_expr(contents);
    print_error(err);

    free(contents);
  }

  return 0;
}
