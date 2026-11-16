TIMEOUT_US				= 1000000
.DEFAULT_GOAL			= a
UTILS					= utils/sigsegv.cpp utils/color.cpp utils/check.cpp utils/gnl.cpp utils/leaks.cpp
TESTS_PATH				= tests/
SHELL					= bash
GNL_DIR					?= ..


MANDATORY_HEADER		= $(GNL_DIR)/get_next_line.h
MANDATORY_FILES			= $(GNL_DIR)/get_next_line.c $(GNL_DIR)/get_next_line_utils.c
# unique objs per BUFFER_SIZE for mandatory
M1_OBJS					= get_next_line.bs1.o get_next_line_utils.bs1.o
M42_OBJS				= get_next_line.bs42.o get_next_line_utils.bs42.o
M10M_OBJS				= get_next_line.bs10M.o get_next_line_utils.bs10M.o

BONUS_HEADER			= $(GNL_DIR)/get_next_line_bonus.h
BONUS_FILES				= $(GNL_DIR)/get_next_line_bonus.c $(GNL_DIR)/get_next_line_utils_bonus.c
# unique objs per BUFFER_SIZE for bonus
B1_OBJS					= get_next_line_bonus.bs1.o get_next_line_utils_bonus.bs1.o
B42_OBJS				= get_next_line_bonus.bs42.o get_next_line_utils_bonus.bs42.o
B10M_OBJS				= get_next_line_bonus.bs10M.o get_next_line_utils_bonus.bs10M.o

MANDATORY				= mandatory
1MANDATORY				= $(addprefix 1, $(MANDATORY))
42MANDATORY				= $(addprefix 42, $(MANDATORY))
10MMANDATORY			= $(addprefix 10M, $(MANDATORY))


BONUS					= bonus
1MBONUS					= $(addprefix m1, $(BONUS))
42MBONUS				= $(addprefix m42, $(BONUS))
10MMBONUS				= $(addprefix m10M, $(BONUS))

1BONUS					= $(addprefix 1, $(BONUS))
42BONUS					= $(addprefix 42, $(BONUS))
10MBONUS				= $(addprefix 10M, $(BONUS))

CFLAGS					= -g3 -Wall -Wextra -Werror
CPPFLAGS				= -g3 -ldl -std=c++11 -I utils/ -I tests/ -I$(GNL_DIR) -Wno-everything

UNAME = $(shell uname -s)
ifneq ($(shell command -v valgrind 2>/dev/null),)
ifeq ($(UNAME), Linux)
    VALGRIND = valgrind -q --leak-check=full
endif
endif

$(1MANDATORY): 1%:
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line.c -o get_next_line.bs1.o -gdwarf-4
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils.c -o get_next_line_utils.bs1.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=1 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(M1_OBJS) -o gnlTest_bs1 && { $(VALGRIND) ./gnlTest_bs1 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bs1; exit $$test_status; }
$(42MANDATORY): 42%:
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line.c -o get_next_line.bs42.o -gdwarf-4
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils.c -o get_next_line_utils.bs42.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=42 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(M42_OBJS) -o gnlTest_bs42 && { $(VALGRIND) ./gnlTest_bs42 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bs42; exit $$test_status; }
$(10MMANDATORY): 10M%:
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line.c -o get_next_line.bs10M.o -gdwarf-4
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils.c -o get_next_line_utils.bs10M.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=10000000 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(M10M_OBJS) -o gnlTest_bs10M && { $(VALGRIND) ./gnlTest_bs10M < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bs10M; exit $$test_status; }


$(1BONUS): 1%:
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs1.o -gdwarf-4
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs1.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=1 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(B1_OBJS) -o gnlTest_bbs1 && { $(VALGRIND) ./gnlTest_bbs1 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bbs1; exit $$test_status; }
$(42BONUS): 42%:
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs42.o -gdwarf-4
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs42.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=42 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(B42_OBJS) -o gnlTest_bbs42 && { $(VALGRIND) ./gnlTest_bbs42 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bbs42; exit $$test_status; }
$(10MBONUS): 10M%:
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs10M.o -gdwarf-4
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs10M.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=10000000 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$*.cpp $(B10M_OBJS) -o gnlTest_bbs10M && { $(VALGRIND) ./gnlTest_bbs10M < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_bbs10M; exit $$test_status; }

