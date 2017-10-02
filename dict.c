#include <stdlib.h>
#include <string.h>
#include "dict.h"

dict_t *dict_new(void) {
	dict_t *d = malloc(sizeof(dict_t));
	d->data = NULL;
}

void dict_set(dict_t *d, char *key, void *value) {

	if (d->data == NULL) {
		dict_entry *entry = malloc(sizeof(dict_entry));
		entry->key = key;
		entry->value = value;
		entry->next = NULL;
		d->data = entry;
		return;
	}

	dict_entry *prev, *item = d->data;
	while (item != NULL) {
		prev = item;
		if (strcmp((char *)item->key, key) == 0) {
			item->value = value;
			return;
		}
		item = item->next;
	}

	dict_entry *entry = malloc(sizeof(dict_entry));
	entry->key = key;
	entry->value = value;
	entry->next = NULL;
	prev->next = entry;
}

void *dict_get(dict_t *d, char *key) {
	dict_entry *item = d->data;
	while (item != NULL) {
		if (strcmp((char *)item->key, key) == 0) {
			return item->value;
		}
		item = item->next;
	}

	return NULL;
}

void dict_release(dict_t *d) {
	dict_entry *item = d->data;
	dict_entry *next;
	while (next != NULL) {
		next = item->next;
		free(item->key);
		free(item->value);
		free(item);
		item = next;
	}
	free(d);
}
