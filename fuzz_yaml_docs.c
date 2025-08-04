#include <stdint.h>
#include "yaml.h"

// Document-loader target
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
    yaml_parser_t parser;
    

    if(!yaml_parser_initialize(&parser))
        return 0;

    yaml_parser_set_input_string(&parser, Data, Size);

    unsigned docs = 0, kMaxDocs = 16;
    
    while(docs++ < kMaxDocs){
        yaml_document_t doc;
        int ok = yaml_parser_load(&parser, &doc);
        if(!ok)
            break;
        yaml_document_delete(&doc);
    }

    yaml_parser_delete(&parser);
    return 0;
}