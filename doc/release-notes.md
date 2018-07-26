Bitcoin Core SQ version 0.16.2 is now available from:

  <https://github.com/bitcoin-cored/bitcoin-cored/releases/tag/v0.16.2>

This release includes the following features and fixes:
- 1-min block target and 1440 blocks per day
  - Block reward cut to 1.25 coins at Block Height 588672
  - Same distribution target of 1800 coins per day for post 2nd halvening.
  - Updated Validation BlockSubsidy mechanism on dealing with the next and all
    subsequent halvings on the original schedule as per 10-min blocks.
  - Hardfork schedule for Block Height 588672 (aprrox Aug 18th, 2018)
  - Next halving scheduled for Block Height 1001952
  - All subsequent halvings will occur every 2,100,000 blocks post 1001952
- 30-period DAA deactivating 288/72 * nPowTargetSpacing in ComputeTarget at fork height
- Incorporate mini-emergency adjustments on top of regular 30 min avg:
  - If 5 blocks happened faster than 3x expected, target 50% slower next block.
    Increase difficulty 100% if 5 blocks took <= 1-min 40-sec
  - If 5 blocks happened slower than 3x expected, target 100% faster next block.
    Decrease difficulty 50% if 5 blocks took >= 15-min
- Creation of a new testnet
- Removed Bitcoin Cash testnet seeders from clashic-seeder
- Increased PROTOCOL_VERSION from 70015 to 80030
- Added mechanism in net_processing to disconnect 70015 nodes one week before hardfork
