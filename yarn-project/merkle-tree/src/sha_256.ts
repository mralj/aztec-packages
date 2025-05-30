import { sha256 } from '@aztec/foundation/crypto';
import { truncateAndPad } from '@aztec/foundation/serialize';
import type { Hasher } from '@aztec/foundation/trees';

/**
 * A helper class encapsulating SHA256 hash functionality.
 * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
 * purposes.
 */
export class SHA256 implements Hasher {
  /*
   * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
   * purposes.
   */
  public hash(lhs: Uint8Array, rhs: Uint8Array) {
    return sha256(Buffer.concat([Buffer.from(lhs), Buffer.from(rhs)])) as Buffer<ArrayBuffer>;
  }

  /*
   * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
   * purposes.
   */
  public hashInputs(inputs: Buffer[]) {
    return sha256(Buffer.concat(inputs)) as Buffer<ArrayBuffer>;
  }
}

/**
 * A helper class encapsulating truncated SHA256 hash functionality.
 * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
 * purposes.
 */
export class SHA256Trunc implements Hasher {
  /*
   * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
   * purposes.
   */
  public hash(lhs: Uint8Array, rhs: Uint8Array) {
    return truncateAndPad(sha256(Buffer.concat([Buffer.from(lhs), Buffer.from(rhs)]))) as Buffer<ArrayBuffer>;
  }

  /*
   * @deprecated Don't call SHA256 directly in production code. Instead, create suitably-named functions for specific
   * purposes.
   */
  public hashInputs(inputs: Buffer[]) {
    return truncateAndPad(sha256(Buffer.concat(inputs))) as Buffer<ArrayBuffer>;
  }
}
