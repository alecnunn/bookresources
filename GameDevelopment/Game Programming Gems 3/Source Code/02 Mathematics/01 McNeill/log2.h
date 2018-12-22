#ifndef LOG2_H
#define LOG2_H

// All of these functions assume sizeof(unsigned) == 4 bytes.

// Find the smallest bit mask greater than or equal to n.
// This can also be stated as finding the smallest non-negative integer x
// such that pow(2,x) > n and returning pow(2,x)-1.

unsigned bitmask ( unsigned n )
{
	if ( n == 0 )
		return 0;

	unsigned mask = 0xffffffff;

	unsigned t = 0xffff;
	if ( t >= n )
		mask = t;

	t = mask >> 8;
	if ( t >= n )
		mask = t;

	t = mask >> 4;
	if ( t >= n )
		mask = t;

	t = mask >> 2;
	if ( t >= n )
		mask = t;

	t = mask >> 1;
	if ( t >= n )
		mask = t;

	return mask;
}

// Find the largest non-negative integer x such that pow(2,x) <= n.
// The exception is n=0, which returns 0.

unsigned log2le ( unsigned n )
{
	unsigned t, log2;

	if ( n >= 0x10000 )
	{
		log2 = 16;
		t = 0x1000000;
	}
	else
	{
		log2 = 0;
		t = 0x100;
	}

	if ( n >= t )
	{
		log2 += 8;
		t <<= 4;
	}
	else
	{
		t >>= 4;
	}

	if ( n >= t )
	{
		log2 += 4;
		t <<= 2;
	}
	else
	{
		t >>= 2;
	}

	if ( n >= t )
	{
		log2 += 2;
		t <<= 1;
	}
	else
	{
		t >>= 1;
	}

	if ( n >= t )
	{
		log2 += 1;
	}

	return log2;
}

// Find the smallest non-negative integer x such that pow(2,x) >= n.

unsigned log2ge ( unsigned n )
{
	if ( n > 0x80000000 )
		return 32;

	unsigned t, log2;

	if ( n > 0x8000 )
	{
		log2 = 16;
		t = 0x800000;
	}
	else
	{
		log2 = 0;
		t = 0x80;
	}

	if ( n > t )
	{
		log2 += 8;
		t <<= 4;
	}
	else
	{
		t >>= 4;
	}

	if ( n > t )
	{
		log2 += 4;
		t <<= 2;
	}
	else
	{
		t >>= 2;
	}

	if ( n > t )
	{
		log2 += 2;
		t <<= 1;
	}
	else
	{
		t >>= 1;
	}

	if ( n > t )
	{
		log2 += 1;
	}

	return log2;
}

#endif // LOG2_H
