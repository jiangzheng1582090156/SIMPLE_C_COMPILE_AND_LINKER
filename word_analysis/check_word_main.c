#include "../whole_value.h"
#include "check_word.h"
#include "../enum_code.h"
#include <stdlib.h>

int
main(int argc, char **argv)
{
        fin = fopen(argv[1], "rb");
        filename = argv[1];
        if (fin == NULL) {
                fprintf(stderr, "不能打开源文件\n");
                exit(1);
        }

        getch();

        init();

        do {
                get_token();
                coloe_token(LEX_NORMAL);
        } while(token != ETC_EOF);

        cleanup();
        fclose(fin);
        printf("%s 词法分析成功, 共有 %d 行\n", argv[1], line_num);
        return 0;
}
