Bitcoin Core SQ version 0.16.2 is now available from:

  <https://github.com/bitcoin-cored/bitcoin-cored/releases/tag/v0.16.2>

This release includes the following features and fixes:
- Hard fork at block height 588672 (approximately Aug 18th, 2018)
  - 1-min block target and 1440 blocks per day
  - Block reward of 1.25 coins to maintain 1800 coins/day target
- Updated Validation BlockSubsidy mechanism on dealing with the next and all
  subsequent halvings on the original schedule as per 10-min blocks.
  - Next halving scheduled for Block Height 1001952
  - All subsequent halvings will occur every 2,100,000 blocks post 1001952
- New 30-period DAA and changes to ComputeTarget at hardfork activation.
  - Incorporate mini-emergency adjustments on top of regular 30-period avg:
    - If 5 blocks happened faster than 3x expected, target 50% slower next block.
      Increase difficulty 100% if 5 blocks took <= 1-min 40-sec
    - If 5 blocks happened slower than 3x expected, target 100% faster next block.
      Decrease difficulty 50% if 5 blocks took >= 15-min
- Creation of a new testnet
- Removed Bitcoin Cash testnet seeders from clashic-seeder
- Increased PROTOCOL_VERSION from 70015 to 80030
- Added mechanism in net_processing to disconnect 70015 nodes one week before hardfork
  - Upgrade is mandatory one week before hardfork due to forced disconnect of legacy nodes
