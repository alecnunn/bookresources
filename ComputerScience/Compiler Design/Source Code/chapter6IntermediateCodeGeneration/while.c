main()
{
int initialInvestment;
int noOfMonths,monthlyIncome;
int requiredIncome;
int check;
int sum;
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
}
