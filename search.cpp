
#ifndef SINGLEFILE
#include "search.h"
#include "main.h"

#endif

float ans_g[7];  //TODO 初始化
alternative best_alt_g; //TODO 初始化

half_game::half_game(const game_manager &m, int subject, SHAPES _curr_type)
: curr_type(shape_order_rev[_curr_type]), gb(m.gb[subject])
{
    copy(m.type_count[subject], m.type_count[subject] + 7, type_count);
}
vector<int> half_game::get_valid_types() {
    auto x = minmax_element(type_count, type_count + 7);
    int m(*x.first), M(*x.second);
    vector<int> ret; ret.reserve(7);
    for (int i = 0; i < 7; ++i)
        if (type_count[i] < M || m > M - 2) ret.push_back(i);
    return ret;
}

half_game::half_game(const half_game& ori, int selected_type)
: curr_type(selected_type), gb(ori.gb)
{
    copy(ori.type_count, ori.type_count + 7, type_count);
    --type_count[selected_type];
}

INLINE half_game::half_game(const half_game& ori, const board& newgb)
    :gb(newgb), curr_type(ori.curr_type)// FIXME 额外多复制了一次（实际上每两层只需复制一次）
{
    copy(ori.type_count, ori.type_count + 7, type_count);
}
constexpr int OUTPUT_DEPTH = 0;
constexpr int MAX_DEPTH = 1;
constexpr size_t MAX_SOLUTIONS = 6;
float search_for_type(half_game g, int depth)
{
    vector<int> tps = g.get_valid_types();
    float ans1 = FLT_MAX;

    for (int x : tps)
    {
        float ans_t = search_for_pos(half_game(g, x), depth);
        if (depth == OUTPUT_DEPTH) mint(ans_g[x], ans_t);
        mint(ans1, ans_t);
    }
    return ans1;
}; //depth == 0 : output;

float search_for_pos(half_game g, int depth) //对敌方调用(..., -1)
{
    auto f = _look_ahead(&g.gb, global_phenotype, g.curr_type, &global_option);
    if (depth == MAX_DEPTH)
    {
        alternative best_alt = *max_element(f.begin(), f.end());
        return best_alt.score;
    } else
    {
        size_t sol_size = min(MAX_SOLUTIONS, f.size());
        partial_sort(f.begin(), f.begin() + sol_size, f.end(),
                     [](const alternative& a, const alternative& b)
            { return a.score > b.score; });

        f.resize(sol_size);
        alternative best_alt;
        float ans = -FLT_MAX;
        for (const auto& x : f)
            if (maxt1(ans, search_for_type(half_game(g, x.b), depth + 1)))
                best_alt = x;
        if (depth == OUTPUT_DEPTH)
            best_alt_g = best_alt;
        return ans;
    }
}
SHAPES worst_for_enemy(const game_manager &m, int subject, SHAPES last_type)
{

    for (int i = 0; i < 7; ++i) ans_g[i] = FLT_MAX;
    search_for_pos(half_game(m, subject,  last_type), -1);
    return shape_order[min_element(ans_g, ans_g + 7) - ans_g];
}

