int check_quotes(const char *input, int input_len)
{
    if (!input || input_len == 0)
    {
        return 0;
    }

    int count = (input[0] == '"'); // Il primo va fatto manualmente perche' nel ciclo si accede all'(i-1)-esimo elemento
    for (int i = 1; i < input_len; i++)
    {
        count += (input[i - 1] != '\\' && input[i] == '"'); // Ignore \" char
    }
    return count % 2;
}