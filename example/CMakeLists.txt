set(BASIC_CPP_SRC
    basiccpp/t_class.cpp
    basiccpp/t_private.cpp
    basiccpp/t_sizeof.cpp
)

add_executable(tast-basiccpp-uliba.exe
    ${BASIC_CPP_SRC}
)
target_link_libraries(tast-basiccpp-uliba.exe
    PRIVATE tinytast
)

add_executable(tast-basiccpp-umain.exe
    ${BASIC_CPP_SRC}
    basiccpp-umain/main.cpp
)

add_library(tast-basiccpp SHARED
    ${BASIC_CPP_SRC}
    basiccpp-udyso/tast_main.cpp
)

add_executable(simplest.exe
    simplest/main.cpp
)

add_executable(tastself.exe
    #tastself/main.cpp
    tastself/t_logprint.cpp
    tastself/t_logprint2.cpp
)

target_link_libraries(tastself.exe
    PRIVATE tinytast
)

add_test(NAME taseself COMMAND tastself.exe)
