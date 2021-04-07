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

int main()
{
    wordCount wc;
    wc.nc = 0;
    wc.nw = 0;
    wc.nl = 0;
    wc.state = Out;
    int c;
    while (c = getchar() != EOF)
    {
        switch (wc.state)
        {
        case Out:

            break;

        case In:

            break;

        case PreComment_In:

            break;

        case PreComment_Out:

            break;

        case Comment:

            break;

        case PreOut:

            break;
        }

        printf("%d %d %d", nl, nw, nc);
    }
}