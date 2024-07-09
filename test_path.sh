#!/bin/bash

MINISHELL_PATH="./minishell"
OUTPUT_FILE="./test_path_results.txt"
> $OUTPUT_FILE

run_test() {
    local test_name="$1"
    local test_cmd="$2"
    echo "Running test: $test_name" >> $OUTPUT_FILE
    MINI_OUTPUT=$(echo -e "$test_cmd" | $MINISHELL_PATH 2>&1)
    echo "Minishell output:" >> $OUTPUT_FILE
    echo "$MINI_OUTPUT" >> $OUTPUT_FILE
    echo -e "\n-----------------------\n" >> $OUTPUT_FILE
}

# Valid PATH
run_test "valid_path_ls" "export PATH=/bin:/usr/bin\nls\nexit\n"
run_test "valid_path_wc" "export PATH=/bin:/usr/bin\nwc -l /etc/passwd\nexit\n"

# Invalid PATH
run_test "invalid_path_ls" "export PATH=/invalid/path\nls\nexit\n"
run_test "invalid_path_wc" "export PATH=/invalid/path\nwc -l /etc/passwd\nexit\n"

# Empty PATH
run_test "empty_path_ls" "export PATH=\nls\nexit\n"
run_test "empty_path_wc" "export PATH=\nwc -l /etc/passwd\nexit\n"

# Relative PATH
run_test "relative_path_ls" "export PATH=.\n./minishell\nexit\n"
run_test "relative_path_wc" "export PATH=.\n./minishell wc -l /etc/passwd\nexit\n"

echo "All tests completed. Results are in $OUTPUT_FILE."
