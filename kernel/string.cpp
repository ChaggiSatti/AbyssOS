#include "string.h"

bool strcmp(const char* a, const char* b){
    int i = 0;
    while(a[i] != '\0' and b[i] != '\0'){
        if(a[i] != b[i]){
            return false;
        }
        i++;
    }
    return (a[i] == '\0' and b[i] == '\0');
}
void itoa(int n, char st[]){
    int i = 0;
    bool is_neg = 0;
    if(n == 0){
        st[i++] = '0';
        st[i] = '\0';
        return;
    }
    if(n < 0){
        n = -n;
        is_neg = 1;
    }
    while(n != 0){
        int r = n % 10;
        n/=10;
        st[i++] = '0' + r;
    }
    if(is_neg){
        st[i++] = '-';
    }
    st[i] = '\0';
    int start = 0, end = i-1;
    while(start < end){
        char temp = st[start];
        st[start] = st[end];
        st[end] = temp;
        start++;
        end--;
    }
}