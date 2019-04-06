#include "../word_analysis/check_word.h"
#include "../whole_value.h"
#include "../enum_code.h"
#include "syntax.h"

#include <stdio.h>
#include <stdlib.h>

void
syntax_indent()
{
        switch (syntax_state) {
        case SNTX_NUL:
                coloe_token(LEX_NORMAL);
                break;
        case SNTX_SP:
                printf(" ");
                coloe_token(LEX_NORMAL);
                break;
        case SNTX_LF_HT:
                if (token == ETC_CLOSEPA)
                        syntax_level--;

                printf("\n");
                print_tab(syntax_level);
                coloe_token(LEX_NORMAL);
                break;
        case SNTX_DELY:
                break;
        default:
                break;
        }

        syntax_state = SNTX_DELY;
}

void
print_tab(int n)
{
        for (int i = 0; i < n; ++i) {
                printf("\t");
        }
}
