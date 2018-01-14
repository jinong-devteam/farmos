/**
 * Copyright © 2017-2018 JiNong Inc. All Rights Reserved.
 * \file cf_base.c
 * \brief 메세지버퍼 관련 공통라이브러리 파일. 기존 코드를 수정했음.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cf_base.h>
#include <cf_msgbuf.h>

#define _CF_MSGBUF_STEP		100
#define _CF_SPARESIZE(pb) ((pb)->bufsize - (pb)->idx - (pb)->len)
#define _CF_REALSPARESIZE(pb) ((pb)->bufsize - (pb)->len)

ret_t
cf_init_msgbuf (cf_msgbuf_t *pmsgbuf) {
	pmsgbuf->buf = CF_MALLOC (sizeof(char) * _CF_MSGBUF_STEP);
	if (pmsgbuf->buf == NULL) {
        cf_errormsg ("mesage buffer initialization failed.");
        return ERR;
    }

	pmsgbuf->bufsize = _CF_MSGBUF_STEP;
	pmsgbuf->len = 0;
	pmsgbuf->idx = 0;

	return OK;
}

ret_t
cf_append_msgbuf (cf_msgbuf_t *pmsgbuf, char *buf, int len){ 
	int tmpsize;
	void *tmpbuf;

	if (_CF_SPARESIZE (pmsgbuf) < len) {
		if (_CF_REALSPARESIZE (pmsgbuf) < len) {
			tmpsize = pmsgbuf->bufsize + (len > _CF_MSGBUF_STEP ? len : _CF_MSGBUF_STEP);
			tmpbuf = CF_REALLOC (pmsgbuf->buf, sizeof (char) * tmpsize);
			if (tmpbuf == NULL){
                cf_errormsg ("message buffer reallocation failed.");
                return ERR;
            }
			pmsgbuf->buf = tmpbuf;
			pmsgbuf->bufsize = tmpsize;
		} 
		memcpy (pmsgbuf->buf, pmsgbuf->buf + pmsgbuf->idx, pmsgbuf->len);
		pmsgbuf->idx = 0;
	}
	memcpy (pmsgbuf->buf + pmsgbuf->idx + pmsgbuf->len, buf, len);
	pmsgbuf->len += len;

	return OK;
}

ret_t
cf_resize_msgbuf (cf_msgbuf_t *pmsgbuf, int size) { 
	if (pmsgbuf->bufsize < size) {
		CF_FREE (pmsgbuf->buf);
		pmsgbuf->buf = CF_MALLOC (sizeof(char) * size);
		if (pmsgbuf->buf == NULL) {
            cf_errormsg ("mesage buffer expanding failed.");
            return ERR;
        }

		pmsgbuf->bufsize = size;
		pmsgbuf->len = 0;
		pmsgbuf->idx = 0;
	}
	return OK;
}

void
cf_setlength_msgbuf (cf_msgbuf_t *pmsgbuf, int length){ 
	pmsgbuf->idx = 0;
	pmsgbuf->len = length;
}
int
cf_getlength_msgbuf (cf_msgbuf_t *pmsgbuf){ 
	//return pmsgbuf->len - pmsgbuf->idx;
	return pmsgbuf->len;
}

int
cf_getsize_msgbuf (cf_msgbuf_t *pmsgbuf){ 
	return pmsgbuf->bufsize;
}

char *
cf_getbuf_msgbuf (cf_msgbuf_t *pmsgbuf){ 
	return pmsgbuf->buf + pmsgbuf->idx;
}

void
cf_setused_msgbuf (cf_msgbuf_t *pmsgbuf, int usedlen){ 
	pmsgbuf->idx += usedlen;
	pmsgbuf->len -= usedlen;
}

void
cf_release_msgbuf (cf_msgbuf_t *pmsgbuf){ 
	CF_FREE (pmsgbuf->buf);
}

