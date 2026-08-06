# Programs
CMAKE = cmake
CTEST = ctest

# Options
PRESET = Debug
SHARED = OFF

all: configure compile test

configure: .always
	$(CMAKE) -S . -B ./build \
		-DCMAKE_BUILD_TYPE:STRING=$(PRESET) \
		-DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
		-DSOURCEMETA_CORE_TESTS:BOOL=ON \
		-DSOURCEMETA_CORE_BENCHMARK:BOOL=ON \
		-DSOURCEMETA_CORE_DOCS:BOOL=ON \
		-DBUILD_SHARED_LIBS:BOOL=$(SHARED)

compile: .always
	$(CMAKE) --build ./build --config $(PRESET) --target clang_format
	$(CMAKE) --build ./build --config $(PRESET)
	$(CMAKE) --install ./build --prefix ./build/dist --config $(PRESET) --verbose \
		--component sourcemeta_core
	$(CMAKE) --install ./build --prefix ./build/dist --config $(PRESET) --verbose \
		--component sourcemeta_core_dev

test: .always
	$(CMAKE) -E env UBSAN_OPTIONS=print_stacktrace=1 \
		$(CTEST) --test-dir ./build --build-config $(PRESET) \
			--output-on-failure --parallel

benchmark: .always
	$(CMAKE) --build ./build --config $(PRESET) --target benchmark_all

coverage: .always
	$(CMAKE) -S . -B ./build/coverage \
		-DCMAKE_BUILD_TYPE:STRING=Debug \
		-DCMAKE_COMPILE_WARNING_AS_ERROR:BOOL=ON \
		-DSOURCEMETA_CORE_TESTS:BOOL=ON \
		-DSOURCEMETA_CORE_COVERAGE:BOOL=ON \
		-DBUILD_SHARED_LIBS:BOOL=OFF
	$(CMAKE) --build ./build/coverage --config Debug
	$(CMAKE) --build ./build/coverage --config Debug --target coverage

doxygen: .always
	$(CMAKE) --build ./build --config $(PRESET) --target doxygen

clean: .always
	$(CMAKE) -E rm -R -f build

# For NMake, which doesn't support .PHONY
.always:
