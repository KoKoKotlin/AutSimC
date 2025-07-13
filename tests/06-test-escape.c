#include <stdio.h>
#include <loader.h>

int main() {
    string s = "abc def \\e ff";
    string exp_res_format = "abc def %c ff";
    sb_t builder = { 0 };
    sb_extend(&builder, "abc def ");
    sb_append(&builder, SYM_EPS);
    sb_extend(&builder, " ff");
    sv_t exp_view = sb_build(&builder);
    sb_free(&builder);
    
    sv_t res_view = sv_from_cstr(s);
    replace_eps(&res_view);

    if (!sv_cmp_sv(&res_view, &exp_view)) {
        fprintf(stderr, "Format string replacement failed! "SV_FMT" != "SV_FMT"\n", 
            SV_ARG(exp_view), SV_ARG(res_view));
        exit(-1);
        sv_free(&res_view);
        sv_free(&exp_view);
    }

    sv_free(&res_view);
    sv_free(&exp_view);
    return 0;
}

