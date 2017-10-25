
typedef struct dict_entry {
	char *key;
	char *value;
	struct dict_entry *next;
} dict_entry;

typedef struct dict_t {
	dict_entry *data;

} dict_t;

dict_t *dict_new(void);
int dict_set(dict_t *d,const char *key, const char *value);
const char *dict_get(dict_t *d, const char *key);
void dict_release(dict_t *d);