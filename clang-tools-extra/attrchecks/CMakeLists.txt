set(LLVM_LINK_COMPONENTS support)

add_clang_executable(attrchecks AttrChecks.cpp)
target_link_libraries(attrchecks
    PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
    )

