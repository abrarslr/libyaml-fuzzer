#include <stdint.h>
#include "yaml.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
    yaml_parser_t parser;
    yaml_document_t doc;

    if(!yaml_parser_initialize(&parser))
        return 0;

    yaml_parser_set_input_string(&parser, Data, Size);

    if(yaml_parser_load(&parser, &doc)){
        yaml_document_delete(&doc);
    }

    yaml_parser_delete(&parser);
    return 0;
}