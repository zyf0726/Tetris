//
// Created by prwang on 2017/4/22.
//
#ifndef SINGLEFILE

#include "main.h"
#include "feature_helpers.h"

#endif

int currBotColor;
int enemyColor;
bool PRINT_FLAG = true;
/*
 Feature Value
3. Holes -35
6. Landing height -51
7. Cell transitions -46
8. Deep wells -12
10. Height differences 19
11. Mean height 6
12. ∆ max height 50
13. ∆ holes 25
15. ∆ mean height 17
18. Wells -19
21. Row transitions -38
23. Cumulative wells -42
24. Min height -41
26. Mean - min height -60
 29. Adjacent column holes -155
 */

options opt = {};
phenotype *global_phenotype;

int main() {
#ifndef SINGLEFILE
    freopen("in.txt", "r", stdin);
#endif
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

    float fwt[] = {5, -33000, 9, -37, -76, -31, -.25f, -65};
    float fwt2[] = {5, -33, 9, -37, -76, -31, -25, -65};
    game_manager g(1, 0, fwt, fwt2, opt);
    istream::sync_with_stdio(false);



    clog << (g.auto_game<0>() == g.enemyColor ? "enemy win" : "cur win") << endl;

    return 0;
}
