
#include "libavutil/avstring.h"
#include "libavutil/mem.h"

#include "pipe.h"
#include "ndninterface.h"
#include "avformat.h"
#include "url.h"

#include <stdio.h>
#include <pthread.h>

#define MIN(a,b) (a) < (b) ? (a) : (b)

typedef struct NDNContext {
    pipe_t              *buffer_pipe;
    pipe_consumer_t     *consumer;
    pthread_t    producer_thread;
    size_t              chunk_index;
    int64_t             size;
} NDNContext;

typedef struct {
    const char* name;
    pipe_producer_t* pipe_pro;
}ndn_thread_t;

static void* thread_func(void* context) {
    ndn_thread_t* ctx = context;
    getbyname(ctx->name, ctx->pipe_pro);
    return NULL;
}

static int ndn_open(URLContext *h, const char* uri, int flags) {
    NDNContext *c = h->priv_data;
    av_strstart(uri, "ndn:", &uri);

    c->buffer_pipe = pipe_new(sizeof(char), 0);
    ndn_thread_t* ctx = malloc(sizeof(ndn_thread_t));
    ctx->name = uri;
    ctx->pipe_pro = pipe_producer_new(c->buffer_pipe);
    pthread_create(&c->producer_thread, NULL, &thread_func, ctx);
    c->consumer = pipe_producer_new(c->buffer_pipe);
    return 0;
}

static int ndn_read(URLContext *h, unsigned char* buf, int size) {
    NDNContext *c = h->priv_data;
    size_t chunk_size = pipe_used_size(c->buffer_pipe), ret;
    if (chunk_size)
        size = MIN(chunk_size, size);
    ret = pipe_pop(c->consumer, buf, size);
    return (ret < 0) ? AVERROR(errno) : ret;
}

static int ndn_close(URLContext* h) {
    NDNContext *c = h->priv_data;
    pipe_free(c->buffer_pipe);
    pthread_join(c->producer_thread, NULL);
    return 0;
}
const URLProtocol ff_ndn_protocol = {
    .name           = "ndn",
    .url_open       = ndn_open,
    .url_read       = ndn_read,
    .url_close      = ndn_close,
    .priv_data_size = sizeof(NDNContext),
    .default_whitelist = "ndn",
};
