#include <stdlib.h>
#include <string.h>
#include <cf_alloc.h>

#include "as_bytes.h"

extern inline uint8_t * as_bytes_tobytes(const as_bytes * s);
extern inline as_val * as_bytes_toval(const as_bytes * s);
extern inline as_bytes * as_bytes_fromval(const as_val * v);

/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

as_bytes * as_bytes_init(as_bytes * v, uint8_t * s, size_t len, bool is_malloc) {
    as_val_init(&v->_, AS_BYTES, false /*is_malloc*/);
    v->value_is_malloc = is_malloc;
    v->value = s;
    v->len = len;
    v->capacity = len;
    return v;
}

as_bytes * as_bytes_empty_init(as_bytes * v, size_t len) {
    as_val_init(&v->_, AS_BYTES, false /*is_malloc*/);
    v->value_is_malloc = true;
    v->value = malloc(len);
    memset(v->value, 0, len);
    v->len = len;
    v->capacity = len;
    return v;
}

as_bytes * as_bytes_new(uint8_t * s, size_t len, bool is_malloc) {
    as_bytes * v = (as_bytes *) malloc(sizeof(as_bytes));
    as_val_init(&v->_, AS_BYTES, true /*is_malloc*/);
    v->value_is_malloc = is_malloc;
    v->value = s;
    v->len = len;
    v->capacity = len;
    return v;
}

as_bytes * as_bytes_empty_new(size_t len) {
    as_bytes * v = (as_bytes *) malloc(sizeof(as_bytes));
    as_val_init(&v->_, AS_BYTES, true /*is_malloc*/);
    v->value_is_malloc = true;
    v->value = malloc(len);
    memset(v->value, 0, len);
    v->len = len;
    v->capacity = len;
    return v;
}

void as_bytes_destroy(as_bytes * s) {
	as_val_val_destroy( (as_val *)s );
}

void as_bytes_val_destroy(as_val * v) {
	as_bytes *s = (as_bytes *) v;
	if ( s->value_is_malloc && s->value ) free(s->value);
}

size_t as_bytes_len(as_bytes * s) {
	return(s->len);
}

int as_bytes_get(const as_bytes * s, int index, uint8_t *buf, int buf_len) {
    if (index + buf_len > s->len) return(-1);
    memcpy(buf, &s->value[index], buf_len);
    return(0);
}

int as_bytes_set(as_bytes * s, int index, const uint8_t *buf, int buf_len) {
    if (index + buf_len > s->len) return(-1);
    memcpy(&s->value[index], buf, buf_len);
    return(0);
}

// create a new as_bytes, a substring of the source
as_bytes *as_bytes_slice_new(const as_bytes *src, int start_index, int end_index) {
    int len = end_index - start_index;
    as_bytes * v = (as_bytes *) malloc(sizeof(as_bytes));
    as_val_init(&v->_, AS_BYTES, true /*is_malloc*/);
    v->value_is_malloc = true;
    v->value = malloc(len);
    memcpy(v->value, &src->value[start_index], len);
    v->len = len;
    v->capacity = len;
    return(v);
}

// create a new as_bytes, a substring of the source
as_bytes *as_bytes_slice_init(as_bytes *v, const as_bytes *src, int start_index, int end_index){
    int len = end_index - start_index;
    as_val_init(&v->_, AS_BYTES, false /*is_malloc*/);
    v->value_is_malloc = true;
    v->value = malloc(len);
    memcpy(v->value, &src->value[start_index], len);
    v->len = len;
    v->capacity = len;
    return v;
}

int as_bytes_append(as_bytes *v, const uint8_t *buf, int buf_len) 
{
    // not enough capacity? increase
    if (v->len + buf_len > v->capacity) {
        uint8_t *t;
        if (v->value_is_malloc == false) {
            t = malloc(v->len + buf_len);
            if (!t) return(-1);
            v->value_is_malloc = true;
            memcpy(t, v->value + v->len, v->len);
        }
        else {
            t = realloc(v->value, v->len + buf_len);
            if (!t) return(-1);
        }
        memcpy(&t[v->len], buf, buf_len);
        v->value = t;
        v->value_is_malloc = true;
        v->len = v->capacity = v->len + buf_len;
    }
    else {
        memcpy(&v->value[v->len],buf,buf_len);
        v->len += buf_len;
    }
    return(0);
}

int as_bytes_append_bytes(as_bytes *s1, as_bytes *s2) 
{
    // not enough capacity? increase
    if (s1->len + s2->len > s1->capacity) {
        uint8_t *t;
        if (s1->value_is_malloc == false) {
            t = malloc(s1->len + s2->len);
            if (!t) return(-1);
            s1->value_is_malloc = true;
            memcpy(t, s1->value + s1->len, s1->len);
            s1->value_is_malloc = true;
        }
        else {
            t = realloc(s1->value, s1->len + s2->len);
            if (!t) return(-1);
        }
        memcpy(&t[s1->len], s2->value, s2->len);
        s1->value = t;
        s1->len = s1->capacity = s1->len + s2->len;
    }
    else {
        memcpy(&s1->value[s1->len],s2->value,s2->len);
        s1->len += s2->len;
    }
    return(0);
}

int as_bytes_delete(as_bytes *v, int d_pos, int d_len)
{
    if (d_pos + d_len > v->len) return(-1);
    // overlapping writes require memmove
    memmove(&v->value[d_pos], &v->value[d_pos+d_len],v->len - (d_pos + d_len));
    return(0);
}

uint32_t as_bytes_hash(const as_bytes * s) {
    if (s->value == NULL) return(0);
    uint32_t hash = 0;
    uint8_t * str = s->value;
    int len = s->len;
    while ( --len ) {
        int c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

uint32_t as_bytes_val_hash(const as_val * v) {
    return as_bytes_hash((as_bytes *) v);
}

char * as_bytes_val_tostring(const as_val * v) {
    as_bytes * s = (as_bytes *) v;
    if (s->value == NULL) return(NULL);
    size_t sl = as_bytes_len(s);
    size_t st = 3 + sl;
    char * str = (char *) malloc(sizeof(char) * st);
    *(str + 0) = '\"';
    memcpy(str + 1, s->value, s->len);
    *(str + 1 + sl) = '\"';
    *(str + 1 + sl + 1) = '\0';
    return str;
}
