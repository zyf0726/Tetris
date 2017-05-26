
#ifndef SINGLEFILE
#include "search.h"
#include "main.h"

#endif

float best_for_type[7]; bool type_ok[7];
alternative best_alt_g;

half_game::half_game(const int count[], const game_board &m_gb, SHAPES _curr_type, phenotype *tempPhenotype)
: curr_type(shape_order_rev[_curr_type]), gb(m_gb)
{
    for (int i = 0; i < 7; ++i)
        type_count[i] = count[shape_order[i]];
    curPhenotype=tempPhenotype;
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
int first_type(const int* type_count)
{
    auto x = minmax_element(type_count, type_count + 7);
    int m(*x.first), M(*x.second);
    for (int i = 0; i < 7; ++i)
        if (type_count[i] < M || m > M - 2)
            return i;
}
template<int MAX_DEPTH> float search_for_type(half_game g, int depth)
{
    float ans1 = FLT_MAX;
    auto x = minmax_element(g.type_count, g.type_count + 7);
    int m(*x.first), M(*x.second);
    for (int i = 0; i < 7; ++i)
        if (g.type_count[i] < M || m > M - 2)
    {
        if (depth == OUTPUT_DEPTH) type_ok[i] = true;
        mint(ans1, search_for_pos<MAX_DEPTH> (half_game(g, i), depth + 1));
        if (depth == OUTPUT_DEPTH) maxt(best_for_type[i], ans1);
    }
    return ans1;
}; //depth == 0 : output;

template<int MAX_DEPTH> float search_for_pos(half_game g, int depth) //对敌方调用(..., -1)
{
    auto f = _look_ahead(&g.gb, g.curPhenotype, g.curr_type, &opt);
    if (f.size()==0){
        return -FLT_MAX / 2;
    }
    float ans = -FLT_MAX;
    if (depth >= MAX_DEPTH)
    {
        alternative best_alt = *f.begin();
        if (depth == OUTPUT_DEPTH)
        {
            for (const auto& x : f)
                if(maxt1(ans, x.static_score + x.dynamic_score))
                    best_alt=x;
            best_alt_g=best_alt;
        } return best_alt.static_score;
    } else
    {
        alternative best_alt{-1,-1,-1};
        if (depth != OUTPUT_DEPTH)
        {
            for (const auto& x : f)
                maxt(ans,  search_for_type<MAX_DEPTH>(half_game(g, x.b), depth + 1));
            return ans;

        } else
        {
            for (const auto& x : f)
                if(maxt1(ans, x.dynamic_score + search_for_type<MAX_DEPTH>(half_game(g, x.b), depth + 1)))
                    best_alt=x;
            best_alt_g=best_alt;
            return ans;
        }
    }
}

template float search_for_pos<2>(half_game g, int depth); //对敌方调用(..., -1)
template float search_for_pos<1>(half_game g, int depth); //对敌方调用(..., -1)
template float search_for_pos<0>(half_game g, int depth); //对敌方调用(..., -1)
