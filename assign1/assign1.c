#include <stdio.h>
#include <ctype.h>

enum DFAState
{
    Out,
    In,
    PreComment_In,
    PreComment_Out,
    Comment,
    PreOut
};

typedef struct _wordCount
{
    int nw;
    int nc;
    int nl;
    enum DFAState state;
} wordCount;

void constructor(int nw, int nc, int nl, enum DFAState state, wordCount *ptr)
{
    ptr->nc = nc;
    ptr->nw = nw;
    ptr->nl = nl;
    ptr->state = state;
}

wordCount out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);
    if (isspace(c) != 0)
    {
        ptr_temp->nc += 1;
        ptr_temp->state = Out;
    }
    else
    {
        if (c == = '/')
        {
            ptr_temp->nc += 1;
            ptr_temp->state = precomment_out;
        }
        else
        {
            ptr_temp->nc += 1;
            ptr_temp->state = In;
            ptr_temp->nw += 1;
        }
    }
    return temp;
}

wordCount in(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);
    return temp;
}

wordCount precomment_in(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);
    return temp;
}

wordCount precomment_out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);
    return temp;
}

wordCount comment(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);
    return temp;
}

wordCount pre_out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr_temp);

    return temp;
}

int main()
{
    wordCount wc;
    wc.nc = 0;
    wc.nw = 0;
    wc.nl = 0;
    wc.state = Out;
    char c;
    const wordCount *ptr = &wc;

    while ((c = getchar()) != EOF)
    {
        switch (wc.state)
        {
        case Out:
            wc = out(ptr, c);
            ptr = &wc;
            break;

        case In:
            wc = in(ptr, c);
            ptr = &wc;
            break;

        case PreComment_In:
            wc = precomment_in(ptr, c);
            ptr = &wc;
            break;

        case PreComment_Out:
            wc = precomment_out(ptr, c);
            ptr = &wc;
            break;

        case Comment:
            wc = comment(ptr, c);
            ptr = &wc;
            break;

        case PreOut:
            wc = pre_out(ptr, c);
            break;
        }

        printf("%d %d %d", wc.nl, wc.nw, wc.nc);
    }
}