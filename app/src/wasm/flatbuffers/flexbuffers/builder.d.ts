import { BitWidth } from './bit-width.js';
import { StackValue } from './stack-value.js';
interface StackPointer {
    stackPosition: number;
    isVector: boolean;
    presorted?: boolean;
}
export declare class Builder {
    private dedupStrings;
    private dedupKeys;
    private dedupKeyVectors;
    buffer: ArrayBuffer;
    view: DataView;
    readonly stack: Array<StackValue>;
    readonly stackPointers: Array<StackPointer>;
    offset: number;
    finished: boolean;
    readonly stringLookup: Record<string, StackValue>;
    readonly keyLookup: Record<string, StackValue>;
    readonly keyVectorLookup: Record<string, StackValue>;
    readonly indirectIntLookup: Record<number, StackValue>;
    readonly indirectUIntLookup: Record<number, StackValue>;
    readonly indirectFloatLookup: Record<number, StackValue>;
    constructor(size?: number, dedupStrings?: boolean, dedupKeys?: boolean, dedupKeyVectors?: boolean);
    private align;
    computeOffset(newValueSize: number): number;
    pushInt(value: number, width: BitWidth): void;
    pushUInt(value: number, width: BitWidth): void;
    private writeInt;
    private writeUInt;
    private writeBlob;
    private writeString;
    private writeKey;
    private writeStackValue;
    private integrityCheckOnValueAddition;
    private integrityCheckOnKeyAddition;
    startVector(): void;
    startMap(presorted?: boolean): void;
    private endVector;
    private endMap;
    private sort;
    end(): void;
    private createVector;
    private nullStackValue;
    private boolStackValue;
    private intStackValue;
    private uintStackValue;
    private floatStackValue;
    private offsetStackValue;
    private finishBuffer;
    add(value: undefined | null | boolean | bigint | number | DataView | string | Array<unknown> | Record<string, unknown> | unknown): void;
    finish(): Uint8Array;
    isFinished(): boolean;
    addKey(key: string): void;
    addInt(value: number, indirect?: boolean, deduplicate?: boolean): void;
    addUInt(value: number, indirect?: boolean, deduplicate?: boolean): void;
    addFloat(value: number, indirect?: boolean, deduplicate?: boolean): void;
}
export {};
