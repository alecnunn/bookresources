#include <stdio.h>
#define _U_	(0x666) 	// DIVIDEND
#define _V_ (0x75)   	// DIVISOR 

#define _POW_ 16   // POWER
#define _POW_POLY_	16

char u[_POW_];
//char v[8] = {1,0,0,0,1,0,1,1};

char u[_POW_];
char v[_POW_]; //= {1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0};
char r[_POW_];

// dst - DESTINATION
// x - NUMBER BEING CONVERTED
// c - NUMBER WIDTH
i2c (char *dst, int x, int c)
{
	int a;
	for ( a = 0; a < c; a++)
	{
		dst[a]=(x & 1)?1:0;
		x >>= 1;
	}			
}

c2i (char *src, int c)
{
	int a; int x=0;
	int mask = 1;
	for (a = 0; a < c; a++)
	{
		if (src[a]) x |= mask;
		mask <<= 1;								
	}
	return x;
}

get_max_pow (char *src, int c)
{
	int a;
	for (a = c - 1; a >= 0; a--)
	{
		if (src[a]) break;
	}
	return a;
}

raw_bit_print(char *src, int c)
{
	int a;
	for (a = c - 1; a >= 0; a--)
		printf("%x", (src[a])?1:0); printf("\n");
}


sub_poly_one(char *sup, char *sub, int c)
{
	int a, b;

	if (sub[0]>sup[0])
	{
		for (b=0; b < c; b++)

		if (sup[b]) {sup[b]=0;break;}else{sup[b]=1;}
		if (b == (c - 1)) 
		{
			printf("dddddd\x7");
			return -1;
		}
	}
		else
	{
		sup[0] = sup[0] - sub[0];
	}
}


sub_poly(char *sup, char *sub, int c)
{
	int a, b;
	for (a=0; a < c; a++)
	{
		if (sub[a]>sup[a])
			
			for (b=a; b < c; b++)
			
				if (sup[b]) {sup[b]=0;break;}else{sup[b]=1;}
		else
			sup[a] = sup[a] - sub[a];
	}
}

div_poly(char *u, char *v, int pow_1, int pow_2)
{
	int m, n;
	int k, j;
	
	m = pow_1; n = pow_2;

	for (k = m - n; k >=0; k--)
	{
		for (j = n + k - 1; j >= k; j--)
		{
			raw_bit_print(u,32);
			printf("%x %x\n",k,j);
            printf("j = %x, u[j] = ",j); raw_bit_print(&u[j],1);
            printf("j-k = %x, v[j-k] = ",j-k); raw_bit_print(&v[j-k],1);
			//u[j] = u[j] ^ v[j-k];
			sub_poly_one(&u[j], &v[j-k], pow_1 - j + 1 );
			//raw_bit_print(u,32);

			
		}	
	}
}

main()
{
	int a;
	int	m;
	int	n;
	int	k, j;
	int max_pow;
	int _u_;
	int _v_;

	// SYSTEMATIC ENCODING
	_u_ = _U_; //_u_ <<= 2;

	// CONVERTING INTO BIT STIRNGS FOR CONVENIENCE
	i2c(u, _u_, _POW_);	i2c(v, _V_, _POW_); //memcpy(r, u, _POW_);


	printf("u:");	raw_bit_print(u, _POW_);
	printf("v:");	raw_bit_print(v, _POW_);

	div_poly(u, v, 10, 6);
	//sub_poly(u, v, 11, 11);
	printf("u:"); raw_bit_print(u, _POW_);
	return 0;
	printf("r:");	raw_bit_print(u, _POW_POLY_ - 1);

	printf("r:");	raw_bit_print(u, _POW_);
	printf("%x == %x\n",c2i(u, _POW_ - 2), _U_ % _V_);
	return 0;
	memcpy(r, u, _POW_POLY_-1);
	printf("c:"); raw_bit_print(r, _POW_);
	r[11] ^= 1;

	div_poly(r,	v, _POW_ , _POW_POLY_ );
	printf("e:"); raw_bit_print(r, _POW_);




	return 0;
	//for (a = 0; a < 16; a++) printf("*%x", r[a]);

	for (a = m; a >= 0; a--) printf("%d", (u[a])?1:0); printf("\n");
}