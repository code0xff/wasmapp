#include <iostream>
#include <wasmer.h>

int main() {
    const char *wat_string = "(module\n"
                             " (table 0 anyfunc)\n"
                             " (export \"add\" (func $add))\n"
                             " (export \"sub\" (func $sub))\n"
                             " (export \"mul\" (func $mul))\n"
                             " (export \"div\" (func $div))\n"
                             " (export \"mod\" (func $mod))\n"
                             " (func $add (; 0 ;) (param $0 i32) (param $1 i32) (result i32)\n"
                             "  (i32.add\n"
                             "   (get_local $1)\n"
                             "   (get_local $0)\n"
                             "  )\n"
                             " )\n"
                             " (func $sub (; 1 ;) (param $0 i32) (param $1 i32) (result i32)\n"
                             "  (i32.sub\n"
                             "   (get_local $0)\n"
                             "   (get_local $1)\n"
                             "  )\n"
                             " )\n"
                             " (func $mul (; 2 ;) (param $0 i32) (param $1 i32) (result i32)\n"
                             "  (i32.mul\n"
                             "   (get_local $1)\n"
                             "   (get_local $0)\n"
                             "  )\n"
                             " )\n"
                             " (func $div (; 3 ;) (param $0 i32) (param $1 i32) (result i32)\n"
                             "  (i32.div_s\n"
                             "   (get_local $0)\n"
                             "   (get_local $1)\n"
                             "  )\n"
                             " )\n"
                             " (func $mod (; 4 ;) (param $0 i32) (param $1 i32) (result i32)\n"
                             "  (i32.rem_s\n"
                             "   (get_local $0)\n"
                             "   (get_local $1)\n"
                             "  )\n"
                             " )\n"
                             ")";

    wasm_byte_vec_t wat;
    wasm_byte_vec_new(&wat, strlen(wat_string), wat_string);
    wasm_byte_vec_t wasm_bytes;
    wat2wasm(&wat, &wasm_bytes);
    wasm_byte_vec_delete(&wat);

    printf("Creating the store...\n");
    wasm_engine_t *engine = wasm_engine_new();
    wasm_store_t *store = wasm_store_new(engine);

    printf("Compiling module...\n");
    wasm_module_t *module = wasm_module_new(store, &wasm_bytes);

    if (!module) {
        printf("> Error compiling module!\n");
        return 1;
    }

    wasm_byte_vec_delete(&wasm_bytes);

    printf("Creating imports...\n");
    wasm_extern_vec_t import_object = WASM_EMPTY_VEC;

    printf("Instantiating module...\n");
    wasm_instance_t *instance = wasm_instance_new(store, module, &import_object, NULL);

    if (!instance) {
        printf("> Error instantiating module!\n");
        return 1;
    }

    printf("Retrieving exports...\n");
    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    if (exports.size == 0) {
        printf("> Error accessing exports!\n");
        return 1;
    }

    printf("Retrieving the `mul` function...\n");
    wasm_func_t *mul_func = wasm_extern_as_func(exports.data[2]);
    if (mul_func == NULL) {
        printf("> Failed to get the `mul` function!\n");
        return 1;
    }

    printf("Calling `mul` function...\n");
    wasm_val_t args_val[2] = {WASM_I32_VAL(9), WASM_I32_VAL(2)};
    wasm_val_t results_val[1] = {WASM_INIT_VAL};
    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    if (wasm_func_call(mul_func, &args, &results)) {
        printf("> Error calling the `mul` function!\n");

        return 1;
    }

    printf("Results of `mul`: %d\n", results_val[0].of.i32);

    wasm_module_delete(module);
    wasm_extern_vec_delete(&exports);
    wasm_instance_delete(instance);
    wasm_store_delete(store);
    wasm_engine_delete(engine);
    return 0;
}
