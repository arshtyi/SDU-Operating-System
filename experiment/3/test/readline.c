#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main()
{
    char *input;

    rl_attempted_completion_function = NULL;
    using_history();

    while (1)
    {
        input = readline("\033[1;34m>> ");
        if (input == NULL)
        {
            puts("\nEOF");
            break;
        }

        if (input[0] != '\0')
        {
            add_history(input);
        }

        printf("You entered: %s\n", input);

        free(input);
    }

    return 0;
}
