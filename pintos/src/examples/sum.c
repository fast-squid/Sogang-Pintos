#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <string.h>
int strToDec(char* str,bool *failure){
	int i, pow = 1;
	int res = 0;
	for(i=strlen(str)-1; i>=0;i--){
		if(str[0]=='-'){
			res*=-1; continue;
		}
		if('0'<=str[i]&&str[i]<='9'){
			res+=(str[i] -'0')*pow;
			pow*=10;
		}
		else{
			*failure = true;
		}
	}
	return res;
}

int main(int argc, char**argv){
	bool fail[4]={0};
	int i;
	int num[4];
	
	for(i=0;i<4;i++){
			num[i] = strToDec(argv[i+1],&fail[i]);
		if(fail[i]) return EXIT_FAILURE;
	}
	printf("%d %d\n",fibonacci(num[0]),
			sum_of_four_integers(num));
	
	
	return EXIT_SUCCESS;
}
