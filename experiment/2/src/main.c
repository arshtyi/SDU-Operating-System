#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int pipe_x[2];
int pipe_y[2];

typedef struct
{
    int x;
} FxArgs;

typedef struct
{
    int y;
} FyArgs;

void *thread_fx(void *arg)
{
    FxArgs *args = (FxArgs *)arg;
    int n = args->x;
    int result = 1;

    for (int i = 2; i <= n; ++i)
    {
        result *= i;
    }

    if (write(pipe_x[1], &result, sizeof(result)) != sizeof(result))
    {
        perror("thread f(x) write failed");
    }
    close(pipe_x[1]);

    printf("thread f(x): f(%d) = %d\n", n, result);
    return NULL;
}

void *thread_fy(void *arg)
{
    FyArgs *args = (FyArgs *)arg;
    int n = args->y;
    int result = 1;

    if (n > 2)
    {
        int a = 1;
        int b = 1;
        for (int i = 3; i <= n; ++i)
        {
            int c = a + b;
            a = b;
            b = c;
        }
        result = b;
    }

    if (write(pipe_y[1], &result, sizeof(result)) != sizeof(result))
    {
        perror("thread f(y) write failed");
    }
    close(pipe_y[1]);

    printf("thread f(y): f(%d) = %d\n", n, result);
    return NULL;
}

void *thread_fxy(void *arg)
{
    (void)arg;
    int fx = 0;
    int fy = 0;

    if (read(pipe_x[0], &fx, sizeof(fx)) != sizeof(fx))
    {
        perror("thread f(x,y) read f(x) failed");
        close(pipe_x[0]);
        close(pipe_y[0]);
        return NULL;
    }
    if (read(pipe_y[0], &fy, sizeof(fy)) != sizeof(fy))
    {
        perror("thread f(x,y) read f(y) failed");
        close(pipe_x[0]);
        close(pipe_y[0]);
        return NULL;
    }

    close(pipe_x[0]);
    close(pipe_y[0]);

    printf("thread f(x,y): f(x,y) = f(x) + f(y) = %d + %d = %d\n", fx, fy, fx + fy);
    return NULL;
}

int main()
{
    FxArgs x_args;
    FyArgs y_args;

    printf("Enter x, y: ");
    if (scanf("%d %d", &x_args.x, &y_args.y) != 2)
    {
        fprintf(stderr, "input error: please enter two integers.\n");
        return EXIT_FAILURE;
    }
    if (x_args.x < 1 || y_args.y < 1)
    {
        fprintf(stderr, "input error: x >= 1 and y >= 1 are required.\n");
        return EXIT_FAILURE;
    }

    if (pipe(pipe_x) < 0 || pipe(pipe_y) < 0)
    {
        perror("create pipe failed");
        exit(EXIT_FAILURE);
    }

    pthread_t thrdx;
    pthread_t thrdy;
    pthread_t thrdxy;

    if (pthread_create(&thrdx, NULL, thread_fx, &x_args) != 0)
    {
        perror("pthread_create f(x) failed");
        return EXIT_FAILURE;
    }
    if (pthread_create(&thrdy, NULL, thread_fy, &y_args) != 0)
    {
        perror("pthread_create f(y) failed");
        return EXIT_FAILURE;
    }
    if (pthread_create(&thrdxy, NULL, thread_fxy, NULL) != 0)
    {
        perror("pthread_create f(x,y) failed");
        return EXIT_FAILURE;
    }

    pthread_join(thrdx, NULL);
    pthread_join(thrdy, NULL);
    pthread_join(thrdxy, NULL);

    return 0;
}
