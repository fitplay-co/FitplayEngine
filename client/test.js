var str = "a\r\nb\r\nc";
var s = str.split("\r\n")
console.log(s[1]=='b')
console.log(str);
str = str.replace(/\r\n/g,"");
console.log(str);