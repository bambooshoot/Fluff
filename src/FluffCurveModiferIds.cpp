#include <FluffCurveModiferRadius.h>
#include <FluffCurveModiferSplit.h>
#include <FluffCurveModiferFuzzy.h>
#include <FluffCurveModiferNormal.h>
#include <FluffCurveModiferClump.h>

NodeId FluffCurveModiferRadius::id = 2;
NodeId FluffCurveModiferSplit::id = 3;
NodeId FluffCurveModiferFuzzy::id = 4;
NodeId FluffCurveModiferNormal::id = 5;
NodeId FluffCurveModiferClump::id = 6;

tbb::spin_mutex FluffCurveModiferClump::mtx;