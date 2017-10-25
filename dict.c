#include <stdlib.h>
#include <string.h>
#include "dict.h"

dict_t *dict_new(void) {
	dict_t *d = calloc(1, sizeof(dict_t));
	d->data = NULL;
}

int dict_set(dict_t *d, const char *key, const char *value) {

	if (key == NULL) {
		return -1;
	}

	dict_entry *prev, *item = d->data;
	while (item != NULL) {
		prev = item;
		if (strcmp(item->key, key) == 0) {
			strcpy(item->value, value);
			return 0;
		}
		item = item->next;
	}

	dict_entry *entry = calloc(1, sizeof(dict_entry));
	entry->key = calloc(strlen(key) + 1, sizeof(char));
	strcpy(entry->key, key);
	entry->value = calloc(strlen(value) + 1, sizeof(char));
	strcpy(entry->value, value);
	entry->next = NULL;

	if (d->data == NULL) {
		d->data = entry;
	} else {
		prev->next = entry;
	}

	return 0;
}

const char *dict_get(dict_t *d, const char *key) {
	dict_entry *item = d->data;
	while (item != NULL) {
		if (strcmp(item->key, key) == 0) {
			return item->value;
		}
		item = item->next;
	}

	return NULL;
}

void dict_release(dict_t *d) {
	dict_entry *item = d->data;
	dict_entry *next;
	while (item != NULL) {
		next = item->next;
		free(item->key);
		free(item->value);
		free(item);
		item = next;
	}
	free(d);
}
