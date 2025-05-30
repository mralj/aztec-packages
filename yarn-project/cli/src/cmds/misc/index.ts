import type { LogFn } from '@aztec/foundation/log';
import { printENR } from '@aztec/p2p/enr';

import type { Command } from 'commander';

import { l1ChainIdOption, prettyPrintJSON } from '../../utils/commands.js';

export function injectCommands(program: Command, log: LogFn) {
  program
    .command('generate-keys')
    .summary('Generates encryption and signing private keys.')
    .description('Generates and encryption and signing private key pair.')
    .option('--json', 'Output the keys in JSON format')
    .action(async ({ json }) => {
      const { generateSecretKey } = await import('./generate_secret_key.js');
      const { secretKey } = generateSecretKey();
      if (json) {
        log(prettyPrintJSON({ secretKey: secretKey.toString() }));
      } else {
        log(`Secret Key: ${secretKey}`);
      }
    });

  program
    .command('generate-p2p-private-key')
    .summary('Generates a LibP2P peer private key.')
    .description('Generates a private key that can be used for running a node on a LibP2P network.')
    .action(async () => {
      const { generateP2PPrivateKey } = await import('./generate_p2p_private_key.js');
      await generateP2PPrivateKey(log);
    });

  program
    .command('generate-bootnode-enr')
    .summary('Generates the encoded ENR record for a bootnode.')
    .description('Generates the encoded ENR record for a bootnode.')
    .argument('<privateKey>', 'The peer id private key of the bootnode')
    .argument('<p2pIp>', 'The bootnode P2P IP address')
    .argument('<p2pPort>', 'The bootnode P2P port')
    .addOption(l1ChainIdOption)
    .action(async (privateKey: string, p2pIp: string, p2pPort: number, options) => {
      const { generateEncodedBootnodeENR } = await import('./generate_bootnode_enr.js');
      await generateEncodedBootnodeENR(privateKey, p2pIp, p2pPort, options.l1ChainId, log);
    });

  program
    .command('decode-enr')
    .summary('Decodes an ENR record')
    .description('Decodes and ENR record')
    .argument('<enr>', 'The encoded ENR string')
    .action(async (enr: string) => {
      await printENR(enr, log);
    });

  program
    .command('example-contracts')
    .description('Lists the example contracts available to deploy from @aztec/noir-contracts.js')
    .action(async () => {
      const { exampleContracts } = await import('./example_contracts.js');
      await exampleContracts(log);
    });

  program
    .command('compute-selector')
    .description('Given a function signature, it computes a selector')
    .argument('<functionSignature>', 'Function signature to compute selector for e.g. foo(Field)')
    .action(async (functionSignature: string) => {
      const { computeSelector } = await import('./compute_selector.js');
      await computeSelector(functionSignature, log);
    });

  program
    .command('generate-secret-and-hash')
    .description('Generates an arbitrary secret (Fr), and its hash (using aztec-nr defaults)')
    .action(async () => {
      const { generateSecretAndHash } = await import('./generate_secret_and_hash.js');
      await generateSecretAndHash(log);
    });

  program
    .command('get-canonical-sponsored-fpc-address')
    .description('Gets the canonical SponsoredFPC address for this any testnet running on the same version as this CLI')
    .action(async () => {
      const { getCanonicalSponsoredFPCAddress } = await import('./get_canonical_sponsored_fpc_address.js');
      await getCanonicalSponsoredFPCAddress(log);
    });

  program
    .command('update')
    .description('Updates Nodejs and Noir dependencies')
    .argument('[projectPath]', 'Path to the project directory', process.cwd())
    .option('--contract [paths...]', 'Paths to contracts to update dependencies', [])
    .option('--aztec-version <semver>', 'The version to update Aztec packages to. Defaults to latest', 'latest')
    .action(async (projectPath: string, options) => {
      const { updateProject } = await import('./update.js');
      const { contract, aztecVersion } = options;
      await updateProject(projectPath, contract, aztecVersion, log);
    });

  return program;
}
