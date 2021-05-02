/**
 * Cryptocurrency neutral profit calculator.
 * Based on bitkub's market.
 */
#include "bitkub.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>

static void ComputeAndPrintProfitReferencePointEntry(double percentageMargin, double cryptoSellPrice, int strFormatWidth, double grossSellPrice, double amountCRYPTO_deductedFee, double amountToSpend, bool includedGrossCompute)
{
	const double actualMultiPercentage = 1.0 + percentageMargin/100.0;
	const double newCryptoSellPrice = cryptoSellPrice*actualMultiPercentage;

	std::printf("Sell at %.2f%% more at  %.4f THB\n", percentageMargin, newCryptoSellPrice);
	if (includedGrossCompute)
	{
		double n = (grossSellPrice * actualMultiPercentage) - amountToSpend;
		if (n < 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", bitkub::kPrefixRedTxt, strFormatWidth, n, bitkub::kPostfixTxt);
		else if (n > 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", bitkub::kPrefixGreenTxt, strFormatWidth, n, bitkub::kPostfixTxt);
		else
			std::printf("|_ Gross profit:         %*.8f     THB\n", strFormatWidth, n);
	}


	{
		double n = (newCryptoSellPrice * amountCRYPTO_deductedFee) - amountToSpend;
		if (n < 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n\n", bitkub::kPrefixRedTxt, strFormatWidth, n, bitkub::kPostfixTxt);
		else if (n > 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n\n", bitkub::kPrefixGreenTxt, strFormatWidth, n, bitkub::kPostfixTxt);
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
	if (bitkub::FoundCmdOption(argc, argv, "--help"))
	{
		std::cout << "Usage: pcalc <amount-to-spend> <CRYPTO-buy-price> <CRYPTO-sell-price>\n";
		return 0;
	}

	// cmd's options
	bool isIncludeGrossCompute = false;
	bool isIncludeProfitReferencePoints = false;

	if (bitkub::FoundCmdOption(argc, argv, "--include-gross"))
		isIncludeGrossCompute = true;
	if (bitkub::FoundCmdOption(argc, argv, "--include-profit-ref-points") || bitkub::FoundCmdOption(argc, argv, "--include-prps"))
		isIncludeProfitReferencePoints = true;

	double amountToSpend = atof(argv[1]);
	double CRYPTO_buyPrice = atof(argv[2]);
	double CRYPTO_sellPrice = atof(argv[3]);

	// these three for holding intermediate artifacts, and profit result
	double grossProfit;
	double netProfit;
	bitkub::IntermediatedArtifactsProfit iArtifactsProfit;

	// compute profits, results are in variable sending in as reference, and pointers
	bitkub::ComputeProfit(amountToSpend, CRYPTO_buyPrice, CRYPTO_sellPrice, grossProfit, netProfit, &iArtifactsProfit);

	constexpr int kStrFormatWidth = 40;

	std::printf("--- Summary ---\n");

	std::printf("* Volume\n");
	std::printf("|_ Buy volume            %*.8f     THB\n", kStrFormatWidth, amountToSpend);
	std::printf("|_ Sell volume           %*.8f     THB\n", kStrFormatWidth, iArtifactsProfit.netSellVolume);
	if (isIncludeGrossCompute)
	{
	std::printf("|_ Gross sell volume     %*.8f     THB\n", kStrFormatWidth, iArtifactsProfit.grossSellVolume);
	}

	std::printf("\n* Price\n");
	std::printf("|_ Buy CRYPTO at price   %*.8f     THB\n", kStrFormatWidth, CRYPTO_buyPrice);
	std::printf("|_ Sell CRYPTO at price  %*.8f     THB\n\n", kStrFormatWidth, CRYPTO_sellPrice);

	std::printf("* Profit\n");
	if (isIncludeGrossCompute)
	{
		if (grossProfit < 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", bitkub::kPrefixRedTxt, kStrFormatWidth, grossProfit, bitkub::kPostfixTxt);
		else if (grossProfit > 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Gross profit:         %s%*.8f%s     THB\n", bitkub::kPrefixGreenTxt, kStrFormatWidth, grossProfit, bitkub::kPostfixTxt);
		else
			std::printf("|_ Gross profit:         %*.8f     THB\n", kStrFormatWidth, grossProfit);
	}

	{
		if (netProfit < 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n", bitkub::kPrefixRedTxt, kStrFormatWidth, netProfit, bitkub::kPostfixTxt);
		else if (netProfit > 0.0 && bitkub::g_supportTTY)
			std::printf("|_ Net profit:           %s%*.8f%s     THB\n", bitkub::kPrefixGreenTxt, kStrFormatWidth, netProfit, bitkub::kPostfixTxt);
		else
			std::printf("|_ Net profit:           %*.8f     THB\n", kStrFormatWidth, netProfit);
	}
	
	std::printf("\n* Fee\n");
	std::printf("|_ Total fee:            %*.8f     THB\n", kStrFormatWidth, iArtifactsProfit.buyFee + iArtifactsProfit.sellFeeCryptoAmount*CRYPTO_sellPrice);
	std::printf("   |_ Buy fee:           %*.8f     THB\n", kStrFormatWidth, iArtifactsProfit.buyFee);
	std::printf("   |_ Sell fee:          %*.8f     THB\n", kStrFormatWidth, iArtifactsProfit.sellFeeCryptoAmount*CRYPTO_sellPrice);

	std::printf("\n* Amount\n");
	if (isIncludeGrossCompute)
	std::printf("|_ Gross CRYPTO amount:  %*.8f     CRYPTO\n", kStrFormatWidth, iArtifactsProfit.grossAmountGainedCrypto);
	std::printf("|_ CRYPTO amount:        %*.8f     CRYPTO\n\n", kStrFormatWidth, iArtifactsProfit.netAmountGainedCrypto);

	if (isIncludeProfitReferencePoints)
	{
		// compute further more margin of sell price for 0.1%, 0.15%, 0.2%, 0.25%, 0.5%, 1%, 1.5%, 2%, and 5%
		std::printf("--- Profit Reference Points ---\n");
		const double amountCRYPTO_deductedFee = iArtifactsProfit.netAmountGainedCrypto - iArtifactsProfit.sellFeeCryptoAmount;
		ComputeAndPrintProfitReferencePointEntry(0.1, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.15, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.2, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.25, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(0.5, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(1.0, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(1.5, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(2.0, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(2.5, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
		ComputeAndPrintProfitReferencePointEntry(5.0, CRYPTO_sellPrice, kStrFormatWidth, iArtifactsProfit.grossSellVolume, amountCRYPTO_deductedFee, amountToSpend, isIncludeGrossCompute);
	}
	return 0;
}
