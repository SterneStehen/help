#!/bin/bash

MINISHELL_PATH="./minishell"  # Path to your minishell executable
OUTPUT_FILE="./test_results.txt"
STATS_FILE="./test_result_count"
> $OUTPUT_FILE  # Clear the file before starting tests
> $STATS_FILE   # Clear the stats file

# Initialize counters for correct and incorrect results
correct=0
incorrect=0

# Function to print initial results to the output file
print_initial_results() {
    echo "Initial results:" >> $OUTPUT_FILE
    echo -e "Correct results: $correct\nIncorrect results: $incorrect" >> $OUTPUT_FILE
    echo -e "\n-----------------------\n" >> $OUTPUT_FILE
}

# Function to run a test and save the result
run_test() {
    local test_number="$1"
    local test_name="$2"
    local test_cmd="$3"
    echo "Running test $test_number: $test_name" >> $OUTPUT_FILE

    # Run the command in minishell
    MINI_OUTPUT=$(echo -e "$test_cmd" | $MINISHELL_PATH 2>&1)
    echo "Minishell output:" >> $OUTPUT_FILE
    echo "$MINI_OUTPUT" >> $OUTPUT_FILE

    # Run the command in bash
    BASH_OUTPUT=$(echo -e "$test_cmd" | bash 2>&1 | sed 's/bash: line [0-9]*: //')
    echo "Bash output:" >> $OUTPUT_FILE
    echo "$BASH_OUTPUT" >> $OUTPUT_FILE

    # Compare the results
    if [ "$MINI_OUTPUT" == "$BASH_OUTPUT" ]; then
        echo "Result: ✅ Match" >> $OUTPUT_FILE
        correct=$((correct + 1))
    else
        echo "Result: ❌ Mismatch" >> $OUTPUT_FILE
        incorrect=$((incorrect + 1))
        echo "Difference:" >> $OUTPUT_FILE
        diff <(echo "$MINI_OUTPUT") <(echo "$BASH_OUTPUT") >> $OUTPUT_FILE
    fi

    echo -e "\n-----------------------\n" >> $OUTPUT_FILE
    echo "Test $test_number $test_name completed"
}

# Print initial results
print_initial_results

# Simple Command & Global Variables
run_test 1 "simple_command_ls" "ls\nexit\n"
run_test 2 "simple_command_empty" "\nexit\n"
run_test 3 "simple_command_spaces" "   \nexit\n"

# Arguments
run_test 4 "arguments_ls" "/bin/ls -l\nexit\n"
run_test 5 "arguments_grep" "/bin/grep root /etc/passwd\nexit\n"
run_test 6 "arguments_wc" "/bin/echo 'Hello World' | wc -w\nexit\n"

# Echo
run_test 7 "echo_simple" "echo Hello, World!\nexit\n"
run_test 8 "echo_n_option" "echo -n No newline\nexit\n"
run_test 9 "echo_multiple" "echo arg1 arg2 arg3\nexit\n"

# Exit
run_test 10 "exit_no_args" "exit\n"
run_test 11 "exit_with_args" "exit 42\n"
run_test 12 "exit_invalid_arg" "exit hello\n"

# Return value of a process
run_test 13 "return_value_ls" "/bin/ls\n echo \$?\nexit\n"
run_test 14 "return_value_invalid" "/bin/ls non_existing_file\n echo \$?\nexit\n"
run_test 15 "return_value_grep" "/bin/grep root /etc/passwd\n echo \$?\nexit\n"

# Double Quotes
run_test 16 "double_quotes" "echo \"This is a test\"\nexit\n"
run_test 17 "double_quotes_with_var" "echo \"Hello \$USER\"\nexit\n"
run_test 18 "double_quotes_empty" "echo \"\"\nexit\n"

# Single Quotes
run_test 19 "single_quotes" "echo 'This is a test'\nexit\n"
run_test 20 "single_quotes_with_var" "echo '\$USER'\nexit\n"
run_test 21 "single_quotes_empty" "echo ''\nexit\n"

# Env
run_test 22 "env_check" "env\nexit\n"
run_test 23 "env_grep_user" "env | grep USER\nexit\n"
run_test 24 "env_empty" "env -i\nexit\n"

