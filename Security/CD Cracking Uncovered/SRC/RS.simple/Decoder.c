/*----------------------------------------------------------------------------
 *
 *				Reed-Solomon decoder
 *				=====================
 *
 *	The procedure of decoding Reed-Solomon codes comprises several steps.
 * First, we calculate 2t-symbol syndrome by means of substituting 
 * alpha**i into
 * recd(x), where recd is the received codeword, previously converted 
 * into the index form. As we calculate recd(x), we record each next
 * symbol of syndrom into s[i], where i ranges from 1 to 2t, 
 * leaving s[0] equal to zero. 
 *	Then, using the iterative Berlekamp algorithm, we
 * find the error locator polynomial - elp[i]. If elp power exceeds
 * t, we cannot correct all errors and limit ourselves by
 * displaying error messages informing the user about 
 * irrecoverable error, after which
 * carry out abnormal termination of the decoder. 
 * If elp power does not exceed t, we substitute
 * alpha**i, where i = 1..n into elp to calculate 
 * the roots of the polynomial. Inversion
 * of the found roots gives us positions of the corrupted symbols. 
 * If the number of positions of corrupted symbols is smaller 
 * than the elp degree, then
 * more than t symbols were corrupted, and it is impossible to recover them.
 * 	In all other cases it is possible to restore the original contents
 * of the corrupted symbols.
 *	In case when number of errors is admittedly high for their correction
 * the symbols being decoded pass through decoder without any changes
 *
 *					based on source codes by
 *					Simon Rockliff, dated 26.06.1991
 *					distributed according 
 *					to the GNU license
-----------------------------------------------------------------------------*/
decode_rs()
{
	int i, j, u, q;
	int s[n-k+1];				// syndrome polynomial
	int elp[n - k + 2][n - k];			// error locator polynomial
	int d[n-k+2];
	int l[n-k+2];
	int u_lu[n-k+2],
	
	int count=0, syn_error=0, root[t], loc[t], z[t+1], err[n], reg[t+1];
	
	// converting the codeword into the index form
	// to simplify the computations
	for (i = 0; i < n; i++) recd[i] = index_of[recd[i]];
	
	// calculating syndrome
	//-------------------------------------------------------------------
	for (i = 1; i <= n - k; i++)
	{
		s[i] = 0;		// initializing s-register
					// by default, zero is supplied
// to its input
		
		// performing s[i] += recd[j]*ij
		// e.g., taking the next symbol of decoded data,
		// multiplying it by the ordinal number of this symbol,
		// multiplied by the number of the next turn and adding
		// the obtained result to the contents of the s-register
		// When all symbols being decoded are exhausted, we
		// repeat the entire loop once again - one time for
		// each parity symbol
		for (j=0; j<n; j++) if (recd[j]!=-1) s[i]^= alpha_to[(recd[j]+i*j)%n];
		
		if (s[i]!=0) syn_error=1;	// is syndrome is not equal to zero
					// set error flag
		
		// convert syndrome from the polynomial form into the index form
		s[i] = index_of[s[i]];
	}
	
	// error correction
	//---------------------------------------------------------------------------
	if (syn_error)			// If there are errors, trying to correct them
	{
		
		// computing the error locator polynomial
		//-------------------------------------------------------------------
		// computing error locator polynomial via Berlecamp
		// iterative algorithm. According to terminology of Lin and Costello 
 		// (see. "Error
		// Control Coding: Fundamentals and Applications" Prentice Hall 1983
		// ISBN 013283796), d[u] represents m expressing
		// discrepancy, where u = m + 1 and m is the step number
		// ranging from -1 to 2t. Blahut designates the same
		// value as ??x).
		// l[u] represents the elp power for the current iteration step,
		// u_l[u] is the difference between the step number and elp power
		
		// initializing table elements
		d[0] = 0;				// index form
		d[1] = s[1];				// index form
		elp[0][0] = 0;				// index form
		elp[1][0] = 1;				// polynomial form
		
		for (i = 1; i < n - k; i++)
		{
			elp[0][i] = -1;		// index form
			elp[1][i] = 0;		// polynomial form
		}
		
		l[0] = 0; l[1] = 0; u_lu[0] = -1; u_lu[1] = 0; u = 0;
		
		do
		{
			u++;
			if (d[u] == -1)
			{
				l[u + 1] = l[u];
				for (i = 0; i <= l[u]; i++)
				{
					elp[u+1][i] = elp[u][i];
					elp[u][i]   = index_of[elp[u][i]];
				}
			}
				else
			{
				// searching for words with maximum u_lu[q], 
 				// so that d[q]!=0
				q = u - 1;
				while ((d[q] == -1) && (q>0)) q--;
				
				// first nonzero d[q] found
				if (q > 0)
				{
					j=q ;
					do
				{
					j-- ;
					if ((d[j]!=-1) && (u_lu[q]<u_lu[j]))
					q = j ;
				} while (j>0);
			};
			
			// as soon as we find q, so that d[u]!=0
			// and u_lu[q] is the maximum
			// record the power of the new elp polynomial
			if (l[u] > l[q]+u-q) l[u+1] = l[u]; else  l[u+1] = l[q]+u-q;
			
			
			// from new elp(x)
			for (i = 0; i < n - k; i++) elp[u+1][i] = 0;
			for (i = 0; i <= l[q]; i++)
				if (elp[q][i]!=-1)
				elp[u+1][i+u-q]=alpha_to[(d[u]+n-d[q]+elp[q][i])%n];
			
			for (i=0; i<=l[u]; i++)
			{
				elp[u+1][i] ^= elp[u][i];
				
				// convert old elp
				// into index form
				elp[u][i] = index_of[elp[u][i]];
			}
		}
		u_lu[u+1] = u-l[u+1];
		
		// form (u + 1)-th discrepancy
		//---------------------------------------------------------------------
		if (u < n-k)			// at last iteration 
		{				// there was no discrepancy
			
			if (s[u + 1]!=-1) d[u+1] = alpha_to[s[u+1]]; else d[u + 1] = 0;
			
			for (i = 1; i <= l[u + 1]; i++)
				if ((s[u + 1 - i] != -1) && (elp[u + 1][i]!=0))
				d[u+1] ^= alpha_to[(s[u+1-i]+index_of[elp[u+1][i]])%n];
			
			// convert d[u+1] into index form
			d[u+1] = index_of[d[u+1]];
		}
	} while ((u < n-k) && (l[u+1]<=t));
	
	// locator computation is accomplished
	//-----------------------------------------------------------------------
	u++ ;
	if (l[u] <= t)
	{					// error correction is possible
	
		// converting elp into index form
		for (i = 0; i <= l[u]; i++) elp[u][i] = index_of[elp[u][i]];
		
		
		// finding roots of the error locator polynomial
		//--------------------------------------------------------------------
		for (i = 1; i <= l[u]; i++) reg[i] = elp[u][i]; count = 0;
		
		for (i = 1; i <= n; i++)
		{
			q = 1 ;
			for (j = 1; j <= l[u]; j++)
			if (reg[j] != -1)
			{
				reg[j] = (reg[j]+j)%n;
				q ^= alpha_to[reg[j]];
			}
			
			if (!q)
			{	// recording root and error position index
				root[count] = i;
				loc[count] = n-i;
				count++;
			}
		}
		
		if (count == l[u])
		{			// no roots - elp power < t errors
		
			// forming z(x) polynomial
			for (i = 1; i <= l[u]; i++)		// Z[0] is always equal to 1
			{
				if ((s[i]!=-1) && (elp[u][i]!=-1))
					z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]];
				else
					if ((s[i]!=-1) && (elp[u][i]==-1))
						z[i] = alpha_to[s[i]];
					else
						if ((s[i]==-1) && (elp[u][i]!=-1))
							z[i] = alpha_to[elp[u][i]];
						else
								z[i] = 0 ;
			for (j=1; j<i; j++)
				if ((s[j]!=-1) && (elp[u][i-j]!=-1))
					z[i] ^= alpha_to[(elp[u][i-j] + s[j])%n];
			
			// converting z[i] into index form
			z[i] = index_of[z[i]];
		}
		
		// compute error values in positions loc[i]
		//--------------------------------------------------------------------
		for (i = 0; i<n; i++)
		{
			err[i] = 0;
			
			// converting recd[] to polynomial form
			if (recd[i]!=-1) recd[i] = alpha_to[recd[i]]; else recd[i] = 0;
		}
		
		// first calculate the error numerator
		for (i = 0; i < l[u]; i++)
		{
			err[loc[i]] = 1;
			for (j=1; j<=l[u]; j++)
				if (z[j]!=-1)
					err[loc[i]] ^= alpha_to[(z[j]+j*root[i])%n];
			
			if (err[loc[i]]!=0)
			{
				err[loc[i]] = index_of[err[loc[i]]];
				q = 0 ;	// form error coefficient denominator
				for (j=0; j<l[u]; j++)
					if (j!=i)
					     q+=index_of[1^alpha_to[(loc[j]+root[i])%n]];
			
				q = q % n; err[loc[i]] = alpha_to[(err[loc[i]]-q+n)%n];
				
				// recd[i] must be in polynomial form
				recd[loc[i]] ^= err[loc[i]];
			}
		}
	}
		else	// no roots,
			// the system has no solutions, since power of elp >= t
	{
		// converting recd[] to polynomial form
		for (i=0; i<n; i++)
			if (recd[i]!=-1) recd[i] = alpha_to[recd[i]];
		else
			recd[i] = 0;	// output data word as is
	}
		else			// elp power > t, no solution
	{
	// convert recd[] into polynomial form
	for (i=0; i<n; i++)
		if (recd[i]!=-1)
			recd[i] = alpha_to[recd[i]] ;
		else
			recd[i] = 0 ;	// output data word as is
	}
		else 			// no errors
	for (i=0;i<n;i++) if(recd[i]!=-1)recd[i]=alpha_to[recd[i]];else recd[i]=0;
}
