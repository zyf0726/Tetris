#ifndef SINGLEFILE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>
#include <mpi.h>
#include <unistd.h>

#include "board.h"
#include "game_manager.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "population.h"
#include "random.h"
#include "selection.h"
#include "tetromino.h"

#endif

#define PRINT_V(string, ...) if (opt.verbose) { printf(string, ##__VA_ARGS__); }

#define MASTER_RANK      0
#define CALC_PERM_TAG    1
#define FEAT_WEIGHTS_TAG 2
#define FEAT_ENABLED_TAG 3
#define RESULT_TAG       4
#define SHUTDOWN_TAG     4

enum tags
{
    ROUND_OVER_TAG = 8,

};

options opt;
char *program_name;

int user_exit = 0;

// Used for MPI.
int zero = 0;
int one = 1;

void print_help_text()
{
    printf(
            "usage: %s [options]\n"
                    "Options:\n"
                    "  -h or --help        show this help message and exit\n"
                    "  -v or --verbose\n"
                    "  -i                  read problem instance from standard input\n"
                    "  --pupulation-size N (defaults to 20)\n"
                    "  --tournament-group-size N (defaults to 10)\n"
                    "  --tournament-group-random-selection P (defaults to 0.1)\n"
                    "                      Don't select the best individual in the tournament\n"
                    "                      with a probability of P. A low P will increase\n"
                    "                      selection pressure, while a high P will decrease it.\n"
                    "  --max-n-generations N (defaults to 100)\n"
                    "  --elitism N (defaults to 0)\n"
                    "                      keep the N best individuals for next generation\n"
                    "  --mutation-rate F (defaults to 0.995)\n"
                    "  --crossover-rate F (defaults to 0.5)\n"
                    "  --crossover-points N (defaults to 2)\n"
                    "                      set equal to genotype size to do uniform crossover\n"
                    "  --selection {TOURNAMENT, SUS} (defaults to TOURNAMENT)\n"
                    "                      which method of parent selection to use\n"
                    "  --no-change-duration N (defaults to 50)\n"
                    "                      If above zero, the EA will reset itself if no change\n"
                    "                      in the winning individual has occurred. The number\n"
                    "                      of individuals to be reset is given by --reset-volume.\n"
                    "  --reset-volume N (defaults to 0)\n"
                    "                      The number of individuals to reset if no change has\n"
                    "                      happened during the no-change-duration. If set to 0,\n"
                    "                      all individuals will be reset.\n"
                    "  --n-trials N (defaults to 10)\n"
                    "                      The number of trials to run in order to determine the\n"
                    "                      average fitness score of an individual.\n"
                    "  --n-piece-lookahead N (defaults to 0)\n"
                    "                      The number of tetrominos that the controller will see\n"
                    "                      ahead of the current tetromino. Higher means that the\n"
                    "                      controller can take more informed choices, but it will\n"
                    "                      result in significantly higher computation times.\n"
                    "  --randomization-range N (defaults to 100)\n"
                    "                      Determines the range of value a weight can obtain during\n"
                    "                      randomization which occurs at initialization.\n"
                    "  --mutation-range n (defaults to 100)\n"
                    "                      Determines the range of value that is added to a weight\n"
                    "                      during mutation.\n"
                    "  --feature-enable-rate F (defaults to 1 / 6)\n"
                    "                      Determines the probability of a feature to be enabled\n"
                    "                      during randomization and mutation. The reason for a\n"
                    "                      seemingly low number is that there is quite a lot of\n"
                    "                      features.\n"
                    "  -l or --log-dir     specify the location for run logs\n"
                    "  --no-log            do not log results (-i or --no-log needs to be defined)\n"
                    "\n"
                    "Additionally, the following arguments can be used to enable features.\n"
                    "\n"
                    "  --f-all             enables all features\n",
            program_name
    );

    for (int i = 0; i < N_FEATURES; i++)
    {
        printf("  %s\n", features[i].name);
    }
}

void sigint_handle(int signal)
{
    printf("Interrupting..\n");
    user_exit = 1;
}

int all_trials(int *trials, struct options *opt)
{
    for (int a = 0; a < opt->N - opt->elitism; a++)
    {
        if (trials[a] < opt->n_trials)
        {
            return a;
        }
    }

    return -1;
}

bool PRINT_FLAG;

inline constexpr int C2(int a) { return a * (a - 1) / 2; }


population *parents(0), *children(0);
int n_workers;
const int MAXN = 500, MAXF = MAXN * N_FEATURES;

pair<int, int> C2ij[MAXN * MAXN];

float gene_buffer[MAXF];
int fitness_buffer[MAXN];
FILE *f(0);

INLINE void master_init()
{
    vector<pair<int, int> > schu(n_workers);
    mint(opt.crossover_points, opt.n_features_enabled);
    FILE *f;
    char time_format[50];
    char run_log_directory[200];
    char run_log_data_file[261];

    if (!opt.no_log)
    {
        time_t timer;
        struct tm *tm_info;
        time(&timer);
        tm_info = localtime(&timer);

        strftime(time_format, 50, "%Y-%m-%d-%H%M%S", tm_info);

        if (snprintf(run_log_directory, 200, "%s/%s", opt.log_directory, time_format) >= 200)
        {
            printf("Unable to allocate run log directory. Terminating.\n");
            throw 233;
        }

        mkdir(run_log_directory, 0755);

        if (snprintf(run_log_data_file, 261, "%s/%s", run_log_directory, "sol.dat") >= 261)
        {
            printf("Unable to allocate data log path. Terminating.\n");
            throw 233;
        }

        f = fopen(run_log_data_file, "a");
        if (!f)
        {
            puts(run_log_data_file);
            assert(0);
        }
    }

    children = initialize_population_pool(opt.N);
    parents = initialize_population_pool(opt.N);

    for (int i = 0; i < opt.N; i++)
    {
        children->in[i] = initialize_phenotype(initialize_genotype(&opt));
        parents->in[i] = initialize_phenotype(initialize_genotype(&opt));

        randomize_genotype(children->in[i]->gen, &opt);
    }
    PRINT_V("A random population has been initialized.\n\n");
}

