#include <stdint.h>
#include "yaml.h"

// Event-drive parsing
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    yaml_parser_t parser; yaml_event_t event;
    if (!yaml_parser_initialize(&parser)) return 0;
    yaml_parser_set_input_string(&parser, Data, Size);

    unsigned steps = 0, kMaxSteps = 100000;   // loop budget (hang guard)
    while (steps++ < kMaxSteps) {
        if (!yaml_parser_parse(&parser, &event)) break;  // error OR end-of-stream
        int is_end = (event.type == YAML_STREAM_END_EVENT);
        yaml_event_delete(&event);                        // free event payload
        if (is_end) break;
    }
    yaml_parser_delete(&parser);
    return 0;
}