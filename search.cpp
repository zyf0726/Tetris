
#ifndef SINGLEFILE
#include "search.h"
#include "main.h"

#endif

float ans_g[7];
alternative best_alt_g;

half_game::half_game(const int count[], const game_board &m_gb, SHAPES _curr_type, phenotype *tempPhenotype)
: curr_type(shape_order_rev[_curr_type]), gb(m_gb)
{
    for (int i = 0; i < 7; ++i)
        type_count[i] = count[shape_order[i]];
    curPhenotype=tempPhenotype;
}
vector<int> half_game::get_valid_types() {
    auto x = minmax_element(type_count, type_count + 7);
    int m(*x.first), M(*x.second);
    vector<int> ret; ret.reserve(7);
    for (int i = 0; i < 7; ++i)
        if (type_count[i] < M || m > M - 2) ret.push_back(i);
    return ret;
}

half_game::half_game(const half_game &ori, int selected_type)
: curr_type(selected_type), gb(ori.gb)
{
    copy(ori.type_count, ori.type_count + 7, type_count);
    ++type_count[selected_type];
    curPhenotype=ori.curPhenotype;
}

INLINE half_game::half_game(const half_game &ori, const board &newgb)
    :gb(newgb), curr_type(ori.curr_type)// FIXME 额外多复制了一次（实际上每两层只需复制一次）
{
    copy(ori.type_count, ori.type_count + 7, type_count);
    curPhenotype=ori.curPhenotype;
}
constexpr int OUTPUT_DEPTH = 0;
template<int MAX_DEPTH> float search_for_type(half_game g, int depth)
{
    vector<int> tps = g.get_valid_types();
    float ans1 = FLT_MAX;

    for (int x : tps)
    {
        float ans_t = search_for_pos<MAX_DEPTH> (half_game(g, x), depth + 1);
        if (depth == OUTPUT_DEPTH) mint(ans_g[x], ans_t);
        mint(ans1, ans_t);
    }
    return ans1;
}; //depth == 0 : output;

template<int MAX_DEPTH> float search_for_pos(half_game g, int depth) //对敌方调用(..., -1)
{
    auto f = _look_ahead(&g.gb, g.curPhenotype, g.curr_type, &opt);
    if (f.size()==0){
        return -FLT_MAX / 2;
    }
    if (depth >= MAX_DEPTH)
    {
        alternative best_alt = *f.begin();
        return best_alt.static_score;
    } else
    {
        alternative best_alt{-1,-1,-1};
        float ans = -FLT_MAX;
        if (depth != OUTPUT_DEPTH)
        {
            for (const auto& x : f) if (maxt1(ans, x.dynamic_score
                               + search_for_type<MAX_DEPTH>(half_game(g, x.b), depth + 1)))
                    best_alt = x;
            return ans;

        } else
        {
            for (const auto& x : f)
                if(maxt1(ans, search_for_type<MAX_DEPTH>(half_game(g, x.b), depth + 1)))
                    best_alt=x;
            best_alt_g=best_alt;
            return ans;
        }
    }
}
template<int MAX_DEPTH> SHAPES worst_for_enemy(const game_manager &m, int subject, SHAPES last_type)
{
    for (int i = 0; i < 7; ++i) ans_g[i] = FLT_MAX;
    search_for_pos<MAX_DEPTH>(
            half_game(m.type_count[subject], m.gb[subject], last_type, m.gamePhenotypes[1-subject]), -1);
    return shape_order[min_element(ans_g, ans_g + 7) - ans_g];
}

template float search_for_pos<2>(half_game g, int depth); //对敌方调用(..., -1)
template SHAPES worst_for_enemy<1>(const game_manager &m, int subject, SHAPES last_type);