# Export
run_test 25 "export_new_var" "export TEST_VAR=Hello\n env | grep TEST_VAR\nexit\n"
run_test 26 "export_override_var" "export TEST_VAR=Hello\nexport TEST_VAR=World\necho \$TEST_VAR\nexit\n"
run_test 27 "export_empty_var" "export EMPTY_VAR=\n env | grep EMPTY_VAR\nexit\n"

# Unset
run_test 28 "unset_var" "export TEST_VAR=Hello\n unset TEST_VAR\n env | grep TEST_VAR\nexit\n"
run_test 29 "unset_multiple_vars" "export VAR1=1 VAR2=2\n unset VAR1 VAR2\n env | grep VAR\nexit\n"
run_test 30 "unset_nonexistent_var" "unset NONEXISTENT_VAR\nexit\n"

# CD
run_test 31 "cd" "cd /tmp\n pwd\nexit\n"
run_test 32 "cd_invalid" "cd non_existing_dir\nexit\n"
run_test 33 "cd_home" "cd ~\n pwd\nexit\n"

# PWD
run_test 34 "pwd" "pwd\nexit\n"
run_test 35 "pwd_with_argument" "pwd oi\nexit\n"
run_test 36 "pwd_after_cd" "cd /tmp\n pwd\nexit\n"

# Relative Path
run_test 37 "relative_path" "./minishell\nexit\n"
run_test 38 "relative_path_cd" "cd ./minishell\n pwd\nexit\n"
run_test 39 "relative_path_invalid" "./non_existing_path\nexit\n"

# Environment Path
run_test 40 "env_path" "ls\nexit\n"
run_test 41 "unset_path" "unset PATH\n ls\nexit\n"
run_test 42 "set_path" "export PATH=/bin\n ls\nexit\n"

# Redirection
run_test 43 "redirection_output" "echo 'Hello' > testfile.txt\nexit\n cat testfile.txt\n rm testfile.txt\nexit\n"
run_test 44 "redirection_append" "echo 'World' >> testfile.txt\nexit\n cat testfile.txt\n rm testfile.txt\nexit\n"
run_test 45 "redirection_input" "echo 'Hello World' > testfile.txt\n cat < testfile.txt\n rm testfile.txt\nexit\n"

# Heredoc
run_test 46 "heredoc_simple" "cat <<EOF\nHello World\nEOF\nexit\n"
run_test 47 "heredoc_variable" "cat <<EOF\nHello \$USER\nEOF\nexit\n"
run_test 48 "heredoc_with_command" "cat <<EOF | grep Hello\nHello World\nEOF\nexit\n"

# Pipes
run_test 49 "pipe_simple" "ls | grep minishell\nexit\n"
run_test 50 "pipe_invalid" "ls non_existing_file | grep minishell\nexit\n"
run_test 51 "pipe_multiple" "ls | grep minishell | wc -l\nexit\n"

# History and Crazy Commands
run_test 52 "crazy_command" "dsbksdgbksdghsd\nexit\n"
run_test 53 "long_command" "echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11 arg12 arg13 arg14 arg15 arg16 arg17 arg18 arg19 arg20\nexit\n"
run_test 54 "command_with_special_chars" "echo '> >> < * ? [ ] | ; [ ] || && ( ) & # $ \\ <<'\nexit\n"

# Environment variables
run_test 55 "env_variable_echo" "echo \$USER\nexit\n"
run_test 56 "env_variable_double_quotes" "echo \"\$USER\"\nexit\n"
run_test 57 "env_variable_export" "export NEW_VAR=Test\n echo \$NEW_VAR\nexit\n"

# Additional test cases
run_test 58 "echo_hello_world" "echo hello world\nexit\n"
run_test 59 "echo_hello_world_quotes" "echo \"hello world\"\nexit\n"
run_test 60 "echo_hello_world_single_quotes" "echo 'hello world'\nexit\n"

# Print final results
echo -e "\n-----------------------\n" >> $OUTPUT_FILE
echo "Final results:" >> $OUTPUT_FILE
echo -e "Correct results: $correct\nIncorrect results: $incorrect" >> $OUTPUT_FILE

# Create a separate file with final statistics
echo "Final results:" >> $STATS_FILE
echo -e "Correct results: $correct\nIncorrect results: $incorrect" >> $STATS_FILE

echo "All tests completed. Results are in the $OUTPUT_FILE file."
