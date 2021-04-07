#include <stdio.h>

enum DFAState
{
    Out,
    In,
    PreComment_In,
    PreComment_Out,
    Comment,
    PreOut
};

typedef struct
{
    int nw;
    int nc;
    int nl;
    enum DFAState state;
} wordCount;

void out(const wordCount *ptr)
{

    return 0;
}

void in(const wordCount *ptr)
{
    return 0;
}

void precomment_in(const wordCount *ptr)
{
    return 0;
}

void precomment_out(const wordCount *ptr)
{
    return 0;
}

void comment(const wordCount *ptr)
{
    return 0;
}

void pre_out(const wordCount *ptr)
{
    return 0;
}

int main()
{
    wordCount wc;
    wc.nc = 0;
    wc.nw = 0;
    wc.nl = 0;
    wc.state = Out;
    int c;
    wordCount *ptr = &wc;
    while (c = getchar() != EOF)
    {
        switch (wc.state)
        {
        case Out:
            void out(wordCount * ptr);
            break;

        case In:
            void in(wordCount * ptr);
            break;

        case PreComment_In:
            void precomment_in(wordCount * ptr);
            break;

        case PreComment_Out:
            void precomment_out(wordCount * ptr);
            break;

        case Comment:
            void comment(wordCount * ptr);
            break;

        case PreOut:
            void preout(wordCount * ptr);
            break;
        }

        printf("%d %d %d", wc.nl, wc.nw, wc.nc);
    }
}