.PHONY: test

test:
	docker run --rm -v "$$PWD":/work -w /work audio-dsp \
	  bash -lc "cmake -S . -B build -G Ninja && cmake --build build && ctest --test-dir build --output-on-failure"
