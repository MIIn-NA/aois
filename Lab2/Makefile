
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TESTFLAGS = -std=c++17 -Wall -Wextra -g

COVERAGEFLAGS = -fprofile-arcs -ftest-coverage -fPIC -O0

SRCDIR = src
TESTDIR = tests

SRC_FILES = $(SRCDIR)/Analyzer.cpp \
            $(SRCDIR)/BooleanDerivative.cpp \
            $(SRCDIR)/CanonicalForms.cpp \
            $(SRCDIR)/ExpressionParser.cpp \
            $(SRCDIR)/KarnaughMap.cpp \
            $(SRCDIR)/Minimization.cpp \
            $(SRCDIR)/PostClasses.cpp \
            $(SRCDIR)/ReportBuilder.cpp \
            $(SRCDIR)/TruthTable.cpp \
            $(SRCDIR)/ZhegalkinPolynomial.cpp

TEST_FILES = $(TESTDIR)/test_all.cpp \
             $(TESTDIR)/test_truth_table.cpp \
             $(TESTDIR)/test_canonical_forms.cpp \
             $(TESTDIR)/test_post_classes.cpp \
             $(TESTDIR)/test_zhegalkin.cpp \
             $(TESTDIR)/test_minimization.cpp

TARGET = main
TEST_TARGET = test_runner
COVERAGE_DIR = coverage

all: $(TARGET)

$(TARGET): main.cpp $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -I. -o $(TARGET) main.cpp $(SRC_FILES)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

test-coverage: CXXFLAGS += $(COVERAGEFLAGS)
test-coverage: clean-test-objects
	@mkdir -p $(COVERAGE_DIR)
	$(CXX) $(TESTFLAGS) $(COVERAGEFLAGS) -I. -I$(TESTDIR) \
		-o $(TEST_TARGET) $(TEST_FILES) $(SRC_FILES)
	./$(TEST_TARGET)
	gcovr -r . --html --html-details -o $(COVERAGE_DIR)/index.html
	gcovr -r . --txt
	@echo ""
	@echo "========================================"
	@echo "Отчёт о покрытии: $(COVERAGE_DIR)/index.html"
	@echo "========================================"

clean-test-objects:
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete

coverage-lcov: CXXFLAGS += $(COVERAGEFLAGS)
coverage-lcov: clean-test-objects
	@mkdir -p $(COVERAGE_DIR)
	$(CXX) $(TESTFLAGS) $(COVERAGEFLAGS) -I. -I$(TESTDIR) \
		-o $(TEST_TARGET) $(TEST_FILES) $(SRC_FILES)
	./$(TEST_TARGET)
	lcov --capture --directory . --output-file $(COVERAGE_DIR)/coverage.info
	lcov --remove $(COVERAGE_DIR)/coverage.info '/usr/*' '*/tests/*' --output-file $(COVERAGE_DIR)/coverage.info
	genhtml $(COVERAGE_DIR)/coverage.info --output-directory $(COVERAGE_DIR)/html
	@echo ""
	@echo "========================================"
	@echo "Отчёт о покрытии: $(COVERAGE_DIR)/html/index.html"
	@echo "========================================"

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf *.out *.log test_report.txt
	find . -name "*.gcno" -delete
	find . -name "*.gcda" -delete
	rm -rf $(COVERAGE_DIR)

run: $(TARGET)
	./$(TARGET)

test-report: $(TEST_TARGET)
	./$(TEST_TARGET) > test_report.txt 2>&1
	cat test_report.txt

.PHONY: all clean run test test-report test-coverage coverage-lcov clean-test-objects
