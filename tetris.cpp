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

options opt;
char *program_name;

int user_exit = 0;

// Used for MPI.
int zero = 0;
int one = 1;

void print_help_text() {
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

    for (int i = 0; i < N_FEATURES; i++) {
        printf("  %s\n", features[i].name);
    }
}

void sigint_handle(int signal) {
    printf("Interrupting..\n");
    user_exit = 1;
}

int all_trials(int *trials, struct options *opt) {
    for (int a = 0; a < opt->population_size - opt->elitism; a++) {
        if (trials[a] < opt->n_trials) {
            return a;
        }
    }

    return -1;
}

bool PRINT_FLAG;
void master_process();

void satellite_process();

clock_t start_time = clock();

inline constexpr int C2(int a) { return a * (a - 1) / 2; }
int main(int argc, char **argv) {

 opt      .n_features_enabled    = 0,
 opt      .n_weights_enabled     = 0,

 opt      .verbose               = 0,
 opt      .population_size       = 20,
 opt      .tournament_group_size = 10,
 opt      .max_n_generations     = 100,
 opt      .crossover_points      = 2,
 opt      .elitism               = 2,
 opt      .no_log                = 0,
 opt      .no_change_duration    = 50,
 opt      .reset_volume          = 0,
 opt      .n_trials              = 10,
 opt      .print_board           = 0,
 opt      .n_piece_lookahead     = 0,
 opt      .randomization_range   = 100,
 opt      .mutation_range        = 100,

 opt      .feature_enable_rate               = 1.f / 6.f,
 opt      .mutation_rate                     = 0.995,
 opt      .crossover_rate                    = 0.5,
 opt      .tournament_group_random_selection = 0.1,
 opt      .log_directory = "/home/prwang/logs",
 opt      .no_log  = false,
 opt      .sel = SUS;

    int rank, is_master = false;
    initalize_rng(&opt);

    program_name = argv[0];

    int log_directory_defined = 0;

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-v") == 0 ||
            strcmp(argv[i], "--verbose") == 0) {
            opt.verbose = 1;
        }
        else if (strcmp(argv[i], "--master") == 0)
        {
            is_master = true;
        } else if (strcmp(argv[i], "--population-size") == 0) {
            opt.population_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--tournament-group-size") == 0) {
            opt.tournament_group_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--tournament-group-random-selection") == 0) {
            opt.tournament_group_random_selection = atof(argv[++i]);
        } else if (strcmp(argv[i], "--max-n-generations") == 0) {
            opt.max_n_generations = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--mutation-rate") == 0) {
            opt.mutation_rate = atof(argv[++i]);
        } else if (strcmp(argv[i], "--crossover-rate") == 0) {
            opt.crossover_rate = atof(argv[++i]);
        } else if (strcmp(argv[i], "--crossover-points") == 0) {
            opt.crossover_points = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--selection") == 0) {
            i++;
            if (strcmp(argv[i], "TOURNAMENT") == 0) {
                opt.sel = TOURNAMENT;
            } else if (strcmp(argv[i], "SUS") == 0) {
                opt.sel = SUS;
            } else if (strcmp(argv[i], "SIGMA") == 0) {
                opt.sel = SIGMA;
            } else {
                printf("Unknown option for selection '%s'.\n", argv[i]);
                throw 233;
            }
        } else if (strcmp(argv[i], "--no-change-duration") == 0) {
            opt.no_change_duration = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--reset-volume") == 0) {
            opt.reset_volume = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--n-trials") == 0) {
            opt.n_trials = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--n-piece-lookahead") == 0) {
            opt.n_piece_lookahead = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--randomization-range") == 0) {
            opt.randomization_range = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--mutation-range") == 0) {
            opt.mutation_range = atoi(argv[++i]);
        }
    }

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



    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER_RANK)
        master_process();
    else satellite_process();
    MPI_Finalize();
}

