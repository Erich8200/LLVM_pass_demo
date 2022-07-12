add_library(MyPass MODULE 
        slice_on_func_param.cpp 
        split_basic_block.cpp 
        insert_basic_block.cpp 
        print_suc_and_next_node.cpp
        change_func_name.cpp
        BogusControlFlow.cpp
        CryptoUtils.cpp
        dom_ana.cpp
        Flattening.cpp
        Utils.cpp
        make_func.cpp
        )

# Use C++11 to compile your pass (i.e., supply -std=c++11).
target_compile_features(MyPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(MyPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)