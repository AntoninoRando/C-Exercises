int count_quotes(const char *input)
{
    int count = 0;

    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == '\\')
        {
            i++; // Skip next char
            continue;
        }

        if (input[i] == '"')
        {
            count++;
        }
    }

    return count;
}