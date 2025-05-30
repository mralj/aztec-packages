import type { EthAddress } from '@aztec/foundation/eth-address';
import type { Fr } from '@aztec/foundation/fields';
import type { EventSelector, FunctionSelector } from '@aztec/stdlib/abi';
import type { AztecAddress } from '@aztec/stdlib/aztec-address';

/** Any type that can be converted into a field for a contract call. */
export type FieldLike = Fr | Buffer | bigint | number | { /** Converts to field */ toField: () => Fr };

/** Any type that can be converted into an EthAddress Aztec.nr struct. */
export type EthAddressLike = { /** Wrapped address */ address: FieldLike } | EthAddress;

/** Any type that can be converted into an AztecAddress Aztec.nr struct. */
export type AztecAddressLike = { /** Wrapped address */ address: FieldLike } | AztecAddress;

/** Any type that can be converted into a FunctionSelector Aztec.nr struct. */
export type FunctionSelectorLike = FieldLike | FunctionSelector;

/** Any type that can be converted into an EventSelector Aztec.nr struct. */
export type EventSelectorLike = FieldLike | EventSelector;

/** Any type that can be converted into a U128. */
export type U128Like = bigint | number;

/** Any type that can be converted into a struct with a single `inner` field. */
export type WrappedFieldLike = { /** Wrapped value */ inner: FieldLike } | FieldLike;
