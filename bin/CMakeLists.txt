set(blackjack_strategy_SRCS 
    ${blackjack_strategy_SOURCE_DIR}/src/bj_sims.c
    ${blackjack_strategy_SOURCE_DIR}/src/bj_strat.c
    ${blackjack_strategy_SOURCE_DIR}/src/hands.c
    ${blackjack_strategy_SOURCE_DIR}/src/main.c
    ${blackjack_strategy_SOURCE_DIR}/src/print_chart.c
   )
set(EXECUTABLE_OUTPUT_PATH ${blackjack_strategy_SOURCE_DIR}/bin)

include_directories(${blackjack_strategy_SOURCE_DIR}/include)
include_directories(${blackjack_strategy_SOURCE_DIR}/util/include)

add_executable(blackjack_strategy ${blackjack_strategy_SRCS})

target_link_libraries(blackjack_strategy util m gsl lapack blas)

