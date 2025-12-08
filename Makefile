.SUFFIXES:				# ignore builtin rules
.PHONY: all all_examples main_lctt_certif main_periodical_uplink main_porting_tests main_ping_pong

TARGET ?= clean_build
TYPE ?= Debug #Release
DOCKER_IMAGE_VERSION ?= 1.0.0

# Default target builds all examples
all: all_examples

# Build individual examples
main_lctt_certif:
	@echo 'Building main_lctt_certif example...!'
	make -C . -f projects/autogen/lorawan_examples_cmake/project_make.mak TARGET=main_lctt_certif TYPE=$(TYPE) CMAKELIST_DIR=projects/autogen/lorawan_examples_cmake EXAMPLE_NAME=main_lctt_certif

main_periodical_uplink:
	@echo 'Building main_periodical_uplink...!'
	make -C . -f projects/autogen/lorawan_examples_cmake/project_make.mak TARGET=main_periodical_uplink TYPE=$(TYPE) CMAKELIST_DIR=projects/autogen/lorawan_examples_cmake EXAMPLE_NAME=main_periodical_uplink

main_porting_tests:
	@echo 'Building main_porting_tests...!'
	make -C . -f projects/autogen/lorawan_examples_cmake/project_make.mak TARGET=main_porting_tests TYPE=$(TYPE) CMAKELIST_DIR=projects/autogen/lorawan_examples_cmake EXAMPLE_NAME=main_porting_tests

main_ping_pong:
	@echo 'Building main_ping_pong...!'
	make -C . -f projects/autogen/lorawan_examples_cmake/project_make.mak TARGET=main_ping_pong TYPE=$(TYPE) CMAKELIST_DIR=projects/autogen/lorawan_examples_cmake EXAMPLE_NAME=main_ping_pong

test:
	@echo 'Running tests...!'
	make -C . -f projects/autogen/lorawan_examples_cmake/project_make.mak TARGET=lbm_hal_utest TYPE=$(TYPE) CMAKELIST_DIR=projects/autogen/lorawan_examples_cmake EXAMPLE_NAME=lbm_hal_utest

# Clean the project by deleting the build directory
clean:
	@echo 'Cleaning build directory...!'
	cmake -E remove_directory projects/autogen/build

# Build all examples
all_examples: clean main_lctt_certif main_periodical_uplink main_porting_tests main_ping_pong test
	@echo 'All examples built successfully!'
