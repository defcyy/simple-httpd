
typedef struct dict_entry {
	void *key;
	void *value;
	struct dict_entry *next;
} dict_entry;

typedef struct dict_t {
	dict_entry *data;

} dict_t;

dict_t *dict_new(void);
void dict_set(dict_t *d, char *key, void *value);
void *dict_get(dict_t *d, char *key);
void dict_release(dict_t *d);