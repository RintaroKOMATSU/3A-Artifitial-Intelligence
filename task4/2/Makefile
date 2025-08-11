CXX = g++
CXXFLAGS = -O2 -Iinclude

# すべての.cppファイルを取得
SRCS = $(wildcard src/*.cpp)
# 各.cppファイルに対応する実行ファイルを生成 (src/file.cpp -> bin/file)
TARGETS = $(patsubst src/%.cpp,bin/%,$(SRCS))
# オブジェクトファイル (必要に応じて)
OBJS = $(SRCS:%.cpp=%.o)

# デフォルトターゲット: すべての実行ファイルをビルド
all: $(TARGETS)

# 各実行ファイルのビルドルール
bin/%: src/%.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

# 特定のファイルのみをビルドするルール (例: make main で src/main.cpp -> bin/main)
%: bin/%
	@echo "Built $@"

# オブジェクトファイルのビルドルール (必要に応じて)
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# デバッグ用: 検出されたファイルとターゲットを表示
show-files:
	@echo "Source files: $(SRCS)"
	@echo "Targets: $(TARGETS)"

.PHONY: all clean tmpclean show-files

clean: tmpclean
	rm -f $(TARGETS) $(OBJS)

tmpclean:
	rm -f *~
	rm -f src/*~
