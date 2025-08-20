#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "yaml.h"

typedef struct {
    unsigned char *buf;
    size_t cap, off;
    int mode; // 0=normal, 1=fail-once, 2=small-cap
    int fail_once_used;  // per-call state, avoids static globals
} wr_ctx;

// libyaml write handler: return 1 on success, 0 on error
static int write_cb(void *data, unsigned char *buffer, size_t size) {
    wr_ctx *ctx = (wr_ctx*)data;

  // Mode 2: tiny capacity to force flush/error
//   size_t room = (ctx->mode == 2 && ctx->cap > 0) ? (ctx->cap - ctx->off) : (ctx->cap - ctx->off);
//   if (size > room) return 0; // trigger YAML_WRITER_ERROR branch

  // Mode 2: tiny capacity to force writer error path deterministically
    size_t room = (ctx->cap > ctx->off) ? (ctx->cap - ctx->off) : 0;
    if (ctx->mode == 2 && size > room) return 0; // trigger YAML_WRITER_ERROR


  // Mode 1: fail exactly once mid-stream to hit error path
//   static int failed_once = 0;
//   if (ctx->mode == 1 && !failed_once) { failed_once = 1; return 0; }

  // Mode 1: fail exactly once per fuzz invocation (NO static globals)
    if (ctx->mode == 1 && !ctx->fail_once_used) { ctx->fail_once_used = 1; return 0; }

    if (size > room) return 0; // still guard normal overflow
    memcpy(ctx->buf + ctx->off, buffer, size);
    ctx->off += size;
    return 1;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  yaml_parser_t parser;
  if (!yaml_parser_initialize(&parser)) return 0;
  yaml_parser_set_input_string(&parser, Data, Size);

  unsigned docs = 0, kMaxDocs = 8;
  while (docs++ < kMaxDocs) {
    yaml_document_t doc;
    if (!yaml_parser_load(&parser, &doc)) break;  // parse error or end

    yaml_emitter_t emitter;
    if (!yaml_emitter_initialize(&emitter)) { yaml_document_delete(&doc); break; }

    // --- writer setup (callback or output_string) ---
    wr_ctx ctx = {0};
    ctx.cap = 64 * 1024;
    ctx.buf = (unsigned char*)malloc(ctx.cap);
    if (!ctx.buf) { yaml_emitter_delete(&emitter); yaml_document_delete(&doc); break; }
    ctx.mode = (int)(Size % 3);
    yaml_emitter_set_output(&emitter, write_cb, &ctx);

    // Branches for writer.c
    switch (Size % 3) {
      case 0: yaml_emitter_set_encoding(&emitter, YAML_UTF8_ENCODING);    break;
      case 1: yaml_emitter_set_encoding(&emitter, YAML_UTF16LE_ENCODING); break;
      case 2: yaml_emitter_set_encoding(&emitter, YAML_UTF16BE_ENCODING); break;
    }
    switch ((Size >> 1) % 3) {
      case 0: yaml_emitter_set_break(&emitter, YAML_LN_BREAK);   break;
      case 1: yaml_emitter_set_break(&emitter, YAML_CRLN_BREAK); break;
      case 2: yaml_emitter_set_break(&emitter, YAML_CR_BREAK);   break;
    }
    yaml_emitter_set_unicode(&emitter, 1);
    yaml_emitter_set_indent(&emitter, 2);
    yaml_emitter_set_width(&emitter, 80);

    // Open → Dump → Close. IMPORTANT: once we call dump(), we NEVER free 'doc' ourselves.
    int opened = yaml_emitter_open(&emitter);
    if (!opened) {                      // we still own doc → safe to delete
      yaml_document_delete(&doc);
      yaml_emitter_delete(&emitter);
      free(ctx.buf);
      break;
    }

    (void)yaml_emitter_dump(&emitter, &doc);   // emitter now owns/handles 'doc' in all cases
    (void)yaml_emitter_close(&emitter);

    yaml_emitter_delete(&emitter);
    free(ctx.buf);
  }

  yaml_parser_delete(&parser);
  return 0;
}