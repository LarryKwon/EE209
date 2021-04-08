#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

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
    int comment_line;
} wordCount;

void constructor(int nw, int nc, int nl, enum DFAState state, int comment_line, wordCount *ptr)
{
    ptr->nc = nc;
    ptr->nw = nw;
    ptr->nl = nl;
    ptr->state = state;
    ptr->comment_line = comment_line;
}

wordCount out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (isspace(c) != 0)
    {
        if (c == '\n')
        {
            ptr_temp->nl += 1;
        }
        ptr_temp->nc += 1;
        ptr_temp->state = Out;
    }
    else
    {
        if (c == '/')
        {
            ptr_temp->nc += 1;
            ptr_temp->state = PreComment_Out;
        }
        else
        {
            ptr_temp->nc += 1;
            ptr_temp->state = In;
            ptr_temp->nw += 1;
            // printf("%d in->out\n", ptr_temp->nw);
        }
    }
    return temp;
}

wordCount in(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (isspace(c) != 0)
    {
        if (c == '\n')
        {
            ptr_temp->nl += 1;
        }
        ptr_temp->nc += 1;
        ptr_temp->state = Out;
    }
    else
    {
        if (c == '/')
        {
            ptr_temp->nc += 1;
            ptr_temp->state = PreComment_In;
        }
        else
        {
            ptr_temp->nc += 1;
            ptr_temp->state = In;
        }
    }
    return temp;
}

wordCount precomment_in(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (isspace(c) != 0)
    {
        if (c == '\n')
        {
            ptr_temp->nl += 1;
        }
        ptr_temp->nc += 1;
        ptr_temp->state = Out;
    }
    else
    {
        if (c == '*')
        {
            //ptr_temp->nw += 1;
            ptr_temp->state = Comment;
            ptr_temp->comment_line = ptr->nl;
        }
        else if (c == '/')
        {
            ptr_temp->nc += 1;
            ptr_temp->state = PreComment_In;
        }
        else
        {
            ptr_temp->nc += 1;
            ptr_temp->state = In;
        }
    }
    return temp;
}

wordCount precomment_out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (isspace(c) != 0)
    {
        if (c == '\n')
        {
            ptr_temp->nl += 1;
        }
        ptr_temp->nc += 1;
        ptr_temp->nw += 1;
        // printf("%d precomment_out -> out\n", ptr_temp->nw);
        ptr_temp->state = Out;
    }
    else
    {
        if (c == '/')
        {
            ptr_temp->nc += 1;
            ptr_temp->nw += 1;
            // printf("%d precomment_out -> precomment_in\n", ptr_temp->nw);

            ptr_temp->state = PreComment_In;
        }
        else if (c == '*')
        {
            ptr_temp->state = Comment;
            ptr_temp->comment_line = ptr->nl;
        }
        else
        {
            ptr_temp->nc += 1;
            ptr_temp->nw += 1;
            ptr_temp->state = In;
        }
    }
    return temp;
}

wordCount comment(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (c == '*')
    {
        ptr_temp->state = PreOut;
    }
    else if (c == '\n')
    {
        ptr_temp->nl += 1;
        ptr_temp->nc += 1;
    }
    else
    {
        ptr_temp->state = Comment;
    }
    return temp;
}

wordCount pre_out(const wordCount *ptr, char c)
{
    wordCount temp;
    wordCount *ptr_temp;
    ptr_temp = &temp;
    constructor(ptr->nw, ptr->nc, ptr->nl, ptr->state, ptr->comment_line, ptr_temp);
    if (c == '*')
    {
        ptr_temp->state = PreOut;
    }
    else if (c == '/')
    {
        ptr_temp->state = Out;
        // printf("pre_out -> out");
    }
    else
    {
        if (c == '\n')
        {
            ptr_temp->nl += 1;
            ptr_temp->nc += 1;
        }
        ptr_temp->state = Comment;
    }
    return temp;
}

int main()
{
    wordCount wc;
    wc.nc = 0;
    wc.nw = 0;
    wc.nl = 1;
    wc.comment_line = 0;
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
            ptr = &wc;
            break;
        }
    }
    if (wc.nc == 0)
    {
        wc.nw == 0;
        wc.nl == 0;
    }
    else if (wc.state == Comment || wc.state == PreOut)
    {
        printf("Error: line %d: unterminated comment\n", wc.comment_line);
        exit(1);
    }
    printf("%d %d %d\n", wc.nl, wc.nw, wc.nc);
}
