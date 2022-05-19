import { ValueType } from './value-type.js';
export declare function isInline(value: ValueType): boolean;
export declare function isNumber(value: ValueType): boolean;
export declare function isIndirectNumber(value: ValueType): boolean;
export declare function isTypedVectorElement(value: ValueType): boolean;
export declare function isTypedVector(value: ValueType): boolean;
export declare function isFixedTypedVector(value: ValueType): boolean;
export declare function isAVector(value: ValueType): boolean;
export declare function toTypedVector(valueType: ValueType, length: number): ValueType;
export declare function typedVectorElementType(valueType: ValueType): ValueType;
export declare function fixedTypedVectorElementType(valueType: ValueType): ValueType;
export declare function fixedTypedVectorElementSize(valueType: ValueType): ValueType;
export declare function packedType(valueType: ValueType, bitWidth: number): ValueType;