void master_process() {
    int n_workers;
    MPI_Comm_size(MPI_COMM_WORLD, &n_workers);
    vector<pair<int, int> > schu(n_workers);

    mint(opt.crossover_points, opt.n_features_enabled);
    FILE *f;
    char time_format[50];
    char run_log_directory[200];
    char run_log_data_file[261];
    char run_log_solution_file[262];

    if (!opt.no_log) {
        time_t timer;
        struct tm *tm_info;
        time(&timer);
        tm_info = localtime(&timer);

        strftime(time_format, 50, "%Y-%m-%d-%H%M%S", tm_info);

        if (snprintf(run_log_directory, 200, "%s/%s", opt.log_directory, time_format) >= 200) {
            printf("Unable to allocate run log directory. Terminating.\n");
            throw 233;
        }

        mkdir(run_log_directory, 0755);

        if (snprintf(run_log_data_file, 261, "%s/%s", run_log_directory, "fitness.dat") >= 261) {
            printf("Unable to allocate data log path. Terminating.\n");
            throw 233;
        }

        if (snprintf(run_log_solution_file, 261, "%s/%s", run_log_directory, "solution.dat") >= 262) {
            printf("Unable to allocate solution log path. Terminating.\n");
            throw 233;
        }

        PRINT_V("Results will be logged to %s.\n", run_log_data_file);

        f = fopen(run_log_data_file, "a");
    }

    struct population *children = initialize_population_pool(opt.population_size);
    struct population *parents = initialize_population_pool(opt.population_size);

    for (int i = 0; i < opt.population_size; i++) {
        children->individuals[i] = initialize_phenotype(initialize_genotype(&opt));
        parents->individuals[i] = initialize_phenotype(initialize_genotype(&opt));

        randomize_genotype(children->individuals[i]->gen, &opt);
    }

    PRINT_V("A random population has been initialized.\n\n");

    signal(SIGINT, sigint_handle);

    struct phenotype *winner = initialize_phenotype(initialize_genotype(&opt));
    winner->fitness = -1;
    int N = opt.population_size - opt.elitism;
    vector<pair<int, int> > C2ij; C2ij.reserve(C2(N));
    for (int a = 0; a < N; ++a)
        for (int b = a + 1; b < N; ++b)
            C2ij.push_back(make_pair(a, b));
    for (int i = 0; (i < opt.max_n_generations || opt.max_n_generations == 0) && !user_exit; i++) {
        PRINT_V("Simulating generation %d.\n", i + 1);
        for (int a = 0; a < opt.population_size; a++)
            free_phenotype(parents->individuals[a]);
        swap_populations(&children, &parents);
        phenotype **parent_pairs = select_parent_pairs(parents, &opt);
        for (int a = 0; a < opt.population_size - opt.elitism; a++)
            children->individuals[a] = mate_individuals(
                    parent_pairs[a * 2 + 0],
                    parent_pairs[a * 2 + 1],
                    &opt);
        free(parent_pairs);
        int curSched = 0, curFinished = 0;
        while (curFinished < C2(N))
        {
            pair<int, int> request;
            MPI_Status status;
            MPI_Recv(&request, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == CALC_PERM_TAG) {
                if (curSched >= C2(N)) {
                    MPI_Send(
                            &zero, 1, MPI_INT, status.MPI_SOURCE, CALC_PERM_TAG,
                            MPI_COMM_WORLD);
                } else {
                    pair<int, int> p = C2ij[curSched++];
                    schu[status.MPI_SOURCE] = p;
                    MPI_Send( &one, 1, MPI_INT, status.MPI_SOURCE, CALC_PERM_TAG,
                            MPI_COMM_WORLD);
                    MPI_Send( children->individuals[p.first]->gen->feature_weights,
                            opt.n_features_enabled, MPI_FLOAT, status.MPI_SOURCE,
                            FEAT_WEIGHTS_TAG, MPI_COMM_WORLD);
                    MPI_Send( children->individuals[p.second]->gen->feature_weights,
                            opt.n_features_enabled, MPI_FLOAT, status.MPI_SOURCE,
                            FEAT_WEIGHTS_TAG, MPI_COMM_WORLD);
                }
            } else if (status.MPI_TAG == RESULT_TAG) {
                pair<int, int> match0  = schu[status.MPI_SOURCE];
                children->individuals[match0.first]->fitness += request.first;
                children->individuals[match0.second]->fitness += request.second;
                curFinished++;
            }
        }

        if (opt.elitism) {
            sort(parents->individuals, parents->individuals + parents->size,
                 [](const phenotype *a, const phenotype *b) { return a->fitness < b->fitness; });
            for (int a = 0; a < opt.elitism; a++) {
                children->individuals[opt.population_size - opt.elitism + a] = parents->individuals[
                        opt.population_size - opt.elitism + a];
                parents->individuals[opt.population_size - opt.elitism + a] = NULL;
            }
        }

        fprintf(stderr, "It%d finished", i);
        if (!opt.no_log) {
            fprintf(f, "------%d------\n", i);
            for (int a = 0; a < opt.population_size; a++) {
                fprintf(f, "%d\t", children->individuals[a]->fitness);
                children->individuals[a]->gen->write(f, &opt);
            }
            PRINT_FLAG = true;
            game_manager g(RAND() % 7, 0, children->individuals[opt.population_size - opt.elitism]->gen->feature_weights,
                           children->individuals[opt.population_size - opt.elitism + 1]->gen->feature_weights, opt);
            g.auto_game<0>();

            fflush(f);
        }

        struct phenotype generational_winner = {.fitness = -1};

        for (int a = 0; a < opt.population_size; a++) {
            if (children->individuals[a]->fitness > generational_winner.fitness) {
                generational_winner = *children->individuals[a];
            }
        }

        if (generational_winner.fitness > winner->fitness) {
            free_phenotype(winner);
            winner = copy_phenotype(&generational_winner, &opt);
        }

        PRINT_V("The best phenotype has a fitness value of %d.\n", generational_winner.fitness);

        if (opt.verbose) {
            write_phenotype(stdout, &generational_winner, &opt);
        }
    }

    if (!opt.no_log) {
        fclose(f);

        f = fopen(run_log_solution_file, "a");
        write_phenotype(f, winner, &opt);
        fclose(f);
    }
    if (opt.verbose) {

        double seconds = (double) (clock() - start_time) / CLOCKS_PER_SEC;

        printf("Execution finished after %.2f seconds.\n", seconds);
    }

    for (int i = 1; i < n_workers; i++) {
        MPI_Status status;

        MPI_Recv(
                &one, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status);

        MPI_Send(
                &one, 1, MPI_INT, status.MPI_SOURCE, SHUTDOWN_TAG,
                MPI_COMM_WORLD);
    }
}