$(1MBONUS): m1%:
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs1.o -gdwarf-4
	@gcc -D BUFFER_SIZE=1 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs1.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=1 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$(MANDATORY).cpp $(B1_OBJS) -o gnlTest_mbs1 && { $(VALGRIND) ./gnlTest_mbs1 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_mbs1; exit $$test_status; }
$(42MBONUS): m42%:
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs42.o -gdwarf-4
	@gcc -D BUFFER_SIZE=42 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs42.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=42 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$(MANDATORY).cpp $(B42_OBJS) -o gnlTest_mbs42 && { $(VALGRIND) ./gnlTest_mbs42 < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_mbs42; exit $$test_status; }
$(10MMBONUS): m10M%:
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line_bonus.c -o get_next_line_bonus.bs10M.o -gdwarf-4
	@gcc -D BUFFER_SIZE=10000000 $(CFLAGS) -c $(GNL_DIR)/get_next_line_utils_bonus.c -o get_next_line_utils_bonus.bs10M.o -gdwarf-4
	@clang++ -D BUFFER_SIZE=10000000 -gdwarf-4 -D TIMEOUT_US=$(TIMEOUT_US) $(CPPFLAGS) $(UTILS) tests/faults.cpp $(TESTS_PATH)$(MANDATORY).cpp $(B10M_OBJS) -o gnlTest_mbs10M && { $(VALGRIND) ./gnlTest_mbs10M < files/alternate_line_nl_with_nl; test_status=$$?; rm -f gnlTest_mbs10M; exit $$test_status; }

$(1BONUS): $(1MBONUS)
$(42BONUS): $(42MBONUS)
$(10MBONUS): $(10MMBONUS)

mandatory_start:
	@for file in $(MANDATORY_FILES); do \
		grep -qE '#[[:space:]]*include[[:space:]]*"get_next_line\.h"' $$file \
		|| { echo "KO: $$file must include \"get_next_line.h\""; exit 1; }; \
	done
	@echo [Mandatory]

bonus_start:
	@for file in $(BONUS_FILES); do \
		grep -qE '#[[:space:]]*include[[:space:]]*"get_next_line_bonus\.h"' $$file \
		|| { echo "KO: $$file must include \"get_next_line_bonus.h\""; exit 1; }; \
	done
	@printf "[Static = " && cat $(GNL_DIR)/*bonus.c | grep -E 'static.*;' | wc -l | tr -d '\n' | xargs /bin/echo -n && printf "]\n"
	@echo [Bonus]

dockerm dockerb dockera: docker%:
	@docker rm -f mc > /dev/null 2>&1 || true
	docker build -qt mi .
	docker run -dti --name mc -v $(shell dirname $(shell pwd)):/project/ mi
	docker exec -ti mc make $* -C gnlTester || true
	@docker rm -f mc > /dev/null 2>&1

m: mandatory_start
	@status=0; $(MAKE) --no-print-directory $(1MANDATORY) $(42MANDATORY) $(10MMANDATORY) || status=$$?; $(MAKE) --no-print-directory cleanMandatory; exit $$status
b: bonus_start
	@status=0; $(MAKE) --no-print-directory $(1MBONUS) $(42MBONUS) $(10MMBONUS) $(1BONUS) $(42BONUS) $(10MBONUS) || status=$$?; $(MAKE) --no-print-directory cleanBonus; exit $$status
a: m
	@$(MAKE) --no-print-directory b

fclean clean cleanMandatory cleanBonus:
	@rm -f *.o gnlTest*

.PHONY:	mandatory_start m bonus_start b a fclean clean cleanMandatory cleanBonus $(1MANDATORY) $(42MANDATORY) $(10MMANDATORY) $(1MBONUS) $(42MBONUS) $(10MMBONUS) $(1BONUS) $(42BONUS) $(10MBONUS)
