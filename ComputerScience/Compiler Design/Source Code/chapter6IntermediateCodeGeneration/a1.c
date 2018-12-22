main()
{
	int i,idx,nums;
	int data[20];
	int data1[20][5];
	int data2[20][5][3];
	int b[10];
	float sum;
	sum = 0;
	for(idx=0;idx<nums;idx=idx+1)
	{
		sum = sum + data[idx];
		sum = sum + data1[idx][idx+1];
		sum = sum + data2[idx][idx+1][idx+3];
	}
	for(i=0;i<10;i=i+1)
	{
		sum = sum + b[i]+i*10;
		sum = sum + data1[i][i+1];
		sum = sum + data2[i][i+1][i+3];
	}
	sum = sum / nums;
}
