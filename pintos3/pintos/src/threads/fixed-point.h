int int_to_fp(int n);
int fp_to_int(int x);
int fp_to_int_nearest(int x);
int add_x_y(int x, int y);
int sub_x_y(int x, int y);
int add_x_n(int x, int n);
int sub_x_n(int x, int n);
int mult_x_y(int x, int y);
int mult_x_n(int x, int n);
int div_x_y(int x, int y);
int div_x_n(int x, int n);

int f=1<<14;
int int_to_fp(int n){
	return n*f;
}

int fp_to_int(int x){
	return x/f;
}

int fp_to_int_nearest(int x){
	if(x >= 0) return (x+f/2)/f;
	else return (x-f/2)/f;
}
int add_x_y(int x, int y){
	return x+y;
}

int sub_x_y(int x, int y){
	return x-y;
}

int add_x_n(int x, int n){
	return x+int_to_fp(n);
}

int sub_x_n(int x, int n){
	return x-int_to_fp(n);
}

int mult_x_y(int x, int y){
	return ((int64_t)x)*y/f;
}

int mult_x_n(int x, int n){
	return x*n;
}

int div_x_y(int x, int y){
	return ((int64_t)x)*f/y;
}

int div_x_n(int x, int n){
	return x/n;
}


