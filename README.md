# The LLVM Compiler Infrastructure

This directory and its subdirectories contain source code for LLVM,
a toolkit for the construction of highly optimized compilers,
optimizers, and runtime environments.

# Custom Attribute Changes

`attr` branch has some changes to introduce Attributes as applied on Statements and Decls, this is inspired by changes done by Matthias.Ghere for `Suppress` attribute.

```cpp

#define ATTRIBUTE(x)           [[my::custattr((x))]]
#define ATTRIBUTE_UNSAFE       ATTRIBUTE("Unsafe")
#define ATTRIBUTE_COMPATIBLE   ATTRIBUTE("Compatible")
#define ATTRIBUTE_SAFE         ATTRIBUTE("Safe")


namespace ATTRIBUTE_COMPATIBLE Utils
{
  class ATTRIBUTE_SAFE ClassType
  {
    public:
    ClassType() = default;
    ~ClassType() = default;
    
    ATTRIBUTE_UNSAFE
    void SomeUnSafeOp() {}
  };
}

```

This prints AST as (look for `MyCustomAttr` in here):

```sh
`-NamespaceDecl 0xaedb20 <d:\code.cpp:8:1, line:19:1> line:8:32 Utils
  |-MyCustomAttr 0xaedb70 <line:2:34, col:50> Compatible
  `-CXXRecordDecl 0xaedc68 <line:10:3, line:18:3> line:10:24 class ClassType definition
    |-DefinitionData pass_in_registers empty aggregate standard_layout trivially_copyable trivial literal has_user_declared_ctor has_constexpr_non_copy_move_ctor can_const_default_init
    | |-DefaultConstructor exists trivial constexpr defaulted_is_constexpr
    | |-CopyConstructor simple trivial has_const_param needs_implicit implicit_has_const_param
    | |-MoveConstructor
    | |-CopyAssignment trivial has_const_param needs_implicit implicit_has_const_param
    | |-MoveAssignment
    | `-Destructor irrelevant trivial user_declared
    |-MyCustomAttr 0xaedd38 <line:2:34, col:50> Safe
    |-CXXRecordDecl 0xaedd90 <line:10:3, col:24> col:24 implicit referenced class ClassType
    |-AccessSpecDecl 0xaeddf0 <line:12:5, col:11> col:5 public
    |-CXXConstructorDecl 0xaede80 <line:13:5, col:25> col:5 constexpr ClassType 'void () __attribute__((thiscall))' default trivial noexcept-unevaluated 0xaede80
    |-CXXDestructorDecl 0xaedf40 <line:14:5, col:26> col:5 ~ClassType 'void () __attribute__((thiscall))' default trivial noexcept-unevaluated 0xaedf40
    `-CXXMethodDecl 0xaee0a0 <line:17:5, col:26> col:10 SomeUnSafeOp 'void () __attribute__((thiscall))'
      |-CompoundStmt 0xaee194 <col:25, col:26>
      `-MyCustomAttr 0xaee118 <line:2:34, col:50> Unsafe
```