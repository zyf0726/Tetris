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
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "population.h"
#include "random.h"
#include "selection.h"
#include "tetromino.h"

#define PRINT_V(string, ...) if (opt.verbose) { printf(string, ##__VA_ARGS__); }

#define MASTER_RANK      0
#define CALC_PERM_TAG    1
#define FEAT_WEIGHTS_TAG 2
#define FEAT_ENABLED_TAG 3
#define RESULT_TAG       4
#define SHUTDOWN_TAG     4

struct options opt = {
    .n_features_enabled    = 0,
    .n_weights_enabled     = 0,

    .verbose               = 0,
    .population_size       = 20,
    .tournament_group_size = 10,
    .max_n_generations     = 100,
    .crossover_points      = 2,
    .elitism               = 2,
    .no_log                = 0,
    .no_change_duration    = 50,
    .reset_volume          = 0,
    .n_trials              = 10,
    .print_board           = 0,
    .n_piece_lookahead     = 0,
    .randomization_range   = 100,
    .mutation_range        = 100,

    .feature_enable_rate               = 1.f / 6.f,
    .mutation_rate                     = 0.995,
    .crossover_rate                    = 0.5,
    .tournament_group_random_selection = 0.1,

    .sel = SUS,
};

char* program_name;

int user_exit = 0;

// Used for MPI.
int zero = 0;
int one  = 1;


void sigint_handle (int signal) {
    printf("Interrupting..\n");
    user_exit = 1;
}

int all_trials (int * trials, struct options * opt) {
    for (int a = 0; a < opt->population_size - opt->elitism; a++) {
        if (trials[a] < opt->n_trials) {
            return 0;
        }
    }

    return 1;
}

int unfinished_individual (int * trials, struct options * opt) {
    for (int a = 0; a < opt->population_size - opt->elitism; a++) {
        if (trials[a] < opt->n_trials) {
            return a;
        }
    }
}

