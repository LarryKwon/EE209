#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

enum DFAState{outside, inside} ;
enum CommentState{initial,semi_start,start,semi_end};

int main(void){
    int c, nw =0 , nl=1, nc=0;
    int comment_line = 0;
    enum DFAState state = outside;
    enum CommentState state_c=initial;

    while((c=getchar())!=EOF){
        switch(state_c){
            case(initial):
                if(c=='/'){
                    state_c = semi_start;
                    if(state == outside){
                        nw++;
                    }
                    else if(state == inside){
                    }
                }
                break;
            case(semi_start):
                if(c=='*'){
                    state_c = start;
                    comment_line = nl;
                    if(state == outside){
                        nw=nw-1;
                    }
                    else if(state == inside){
                        // nw++;
                    }
                }
                else if(c=='/'){
                    state_c = semi_start;
                    state=inside;
                }
                else{
                    state_c=initial;
                    if(isspace(c)){
                        state = outside;
                    }
                    else{
                        state = inside;
                    }
                }
                break;
            case(start):
                if(c=='*'){
                    state_c = semi_end;
                }
                else{
                    state_c = start;
                }
                break;
            case(semi_end):
                if(c=='/'){
                    state_c = initial;
                    state = outside;
                    continue;
                }
                else if(c=='*'){
                    state_c = semi_end;
                }
                else{
                    state_c = start;
                }
                break;
        }
        switch(state){
            case(outside):
                if(c=='/'){
                    state = outside;
                    nc++;
                }
                else if(state_c == start || state_c==semi_end){
                    if(c=='\n'){
                        nl++;
                        nc++;
                    }
                    break;
                }
                else if(!isspace(c)) {
                    state = inside;
                    nw++;
                    nc++;
                    }
                else{
                    if(c == '\n'){
                         nl++;
                    }
                    nc++;
                }
                break;

            case(inside):
                if(state_c == start || state_c == semi_end){
                    if(c=='\n'){
                        nl++;
                        nc++;
                    }

                }
                else if(state_c == initial && isspace(c)){
                    if(c == '\n'){
                        nl++;
                    }
                    state = outside;
                    nc++;
                }
                else{
                    nc++;
                }
                break;
        }
    }
    if(nc==0){
        nl=0;
        nw=0;
    }
    if(state_c==start||state_c==semi_end){
        printf("Error: line %d: unterminated comment",comment_line);
        exit(1);
    }
    else{
        printf("%d %d %d\n", nl, nw, nc);
        exit(0);
    }

    return 0;

}