inline void master_pre()
{

    for (int a = 0; a < opt.N; a++)
        free_phenotype(parents->in[a]);
    swap_populations(&children, &parents);
    phenotype **parent_pairs = select_parent_pairs(parents, &opt);
    for (int a = 0; a < opt.N - opt.elitism; a++)
        children->in[a] = mate_individuals(
                parent_pairs[a * 2 + 0],
                parent_pairs[a * 2 + 1], &opt);
    free(parent_pairs);

    float *ptr = gene_buffer;
    for (auto x : *children)
        for (int i = 0; i < opt.n_features_enabled; ++i)
            *ptr++ = x->gen->feature_weights[i];
}

inline void master_post(int itera)
{
    if (opt.elitism)
    {
        sort(parents->in, parents->in + parents->size,
             [](const phenotype *a, const phenotype *b) { return a->fitness < b->fitness; });
        for (int a = opt.N - opt.elitism; a < opt.N; a++)
        {
            (children->in[a] = parents->in[a])->fitness = 0;
            parents->in[a] = NULL;
        }
    }

    fprintf(stderr, "It%d finished %lld\n", itera, f);
    if (!opt.no_log)
    {
        fprintf(f, "------%d------\n", itera);
        for (int a = 0; a < opt.N; a++)
        {
            fprintf(f, "%d\t", children->in[a]->fitness);
            children->in[a]->gen->write(f, &opt);
        }
        PRINT_FLAG = true;
        game_manager g(RAND() % 7, 0, children->in[opt.N - opt.elitism]->gen->feature_weights,
                       children->in[opt.N - opt.elitism + 1]->gen->feature_weights, opt);
        g.auto_game<0>();

        fflush(f);
    }

}


int sp_si, c_begin, c_end;

int main(int argc, char **argv)
{

    opt.n_features_enabled = 0,
    opt.n_weights_enabled = 0,

    opt.verbose = 0,
    opt.N = 64,
    opt.tournament_group_size = 10,
    opt.max_n_generations = 1000,
    opt.crossover_points = 2,
    opt.elitism = 2,
    opt.no_log = 0,
    opt.no_change_duration = 50,
    opt.reset_volume = 0,
    opt.n_trials = 10,
    opt.print_board = 0,
    opt.n_piece_lookahead = 0,
    opt.randomization_range = 100,
    opt.mutation_range = 100,

    opt.feature_enable_rate = 1.f / 6.f,
    opt.mutation_rate = 0.995,
    opt.crossover_rate = 0.5,
    opt.tournament_group_random_selection = 0.1,
    opt.log_directory = "/Users/admin",
    opt.no_log = false,
    opt.sel = SUS;


    initialize_feature_helpers(&opt);
#define EF(x) enable_feature(feature_index(x), &opt);
    EF("--f-n-holes");
    EF("--f-landing-height");
    EF("--f-eroded-piece-cells");
    EF("--f-row-transitions");
    EF("--f-column-transitions");
    EF("--f-cumulative-wells-fast");
    EF("--f-max-height");
    EF("--f-n-rows-with-holes");


    for (int a = 0, cnt = 0; a < opt.N; ++a)
        for (int b = a + 1; b < opt.N; ++b)
            C2ij[cnt++] = make_pair(a, b);

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_workers);
    if (n_workers <= 1)
    {
        fprintf(stderr, "n_worker must be more than 2");
        throw 233;
    }
    sp_si = C2(opt.N) / (n_workers - 1);
#define IFMA if (rank == MASTER_RANK)

    c_begin = (n_workers - 1 - rank) * sp_si;
    c_end = min(c_begin + sp_si, C2(opt.N));

    signal(SIGINT, sigint_handle);
    IFMA master_init();
    for (int i = 0; (i < opt.max_n_generations || opt.max_n_generations == 0) && !user_exit; i++)
    {
        IFMA master_pre();
        fill(fitness_buffer, fitness_buffer + opt.N, 0);
        MPI_Bcast(gene_buffer, opt.N * opt.n_features_enabled, MPI_FLOAT, MASTER_RANK, MPI_COMM_WORLD);
        static int fitness_buffer_tmp[MAXN];
        fill(fitness_buffer_tmp, fitness_buffer_tmp + opt.N, 0);
        for (pair<int, int> *i = C2ij + c_begin; i != C2ij + c_end; ++i)
        {
            float *kw1 = gene_buffer + i->first * opt.n_features_enabled,
                    *kw2 = gene_buffer + i->second * opt.n_features_enabled;
            for (int t = 0; t < 7; ++t)
            {
                game_manager g(t, 0, kw1, kw2, opt);
                fitness_buffer[g.auto_game<0>() == g.enemyColor ? i->second : i->first]++;
            }
        }

        MPI_Reduce(fitness_buffer_tmp, fitness_buffer, opt.N, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

        IFMA master_post(i);
    }

    MPI_Finalize();
}