int main (int argc, char **argv) {
    clock_t begin = clock(),
            end;

    initalize_rng(&opt);

    program_name = argv[0];

    int log_directory_defined = 0;

    initialize_feature_helpers(&opt);

    for (int i = 1; i < argc; i++) {
        if (feature_exists(argv[i])) {
            enable_feature(feature_index(argv[i]), &opt);
        } else if (strcmp(argv[i], "--f-all") == 0) {
            for (int a = 0; a < N_FEATURES; a++) {
                enable_feature(a, &opt);
            }
        }
    }

    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER_RANK)
    {
        int n_workers;

        MPI_Comm_size(MPI_COMM_WORLD, &n_workers);

        if (opt.crossover_points > opt.n_features_enabled) {
            opt.crossover_points = opt.n_features_enabled;
        }

        if (!log_directory_defined && !opt.no_log) {
            printf("Missing log directory. Terminating.\n");
            return 1;
        }

        FILE * f;

        char time_format[50];
        char run_log_directory[200];
        char run_log_data_file[261];
        char run_log_solution_file[262];

        if (!opt.no_log) {
            time_t timer;
            struct tm* tm_info;
            time(&timer);
            tm_info = localtime(&timer);

            strftime(time_format, 50, "%Y-%m-%d-%H%M%S", tm_info);

            if (snprintf(run_log_directory, 200, "%s/%s", opt.log_directory, time_format) >= 200) {
                printf("Unable to allocate run log directory. Terminating.\n");
                return 1;
            }

            mkdir(run_log_directory, 0755);

            if (snprintf(run_log_data_file, 261, "%s/%s", run_log_directory, "fitness.dat") >= 261) {
                printf("Unable to allocate data log path. Terminating.\n");
                return 1;
            }

            if (snprintf(run_log_solution_file, 261, "%s/%s", run_log_directory, "solution.dat") >= 262) {
                printf("Unable to allocate solution log path. Terminating.\n");
                return 1;
            }

            PRINT_V("Results will be logged to %s.\n", run_log_data_file);

            f = fopen(run_log_data_file, "a");
        }

        struct population* children = initialize_population_pool(opt.population_size);
        struct population* parents = initialize_population_pool(opt.population_size);

        for (int i = 0; i < opt.population_size; i++) {
            children->individuals[i] = initialize_phenotype(initialize_genotype(&opt));
            parents->individuals[i] = initialize_phenotype(initialize_genotype(&opt));

            randomize_genotype(children->individuals[i]->gen, &opt);
        }

        PRINT_V("A random population has been initialized.\n\n");

        signal(SIGINT, sigint_handle);

        struct phenotype* winner = initialize_phenotype(initialize_genotype(&opt));
        winner->fitness = -1;

        unsigned long long total_cleared_lines = 0;

        for (int i = 0; (i < opt.max_n_generations || opt.max_n_generations == 0) && !user_exit; i++) {
            PRINT_V("Simulating generation %d.\n", i + 1);

            for (int a = 0; a < opt.population_size; a++) {
                free_phenotype(parents->individuals[a]);
            }

            swap_populations(&children, &parents);

            struct phenotype** parent_pairs = select_parent_pairs(parents, &opt);

            for (int a = 0; a < opt.population_size - opt.elitism; a++) {
                children->individuals[a] = mate_individuals(
                    parent_pairs[a * 2 + 0],
                    parent_pairs[a * 2 + 1],
                    &opt);
            }

            free(parent_pairs);

            int scheduled_trials_per_individual[opt.population_size - opt.elitism];
            int finished_trials_per_individual[opt.population_size - opt.elitism];
            int scheduled_individual_per_worker[n_workers];

            for (int a = 0; a < opt.population_size - opt.elitism; a++) {
                if (children->individuals[a]->has_fitness) {
                    scheduled_trials_per_individual[a] = opt.n_trials;
                    finished_trials_per_individual[a] = opt.n_trials;
                } else {
                    scheduled_trials_per_individual[a] = 0;
                    finished_trials_per_individual[a] = 0;
                }
            }

            while (!all_trials(finished_trials_per_individual, &opt)) {
                int request;

                MPI_Status status;

                MPI_Recv(
                    &request, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                    MPI_COMM_WORLD, &status);

                if (status.MPI_TAG == CALC_PERM_TAG) {
                    if (all_trials(scheduled_trials_per_individual, &opt)) {
                        MPI_Send(
                            &zero, 1, MPI_INT, status.MPI_SOURCE, CALC_PERM_TAG,
                            MPI_COMM_WORLD);
                    } else {
                        int individual = unfinished_individual(scheduled_trials_per_individual, &opt);

                        scheduled_trials_per_individual[individual]++;
                        scheduled_individual_per_worker[status.MPI_SOURCE] = individual;

                        MPI_Send(
                            &one, 1, MPI_INT, status.MPI_SOURCE, CALC_PERM_TAG,
                            MPI_COMM_WORLD);

                        MPI_Send(
                            children->individuals[individual]->gen->feature_weights,
                            opt.n_features_enabled, MPI_FLOAT, status.MPI_SOURCE,
                            FEAT_WEIGHTS_TAG, MPI_COMM_WORLD);

                        MPI_Send(
                            children->individuals[individual]->gen->feature_enabled,
                            opt.n_features_enabled, MPI_INT, status.MPI_SOURCE,
                            FEAT_ENABLED_TAG, MPI_COMM_WORLD);
                    }
                } else if (status.MPI_TAG == RESULT_TAG) {
                    int individual = scheduled_individual_per_worker[status.MPI_SOURCE];
                    finished_trials_per_individual[individual]++;
                    children->individuals[individual]->fitness += request;
                    total_cleared_lines += request;

                    if (finished_trials_per_individual[individual] == opt.n_trials) {
                        children->individuals[individual]->has_fitness = 1;
                        children->individuals[individual]->fitness /= opt.n_trials;
                    }
                }
            }

            if (opt.elitism) {
                //qsort(parents->individuals, parents->size, sizeof(struct phenotype*), compare_phenotypes);
                sort(parents->individuals, parents->individuals + parents->size, [](const phenotype*a, const phenotype* b) { return a->fitness < b->fitness; });
                for (int a = 0; a < opt.elitism; a++) {
                    children->individuals[opt.population_size - opt.elitism + a] = parents->individuals[opt.population_size - opt.elitism + a];

                    parents->individuals[opt.population_size - opt.elitism + a] = NULL;
                }
            }

            if (!opt.no_log) {
                for (int a = 0; a < opt.population_size; a++) {
                    fprintf(f, "%d", children->individuals[a]->fitness);

                    if (a == opt.population_size - 1) {
                        fprintf(f, "\n");
                    } else {
                        fprintf(f, "\t");
                    }
                }

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
        if (opt.verbose)
        {
            end = clock();

            double seconds = (double)(end - begin) / CLOCKS_PER_SEC;

            printf("Total cleared lines was %'llu.\n", total_cleared_lines);
            printf("Execution finished after %.2f seconds.\n", seconds);
            printf("This amounts to %.2f cleared lines per second.\n", total_cleared_lines / seconds);
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
    } else {
        while (1) {
            int result;

            MPI_Status status;

            MPI_Send(
                &one, 1, MPI_INT, MASTER_RANK, CALC_PERM_TAG, MPI_COMM_WORLD);

            MPI_Recv(
                &result, 1, MPI_INT, MASTER_RANK, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == CALC_PERM_TAG) {
                if (result) {
                    struct genotype * g = initialize_genotype(&opt);

                    MPI_Recv(
                        g->feature_weights, opt.n_features_enabled, MPI_FLOAT, MASTER_RANK, FEAT_WEIGHTS_TAG,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    MPI_Recv(
                        g->feature_enabled, opt.n_features_enabled, MPI_INT, MASTER_RANK, FEAT_ENABLED_TAG,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    struct phenotype * p = initialize_phenotype(g);

                    int fitness = phenotype_fitness(p, &opt);

                    MPI_Send(
                        &fitness, 1, MPI_INT, MASTER_RANK, RESULT_TAG, MPI_COMM_WORLD);

                    free_phenotype(p);
                } else {
                    sleep(1);
                }
            } else if (status.MPI_TAG == SHUTDOWN_TAG) {
                break;
            }
        }
    }

    MPI_Finalize();
}
