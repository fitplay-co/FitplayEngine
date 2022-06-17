
const readline = require('readline');
 
readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);
 
process.stdin.on('keypress', (str, key) => {
    console.log(str)
    if(str == 'd') console.log("fuck")
    console.log(key)
//按住ctrl+d退出
    if(key.ctrl === true && key.name === 'd'){
        process.exit(0)
    }
 
})