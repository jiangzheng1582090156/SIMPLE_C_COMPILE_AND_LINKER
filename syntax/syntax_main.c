#include "syntax.h"
#include "../word_analysis/check_word.h"
#include "../whole_value.h"
#include "../gramma_analysis/sentence_pattern.h"

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
        if (argc < 1) {
                fprintf(stderr, "<application> <filename>\n");
                exit(-1);
        }

        fin = fopen(argv[1], "rb");
        filename = argv[1];

        if (!fin) {
                fprintf(stderr, "打开文件失败\n");
                exit(-1);
        }

        init();
        getch();
        get_token();

        translation_unit();
        cleanup();
        fclose(fin);
        printf("\n语法分析通过!!!!\n", argv[1]);
        return 0;
}
