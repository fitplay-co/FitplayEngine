"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.StackValue = void 0;
var bit_width_js_1 = require("./bit-width.js");
var bit_width_util_js_1 = require("./bit-width-util.js");
var value_type_js_1 = require("./value-type.js");
var value_type_util_js_1 = require("./value-type-util.js");
var StackValue = /** @class */ (function () {
    function StackValue(builder, type, width, value, offset) {
        if (value === void 0) { value = null; }
        if (offset === void 0) { offset = 0; }
        this.builder = builder;
        this.type = type;
        this.width = width;
        this.value = value;
        this.offset = offset;
    }
    StackValue.prototype.elementWidth = function (size, index) {
        if ((0, value_type_util_js_1.isInline)(this.type))
            return this.width;
        for (var i = 0; i < 4; i++) {
            var width = 1 << i;
            var offsetLoc = size + (0, bit_width_util_js_1.paddingSize)(size, width) + index * width;
            var offset = offsetLoc - this.offset;
            var bitWidth = (0, bit_width_util_js_1.uwidth)(offset);
            if (1 << bitWidth === width) {
                return bitWidth;
            }
        }
        throw "Element is unknown. Size: ".concat(size, " at index: ").concat(index, ". This might be a bug. Please create an issue https://github.com/google/flatbuffers/issues/new");
    };
    StackValue.prototype.writeToBuffer = function (byteWidth) {
        var newOffset = this.builder.computeOffset(byteWidth);
        if (this.type === value_type_js_1.ValueType.FLOAT) {
            if (this.width === bit_width_js_1.BitWidth.WIDTH32) {
                this.builder.view.setFloat32(this.builder.offset, this.value, true);
            }
            else {
                this.builder.view.setFloat64(this.builder.offset, this.value, true);
            }
        }
        else if (this.type === value_type_js_1.ValueType.INT) {
            var bitWidth = (0, bit_width_util_js_1.fromByteWidth)(byteWidth);
            this.builder.pushInt(this.value, bitWidth);
        }
        else if (this.type === value_type_js_1.ValueType.UINT) {
            var bitWidth = (0, bit_width_util_js_1.fromByteWidth)(byteWidth);
            this.builder.pushUInt(this.value, bitWidth);
        }
        else if (this.type === value_type_js_1.ValueType.NULL) {
            this.builder.pushInt(0, this.width);
        }
        else if (this.type === value_type_js_1.ValueType.BOOL) {
            this.builder.pushInt(this.value ? 1 : 0, this.width);
        }
        else {
            throw "Unexpected type: ".concat(this.type, ". This might be a bug. Please create an issue https://github.com/google/flatbuffers/issues/new");
        }
        this.offset = newOffset;
    };
    StackValue.prototype.storedWidth = function (width) {
        if (width === void 0) { width = bit_width_js_1.BitWidth.WIDTH8; }
        return (0, value_type_util_js_1.isInline)(this.type) ? Math.max(width, this.width) : this.width;
    };
    StackValue.prototype.storedPackedType = function (width) {
        if (width === void 0) { width = bit_width_js_1.BitWidth.WIDTH8; }
        return (0, value_type_util_js_1.packedType)(this.type, this.storedWidth(width));
    };
    StackValue.prototype.isOffset = function () {
        return !(0, value_type_util_js_1.isInline)(this.type);
    };
    return StackValue;
}());
exports.StackValue = StackValue;
