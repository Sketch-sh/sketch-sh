const fs = require("fs");
const path = require("path");

module.exports = function serializeBinary(binFileName, jsFileName) {
  const binContent = fs.readFileSync(binFileName, "binary");
  const binLength = binContent.length;
  const arrayStr1 = [];
  for (let i = 0; i < 256; i++) {
    arrayStr1.push(String.fromCharCode(i));
  }
  const arrayConv = [
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
  ];
  const fd = fs.openSync(jsFileName, "a");
  fs.writeSync(fd, '"');
  for (let i = 0, i_finish = (binLength - 1) | 0; i <= i_finish; i++) {
    const c = binContent.charCodeAt(i);
    let exit = 0;
    if (c >= 32) {
      if (c >= 127) {
        if (c >= 128) {
          fs.writeSync(fd, "\\x");
          fs.writeSync(fd, arrayConv[c >>> 4]);
          fs.writeSync(fd, arrayConv[c & 15]);
        } else {
          exit = 1;
        }
      } else if (c !== 92) {
        if (c === /* "\"" */ 34) {
          fs.writeSync(fd, "\\");
          fs.writeSync(fd, arrayStr1[c]);
        } else {
          fs.writeSync(fd, arrayStr1[c]);
        }
      } else {
        fs.writeSync(fd, "\\\\");
      }
    } else if (c >= 14) {
      exit = 1;
    } else {
      switch (c) {
        case 0:
          if (
            i === ((binLength - 1) | 0) ||
            binContent.charCodeAt((i + 1) | 0) < /* "0" */ 48 ||
            binContent.charCodeAt((i + 1) | 0) > /* "9" */ 57
          ) {
            fs.writeSync(fd, "\\0");
          } else {
            exit = 1;
          }
          break;
        case 8:
          fs.writeSync(fd, "\\b");
          break;
        case 9:
          fs.writeSync(fd, "\\t");
          break;
        case 10:
          fs.writeSync(fd, "\\n");
          break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 11:
          exit = 1;
          break;
        case 12:
          fs.writeSync(fd, "\\f");
          break;
        case 13:
          fs.writeSync(fd, "\\r");
          break;
      }
    }
    if (exit === 1) {
      fs.writeSync(fd, "\\x");
      fs.writeSync(fd, arrayConv[c >>> 4]);
      fs.writeSync(fd, arrayConv[c & 15]);
    }
  }
  fs.writeSync(fd, '"');
  fs.closeSync(fd);
  return /* () */ 0;
};