void satellite_process() {
    int cnt = 0;
    while (++cnt)
    {
        int can_continue;
        PRINT_FLAG = false;
        MPI_Status status;

        MPI_Send(&one, 1, MPI_INT, MASTER_RANK, CALC_PERM_TAG, MPI_COMM_WORLD);

        MPI_Recv(&can_continue, 1, MPI_INT, MASTER_RANK, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == CALC_PERM_TAG) {
            if (can_continue)
            {
                float wg1[opt.n_features_enabled], wg2[opt.n_features_enabled];
                MPI_Recv(wg1, opt.n_features_enabled, MPI_FLOAT, MASTER_RANK, FEAT_WEIGHTS_TAG,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(wg2, opt.n_features_enabled, MPI_FLOAT, MASTER_RANK, FEAT_WEIGHTS_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                pair<int, int> ret(0, 0);
                for (int i = 0; i < 7; ++i)
                {
                    game_manager g(i, 0, wg1, wg2, opt);
                    g.auto_game<0>() == g.enemyColor ? ret.second++ : ret.first++;
                }

                MPI_Send(&ret, 2, MPI_INT, MASTER_RANK, RESULT_TAG, MPI_COMM_WORLD);
            } else {
                sleep(1);
            }
        } else if (status.MPI_TAG == SHUTDOWN_TAG) {
            break;
        }
    }
}
