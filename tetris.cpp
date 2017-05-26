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

#define PRINT_V(string, ...) if (opt.verbose) { fprintf(stderr, string, ##__VA_ARGS__); }

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

double elps(0);

void sigint_handle(int signal)
{
    printf("Interrupting..\n");
    user_exit = 1;
}



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
    char time_format[50];
    char run_log_directory[200];
    char run_log_data_file[261];

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
    elps = MPI_Wtime();
    for (int a = 0; a < opt.N; a++)
        free_phenotype(parents->in[a]);
    swap_populations(&children, &parents);
    phenotype **parent_pairs = select_parent_pairs(parents, &opt);
    for (int a = 0; a < opt.N; a++)
        children->in[a] = mate_individuals(
                parent_pairs[a * 2 + 0],
                parent_pairs[a * 2 + 1], &opt);
    free(parent_pairs);

    float *ptr = gene_buffer;
    for (auto x : *children)
        for (int i = 0; i < opt.n_features_enabled; ++i)
            *ptr++ = x->gen->feature_weights[i];
    PRINT_V("pre done.\n\n");
}

inline void master_post(int itera)
{

    fprintf(stderr, "It%d finished %lld\n", itera, f);
        fprintf(f, "------%d------\n", itera);
        for (int a = 0; a < opt.N; a++)
        {
            fprintf(f, "%d\t", children->in[a]->fitness = fitness_buffer[a]);
            children->in[a]->gen->write(f, &opt);
        }
        game_manager g(RAND() % 7, 0, children->in[RAND() % opt.N]->gen->feature_weights,
                       children->in[RAND() % opt.N]->gen->feature_weights, opt);
        g.auto_game<0, true>();
        fflush(f);
    fprintf(stderr, "time = %fs", MPI_Wtime() - elps);
}


int sp_si, c_begin, c_end;

int main(int argc, char **argv)
{

    opt.n_features_enabled = 0,
    opt.n_weights_enabled = 0,

    opt.verbose = 1,
    opt.N = 50,
    opt.tournament_group_size = 10,
    opt.max_n_generations = 1000,
    opt.crossover_points = 2,
    opt.no_change_duration = 50,
    opt.reset_volume = 0,
    opt.n_trials = 10,
    opt.print_board = 0,
    opt.n_piece_lookahead = 0,
    opt.randomization_range = 100,
    opt.mutation_stdev = .1,
    opt.crossover_rate = 0.5,
    opt.tournament_group_random_selection = 0.1,
    opt.log_directory = "/Users/admin/logs",
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
    PRINT_V("nworkers is %d cbegin is %d cend is %d rank is %d\n", n_workers, c_begin, c_end, rank);
    signal(SIGINT, sigint_handle);
    IFMA master_init();
    for (int i = 0; (i < opt.max_n_generations || opt.max_n_generations == 0) && !user_exit; i++)
    {
        IFMA master_pre();
        fill(fitness_buffer, fitness_buffer + opt.N, 0);
        IFMA PRINT_V("bcast begin.\n\n");
        MPI_Bcast(gene_buffer, opt.N * opt.n_features_enabled, MPI_FLOAT, MASTER_RANK, MPI_COMM_WORLD);
        IFMA PRINT_V("bcast done. cal begin\n\n");
        static int fitness_buffer_tmp[MAXN];
        fill(fitness_buffer_tmp, fitness_buffer_tmp + opt.N, 0);
        for (pair<int, int> *i = C2ij + c_begin; i != C2ij + c_end; ++i)
        {
            float *kw1 = gene_buffer + i->first * opt.n_features_enabled,
                    *kw2 = gene_buffer + i->second * opt.n_features_enabled;
            for (int t = 0; t < 7; ++t)
            {
                game_manager g(t, 0, kw1, kw2, opt);
                fitness_buffer_tmp[g.auto_game<0>() == g.enemyColor ? i->second : i->first]++;
            }
        }

        IFMA PRINT_V("reduce begin\n\n");
        MPI_Reduce(fitness_buffer_tmp, fitness_buffer, opt.N, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        IFMA PRINT_V("reduce done\n\n");
        IFMA master_post(i);
    }

    MPI_Finalize();
}

