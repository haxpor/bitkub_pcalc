/**
 * Cryptocurrency neutral profit calculator.
 * Based on bitkub's market.
 */
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

// fee percentage (already applied percentage)
static constexpr double kFeePercentage = 0.0025;

inline constexpr const char* kPrefixRedTxt = "\033[1;31m";
inline constexpr const char* kPrefixGreenTxt = "\033[1;32m";
inline constexpr const char* kPostfixTxt = "\033[0m";

static const bool g_supportTTY = isatty(STDOUT_FILENO);

static bool FoundCmdOption(int argc, char* argv[], const char* option)
{
	for (int i=1; i<argc; ++i)
	{
		if (std::strcmp(argv[i], option) == 0)
		{
			return true;
		}
	}
	return false;
}

static void ComputeAndPrintProfitReferencePointEntry(double percentageMargin, double cryptoSellPrice, int strFormatWidth, double grossSellPrice, double amountCRYPTO_deductedFee, double amountToSpend, bool includedGrossCompute)
{
	const double actualMultiPercentage = 1.0 + percentageMargin/100.0;
	const double newCryptoSellPrice = cryptoSellPrice*actualMultiPercentage;

	std::printf("Sell at %.2f%% more at  %.4f THB\n", percentageMargin, newCryptoSellPrice);
	if (includedGrossCompute)
	{
		double n = (grossSellPrice * actualMultiPercentage) - amountToSpend;
		if (n < 0.0 && g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", kPrefixRedTxt, strFormatWidth, n, kPostfixTxt);
		else if (n > 0.0 && g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", kPrefixGreenTxt, strFormatWidth, n, kPostfixTxt);
		else
			std::printf("|_ Gross profit:         %*.8f     THB\n", strFormatWidth, n);
	}


	{
		double n = (newCryptoSellPrice * amountCRYPTO_deductedFee) - amountToSpend;
		if (n < 0.0 && g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n\n", kPrefixRedTxt, strFormatWidth, n, kPostfixTxt);
		else if (n > 0.0 && g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n\n", kPrefixGreenTxt, strFormatWidth, n, kPostfixTxt);
		else
			std::printf("|_ Net profit:           %*.8f     THB\n\n", strFormatWidth, n);
	}
}

// accept the target buy price, and target sell price later
// to compute the profit
int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		std::cerr << "Usage: pcalc <amount-to-spend> <CRYPTO-buy-price> <CRYPTO-sell-price>\n";
		return -1;
	}	

	// this option takes higest priority than all others if included along with other options
	if (FoundCmdOption(argc, argv, "--help"))
	{
		std::cout << "Usage: pcalc <amount-to-spend> <CRYPTO-buy-price> <CRYPTO-sell-price>\n";
		return 0;
	}

	// cmd's options
	bool isIncludeGrossCompute = false;
	bool isIncludeProfitReferencePoints = false;

	if (FoundCmdOption(argc, argv, "--include-gross"))
		isIncludeGrossCompute = true;
	if (FoundCmdOption(argc, argv, "--include-profit-ref-points") || FoundCmdOption(argc, argv, "--include-prps"))
		isIncludeProfitReferencePoints = true;

	double amountToSpend = atof(argv[1]);
	double CRYPTO_buyPrice = atof(argv[2]);
	double CRYPTO_sellPrice = atof(argv[3]);

	double buyFee = amountToSpend * kFeePercentage;
	double grossAmountGainedCRYPTO = amountToSpend / CRYPTO_buyPrice;
	double netAmountGainedCRYPTO = (amountToSpend - buyFee) / CRYPTO_buyPrice;

	double grossSellPrice = grossAmountGainedCRYPTO * CRYPTO_sellPrice;
	// sell fee needs to be deducted from crypto amount, not total sell price
	double sellFee_cryptoAmount = netAmountGainedCRYPTO * kFeePercentage;
	double netSellPrice = (netAmountGainedCRYPTO - sellFee_cryptoAmount) * CRYPTO_sellPrice;
	
	double grossProfit = grossSellPrice - amountToSpend;
	double profit = netSellPrice - amountToSpend;

	constexpr int kStrFormatWidth = 40;

	std::printf("--- Summary ---\n");

	std::printf("* Volume\n");
	std::printf("|_ Buy volume            %*.8f     THB\n", kStrFormatWidth, amountToSpend);
	std::printf("|_ Sell volume           %*.8f     THB\n", kStrFormatWidth, netSellPrice);
	if (isIncludeGrossCompute)
	{
	std::printf("|_ Gross sell volume     %*.8f     THB\n", kStrFormatWidth, grossSellPrice);
	}

	std::printf("\n* Price\n");
	std::printf("|_ Buy CRYPTO at price   %*.8f     THB\n", kStrFormatWidth, CRYPTO_buyPrice);
	std::printf("|_ Sell CRYPTO at price  %*.8f     THB\n\n", kStrFormatWidth, CRYPTO_sellPrice);

	std::printf("* Profit\n");
	if (isIncludeGrossCompute)
	{
		if (grossProfit < 0.0 && g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", kPrefixRedTxt, kStrFormatWidth, grossProfit, kPostfixTxt);
		else if (grossProfit > 0.0 && g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", kPrefixGreenTxt, kStrFormatWidth, grossProfit, kPostfixTxt);
		else
			std::printf("|_ Gross profit:         %*.8f     THB\n", kStrFormatWidth, grossProfit);
	}

	{
		if (profit < 0.0 && g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n", kPrefixRedTxt, kStrFormatWidth, profit, kPostfixTxt);
		else if (profit > 0.0 && g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n", kPrefixGreenTxt, kStrFormatWidth, profit, kPostfixTxt);
		else
			std::printf("|_ Net profit:           %*.8f     THB\n", kStrFormatWidth, profit);
	}
	
	std::printf("\n* Fee\n");
	std::printf("|_ Total fee:            %*.8f     THB\n", kStrFormatWidth, buyFee + sellFee_cryptoAmount*CRYPTO_sellPrice);
	std::printf("   |_ Buy fee:           %*.8f     THB\n", kStrFormatWidth, buyFee);
	std::printf("   |_ Sell fee:          %*.8f     THB\n", kStrFormatWidth, sellFee_cryptoAmount*CRYPTO_sellPrice);

	std::printf("\n* Amount\n");
	if (isIncludeGrossCompute)
	std::printf("|_ Gross CRYPTO amount:  %*.8f     CRYPTO\n", kStrFormatWidth, grossAmountGainedCRYPTO);
	std::printf("|_ CRYPTO amount:        %*.8f     CRYPTO\n\n", kStrFormatWidth, netAmountGainedCRYPTO);

	if (isIncludeProfitReferencePoints)
	{
		// compute further more margin of sell price for 0.1%, 0.15%, 0.2%, 0.25%, 0.5%, 1%, 1.5%, 2%, and 5%
		std::printf("--- Profit Reference Points ---\n");
		const double amountCRYPTO_deductedFee = netAmountGainedCRYPTO - sellFee_cryptoAmount;
		ComputeAndPrintProfitReferencePointEntry(0.1, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.15, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.2, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.25, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.5, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(1.0, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(1.5, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(2.0, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(2.5, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(5.0, CRYPTO_sellPrice, kStrFormatWidth, grossSellPrice, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
	}
	return 0;
}
