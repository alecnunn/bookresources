int a,b,c;
int main()
{
	int idx,qty;
	double average;
	int nums,sum;
	int initialInvestment;
	int noOfMonths,monthlyIncome;
	int requiredIncome;
	int check;
	int balance,loanAmount, noOfInstallments, installments;
	int repayAmount,excess;
	sum=0;
	for(idx=0;idx<nums;idx=idx+1)
		{sum = sum +qty;}
	L1: average = sum / nums;
	if(average < 10.0)
		{a=10;}
	else
		{b=20;}
	c=30;
	sum = 0;
	requiredIncome = 100000;
	monthlyIncome = 2000;
	noOfMonths = 0;
	while (sum <= requiredIncome)
	{
		sum = sum + monthlyIncome;
		noOfMonths = noOfMonths + 1;
	}
	check = requiredIncome == sum;
	loanAmount = 156000;
	balance = 0;
	installments = 1000;
	noOfInstallments  = 0;
	do
	{
		balance = loanAmount - installments;
		noOfInstallments = noOfInstallments + 1;
	} while(balance < 0);
	excess = -balance;
	goto L1;

}
