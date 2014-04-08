set(util_SRCS 
    ${blackjack_strategy_SOURCE_DIR}/util/src/boolean.c
    ${blackjack_strategy_SOURCE_DIR}/util/src/error.c
    ${blackjack_strategy_SOURCE_DIR}/util/src/linal.c
    ${blackjack_strategy_SOURCE_DIR}/util/src/moremath.c
    ${blackjack_strategy_SOURCE_DIR}/util/src/stp.c
   )
set(LIBRARY_OUTPUT_PATH ${blackjack_strategy_SOURCE_DIR}/lib)

include_directories(${blackjack_strategy_SOURCE_DIR}/util/include)
add_library(util SHARED ${util_SRCS})



