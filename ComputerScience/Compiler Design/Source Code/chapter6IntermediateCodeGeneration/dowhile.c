main()
{
	int balance,loanAmount, noOfInstallments, installments;
	int repayAmount,excess;
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
}

