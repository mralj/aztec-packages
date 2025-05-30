import { L2_TO_L1_MESSAGE_LENGTH, SCOPED_L2_TO_L1_MESSAGE_LENGTH } from '@aztec/constants';
import { EthAddress } from '@aztec/foundation/eth-address';
import { Fr } from '@aztec/foundation/fields';
import { schemas } from '@aztec/foundation/schemas';
import { BufferReader, FieldReader, serializeToBuffer, serializeToFields } from '@aztec/foundation/serialize';
import type { FieldsOf } from '@aztec/foundation/types';

import { z } from 'zod';

import { AztecAddress } from '../aztec-address/index.js';

export class L2ToL1Message {
  constructor(
    public recipient: EthAddress,
    public content: Fr,
    public counter: number,
  ) {}

  static get schema() {
    return z
      .object({
        recipient: schemas.EthAddress,
        content: schemas.Fr,
        counter: z.number().int().nonnegative(),
      })
      .transform(({ recipient, content, counter }) => new L2ToL1Message(recipient, content, counter));
  }

  /**
   * Creates an empty L2ToL1Message with default values.
   * @returns An instance of L2ToL1Message with empty fields.
   */
  static empty(): L2ToL1Message {
    return new L2ToL1Message(EthAddress.ZERO, Fr.zero(), 0);
  }

  /**
   * Checks if another L2ToL1Message is equal to this instance.
   * @param other Another L2ToL1Message instance to compare with.
   * @returns True if both recipient and content are equal.
   */
  equals(other: L2ToL1Message): boolean {
    return (
      this.recipient.equals(other.recipient) && this.content.equals(other.content) && this.counter === other.counter
    );
  }

  /**
   * Serialize this as a buffer.
   * @returns The buffer.
   */
  toBuffer(): Buffer {
    return serializeToBuffer(this.recipient, this.content, this.counter);
  }

  /**
   * Serializes the L2ToL1Message into an array of fields.
   * @returns An array of fields representing the serialized message.
   */
  toFields(): Fr[] {
    const fields = [this.recipient.toField(), this.content, new Fr(this.counter)];
    if (fields.length !== L2_TO_L1_MESSAGE_LENGTH) {
      throw new Error(
        `Invalid number of fields for L2ToL1Message. Expected ${L2_TO_L1_MESSAGE_LENGTH}, got ${fields.length}`,
      );
    }
    return fields;
  }

  /**
   * Deserializes an array of fields into an L2ToL1Message instance.
   * @param fields An array of fields to deserialize from.
   * @returns An instance of L2ToL1Message.
   */
  static fromFields(fields: Fr[] | FieldReader): L2ToL1Message {
    const reader = FieldReader.asReader(fields);
    return new L2ToL1Message(reader.readObject(EthAddress), reader.readField(), reader.readU32());
  }

  /**
   * Deserializes from a buffer or reader.
   * @param buffer - Buffer or reader to read from.
   * @returns A new instance of L2ToL1Message.
   */
  static fromBuffer(buffer: Buffer | BufferReader): L2ToL1Message {
    const reader = BufferReader.asReader(buffer);
    return new L2ToL1Message(reader.readObject(EthAddress), reader.readObject(Fr), reader.readNumber());
  }

  /**
   * Convenience method to check if the message is empty.
   * @returns True if both recipient and content are zero.
   */
  isEmpty(): boolean {
    return this.recipient.isZero() && this.content.isZero() && !this.counter;
  }

  scope(contractAddress: AztecAddress) {
    return new ScopedL2ToL1Message(this, contractAddress);
  }
}

export class ScopedL2ToL1Message {
  constructor(
    public message: L2ToL1Message,
    public contractAddress: AztecAddress,
  ) {}

  static get schema() {
    return z
      .object({
        message: L2ToL1Message.schema,
        contractAddress: AztecAddress.schema,
      })
      .transform(({ message, contractAddress }) => new ScopedL2ToL1Message(message, contractAddress));
  }

  static getFields(fields: FieldsOf<ScopedL2ToL1Message>) {
    return [fields.message, fields.contractAddress] as const;
  }

  static empty() {
    return new ScopedL2ToL1Message(L2ToL1Message.empty(), AztecAddress.ZERO);
  }

  equals(other: ScopedL2ToL1Message): boolean {
    return this.message.equals(other.message) && this.contractAddress.equals(other.contractAddress);
  }

  toBuffer(): Buffer {
    return serializeToBuffer(this.message, this.contractAddress);
  }

  static fromBuffer(buffer: Buffer | BufferReader) {
    const reader = BufferReader.asReader(buffer);
    return new ScopedL2ToL1Message(reader.readObject(L2ToL1Message), reader.readObject(AztecAddress));
  }

  static fromFields(fields: Fr[] | FieldReader) {
    const reader = FieldReader.asReader(fields);
    return new ScopedL2ToL1Message(reader.readObject(L2ToL1Message), reader.readObject(AztecAddress));
  }

  toFields(): Fr[] {
    const fields = serializeToFields(...ScopedL2ToL1Message.getFields(this));
    if (fields.length !== SCOPED_L2_TO_L1_MESSAGE_LENGTH) {
      throw new Error(
        `Invalid number of fields for ScopedL2ToL1Message. Expected ${SCOPED_L2_TO_L1_MESSAGE_LENGTH}, got ${fields.length}`,
      );
    }
    return fields;
  }

  isEmpty(): boolean {
    return this.message.isEmpty() && this.contractAddress.isZero();
  }
}
